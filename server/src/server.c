#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <pthread.h>

#include "common.h"

int main(int argc, char **argv) {
	if (!ArgHandler(argc, argv))
	{
		printf("arg error\n");
		return 1;
	}
	//printf("port = %d, root = %s\n", listenPort, rootPath);
	//SocketInit();
	int connfd;
	struct sockaddr_in addr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}
	
	// set ip and port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(listenPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// bind socket with ip and port
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}
	
	// max client number
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	while (1) {
		if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			continue;
		}
		pthread_t pthreadfd;
		struct ThreadParam* params = (struct ThreadParam*)malloc(sizeof(struct ThreadParam));
		params->connfd = connfd;
		params->datafd = params->listenfd = params->dataPort = -1;
		memset(params->sentence, 0, SENTENCE_LENGTH);
		memset(params->currDir, 0, PATH_LENGTH);
		strcpy(params->currDir, "/");
		// strcpy(params->currDir, rootPath);
		params->clientState = NO_USER;
		params->dataConnectionMode = NO_CONNECTION;
		memset(params->request.arg, 0, SENTENCE_LENGTH);
		memset(params->clientAddr.IP, 0, 30);
		params->clientAddr.port = -1;
		params->readPos = 0;
		// params->clientState = NO_USER;
		// memset(&(params->sentence), 0, sizeof(params->sentence));
		// memset(&(params->request), 0, sizeof(params->request));
		pthread_create(&pthreadfd, NULL, EstablishConnection, (void*)params);
	}
	close(listenfd);
	return 0;
}

/*
main
> ArgHandler
> SocketInit
> while{ EstablishConnection }
> Login
> > 2. HandleCommand
> > >while { switch command }
*/