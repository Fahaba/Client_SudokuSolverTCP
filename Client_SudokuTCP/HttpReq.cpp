#include "HttpReq.h"

HttpReq::HttpReq(
	const char* verb,
	const char* hostname,
	int port,
	const char* resource,
	const char* opt_urlencoded,
	std::string& response)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "WSAStartup failed.\n";
		exit(1);
	}

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct addrinfo hints = {};
	struct sockaddr_in addr = {};
	addrinfo* pResultList = NULL;
	int result = getaddrinfo(hostname, NULL, &hints, &pResultList);
	if (result == 0)
		memcpy(&addr, pResultList->ai_addr, sizeof(addr));

	/*struct hostent *host;
	host = gethostbyname(hostname);*/
	
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	

	std::cout << "Connecting...\n";

	if (connect(Socket, (SOCKADDR*)(&addr), sizeof(addr)) != 0)
	{
		std::cout << "Could not connect";
		exit(1);
	}
	std::cout << "Connected.\n";

	// Build request
	std::string req = verb; // GET | POST
	req.append(" ");
	// Note, on GET, 'resource' must contain the encoded parameters, if any:
	req.append(resource);
	req.append(" HTTP/1.1\r\n");

	req.append("Host: ");
	req.append(hostname);
	req.append(":");
	req.append(std::to_string(port));
	req.append("\r\n");

	if (strcmp(verb, "POST") == 0)
	{
		req.append("Cache-Control: no-cache\r\n");
		req.append("Content-length: ");
		req.append(std::to_string(strlen(opt_urlencoded)));
		req.append("\r\n");
		req.append("Content-Type: application/x-www-form-urlencoded\r\n\r\n");

		// User is required to handle URI encoding for this value
		req.append(opt_urlencoded);

	}
	else // default, GET
	{
		req.append("Cache-Control: no-cache\r\n");
		req.append("Connection: close\r\n\r\n");
	}

	std::cout << "=============================== request"
		<< std::endl
		<< req
		<< std::endl
		<< "=============================== "
		<< std::endl;

	send(Socket, req.c_str(), req.size(), 0);

	char buffer[1024 * 10];
	int nlen;

	while ((nlen = recv(Socket, buffer, 1024 * 10, 0)) > 0)
	{
		response.append(buffer, 0, nlen);
	}
	closesocket(Socket);
	WSACleanup();

} // HTTPReq