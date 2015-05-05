#include <iostream>
#include <fstream>
#include <string.h>
#include <winsock2.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <algorithm>
#include <sstream>
#include <vector>

#define port	1100
#define length_bytes 4
#define filename_bytes 12

#pragma comment(lib, "Ws2_32.lib")

int main(void) {
	struct sockaddr_in stSockAddr;

	//startup WSA
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		perror("Socket Initialization: Error with WSAStartup\n");
		WSACleanup();
		std::cout << "Socket Initialization: Error with WSAStartup";
		system("pause");
		exit(10);
	}

	//create socket
	int i32SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (i32SocketFD == -1) {
		perror("ошибка при создании сокета");
		exit(EXIT_FAILURE);
	}

	//cleanup and initialize addres structure
	memset(&stSockAddr, 0, sizeof(stSockAddr));

	stSockAddr.sin_family = PF_INET;
	stSockAddr.sin_port = htons(port);
	stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind to port
	if (bind(i32SocketFD, (struct sockaddr*) &stSockAddr, sizeof(stSockAddr)) == -1) {
		perror("Ошибка: связывания");

		closesocket(i32SocketFD);
		exit(EXIT_FAILURE);
	}

	//start listening
	if (listen(i32SocketFD, 10) == -1) {
		perror("Ошибка: прослушивания");

		closesocket(i32SocketFD);
		exit(EXIT_FAILURE);
	}

	//wait for incoming connections
	for (;;)
	{
		//block until an incoming connection is received
		std::cout << "for (;;)";
		int i32ConnectFD = accept(i32SocketFD, 0, 0);

		if (i32ConnectFD < 0) {
			perror("Ошибка: принятия");
			closesocket(i32SocketFD);
			exit(EXIT_FAILURE);
		}

		//receive file length
		size_t length_received = 0;
		size_t messageLength;
		while (length_received < length_bytes){
			int length_read = recv(i32ConnectFD, ((char*)&messageLength) + length_received, length_bytes - length_received, 0);
			if ((length_read == SOCKET_ERROR) || (length_read == 0))
			{
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}
			length_received += length_read;
		}
		messageLength = ntohl(messageLength);

		//receive file name
		size_t filename_received = 0;
		std::stringstream filename_ss;
		std::string filename;
		std::ofstream os;
		while (filename_received < filename_bytes){
			std::vector<char> filename_buf(filename_bytes / sizeof(char));
			int filename_read = recv(i32ConnectFD, filename_buf.data() + filename_received, filename_bytes - filename_received, 0);
			if ((filename_read == SOCKET_ERROR) || (filename_read == 0))
			{
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}
			filename_received += filename_read;
			filename_ss << filename_buf.data();
		}
		filename_ss << '\0';
		filename = filename_ss.str();

		//receive file data
		size_t filedata_received = 0;


		while (filedata_received < messageLength) {
			std::vector<char> filedata_buf(messageLength);
			int filedata_read = 0;
			//std::fill(filedata_buf.begin() + filedata_received, filedata_buf.end(), 0);
			if (filedata_received == 0) {
				os.open(filename);
				os.clear();
				os.close();
			}
			filedata_read = recv(i32ConnectFD, filedata_buf.data(), 10, 0);
			if ((filedata_read == SOCKET_ERROR) || (filedata_read == 0)) {
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}
			else {
				filedata_received += filedata_read;

				os.open(filename, std::ios_base::out | std::ios_base::binary | std::ios::app);
				if (os) {
					os << filedata_buf.data();
					os.close();
				}
			}
		}

		//close connection and socket
		shutdown(i32ConnectFD, 2);

		closesocket(i32ConnectFD);
	}

	WSACleanup();
	return 0;
}