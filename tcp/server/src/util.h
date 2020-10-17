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

int ReadRequest(int fd, int len, char* sentence) {
	int p = 0;
	while (1) {
		int n = read(fd, sentence + p, len - 1 - p);
		if (n < 0) {
			printf("Error read(): %s(%d)\n", strerror(errno), errno);
			close(fd);
			return 0;
		}
		else if (n == 0) {
			break;
		}
		else {
			p += n;
			if (sentence[p - 1] == '\n') {
				break;
			}
		}
	}
	return 1;
	};

int ReadData() {

};

int WriteResponse(int fd, int len, const char* sentence) {
	int p = 0;
	while (p < len) {
		int n = write(fd, sentence + p, len - p);
		if (n < 0) {
			printf("Error write(): %s(%d)\n", strerror(errno), errno);
			return 0;
		}
		else {
			p += n;
		}			
	}
	return 1;
};