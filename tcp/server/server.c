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
	ArgHandler(argc, argv);
	SocketInit();
	int connfd;
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
		strcpy(params->currDir, rootPath);
		params->clientState = NO_USER;
		params->dataConnectionMode = NO_CONNECTION;
		memset(params->request.arg, 0, SENTENCE_LENGTH);
		memset(params->clientAddr.IP, 0, 30);
		params->clientAddr.port = -1;
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