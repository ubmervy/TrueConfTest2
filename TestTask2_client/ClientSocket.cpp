#include "ClientSocket.h"
#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <sys/types.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include "Socket.h"
#include "Exception.h"

ClientSocket::ClientSocket()
{
}

ClientSocket::~ClientSocket()
{
}

void ClientSocket::ConnectToServer(std::string ip_addr, int port)
{
	try {
		//cleanup and initialize addres structure
		memset(&stSockAddr, 0, sizeof(stSockAddr));

		stSockAddr.sin_family = AF_INET;
		stSockAddr.sin_port = htons(port);
		int Res = inet_pton(AF_INET, (char *)ip_addr.c_str(), &stSockAddr.sin_addr);

		if (Res < 0)
		{
			throw Exception(13, "ClientSocket: cannot connect to server - address family is not valid.");
		}
		else if (Res == 0)
		{
			throw Exception(14, "ClientSocket: cannot connect to server - ip address is not valid.");
		}

		//set connection
		if (connect(SocketFD, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)) == -1)
		{
			throw Exception(15, "ClientSocket: connect failed.");
		}
	}
	catch (Exception& excp) {
		excp.Handle(*this);
		exit(EXIT_FAILURE);
	}
}

int ClientSocket::SendFile(std::string filename)
{
	std::vector<char> filedata_buf;
	ReadFile(filename, filedata_buf); // Read file data to buffer
	ExtractFilename(filename); //Extract filename from path

	//send file length
	size_t converted_length = htonl(filedata_buf.size()); // convert from local byte order to network byte order
	Send_parts(SocketFD, (char *)&converted_length, sizeof(converted_length));

	//send filename
	std::vector<char> filename_buf(filename.begin(), filename.end());
	filename_buf.resize(filename_maxlen);
	Send_parts(SocketFD, filename_buf.data(), filename_buf.size());

	//send data if file is not empty
	if (filedata_buf.size() != 0) {
		Send_parts(SocketFD, filedata_buf.data(), filedata_buf.size(), chunk_maxlen);
	}
	return 0;
}

void ClientSocket::ReadFile(std::string filename, std::vector<char>& filedata_buf)
{
	size_t length = 0;
	std::ifstream ifs;
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		ifs.open(filename, std::ifstream::binary);
		ifs.seekg(0, ifs.end);
		length = ifs.tellg();
		if (length != 0) {
			filedata_buf.resize(length);
			ifs.seekg(0, ifs.beg);
			ifs.read(filedata_buf.data(), length);
			filedata_buf.emplace_back('\0');
		}
		ifs.close();
	}
	catch (std::ifstream::failure e) {
		Exception excp(17, "File opening/reading/closing failed");
		excp.Handle(*this);
	}
}


void ClientSocket::Send_parts(int sock, char *buffer, int len, int chunk_size) {
	int nsent = 0;

	try {
		while (len > 0) {
			nsent = send(sock, buffer, chunk_size, 0);
			if (nsent == SOCKET_ERROR)
			{
				int er_code = WSAGetLastError();
				throw Exception(er_code, "ClientSocket: Send failed.");
			}

			buffer += nsent;
			len -= nsent;
		}
	}
	catch (Exception& excp) {
		excp.Handle(*this);
		exit(EXIT_FAILURE);
	}
}

void ClientSocket::ExtractFilename(std::string &filename)
{
	//char path_buffer[_MAX_PATH];
	char * fullpath = (char *)filename.c_str();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	errno_t err;

	try
	{
		err = _splitpath_s(fullpath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		if (err == 0) {
			filename.clear();
			filename += fname;
			filename += ext;
		}
		else {
			throw Exception(19, "Error exctracting filename from path.");
		}
	}
	catch (Exception& excp) {
		excp.Handle(*this);
		exit(EXIT_FAILURE);
	}
}
