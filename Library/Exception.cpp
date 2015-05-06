#ifdef UNICODE
#undef UNICODE
#endif

#include "Exception.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

Exception::Exception(int err_no, std::string err_msg) :
											error_number(err_no),
											error_message(err_msg)
{
	if (err_no > 100) {
		char err[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err_no,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
		printf("%s\n", err);//just for the safe case
		LocalFree(err);
	}

	/*if (err_no > 100) {
		LPSTR Error = NULL;
		int size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			err_no,
			LANG_NEUTRAL,
			(LPSTR)&Error,
			0,
			NULL);
		printf("Error code %d:  %s\n\nMessage was %d bytes, in case you cared to know this.\n\n", err_no, Error, size);
		LocalFree((LPTSTR)&Error);
	}*/
}

Exception::~Exception()
{
}

void Exception::Handle(Socket sock)
{
	std::cerr << "Error " << error_number << ": " << error_message << std::endl;
	system("pause");
	WSACleanup();
	closesocket(sock.SocketFD);
}
