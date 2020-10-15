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
		pthread_t = pthreadfd;
		struct ThreadParam* params = (struct ThreadParam*)malloc(sizeof(struct ThreadParam));
		params->connfd = connfd;
		pthread_create(&pthreadfd, NULL, EstablishConnection, (void*)params);
	}
	close(listenfd);
	return 0;
}