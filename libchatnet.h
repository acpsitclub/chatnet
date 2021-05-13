#include <dirent.h>
#include "cJSON.h"
#include <curl/curl.h>
#include "libcpyb.h"    //Must be included at last.

const char* err = "\033[0;31m[Error]\033[0m ";
const char* info = "\033[0;32m[Info]\033[0m ";
const char* boot = "\033[0;33m[Bootstrap]\033[0m ";

#define MAX_SHKEY_LEN 100
#define MAX_UNAME_LEN 20
#define MAX_MSGTEXT_LEN 5000


char *get_uSend() {
	const char *fn = "./chatnet.conf.d/PROFILE"; 
	char *profile = file_read(fn);
	profile[strlen(profile)-1] = '\0';
	return profile; 
}
void bootstrap_once_profile() {
	FILE* profile = fopen("./chatnet.conf.d/PROFILE", "r");
	if (!profile) {
		char* uSend = (char*)malloc(MAX_UNAME_LEN);
		printf("%sEnter username:\t", boot);
		fgets(uSend, MAX_UNAME_LEN, stdin);
		file_write("./chatnet.conf.d/PROFILE", uSend);
	}
}
void bootstrap_once() {
	DIR* d = opendir("./chatnet.conf.d");
	if (d){bootstrap_once_profile();}
	else if(!d) {
		mkdir("./chatnet.conf.d", 0755);
		bootstrap_once_profile();
	}
}

char *get_shkey(const char *uname) {
	char *shkey = (char*)malloc(MAX_SHKEY_LEN);
	strcpy(shkey, "");
	strcpy(shkey, file_read(str_add("./chatnet.conf.d/", uname)));
	shkey[strlen(shkey)-2] = '\0'; 

// Update: this function outputs characters after the length of shkey -- very weird. I don't know.


// Above line cuts off EOF. Required for other files, but..
// for shkey it's cutting off 2 characters.

// Maybe because handwritten-files add an EOF. And fprintf() didn't 
// add EOF. Yes. It's true. `$ cat AbdurRahman $cat MuhammadBinZafar`

// shkey isn't handwritten. So, not cutting off. fread(), fprintf --same.
	return shkey;
}




size_t curl_writefunc_callback(void *p, size_t size, size_t count, struct string *cResp) {
	size_t newLen = cResp->len + size*count;
	cResp->str = (char*)realloc(cResp->str, newLen + 1);
	if (cResp->str==NULL) {printf("curl_writefunc() failed\n"); exit(1);}
	memcpy(cResp->str + cResp->len, p, size*count);
	cResp->str[newLen] = '\0';
	cResp->len = newLen;
	return size*count;
}
const char *NETADDR = "https://yuva.life/wp-admin/net.php";
// NETADDR could be any address where `net.php` is stored.
char *server_comm(const char *PostData) {
	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL *curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, NETADDR);
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, PostData);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writefunc_callback);

		struct string cwfResp; //curl write func : response
		str_init(&cwfResp);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &cwfResp); //Address of cwfResp (&cwfResp), not just (cwfResp). Else: Errors!!!!
	
		CURLcode res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			printf("Couldn't send msg\n");
			exit(1);
		}
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		return cwfResp.str;  //Raw json. Needs parsing.
	}
	else {printf("Couldn't init comm\n"); exit(1);}
}


char* send_conn_req(const char* uRecv) {
	//uRecv will receive the connection request
	cJSON *_msg = cJSON_CreateObject();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(get_uSend()));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString(str_add("!connect ", uRecv)));
	
	char* msg = cJSON_Print(_msg);
	char* resp = server_comm(msg);
	if (strcmp(resp, "-1")==0) return str_add(err, " uRecv/uSend is empty\n");
	
	else if (strcmp(resp, "0") == 0) return str_addva(info, " ", uRecv, " ", RED, "denied", R0, " your connection request.\n");
	
	else if (strcmp(resp, "-2")==0) return str_addva(info, " ", uRecv, " ", RED, "didn't accept conn req", R0, " in 30 sec.\n");
	
	else {
		// Key was echoed, so writing it.
		file_write(str_add("./chatnet.conf.d/", uRecv), resp);
		return str_add(info, " Request Accepted.\n");
	}
}


void accepted_conn_req(const char* uRecv) {
	cJSON* _msg = cJSON_CreateObject();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(get_uSend()));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString(str_add("!connaccept ", uRecv)));

	char* msg = cJSON_Print(_msg);
	char* resp = server_comm(msg);

	if (strcmp(resp, "-1")==0) {printf("%s uSend/uRecv was empty while accepting conn request.\n", err); exit(1);}

	else {
		// Key echoed.
		file_write(str_add("./chatnet.conf.d/", uRecv), resp);
		printf("%s Secure connection established with %s%s%s.\n", info, BLU, uRecv, R0);
	}
}


void denied_conn_req(char const* uRecv) {
	cJSON* _msg = cJSON_CreateObject();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(get_uSend()));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString(str_add("!conndenied ", uRecv)));

	char* msg = cJSON_Print(_msg);
	char* resp = server_comm(msg);

	if (strcmp(resp, "-1")==0) {printf("%s uSend was empty while denying conn request.\n", err); exit(1);}

}


char* send_msg(const char* uRecv, const char* msgText);
char* recv_msg(const char* uRecv);

void recv_conn_req() {
	cJSON* _msg = cJSON_CreateObject();
	char* uSend = get_uSend();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(uSend));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString("!anyconnreq"));
	
	char* msg=cJSON_Print(_msg);
	char* uRecv = server_comm(msg); //uRecv is wanting to connect.
	if (strcmp(uRecv, "-1")==0) {
		printf("%s uSend is empty\n", err);
	}
	else if (strcmp(uRecv, "0")==0) {
		//No requests
		printf("%s No connection requests.\n", info);
	}
	else {
		//There's a request
		printf("%s Accept connection request from: %s? (y/N)  ", info, uRecv);
		char prompt;
		scanf("%c", &prompt);
		if (prompt == 'n' || prompt == 'N') denied_conn_req(uRecv);
		else  accepted_conn_req(uRecv);
	}
}


char *recv_msg(const char *uRecv) {
	// uRecv is sending the message.
	cJSON* _msg = cJSON_CreateObject();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(get_uSend()));
	cJSON_AddItemToObject(_msg, "uRecv", cJSON_CreateString(uRecv));
	char* shkey = get_shkey(uRecv);
	cJSON_AddItemToObject(_msg, "shkey", cJSON_CreateString(shkey));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString("!anynewmsg"));
	
	char *msg = cJSON_Print(_msg);
	//printf("(RECV_MSG) %s", msg);
	return server_comm(msg);
}


char* send_msg(const char* uRecv, const char* msgText) {
	// msgText will be sent to uRecv.
	cJSON* _msg = cJSON_CreateObject();
	cJSON_AddItemToObject(_msg, "uSend", cJSON_CreateString(get_uSend()));
	cJSON_AddItemToObject(_msg, "uRecv", cJSON_CreateString(uRecv));
	cJSON_AddItemToObject(_msg, "shkey", cJSON_CreateString(get_shkey(uRecv)));
	cJSON_AddItemToObject(_msg, "msgText", cJSON_CreateString(msgText));

	char *msg = cJSON_Print(_msg);
	//printf("(SEND_MSG)%s ", msg);
	return server_comm(msg);
}
