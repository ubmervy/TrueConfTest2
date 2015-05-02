#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#pragma comment(lib, "Ws2_32.lib")

#define msg_length_bytes 4

int send_all(int sock, char *buffer, int len) {
	int nsent;

	while (len > 0) {
		
		nsent = send(sock, buffer, 10, 0);
		if (nsent == -1) // error
			return -1;

		buffer += nsent;
		len -= nsent;
	}
	return 0; // ok, all data sent
}

int main(int argc, char* argv[])
{
	struct sockaddr_in stSockAddr;
	int Res;
	std::string filename;
	std::string server_addr;
	char * buffer;


	//get file name and server address
	if (argc != 3) {
		perror("Two argument must be provided: 1 - file name, 2 - server address");
		exit(EXIT_FAILURE);
	}
	else {
		filename = argv[1];
		server_addr = argv[2];
	}

	//startup WSA
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		perror("Socket Initialization: Error with WSAStartup\n");
		WSACleanup();
		std::cout << "Socket Initialization: Error with WSAStartup";
		exit(10);
	}

	//create socket
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (-1 == SocketFD)
	{
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}

	//cleanup and initialize addres structure
	memset(&stSockAddr, 0, sizeof(stSockAddr));

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(1100);
	Res = inet_pton(AF_INET, server_addr.c_str(), &stSockAddr.sin_addr);

	if (0 > Res)
	{
		perror("error: first parameter is not a valid address family");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}
	else if (0 == Res)
	{
		perror("char string (second parameter does not contain valid ipaddress)");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}

	//set connection
	if (-1 == connect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)))
	{
		perror("connect failed");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}

	//read file
	size_t length;
	std::ifstream is(filename, std::ifstream::binary);
	if (is) {
		// get length of file:
		is.seekg(0, is.end);
		length = is.tellg();
		is.seekg(0, is.beg);

		// allocate memory:
		buffer = new char[length];
		memset(buffer, 0, length);
		// read data as a block:
		is.read(buffer, length);

		is.close();
	}
	else {
		perror("file reading error");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}

	send_all(SocketFD, buffer, length);
	

	//close socket
	closesocket(SocketFD);

	WSACleanup();

	return 0;
}