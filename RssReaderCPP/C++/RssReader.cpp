#include "RssReader.hpp"
#include <string>

CRssReader::~CRssReader()
{
    //ASSERT(_ulRef == 1);
    SAFE_RELEASE(_spXmlReader);
}

HRESULT CRssReader::ReadAsync()
{
	LPCWSTR pwszUrl = _rssSite;
    HRESULT hr = S_OK;

    IBindCtx* spBindCtx = NULL;
    IMoniker* spMoniker = NULL;
    IStream*  spStream = NULL;

    _bAsync = true;

    CHKHR(::CreateURLMonikerEx(NULL, pwszUrl, &spMoniker, URL_MK_UNIFORM));
    CHKHR(::CreateAsyncBindCtx(0, static_cast<IBindStatusCallback*>(this), NULL, &spBindCtx));
    CHKHR(::CreateXmlReader(IID_IXmlReader, (void**) &_spXmlReader, NULL));

    // Kicks off the binding.
    #pragma warning(suppress: 6011) //spMoniker wouldn't be null if CreateURLMonikerEx() succeeds.
    CHKHR(spMoniker->BindToStorage(spBindCtx, NULL, IID_IStream, (void **)&spStream));

    // Pumps message until the download is finished.
    MSG msg;
    while (_bCompleted == false)
    {
        if (::PeekMessage(&msg, 0, 0 ,0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    CHKHR(::RevokeBindStatusCallback(spBindCtx, static_cast<IBindStatusCallback*>(this)));  

CleanUp:
    SAFE_RELEASE(spBindCtx);
    SAFE_RELEASE(spMoniker);
    SAFE_RELEASE(spStream);
    SAFE_RELEASE(_spXmlReader);
    return hr;
}

void CRssReader::ReadLoop()
{
	auto start = std::chrono::system_clock::now();

	while (true)
	{
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		if (elapsed_seconds.count() > 5)
		{
			start = std::chrono::system_clock::now();
			ReadSync();
		}
	}
}

HRESULT CRssReader::ReadSync()
{
	LPCWSTR pwszUrl = _rssSite;
    HRESULT hr = S_OK;

    IBindCtx* spBindCtx = NULL;
    IMoniker* spMoniker = NULL;
    IStream*  spStream = NULL;
    _bAsync = false;
    CHKHR(URLOpenBlockingStream(NULL, pwszUrl, &spStream, NULL, static_cast<IBindStatusCallback*>(this)));
    CHKHR(::CreateXmlReader(IID_IXmlReader, (void**) &_spXmlReader, NULL));
    CHKHR(_spXmlReader->SetInput(spStream));
    
    CHKHR(Parse());

CleanUp:
    SAFE_RELEASE(spBindCtx);
    SAFE_RELEASE(spMoniker);
    SAFE_RELEASE(spStream);
    SAFE_RELEASE(_spXmlReader);
    return hr;
}

HRESULT STDMETHODCALLTYPE CRssReader::QueryInterface(REFIID riid, void **ppvObject)
{
    if (ppvObject == NULL)
        return E_INVALIDARG;

    if (riid == IID_IUnknown || riid == IID_IBindStatusCallback)
    {
        *ppvObject = dynamic_cast<IBindStatusCallback*>(this);
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE CRssReader::AddRef(void)
{
    _ulRef++;

    return _ulRef;
}

ULONG STDMETHODCALLTYPE CRssReader::Release(void)
{
    _ulRef--;

    return _ulRef;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnStartBinding(DWORD , __RPC__in_opt IBinding*)
{
    _bCompleted = false;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::GetPriority(__RPC__out LONG *pnPriority)
{
    *pnPriority = NORMAL_PRIORITY_CLASS;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnLowResource(DWORD )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnProgress(ULONG , ULONG , ULONG , __RPC__in_opt LPCWSTR )
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnStopBinding(HRESULT , __RPC__in_opt LPCWSTR )
{
    _bCompleted = true;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo)
{
    // Describes how the bind process is handled.
    *grfBINDF = BINDF_NOWRITECACHE | BINDF_RESYNCHRONIZE;

    if (_bAsync)
        *grfBINDF |= BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE;

    // Describes how we want the binding to occur.
    DWORD cbSize;
    cbSize = pbindinfo->cbSize;
    memset(pbindinfo, 0, cbSize);
    pbindinfo->cbSize = cbSize;
    pbindinfo->dwBindVerb = BINDVERB_GET;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnDataAvailable(DWORD , DWORD , FORMATETC *, STGMEDIUM *pstgmed)
{
    HRESULT hr = S_OK; 
    static BOOL firstDataNotification = TRUE;
    if (firstDataNotification)
    {
        hr = _spXmlReader->SetInput(pstgmed->pstm);
        if (SUCCEEDED(hr))
        {
            firstDataNotification = FALSE;
        
            hr = Parse();
            // Expects only partial data is available.
            if (hr == E_PENDING)
                hr = S_OK;
        }
    }
    else
    {
        hr = Parse();

        // Expects only partial data is available.
        if (hr == E_PENDING)
            hr = S_OK;
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CRssReader::OnObjectAvailable(__RPC__in REFIID , __RPC__in_opt IUnknown* )
{
    return S_OK;
}

HRESULT CRssReader::Parse()
{
    HRESULT hr;
	bool foundItem = false;
	bool foundDesc = false;
	bool foundPubDate = false;
    XmlNodeType nodeType;
    const wchar_t* pwszPrefix;
    const wchar_t* pwszLocalName;
	const wchar_t* pwszValue;
    UINT cwchPrefix;
	bool shouldStop = false;
    // Parses the stream with XmlLite when progressing.
	// for Sudoku use case we only need the description of items
    while (S_OK == (hr = _spXmlReader->Read(&nodeType)))
    {
		// no new information found in Rss Feed
		// so stop parsing
		if (shouldStop)
			break;

        const UINT buffSize = 8;
        wchar_t buff[buffSize];
        UINT charsRead = 0;
		
        switch (nodeType)
        {
        case XmlNodeType_Element:
            CHKHR(_spXmlReader->GetPrefix(&pwszPrefix, &cwchPrefix));
            CHKHR(_spXmlReader->GetLocalName(&pwszLocalName, NULL));

			if ((foundItem && foundDesc) || foundPubDate)
				break;

			if (std::wcscmp(pwszLocalName, L"pubDate") == 0)
				foundPubDate = true;

			if (std::wcscmp(pwszLocalName, L"item") == 0)
				foundItem = true;

			if (foundItem && std::wcscmp(pwszLocalName, L"description") == 0)
				foundDesc = true;

            break;
        case XmlNodeType_EndElement:
            CHKHR(_spXmlReader->GetPrefix(&pwszPrefix, &cwchPrefix));
            CHKHR(_spXmlReader->GetLocalName(&pwszLocalName, NULL));
			
			if (std::wcscmp(pwszLocalName, L"pubDate") == 0)
				foundPubDate = false;

			if (std::wcscmp(pwszLocalName, L"item") == 0)
				foundItem = false;

			if (std::wcscmp(pwszLocalName, L"description") == 0)
				foundDesc = false;
            
            break;
        case XmlNodeType_Text:
			CHKHR(_spXmlReader->GetValue(&pwszValue, NULL));

			wchar_t* buffer;
			wchar_t* input = new wchar_t[4096];
			wcscpy(input, pwszValue);
			std::vector<wchar_t*> tokens;
			if (std::wcscmp(pwszLocalName, L"title") == 0)
			{
				wchar_t* token = wcstok_s(input, L":", &buffer);
				tokens.push_back(token);
			}

			// only continue if new message received
			
			if (tokens.size() > 1)
			{
				if (_wtoi(tokens[1]) <= m_ILastRecognizedMessage)
					break;

				m_ILastRecognizedMessage = _wtoi(tokens[1]);
			}

			if ((!foundItem || !foundDesc) && !foundPubDate)
				break;

			tokens.clear();

			if (foundPubDate)
			{
				foundPubDate = false;

				time_t rawtime;
				struct tm * timeInfo;
				time(&rawtime);
				timeInfo = localtime(&rawtime);
				//const size_t length = wcslen(pwszValue);
				wcscpy(input, pwszValue);
				wchar_t* token = wcstok_s(input, L",", &buffer);
				while (token)
				{
					//std::wcout << token << "\n";
					tokens.push_back(token);
					token = std::wcstok(nullptr, L" ", &buffer);
				}
				// split time to tokens
				token = wcstok_s(tokens[4], L":", &buffer);
				while (token)
				{
					//std::wcout << token << "\n";
					tokens.push_back(token);
					token = std::wcstok(nullptr, L":", &buffer);
				}

				timeInfo->tm_mday = _wtoi(tokens[1]);
				timeInfo->tm_mon  = get_month_index(tokens[2]);
				timeInfo->tm_year = _wtoi(tokens[3]) - 1900;
				timeInfo->tm_hour = _wtoi(tokens[6]);
				timeInfo->tm_min  = _wtoi(tokens[7]);
				timeInfo->tm_sec  = _wtoi(tokens[8]);

				time_t feedPubDate = mktime(timeInfo);

				if (feedPubDate <= _feedPubDate)
					shouldStop = true;

				_feedPubDate = feedPubDate;
			}

			// new description or pubDate found..
			// redirect this to the box
			// maybe check for old entries first
			if (!shouldStop)
			{
				//wprintf(L"Text: %s\n", pwszValue);
				// handle messages 

				char ch[260];
				char DefChar = ' ';
				WideCharToMultiByte(CP_ACP, 0, pwszValue, -1, ch, 260, &DefChar, NULL);

				std::stringstream ss(ch);
				std::string to = ss.str();

				std::regex reg("^BOX_[A,D,G][1,4,7],[0-2],[0-2],[1-9]$");
				std::smatch match;

				while (std::regex_search(to, match, reg))
				{
					// found a valid message
					// set intitial values for other boxes
					std::stringstream ss_to(to);
					std::string msg_toc;
					std::vector<std::string> parts;
					// split string and extract values
					while (std::getline(ss_to, msg_toc, ','))
					{
						parts.push_back(msg_toc);

					}
					// elim match to prevent loop
					to = match.suffix().str();

					//std::vector<std::pair<std::string, Box::BoxField> >::iterator it = m_box.m_boxGrid.begin();

					m_box.SetValueInGrid(parts[0], std::stoi(parts[1]), std::stoi(parts[2]), std::stoi(parts[3]));
					//if (m_box.possibleForBox == 1)
					//{
					//	// close reader
					//	exit(1);
					//}
				}
			}
            break;
        }
    }

CleanUp:
    return hr;
}

int CRssReader::get_month_index(wchar_t* name)
{
	std::map<std::string, int> months
	{
		{ "Jan", 1 },
		{ "Feb", 2 },
		{ "Mar", 3 },
		{ "Apr", 4 },
		{ "May", 5 },
		{ "Jun", 6 },
		{ "Jul", 7 },
		{ "Aug", 8 },
		{ "Sep", 9 },
		{ "Oct", 10 },
		{ "Nov", 11 },
		{ "Dec", 12 }
	};
	char str[4];
	wcstombs(str, name, 3);
	str[3] = '\0';
	const auto iter(months.find(str));

	if (iter != months.cend())
		return iter->second;
	return -1;
}