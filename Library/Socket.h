#pragma once
#include <winsock2.h>

class Socket
{
public:
	const size_t length_maxlen = sizeof(size_t); //number of bytes in a message length
	const size_t filename_maxlen = 260; //max number of bytes in file name
	const size_t chunk_maxlen = 10; //max number of bytes of file chunk

	Socket();
	~Socket();

	WSADATA wsaData;
	struct sockaddr_in stSockAddr;
	int SocketFD;
	int ConnectFD;
};

