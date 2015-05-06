#include "ClientSocket.h"
#include <vector>
#include <string>
#include <iostream>
#include  <io.h>

#define port	1100

int main(int argc, char* argv[]) {

	std::string server_addr;
	std::string filename;

	//get file name and server address
	if (argc != 3) {
		std::cerr << "Two argument must be provided: 1 - file name, 2 - server address" << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	else {
		
		if ((_access(argv[1], 0)) == -1) {
			std::cerr << "File doesn't exist." << std::endl;
			system("pause");
			exit(EXIT_FAILURE);
		}

		filename = argv[1];
		server_addr = argv[2];
		
	}

	ClientSocket client_socket;
	client_socket.ConnectToServer(server_addr, port);
	client_socket.SendFile(filename);
	std::cout << filename << " is successfully sent to " << server_addr << "." << std::endl;
	system("pause");
	return 0;
}