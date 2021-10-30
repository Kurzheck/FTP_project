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
	char responseStr[RESPONSE_LENGTH] = "220 zyh Anonymous FTP server ready.\r\n";
	if (!WriteResponse(connfd, strlen(responseStr), responseStr))
	{
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
	// USER
	while (1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		if (!SetRequest(data)) {
			INVALID_Handler(data);
			continue;
		}
		if (data->request.type == USER) {
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
			INVALID_Handler(data);
			continue;
		}
		if (data->request.type == PASS) {
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
	int connfd = data->connfd;
	char* sentence = data->sentence;

	while(1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
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

#endif