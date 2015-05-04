#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <bitset>
#pragma comment(lib, "Ws2_32.lib")

#define msg_length_bytes 4
#define filename_bytes 12

//place value in n-byte 
void place_in_n_bytes(size_t bytes_num, char* buffer, size_t value) {
	for (int i = 0; i < bytes_num; ++i) {
		buffer[i] = (value >> (bytes_num - i - 1) * 8) & 0xFF;
	}
}

//send file
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
	char* buffer;
	int header_bytes = msg_length_bytes + filename_bytes;
	std::ifstream is(filename, std::ifstream::binary);
	if (is) {
		// get length of file:
		is.seekg(0, is.end);
		length = is.tellg();
		is.seekg(0, is.beg);

		// allocate memory:
		buffer = new char[length];
		memset(buffer, 0, length);

		/*place_in_n_bytes(msg_length_bytes, buffer, length);
		memcpy(buffer + msg_length_bytes + filename_bytes - filename.size(), (char *)filename.c_str(), filename.size());*/

		/*for (int i = 0; i < 4; ++i)
			std::cout << "sendbuffer_length = " << std::bitset<8>(buffer[i]) << std::endl;
		for (int i = 0; i < 12; ++i)
			std::cout << "sendbuffer_filename = " << std::bitset<8>((buffer + 4)[i]) << std::endl;*/

		// read data as a block:
		is.read(buffer, length);

		is.close();
	}
	else {
		perror("file reading error");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}
	u_long messageLength = length;
	u_long converted = htonl(messageLength); // convert from local byte order to network byte order
	send(SocketFD, (char*)&converted, sizeof(converted), 0);

	//send_all(SocketFD, buffer, header_bytes);
	send_all(SocketFD, buffer, length);

	//close socket
	closesocket(SocketFD);

	WSACleanup();

	return 0;
}