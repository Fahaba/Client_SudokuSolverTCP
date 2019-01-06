#include "BoxConnection.h"
#include <socketapi.h>


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


    //fd_set master;
    FD_ZERO(&m_master);
    FD_SET(sock, &m_master);

    char buf[4096];

    m_socket = sock;
    Sleep(2000);

    while (true)
    {

        fd_set copy = m_master;
        //fd_set copy = master;

        int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
        std::cout << socketCount << std::endl;
        for (int i = 0; i < socketCount; i++)
        {
            memset(buf, 0, sizeof(buf));
            SOCKET s = copy.fd_array[i];
            if (s == sock)
            {
                std::cout << "waiting for a connection " << std::endl; // accept connection
                SOCKET client = accept(sock, nullptr, nullptr);

                // add new connection to list of connected clients
                FD_SET(client, &m_master);

            }
            else
            {
                memset(buf, 0, sizeof(buf));
                int bytesIn = recv(s, buf, sizeof(buf), 0);
                if (bytesIn < 0)
                {
                    //drop client
                    closesocket(s);
                    FD_CLR(s, &m_master);
                }
                else
                {
                    HandleMessages(buf);
                    std::cout << "receiving: " << buf << " " << std::endl;
                }
                // accept a new message
                // send message to other clients ++ NOT LISTEN TO SOCKET
            }

        }
    }
}

void BoxConnection::HandleMessages(char* buf)
{
    // split messages on \n
    std::stringstream ss(buf);
    std::string to;

    if (buf != nullptr)
    {
        while (std::getline(ss, to, '\n')) {
            //std::cout << "splitted -> " << to << std::endl;

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
                    //std::cout << "tokens: " << msg_toc << std::endl;

                }
                // elim match to prevent loop
                to = match.suffix().str();

                std::vector<std::pair<std::string, Box::BoxField> >::iterator it = m_box.m_boxGrid.begin();
                
                m_box.SetValueInGrid(parts[0], std::stoi(parts[1]), std::stoi(parts[2]), std::stoi(parts[3]));
                

                for (; it != m_box.m_boxGrid.end(); ++it)
                {
                    if ((*it).first == parts[0])
                    {
                        (*it).second[std::stoi(parts[1])][std::stoi(parts[2])] = std::stoi(parts[3]);
                    }
                }
            }
        }
    }
    std::vector<newVal> newValues = m_box.CalculatePossibleValues();
    // send new Values to neighbor boxes

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

        std::stringstream ss;
        
        for (auto arr : m_box.m_boxField)
        {
            int row = 0;
            for (int col = 0; col < arr.size(); col++)
            {
                ss.str("");
                ss << m_box.m_name;
                ss << "," << row + 1 << "," << col + 1 << m_box.m_boxField[row][col] << std::endl;;
                memset(&buf, 0, sizeof(buf));
                strcpy_s(buf, ss.str().c_str());

                int res = sendto(boxSocket, buf, sizeof(buf) + 1, 0, (sockaddr*)&hint_box, sizeof(hint_box));
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
                    std::cout << "Send " << buf << " to box" << std::endl;
                }
            }
        }
        

        
    }
}

BoxConnection::~BoxConnection()
{
}
