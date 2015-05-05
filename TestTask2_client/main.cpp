#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>


#pragma comment(lib, "Ws2_32.lib")

#define length_bytes 4
#define max_chunk_size 10

//send file
int send_parts(int sock, char *buffer, int len, int chunk_size) {
	int nsent = 0;

	while (len > 0) {
		nsent = send(sock, buffer, chunk_size, 0);
		if (nsent == -1)
			return -1;

		buffer += nsent;
		len -= nsent;
	}
	return 0;
}
inline int send_parts(int sock, char *buffer, int len) {
	if (send_parts(sock, buffer, len, len) == -1) {
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	struct sockaddr_in stSockAddr;
	std::string server_addr;
	std::vector<char> filename;

	//get file name and server address
	if (argc != 3) {
		perror("Two argument must be provided: 1 - file name, 2 - server address");
		exit(EXIT_FAILURE);
	}
	else {
		for (unsigned int i = 0; i < strlen(argv[1]); ++i){
			if (argv[1][i] == '\0')
				continue;
			filename.emplace_back(argv[1][i]);

		}
		filename.emplace_back('\0');

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
	int Res = inet_pton(AF_INET, server_addr.c_str(), &stSockAddr.sin_addr);

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
	size_t length = 0;
	std::vector<char> buffer_filedata;
	std::string is_buf(filename.begin(), filename.end());
	std::ifstream is((char *)is_buf.c_str(), std::ifstream::binary);
	if (is) {
		// get length of file:
		is.seekg(0, is.end);
		length = is.tellg();

		// allocate memory:
		buffer_filedata.resize(length);

		// read data as a block:
		is.seekg(0, is.beg);
		is.read(buffer_filedata.data(), length);
		buffer_filedata.emplace_back('\0');
		is.close();
	}
	else {
		perror("file reading error");
		closesocket(SocketFD);
		exit(EXIT_FAILURE);
	}

	//send file length
	size_t converted_length = htonl(buffer_filedata.size()); // convert from local byte order to network byte order
	send_parts(SocketFD, (char*)&converted_length, sizeof(converted_length));

	//send filename
	send_parts(SocketFD, filename.data(), (filename.size())*sizeof(char));

	//send file data
	send_parts(SocketFD, buffer_filedata.data() - 3, (buffer_filedata.size())*sizeof(char), max_chunk_size);
	//close socket
	closesocket(SocketFD);

	WSACleanup();

	return 0;
}