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
#define msg_length_bytes 4

#pragma comment(lib, "Ws2_32.lib")

int main(void) {
	struct sockaddr_in stSockAddr;
	char buf[10];
	int left = msg_length_bytes;
	int received = 0;
	uint32_t len_buf = 0;

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

		//receive data
		int iRecvResult;
		int received_bytes = 0;
		int total_bytes = 20;
		std::ofstream os("received.txt", std::ifstream::binary);


		while (received_bytes < total_bytes) {
			memset(buf, 0, 10);
			iRecvResult = recv(i32ConnectFD, buf, 10, 0);
			std::cout << "received_bytes = " << iRecvResult << std::endl;


			if ((iRecvResult == SOCKET_ERROR) || (iRecvResult == 0))
			{
				int er_code = WSAGetLastError();
				printf(" Receiving data failed.\n Error code: %d\n", er_code);
				closesocket(i32SocketFD);
				exit(EXIT_FAILURE);
			}

			if (os)
			{
				//buf[iRecvResult] = NULL;
				os.write(buf, sizeof(buf));
			}

			received_bytes += iRecvResult;

		}
		os.close();

		//close connection and socket
		shutdown(i32ConnectFD, 2);

		closesocket(i32ConnectFD);
	}

	WSACleanup();
	return 0;
}