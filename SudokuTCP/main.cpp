#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include "Box.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std; // maybe not good

void main()
{
    Box *box = new Box("./fieldCFG.conf");
    box->print();


    string ipAdress = "127.0.0.1";  // IP Adress of the server
    int port = 1337;               // listening port number on the server

    // initialize winsock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "cant start winsock, ERR " << wsResult << endl;
        return;
    }
    // create socket

    // INTERNET ADRESS STYLE FAMILY AF_INET[6]
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "cant create socket ERR " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // fill in a hint structure
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port); // host to network short
    inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr); // inet pointer to string to number
    
    // connect to server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "cant connect to server ERR" << WSAGetLastError() << endl;
        // cleanup on ERR
        closesocket(sock);
        WSACleanup();
        return;
    }
    else
    {
        cout << "connected!!!" << endl;
    }
    // do while loop to send and reveive data
    char buf[4096];
    string userInput;

    do
    {
        // wait for input
        cout << "> ";
        getline(cin, userInput);

        if (userInput.size() > 0)
        {
            userInput += '\n';
            // send
            int sendResult = send(sock, userInput.c_str(), sizeof(userInput), 0); // \0 at the end > +1
            if (sendResult != SOCKET_ERROR)
            {
                // wait response
                ZeroMemory(buf, 4096);
                int bytesReceived = recv(sock, buf, 4096, 0);
                if (bytesReceived > 0)
                {
                    // echo response to console
                    cout << "SERVER> " << string(buf, 0, bytesReceived) << endl;
                }
            }
        }

    } while (userInput.size() > 0);

    // close everything
    closesocket(sock);
    WSACleanup();

}