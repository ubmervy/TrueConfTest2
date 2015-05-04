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
#include <bitset>

#define port	1100
#define length_bytes 4

#pragma comment(lib, "Ws2_32.lib")

int main(void) {
	struct sockaddr_in stSockAddr;
	int left = length_bytes;
	int received = 0;
	uint32_t len_buf = 0;
	//std::ofstream os;

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

	char* filename = "received.txt";
	std::ofstream os;

	os.clear();
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

		//receive data
		char* buf;
		int iRecvResult;
		int total_bytes = length_bytes;

		int received_bytes = 0;
		int header_bytes = 16;
		//std::ofstream os("received.txt", std::ifstream::binary);

		/*iRecvResult = recv(i32ConnectFD, buf, header_bytes, 0);
		memcpy(&total_bytes, buf, 4);
		memcpy(&filename, buf + 4, 12);
		std::cout << "length = " << total_bytes << std::endl;
		std::cout << "filename = " << filename << std::endl;*/

		int length_received = 0;
		u_long messageLength;
		while (length_received < length_bytes){
			int read = recv(i32ConnectFD, ((char*)&messageLength) + length_received, length_bytes - length_received, 0);
			if ((read == SOCKET_ERROR) || (read == 0))
			{
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}
			length_received += read;
		}
		messageLength = ntohl(messageLength);

		buf = new char[messageLength];
		memset(buf, 0, messageLength);


		//os.open(filename, std::ofstream::out | std::ofstream::app);
		//std::ofstream os(filename, std::ifstream::binary);
		while (received_bytes < messageLength) {
			if (received_bytes == 0) {
				os.open(filename, std::ios::trunc);
				os.clear();
				os.close();
			}

			memset(buf, 0, messageLength);
			iRecvResult = recv(i32ConnectFD, buf, 10, 0);
			std::cout << "received_bytes = " << iRecvResult << std::endl;

			if ((iRecvResult == SOCKET_ERROR) || (iRecvResult == 0)){
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}
			else {

				received_bytes += iRecvResult;


				//buf[iRecvResult] = NULL;
				//os.write(buf, strlen(buf));
				os.open(filename, std::ios_base::out | std::ios_base::binary | std::ios::app);
				if (os)
				{
					os << buf;
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