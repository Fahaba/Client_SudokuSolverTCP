#include <string>
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <sstream>
#include "Box.h"
#include <stdlib.h> 
#include <regex>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


class BoxConnection : public std::thread
{
public:
    Box &m_box;
    int m_port;
    SOCKET m_socket;
    BoxConnection(Box &box, int port) : m_port(port), m_box(box) {}
    void CreateServer();
    ~BoxConnection();
    void ConnectToOtherBoxes();
    void HandleMessages(char* buf);
    void CalculatePossibleValues();
};

