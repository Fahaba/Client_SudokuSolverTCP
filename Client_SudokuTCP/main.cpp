#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <sstream>
#include <thread>

#include <stdio.h>
#include <ole2.h>
#include <xmllite.h>
#include <stdlib.h>
//#include <io.h>
//#include <fcntl.h>
#include "Box.h"
#include "BoxConnection.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "xmllite.lib")
// this was hard oO

#include "RssReader.hpp"
using namespace std; // maybe not good

char sendbuf[256];

string GetLocalIPAdress()
{
	// get local ip adress table
	PMIB_IPADDRTABLE pIPAddrTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	IN_ADDR IPAddre;

	pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));

	if (pIPAddrTable)
	{
		if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
			ERROR_INSUFFICIENT_BUFFER)
		{
			free(pIPAddrTable);
			pIPAddrTable = (MIB_IPADDRTABLE *)malloc(dwSize);
		}
	}
	if (pIPAddrTable == nullptr)
	{
		cerr << "coudlnt malloc for GetIpAddrTable" << endl;
		exit(-1);
	}

	// get ipAddrTable
	if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR)
	{
		// error
		cerr << "GetIpAddr failed with error " << dwRetVal << endl;
		exit(-1);
	}

	char ipbuf[INET_ADDRSTRLEN];
	IPAddre.S_un.S_addr = (u_long)pIPAddrTable->table[0].dwAddr;
	return inet_ntop(AF_INET, &IPAddre, ipbuf, sizeof(ipbuf));
}

sockaddr_in CreateHintStructure(string ip, int port)
{
	sockaddr_in hint;
	memset(&hint, 0, sizeof(sockaddr_in));
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); // host to network short
	// change this
	if (ip != "")
		inet_pton(AF_INET, ip.c_str(), &hint.sin_addr); // inet pointer to string to number
	return hint;
}

int main(int argc, char *argv[])
{	

    if (argc < 5)
    {
        exit(-1);
        return -1;
    }
	
    //_setmode(_fileno(stdout), _O_U16TEXT);

    string boxName = argv[1];
    //string configPath = argv[2];
	string configStr = argv[2];
    string hostAddr = argv[3];
    int port = stoi(argv[4]);

    Box *box = new Box(configStr, boxName);
    box->print();

	// setup rss reader
	CRssReader* reader = new CRssReader(L"http://localhost:80/rss.xml", *box);
	std::thread CRssThread(&CRssReader::ReadLoop, reader);

    string ipAdress = hostAddr;  // IP Adress of the server
	string localIPAddr = GetLocalIPAdress();
    
    // initialize winsock
    WSAData data;
    int res;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "cant start winsock, ERR " << wsResult << endl;
        return -1;
    }
    // create socket

    // INTERNET ADRESS STYLE FAMILY AF_INET[6]
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "cant create socket ERR " << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    // fill in a hint structure
    sockaddr_in hint = CreateHintStructure(ipAdress, 4242);
    sockaddr_in client = CreateHintStructure("", port);

    bind(sock, (sockaddr*)&client, sizeof(client));
    box->SetSocket(sock);

   
    char buf[4096];

    stringstream ss;
   
	CRssThread.join();
    closesocket(sock);
    WSACleanup();
    return 0;
}