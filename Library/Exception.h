#pragma once
#include <string>
#include "Socket.h"

class Exception
{
public:
	Exception(int err_no, std::string err_msg);
	~Exception();
	void Handle(Socket sock);

private:
	int error_number;
	std::string error_message;
};

