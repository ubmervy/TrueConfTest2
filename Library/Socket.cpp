#include "Socket.h"
#include <winsock2.h>
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")

Socket::Socket()
{
	//startup WSA
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		std::cerr << "Socket Initialization: Error with WSAStartup\n" << std::endl;
		system("pause");
		WSACleanup();
		exit(EXIT_FAILURE);
	}

	//create socket
	SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (-1 == SocketFD)
	{
		std::cerr << "Socket Initialization: Error creating socket" << std::endl;
		system("pause");
		WSACleanup();
		exit(EXIT_FAILURE);
	}
}


Socket::~Socket()
{
	closesocket(SocketFD);
	WSACleanup();
	std::cout << "close";
}
