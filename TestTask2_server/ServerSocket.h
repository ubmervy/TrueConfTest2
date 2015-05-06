#pragma once
#include <string>
#include <vector>
#include "Socket.h"


class ServerSocket : public Socket
{
public:
	ServerSocket();

	~ServerSocket();

	void StartServer(int port);

	void Bind(int port);

	void Listen();

private:
	void CreateEmptyFile(std::string filename);

	void WriteFile(std::string filename, std::vector<char>& data_buf);

};

