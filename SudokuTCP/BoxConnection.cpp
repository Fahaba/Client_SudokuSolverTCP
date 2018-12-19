#include "BoxConnection.h"


void BoxConnection::CreateServer()
{
    int result;
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        std::cerr << "cant start winsock, ERR " << wsResult << std::endl;
        exit(-1);
    }
    // create socket

    // INTERNET ADRESS STYLE FAMILY AF_INET[6]
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "cant create socket ERR " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(-1);
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(m_port);
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    bind(sock, (sockaddr*)&server, sizeof(server));

    result = listen(sock, SOMAXCONN);
    if (result != SOCKET_ERROR)
        std::cout << "listen() läuft" << std::endl;
    else
        std::cout << "listen() ERR " << WSAGetLastError() << std::endl;


    sockaddr_in boxInfo;
    int boxInfoLen = sizeof(boxInfo);

    SOCKET boxCon = accept(sock, (sockaddr*)&boxInfo, &boxInfoLen);
    if (boxCon != SOCKET_ERROR)
    {
        char ipbuf[INET_ADDRSTRLEN];
        std::string localIPAddr = inet_ntop(AF_INET, &boxInfo.sin_addr, ipbuf, sizeof(ipbuf));
        std::cout << "client accepted" << ipbuf << ":" << ntohs(boxInfo.sin_port) << std::endl;
    }

    char host[NI_MAXHOST];
    char service[NI_MAXHOST];

    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXHOST);

    if (getnameinfo((sockaddr*)&boxInfo, sizeof(boxInfo), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
    {
        std::cout << host << " connected on port " << service << std::endl;
    }
    else
    {
        inet_ntop(AF_INET, &boxInfo.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on port " <<
            ntohs(boxInfo.sin_port) << std::endl;
    }

    // close socket - not
    char buf[4096];
    while (true)
    {
        memset(buf, 0, sizeof(buf));
        int bytesReceived = recv(boxCon, buf, 4096, 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            //cerr << "rip" << endl;
        }

        if (bytesReceived == 0)
        {
            std::cout << "client disconnected " << std::endl;
            break;
        }

        send(boxCon, buf, bytesReceived + 1, 0);
    }
}


BoxConnection::~BoxConnection()
{
}
