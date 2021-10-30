#ifndef COMMON
#define COMMON

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "global.h"
#include "data_structure.h"
#include "command_handlers.h"

int ArgHandler(int argc, char **argv) {
	if (argc == 1) {
		return 1;
	}
	else if (argc == 3) {
		if (strcmp(argv[1], "-port") == 0) {
			int port = atoi(argv[2]);
			if (port >= 0 && port < 65535) {
				listenPort = port;
				return 1;
			}
		}
		else if (strcmp(argv[1], "-root") == 0) {
			if (argv[2])
			{
				strcpy(rootPath, argv[2]);
				return 1;
			}
		}
	}
	else if (argc == 5) {
		if (strcmp(argv[1], "-port") == 0 && strcmp(argv[3], "-root") == 0) {
			int port = atoi(argv[2]);
			if (port >= 0 && port < 65535 && argv[4]) {
				listenPort = port;
				strcpy(rootPath, argv[4]);
				return 1;
			}
		}
		else if (strcmp(argv[1], "-root") == 0 && strcmp(argv[3], "-port") == 0) {
			int port = atoi(argv[4]);
			if (port >= 0 && port < 65535 && argv[2]) {
				listenPort = port;
				strcpy(rootPath, argv[2]);
				return 1;
			}
		}
	}
	return 0;
};

void Login(struct ThreadParam* data);
void HandleCommand(struct ThreadParam* data);

void* EstablishConnection(void* params)
{
	struct ThreadParam* connectionData = (struct ThreadParam*)params;
	int connfd = connectionData->connfd;
	printf("connect\n");
	fflush(stdout);
	char responseStr[RESPONSE_LENGTH] = "220 zyh Anonymous FTP server ready.\r\n";
	if (!WriteResponse(connfd, strlen(responseStr), responseStr))
	{
		printf("exit thread\n");
		fflush(stdout);
		sleep(2);
		free(connectionData);
		close(connfd);
		return 0;
	}
	Login(connectionData);
	HandleCommand(connectionData);
}

void Login(struct ThreadParam* data) {
	int connfd = data->connfd;
	char* sentence = data->sentence;
	printf("enter login while loop...\n");
	fflush(stdout);
	// USER
	while (1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		if (!SetRequest(data)) {
			printf("invalid command\n");
			fflush(stdout);
			INVALID_Handler(data);
			continue;
		}
		// memset(data->sentence, 0, SENTENCE_LENGTH);
		if (data->request.type == USER) {
			printf("repuest type == USER\n");
			fflush(stdout);
			USER_Handler(data);
			if (data->clientState == HAS_USER)
				break;
		}
		else {
			printf("Error user(): %s(%d)\n", strerror(errno), errno);
			char responseStr[RESPONSE_LENGTH] = "530 needs to login.\r\n";
			WriteResponse(data->connfd, strlen(responseStr), responseStr);
			continue;
		}
	}

	// PASS
	while (1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		if (!SetRequest(data)) {
			printf("invalid command\n");
			INVALID_Handler(data);
			continue;
		}
		if (data->request.type == PASS) {
			printf("repuest type == PASS\n");
			PASS_Handler(data);
			if (data->clientState == HAS_PASS)
				break;
		}
		else {
			printf("Error pass(): %s(%d)\n", strerror(errno), errno);
			char responseStr[RESPONSE_LENGTH] = "530 needs password.\r\n";
			WriteResponse(data->connfd, strlen(responseStr), responseStr);
			continue;
		}
	}
};

void HandleCommand(struct ThreadParam* data) {
	printf("enter handle command\n");
	int connfd = data->connfd;
	char* sentence = data->sentence;

	while(1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		// printf("type = %d.\n", data->request.type);
		if (!SetRequest(data)) {
			INVALID_Handler(data);
			continue;
		}
		switch (data->request.type) {
			case RETR:
				RETR_Handler(data);
				break;
			case STOR:
				STOR_Handler(data);	
				break;
			case QUIT:
				QUIT_Handler(data);
				free(data);
				// pthread_exit(0);
				//break;
				close(connfd);
				return;
			case SYST:
				SYST_Handler(data);
				break;
			case TYPE:
				TYPE_Handler(data);
				break;
			case PORT:
				PORT_Handler(data);
				break;
			case PASV:
				PASV_Handler(data);
				break;
			case MKD:
				MKD_Handler(data);
				break;
			case CWD:
				CWD_Handler(data);
				break;
			case PWD:
				PWD_Handler(data);
				break;
			case LIST:
				LIST_Handler(data);
				break;
			case RMD:
				RMD_Handler(data);
				break;
			case RNFR:
				RNFR_Handler(data);
				break;
			case RNTO:
				RNTO_Handler(data);
				break;
			case REST:
				REST_Handler(data);
				break;
			default:
				INVALID_Handler(data);
				break;
		}
	}
};
/*
×÷ÒµÌá¹©µÄÔ­°æmainº¯Êý
int main(int argc, char **argv) {
	int listenfd, connfd;		//ï¿½ï¿½ï¿½ï¿½socketï¿½ï¿½ï¿½ï¿½ï¿½ï¿½socketï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ý´ï¿½ï¿½ï¿½
	struct sockaddr_in addr;
	char sentence[8192];
	int p;
	int len;

	//ï¿½ï¿½ï¿½ï¿½socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//ï¿½ï¿½ï¿½Ã±ï¿½ï¿½ï¿½ï¿½ï¿½ipï¿½ï¿½port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = 6789;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//ï¿½ï¿½ï¿½ï¿½"0.0.0.0"

	//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ipï¿½ï¿½portï¿½ï¿½socketï¿½ï¿½
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½socket
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	while (1) {
		//ï¿½È´ï¿½clientï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ -- ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			continue;
		}
		
		//Õ¥ï¿½ï¿½socketï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		p = 0;
		while (1) {
			int n = read(connfd, sentence + p, 8191 - p);
			if (n < 0) {
				printf("Error read(): %s(%d)\n", strerror(errno), errno);
				close(connfd);
				continue;
			} else if (n == 0) {
				break;
			} else {
				p += n;
				if (sentence[p - 1] == '\n') {
					break;
				}
			}
		}
		//socketï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½'\0'
		sentence[p - 1] = '\0';
		len = p - 1;
		
		//ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		for (p = 0; p < len; p++) {
			sentence[p] = toupper(sentence[p]);
		}

		//ï¿½ï¿½ï¿½ï¿½ï¿½Ö·ï¿½ï¿½ï¿½ï¿½ï¿½socket
 		p = 0;
		while (p < len) {
			int n = write(connfd, sentence + p, len + 1 - p);
			if (n < 0) {
				printf("Error write(): %s(%d)\n", strerror(errno), errno);
				return 1;
	 		} else {
				p += n;
			}			
		}

		close(connfd);
	}

	close(listenfd);
}
*/

#endif