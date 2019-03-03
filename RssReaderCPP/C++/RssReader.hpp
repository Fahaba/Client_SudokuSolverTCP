#pragma once

#include "Box.h"
#include <stdio.h>
#include <ole2.h>
#include <xmllite.h>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <vector>
#include <sstream>
#include <iostream>
#include <cwchar>
#include <map>
#include <regex>

#pragma warning(disable : 4127)  // conditional expression is constant
#define CHKHR(stmt)             do { hr = (stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define ASSERT(stmt)            do { if(!(stmt)) { DbgRaiseAssertionFailure(); } } while(0)
#define SAFE_RELEASE(I)         do { if (I){ I->Release(); } I = NULL; } while(0)

class CRssReader : public IBindStatusCallback
{
public:
	CRssReader(const wchar_t* destRss, Box &box) : _rssSite(destRss), m_box(box), _ulRef(1), _spXmlReader(NULL) {};
    virtual ~CRssReader();

    HRESULT ReadAsync();
    HRESULT ReadSync();
	void ReadLoop();
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // IBindStatusCallback
    virtual HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, __RPC__in_opt IBinding *pib);
    virtual HRESULT STDMETHODCALLTYPE GetPriority(__RPC__out LONG *pnPriority);        
    virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved);
    virtual HRESULT STDMETHODCALLTYPE OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, __RPC__in_opt LPCWSTR szStatusText);
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, __RPC__in_opt LPCWSTR szError);
    virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD *grfBINDF, BINDINFO *pbindinfo);
    virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pformatetc, STGMEDIUM *pstgmed);
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(__RPC__in REFIID riid, __RPC__in_opt IUnknown *punk);

	const wchar_t* _rssSite;
	int get_month_index(wchar_t* name);
	time_t _feedPubDate;
	Box &m_box;
	int m_ILastRecognizedMessage = 0;
	
private:
    HRESULT Parse();

private:
    ULONG _ulRef;
    bool _bCompleted;
    bool _bAsync;
	

    IXmlReader* _spXmlReader;
};