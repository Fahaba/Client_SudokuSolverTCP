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


    fd_set master;
    FD_ZERO(&master);

    FD_SET(sock, &master);

    char buf[4096];
    // connect to other boxes
    //for (auto otherBox : m_box.m_otherBoxes)
    //{
    //    if (otherBox.first == m_box.m_name)
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

    //    /*sockaddr_in client_box;
    //    memset(&client_box, 0, sizeof(sockaddr_in));
    //    client_box.sin_family = AF_INET;
    //    client_box.sin_port = htons(port);

    //    bind(boxSocket, (sockaddr*)&client_box, sizeof(client_box));*/

    //    int connResult = connect(boxSocket, (sockaddr*)&hint_box, sizeof(hint_box));
    //    if (connResult == SOCKET_ERROR)
    //    {
    //        std::cerr << "cant connect to other box" << otherBox.first << "ERRORCODE: " << WSAGetLastError() << std::endl;
    //        // cleanup on ERR
    //        closesocket(boxSocket);
    //        WSACleanup();
    //        return;
    //    }
    //    else
    //    {
    //        std::cout << "connected to box" << std::endl;
    //    }

    //    int res = sendto(boxSocket, otherBox.first.c_str(), sizeof(otherBox.first)+1, 0, (sockaddr*)&hint_box, sizeof(hint_box));
    //    if (res == SOCKET_ERROR)
    //    {
    //        std::cerr << "cant connect to server ERR" << WSAGetLastError() << std::endl;
    //        // cleanup on ERR
    //        closesocket(sock);
    //        WSACleanup();
    //        return;
    //    }
    //    else
    //    {
    //        std::cout << "Send " << otherBox.first.c_str() << " to box" << std::endl;
    //    }
    //}

    m_socket = sock;
    Sleep(2000);
    //ConnectToOtherBoxes();

    while (true)
    {
        //ConnectToOtherBoxes();
        fd_set copy = master;

        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
        
        for (int i = 0; i < socketCount; i++)
        {
            memset(buf, 0, sizeof(buf));
            SOCKET s = copy.fd_array[i];
            if (s == sock)
            { 
                std::cout << "waiting for a connection " << std::endl; // accept connection
                SOCKET client = accept(sock, nullptr, nullptr);

                // add new connection to list of connected clients
                FD_SET(client, &master);
                
            }
            else
            {
                memset(buf, 0, sizeof(buf));
                int bytesIn = recv(s, buf, sizeof(buf), 0);
                if (bytesIn < 0)
                {
                    //drop client
                    closesocket(s);
                    FD_CLR(s, &master);
                }
                else
                {
                    std::cout << buf << " " << std::endl;
                    for (int i = 0; i < master.fd_count; i++)
                    {
                        SOCKET outSock = master.fd_array[i];
                        if (outSock != sock && outSock != s)
                        {
                            send(outSock, "asd123\0\n", strlen("asd123\0\n")+1/*bytesIn*/, 0);
                        }
                    }
                }
                // accept a new message
                // send message to other clients ++ NOT LISTEN TO SOCKET
            }

        }
    }

    //sockaddr_in boxInfo;
    //int boxInfoLen = sizeof(boxInfo);

    //SOCKET boxCon = accept(sock, (sockaddr*)&boxInfo, &boxInfoLen);
    //if (boxCon != SOCKET_ERROR)
    //{
    //    char ipbuf[INET_ADDRSTRLEN];
    //    std::string localIPAddr = inet_ntop(AF_INET, &boxInfo.sin_addr, ipbuf, sizeof(ipbuf));
    //    std::cout << "client accepted" << ipbuf << ":" << ntohs(boxInfo.sin_port) << std::endl;
    //}

    //char host[NI_MAXHOST];
    //char service[NI_MAXHOST];

    //memset(host, 0, NI_MAXHOST);
    //memset(service, 0, NI_MAXHOST);

    //if (getnameinfo((sockaddr*)&boxInfo, sizeof(boxInfo), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
    //{
    //    std::cout << host << " connected on port " << service << std::endl;
    //}
    //else
    //{
    //    inet_ntop(AF_INET, &boxInfo.sin_addr, host, NI_MAXHOST);
    //    std::cout << host << " connected on port " <<
    //        ntohs(boxInfo.sin_port) << std::endl;
    //}

    //// close socket - not
    //char buf[4096];
    //while (true)
    //{
    //    memset(buf, 0, sizeof(buf));
    //    int bytesReceived = recv(boxCon, buf, 4096, 0);
    //    if (bytesReceived == SOCKET_ERROR)
    //    {
    //        //cerr << "rip" << endl;
    //    }

    //    if (bytesReceived == 0)
    //    {
    //        std::cout << "client disconnected " << std::endl;
    //        break;
    //    }

    //    send(boxCon, buf, bytesReceived + 1, 0);
    //}
}

void BoxConnection::ConnectToOtherBoxes()
{
    SOCKET sock = m_socket;
    char buf[4096];
    // connect to other boxes
    for (auto otherBox : m_box.m_otherBoxes)
    {
        if (otherBox.first == m_box.m_name)
            continue;

        std::cout << "sending to " << otherBox.first.c_str() << std::endl;
        SOCKET boxSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in hint_box;
        memset(&hint_box, 0, sizeof(sockaddr_in));
        hint_box.sin_family = AF_INET;
        hint_box.sin_port = htons(otherBox.second.port); // host to network short
        //hint_box.sin_addr.S_un.S_addr = otherBox.second.ip.c_str();
        inet_pton(AF_INET, otherBox.second.ip.c_str(), &hint_box.sin_addr); // inet pointer to string to number
        memset(buf, 0, sizeof(buf));

        sockaddr_in client_box;
        memset(&client_box, 0, sizeof(sockaddr_in));
        client_box.sin_family = AF_INET;
        client_box.sin_port = htons(m_port);
        
        bind(boxSocket, (sockaddr*)&client_box, sizeof(client_box));

        int connResult = connect(boxSocket, (sockaddr*)&hint_box, sizeof(hint_box));
        if (connResult == SOCKET_ERROR)
        {
            std::cerr << "cant connect to other box" << otherBox.first << "ERRORCODE: " << WSAGetLastError() << std::endl;
            // cleanup on ERR
            //closesocket(boxSocket);
            //WSACleanup();
            //return;
        }
        else
        {
            std::cout << "connected to box" << std::endl;
        }

        std::string sendStr = otherBox.first.c_str();
        sendStr.append("\n");
        memset(&buf, 0, sizeof(buf));
        strcpy_s(buf, sendStr.c_str());

        int res = sendto(boxSocket, otherBox.first.c_str(), sizeof(otherBox.first) + 1, 0, (sockaddr*)&hint_box, sizeof(hint_box));
        if (res == SOCKET_ERROR)
        {
            std::cerr << "cant connect to server ERR" << WSAGetLastError() << std::endl;
            // cleanup on ERR
            closesocket(sock);
            WSACleanup();
            return;
        }
        else
        {
            std::cout << "Send " << otherBox.first.c_str() << " to box" << std::endl;
        }
    }
}

BoxConnection::~BoxConnection()
{
}
