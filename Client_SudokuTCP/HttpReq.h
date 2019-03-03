#pragma once
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
class HttpReq
{
public:
	HttpReq(
		const char* verb,
		const char* hostname,
		int port,
		const char* resource,
		const char* opt_urlencoded,
		std::string& response);
};