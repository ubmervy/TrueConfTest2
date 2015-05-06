#pragma once

#include <vector>
#include <string>
#include "Socket.h"

class ClientSocket :
	public Socket
{
public:
	ClientSocket();

	~ClientSocket();

	void ConnectToServer(std::string ip_addr, int port);

	void ReadFile(std::string filename, std::vector<char>& buffer_filedata);

	int SendFile(std::string filename);

	void Send_parts(int sock, char *buffer, int len, int chunk_size);

	inline void Send_parts(int sock, char *buffer, int len) {
		Send_parts(sock, buffer, len, len);
	}

	void Close();

//private:
	void ExtractFilename(std::string &filename);

};

