#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "./libserver.h"



//htons = host to network (short integer)
//SOCK_STREAM = for tcp sockets

int main(int argc, const char *argv[]) {
	//----creating IP socket
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {printf(err "Socket not created\n"); return -1;}

	
	//----socket addresss internet
	struct sockaddr_in server;
	server.sin_family = AF_INET; //internet family
	server.sin_port = htons(PORT); //internet port
	//printf(ack "Port in network-byte order: %d\n", server.sin_port);

	
	//----text ip address into binary
	inet_pton(server.sin_family, IPADDR, &server.sin_addr);

	
	//----connect
	//printf(ack "Attempting to connect\n");
	//--setting to timeout
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);
	//enable the above line, and: "Operation in progress"
	//so don't
	struct timeval tv;
	tv.tv_sec = 0;

	int connection = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
	if (connection < 0) {perror(err "Bind failed\n"); return -1;}
	//--timeout select
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sockfd, &fdset);
	select(sockfd + 1, NULL, &fdset, NULL, &tv);
	while (1) {
		printf("\033[0;32m" "[muhammad] " "\033[0m");
		//char *hello = (char*)malloc(1000);
		char hello[1024] = {0};
		fgets(hello, sizeof(hello), stdin);
		send(sockfd, hello, sizeof(hello), 0);
		
		//--reading response
		char buffer[1024] = {0};
		read(sockfd, buffer, 1024);
		printf("\033[0;34m[abdullah]\033[0m " "%s", buffer);
	}
	return 0;
}
