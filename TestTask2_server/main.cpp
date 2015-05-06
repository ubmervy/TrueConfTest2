#include "ServerSocket.h"

#define port	1100

int main(void) {

	ServerSocket server_socket;
	server_socket.Bind(port);
	server_socket.Listen();
	return 0;
}