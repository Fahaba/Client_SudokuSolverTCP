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
	CRssReader* reader = new CRssReader(L"http://localhost/rss.xml", *box);
	std::thread CRssThread(&CRssReader::ReadLoop, reader);

    string ipAdress = hostAddr;  // IP Adress of the server
	string localIPAddr = GetLocalIPAdress();
    

	// setup RssReader functionality
	

    
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

    // connect to server
    //int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    //if (connResult == SOCKET_ERROR)
    //{
    //    cerr << "cant connect to server ERR" << WSAGetLastError() << endl;
    //    // cleanup on ERR
    //    closesocket(sock);
    //    WSACleanup();
    //    return -1;
    //}
    //else
    //{
    //    cout << "connected!!!" << endl;
    //}
    // do while loop to send and reveive data
    char buf[4096];

    stringstream ss;
    //ss << boxName << "," << localIPAddr << "," << port << endl;
    //string initMessage = ss.str();
    //ss.str("");

    //    if (initMessage.size() > 0)
    //    {
    //        //userInput += '\n';
    //        // send
    //        int sendResult = send(sock, initMessage.c_str(), sizeof(initMessage), 0); // \0 at the end > +1
    //        if (sendResult != SOCKET_ERROR)
    //        {
    //            
    //            // wait response
    //            memset(&buf, 0, sizeof(buf));
    //            int bytesReceived = recv(sock, buf, 4096, 0);
    //            if (bytesReceived > 0)
    //            {
    //                // echo response to console
    //                cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
    //                //stringstream ss;
    //                string boxName;
    //                vector<string> boxNames = box->GetBoxNames();
    //                for (string boxStr : boxNames)
    //                {
    //                    boxName = boxStr;
    //                    boxStr.append("\n\0");
    //                    memset(&sendbuf, 0, sizeof(sendbuf));
    //                    strcpy_s(sendbuf, boxStr.c_str());
    //                    
    //                    memset(&buf, 0, sizeof(buf));
    //                    ss.str("");
    //                    ss << boxStr << endl;
    //                    //string boxQuery = "";
    //                    //cin.getline(sendbuf, 256);
    //                    //cout << boxQuery << "\n";
    //                    
    //                    int sendResult = send(sock, sendbuf, strlen(sendbuf), 0);
    //                    if (sendResult != SOCKET_ERROR)
    //                    {
    //                        
    //                        int bytesReceived;
    //                            bytesReceived = recv(sock, buf, 4096, 0);
    //                            if (bytesReceived > 0)
    //                            {
    //                                cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
    //                                box->AddOtherBox(boxName, string(buf, 0, bytesReceived));
    //                            }
    //                    }
    //                    
    //                    Sleep(100);
    //                }                    
    //            }
    //        }
    //    }


        //BoxConnection *pboxCon = new BoxConnection(*box, port);
        
    //std::thread threatObj(&BoxConnection::CreateServer, pboxCon);
    
    // connect to other boxes
    //for (auto otherBox : box->m_otherBoxes)
    //{
    //    if (otherBox.first == box->m_name)
    //        continue;

    //    std::cout << "sending to " << otherBox.first.c_str() << std::endl;
    //    SOCKET boxSocket = socket(AF_INET, SOCK_STREAM, 0);
    //    sockaddr_in hint_box;
    //    memset(&hint_box, 0, sizeof(sockaddr_in));
    //    hint_box.sin_family = AF_INET;
    //    hint_box.sin_port = htons(otherBox.second.port); // host to network short
    //    //hint_box.sin_addr.S_un.S_addr = otherBox.second.ip.c_str();
    //    inet_pton(AF_INET, otherBox.second.ip.c_str(), &hint_box.sin_addr); // inet pointer to string to number
    //    memset(buf, 0, sizeof(buf));

    //    sockaddr_in client_box;
    //    memset(&client_box, 0, sizeof(sockaddr_in));
    //    client_box.sin_family = AF_INET;
    //    client_box.sin_port = htons(port);

    //    bind(boxSocket, (sockaddr*)&client_box, sizeof(client_box));

    //    int connResult = connect(boxSocket, (sockaddr*)&hint_box, sizeof(hint_box));
    //    if (connResult == SOCKET_ERROR)
    //    {
    //        std::cerr << "cant connect to other box" << otherBox.first << "ERRORCODE: " << WSAGetLastError() << std::endl;
    //        // cleanup on ERR
    //        //closesocket(boxSocket);
    //        //WSACleanup();
    //        //return;
    //    }
    //    else
    //    {
    //        std::cout << "connected to box" << std::endl;
    //        box->AddConnection(otherBox.first, boxSocket, hint_box, client_box);
    //    }

    //    // send initial config to boxes
    //    int row = 0;
    //    for (auto arr : box->m_boxField)
    //    {
    //        for (int col = 0; col < arr.size(); col++)
    //        {
    //            if (box->m_boxField[row][col] == 0)
    //                continue;

    //            ss = std::stringstream();
    //            ss << box->m_name;
    //            ss << "," << row << "," << col << "," << box->m_boxField[row][col] << std::endl << "\0";
    //            memset(&buf, 0, sizeof(buf));
    //            strcpy_s(buf, ss.str().c_str());
    //            //sendto(boxSocket, ">> ", 3, 0, (sockaddr*)&hint_box, sizeof(hint_box));
    //            int res = sendto(boxSocket, ss.str().c_str(), strlen(ss.str().c_str()), 0, (sockaddr*)&hint_box, sizeof(hint_box));
    //            //int res = sendto(boxSocket, buf, sizeof(buf) + 1, 0, (sockaddr*)&hint_box, sizeof(hint_box));
    //            memset(&buf, 0, sizeof(buf));
    //            if (res == SOCKET_ERROR)
    //            {
    //                std::cerr << "cant connect to server ERR" << WSAGetLastError() << std::endl;
    //                // cleanup on ERR
    //                closesocket(sock);
    //                WSACleanup();
    //                return -1;
    //            }
    //            else
    //            {
    //                Sleep(1);
    //                //std::cout << "Send " << buf << " to box" << std::endl;
    //            }
    //        }
    //        row++;
    //    }
    //}
	CRssThread.join();
    //threatObj.join();
    closesocket(sock);
    WSACleanup();
    return 0;
}