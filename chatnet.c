#include "libchatnet.h"
int main(int argc, char* argv[]) {
	if (argc < 2){
		printf(
		"CHATNET\n"
		"Commands: connect, send_msg, recv_msg, send_conn_req, accepted_conn_req, denied_conn_req, recv_conn_req\n\n");
		exit(1);
	}

	
	if (strcmp("boot", argv[1])==0) {bootstrap_once();}
	else if (strcmp("connect", argv[1])==0) {
		printf("%s\n", send_conn_req(argv[2]));
	}
	else if (strcmp("anyconnreq", argv[1])==0) {
		recv_conn_req();
	}
	else if (strcmp("recv_msg", argv[1])==0) {
		printf("%s\n", recv_msg(argv[2]));
	}
	
	else if (strcmp("send_msg", argv[1])==0) {
		char* msgText = (char*)malloc(MAX_MSGTEXT_LEN);
		strcpy(msgText, "");
		int i = 3;
		while (argv[i] != NULL) {
			//printf("argv[%d]: %s\n", i, argv[i]);
			strcat(msgText, argv[i]);
			strcat(msgText, " ");
			//str_addpva(msgText, argv[i], " ");
			i++;
		}
		//printf("----SENDING MSG----");
		//printf("(msgText) %s\n", msgText);
		printf("%s\n", send_msg(argv[2], msgText));
	}


	else if (strcmp("get_shkey", argv[1])==0) {
		char* shkey = get_shkey(argv[2]);
		printf("%s\n", shkey);
	}
	return 0;
}
