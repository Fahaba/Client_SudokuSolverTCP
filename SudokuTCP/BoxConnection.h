#include <string>
#include <iostream>
#include <thread>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include "Box.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")


class BoxConnection : public std::thread
{
public:
    std::string m_ipAddr;
    int m_port;
    BoxConnection(std::string ipAddr, int port) : m_port(port), m_ipAddr(ipAddr) {}
    void CreateServer();
    ~BoxConnection();
};

