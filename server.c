#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include "./libserver.h"


int main(int argc, char *argv[]) {
	//----Creating socket file des
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd == 0) {printf(err "Socket not created\n"); exit(1);}

	
	//----Try reusing local addresses and port
	int opt=1;
	int option = setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if (option) {
		perror("Attach error\n"); exit(1);}
	

	//----Attaching socket to port PORT
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	int bindret = bind(serverfd, (struct sockaddr *)&server, sizeof(server));
	if (bindret != 0) {perror(err "Socket bind error\n"); exit(1);}
	if (listen(serverfd, 10000) < 0) {printf(err "Listen error\n"); exit(1);}

	
	//----Server runtime
	while(1) {
		//----Server messages
		//char *hello = (char*)malloc(1000);
		//char *buffer = (char*)malloc(1000);
		char hello[1024] = {0};
		char buffer[1024] = {0};

		int addrlen = sizeof(server);
		int acceptret = accept(serverfd, (struct sockaddr*)&server, (socklen_t*)&addrlen);
		if (acceptret < 0) {perror(err "Accept error\n"); exit(1);}
		
		int readret = read(acceptret, buffer, 1000);
		if (readret != 0) {
			printf("[muhammad] " "%s", buffer);
			printf("[abdullah] "); 
			fgets(hello, sizeof(hello), stdin);
			send(acceptret, hello, sizeof(hello), 0);
			//printf(ack "Read: %d\n", readret);
			//printf(ack "Message sent\n");
		}
	}
	return 0;
}
