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

void CloseConnection(int fd) {
	if (fd > -1) {
		printf("close connection %d\n", fd);
		close(fd);
	}
};

int RandomPort() {
	return rand() % 45536 + 20000;
};

char* AddrToString(int port) {
	int p1 = port / 256;
	int p2 = port % 256;

	char ipStr[30] = serverIP;
	for (int i = 0; i < 30; i++) {
		if (ipStr[i] == '.') {
			ipStr[i] = ',';
		}
	}

	char* returnStr = (char*)malloc(30 * sizeof(char));
	sprintf(returnStr, "%s,%d,%d", ipStr, p1, p2);
	return returnStr;
};

int ParseIPPort(struct ClientAddr* addr, char* str) {
	// "192,168,0,105,255,254"
	strcpy(addr->IP, str);
	int comma[5];
	int j = 0;
	for (int i = 0; i < strlen(str), i++) {
		if (str[i] == ',') {
			comma[j] = 0;
			addr->IP[i] = '.';
			j++;
		}
		if (j == 5) {
			break;
		}
	}
	if (j != 5) {
		return 0;
	}
	addr->IP[comma[3]] = '\0';
	addr->IP[comma[4]] = '\0';
	char* p = &(addr->IP[comma[3]]);
	int p1 = atoi(p + 1);
	p = &(addr->IP[comma[4]]);
	int p2 = atoi(p + 1);
	addr->port = p1 * 256 + p2;
	return 1;
};

int ReadFile(struct ThreadParam* data) {
	// TODO
};

int WriteFile(struct ThreadParam* data) {
	// TODO
}

int ChangePath(struct ThreadParam* data) {
	// TODO
}