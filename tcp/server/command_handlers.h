#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

#include "util.h"
#include "global.h"
#include "data_structure.h"

int INVALID_Handler(struct ThreadParam* data) {
	// TODO
	return 0;
}

int USER_Handler(struct ThreadParam* data) {
	// anonymous
	if (strcmp(data->request.arg, "anonymous") == 0) {
		printf("USER anonymous received, connfd = %d\n", data->connfd);
		data->clientState = HAS_USER;
		char responseStr[RESPONSE_LENGTH] = "331 USER ok, PASS please.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
	else {
		printf("Error username(): %s(%d)\n", strerror(errno), errno);
		char responseStr[RESPONSE_LENGTH] = "530 USER needs to be anonymous.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
};

int PASS_Handler(struct ThreadParam* data) {
	switch (data->clientState) {
		case NO_USER:
			printf("Error pass(): %s(%d)\n", strerror(errno), errno);
			char responseStr[RESPONSE_LENGTH] = "503 needs USER first.\r\n";
			return WriteResponse(data->connfd, strlen(responseStr), responseStr);
			break;
		case HAS_USER:
			printf("password received, connfd = %d\n", data->connfd);
			char responseStr[RESPONSE_LENGTH] = "230 login successful, enjoy the server.\r\n";
			return WriteResponse(data->connfd, strlen(responseStr), responseStr);
			break;
		case HAS_PASS:
		default:
			printf("already logged in, connfd = %d\n", data->connfd);
			char responseStr[RESPONSE_LENGTH] = "530 already logged in.\r\n";
			return WriteResponse(data->connfd, strlen(responseStr), responseStr);
			break;
	}
};

// connect()
int PORT_Handler(struct ThreadParam* data) {
	// close current connections
	CloseConnection(data->datafd);
	CloseConnection(data->listenfd);
	data->datafd = -1;
	data->listenfd = -1;

	char responseStr[RESPONSE_LENGTH]
	if (ParseIPPort(&(data->clientAddr), &(data->request.arg)) {
		printf("port mode on, connfd = %d\n", data->connfd);s
		data->dataConnectionMode = PORT_MODE;
		strcpy(responseStr, "200 port success.\r\n");
	}
	else {
		printf("Error ParseIPPort(): %s(%d)\n", strerror(errno), errno);
		strcpy(responseStr, "500 invalid arguments.\r\n");
	}
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

// accept()
int PASV_Handler(struct ThreadParam* data) {
	// close current connections
	CloseConnection(data->datafd);
	CloseConnection(data->listenfd);
	data->datafd = -1;
	data->listenfd = -1;

	data->dataConnectionMode = PASV_MODE;
	data->dataPort = RandomPort();

	struct sockaddr_in addr;
	if ((data->listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		goto PASV_failed;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(data->dataPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (inet_pton(AF_INET, serverIP, &addr.sin_addr) == -1) {
		printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
		goto PASV_failed;
	}

	if (bind(data->listenfd,(struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		goto PASV_failed;
	}

	if (listen(data->listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		goto PASV_failed;
	}

	printf("passive mode on, connfd = %d\n", data->connfd);
	char addrStr[30];
	strcpy(addrStr, AddrToString());
	char responseStr[RESPONSE_LENGTH];
	sprintf(responseStr, "227 passive mode (%s).\r\n", addrStr);

	return WriteResponse(data->connfd, strlen(responseStr), responseStr);

PASV_failed:
	char responseStr[RESPONSE_LENGTH] = "425 PASV command failed.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

// write()
int RETR_Handler(struct ThreadParam* data) {
	if (data->dataConnectionMode == NO_CONNECTION) {
		printf("no connection, connfd = %d\n", data->connfd);
		goto RETR_connection_failed;
	}

	char filePath[PATH_LENGTH] = {0};
	if (!AbsPath(filePath, rootPath, data->currDir, data->request.arg)) {
		char responseStr[RESPONSE_LENGTH] = "530 invalid path.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}

	struct stat s = {0};
	stat(filePath, &s);
	if (!(s.st_mode & S_IFREG)) {
		char responseStr[RESPONSE_LENGTH] = "530 invalid path.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}

	if (data->dataConnectionMode == PORT_MODE) { // connect()
		struct sockaddr_in addr;
		if ((data->datafd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			printf("Error socket(): %s(%d)\n", strerror(errno), errno);
			goto RETR_connection_failed;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(data->clientAddr.port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (inet_pton(AF_INET, data->clientAddr.IP, &addr.sin_addr) == -1) {			
			printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
			goto RETR_connection_failed;
		}
		if (connect(data->datafd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
			printf("Error connect(): %s(%d)\n", strerror(errno), errno);
			// CloseConnection(data->datafd);
			// data->datafd = -1;
			goto RETR_connection_failed;
		}
	}

	else if (data->dataConnectionMode == PASV_MODE) { // accept()
		if ((data->datafd = accept(data->listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			// CloseConnection(data->listenfd);
			// data->listenfd = -1;
			goto RETR_connection_failed;
		}
	}
	// data connection established
	char responseStr[RESPONSE_LENGTH] = "150 RETR start.\r\n";
	if (!WriteResponse(data->connfd, strlen(responseStr), responseStr)) {
		return 0;
	}
	if (WriteFile(data, filePath)) { // file transfer succeeded
		strcpy(responseStr, "226 transmission finished.\r\n");
	}
	else {
		strcpy(responseStr, "451 transmission failed.\r\n");
	}
	CloseConnection(data->listenfd);
	CloseConnection(data->datafd);
	data->listenfd = -1;
	data->datafd = -1;
	data->dataConnectionMode = NO_CONNECTION;
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
RETR_connection_failed:
	char responseStr[RESPONSE_LENGTH] = "425 no data connection.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

// read()
int STOR_Handler(struct ThreadParam* data) {
	if (data->dataConnectionMode == NO_CONNECTION) {
		printf("no connection, connfd = %d\n", data->connfd);
		goto STOR_connection_failed;
	}

	char filePath[PATH_LENGTH] = {0};
	if (!AbsPath(filePath, rootPath, data->currDir, data->request.arg)) {
		char responseStr[RESPONSE_LENGTH] = "530 invalid path.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}

	if (data->dataConnectionMode == PORT_MODE) { // connect()
		struct sockaddr_in addr;
		if ((data->datafd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			printf("Error socket(): %s(%d)\n", strerror(errno), errno);
			goto STOR_connection_failed;
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(data->clientAddr.port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (inet_pton(AF_INET, data->clientAddr.IP, &addr.sin_addr) == -1) {			
			printf("Error inet_pton(): %s(%d)\n", strerror(errno), errno);
			goto STOR_connection_failed;
		}
		if (connect(data->datafd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
			printf("Error connect(): %s(%d)\n", strerror(errno), errno);
			// CloseConnection(data->datafd);
			// data->datafd = -1;
			goto STOR_connection_failed;
		}
	}

	else if (data->dataConnectionMode == PASV_MODE) { // accept()
		if ((data->datafd = accept(data->listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			// CloseConnection(data->listenfd);
			// data->listenfd = -1;
			goto STOR_connection_failed;
		}
	}
	// data connection established
	char responseStr[RESPONSE_LENGTH] = "150 STOR start.\r\n";
	if (!WriteResponse(data->connfd, strlen(responseStr), responseStr)) {
		return 0;
	}
	if (ReadFile(data, filePath)) { // file transfer succeeded
		strcpy(responseStr, "226 transmission finished.\r\n");
	}
	else {
		strcpy(responseStr, "451 transmission failed.\r\n");
	}
	CloseConnection(data->listenfd);
	CloseConnection(data->datafd);
	data->listenfd = -1;
	data->datafd = -1;
	data->dataConnectionMode = NO_CONNECTION;
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
STOR_connection_failed:
	char responseStr[RESPONSE_LENGTH] = "425 no data connection.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int QUIT_Handler(struct ThreadParam* data) {
	printf("quit, connfd = %d\n", data->connfd);
	char responseStr[RESPONSE_LENGTH] = "221 connection closed, goodbye.\r\n";
	CloseConnection(data->connfd);
	CloseConnection(data->datafd);
	// data->connfd = -1;
	// data->datafd = -1;
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int SYST_Handler(struct ThreadParam* data) {
	printf("syst command received, connfd = %d\n", data->connfd);
	char responseStr[RESPONSE_LENGTH] = "215 UNIX Type: L8\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int TYPE_Handler(struct ThreadParam* data) {
	if (strcmp(data->request.arg, "I") == 0) {
		char responseStr[RESPONSE_LENGTH] = "200 type set to I.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
	char responseStr[RESPONSE_LENGTH] = "425 invalid type.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int MKD_Handler(struct ThreadParam* data) {
	if (MakeDir(data)) {
		char responseStr[RESPONSE_LENGTH] = "250 MKD succeeded.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
	char responseStr[RESPONSE_LENGTH] = "550 MKD failed.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int CWD_Handler(struct ThreadParam* data) {
	if (ChangeDir(data)) {
		char responseStr[RESPONSE_LENGTH] = "250 CWD succeeded.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
	char responseStr[RESPONSE_LENGTH] = "550 CWD failed.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int PWD_Handler(struct ThreadParam* data) {
	char responseStr[RESPONSE_LENGTH];
	sprintf(responseStr, "257 \"%s\".\r\n", data->currDir);
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int LIST_Handler(struct ThreadParam* data) {

};

int RMD_Handler(struct ThreadParam* data) {
	if (RemoveDir(data)) {
		char responseStr[RESPONSE_LENGTH] = "250 RMD succeeded.\r\n";
		return WriteResponse(data->connfd, strlen(responseStr), responseStr);
	}
	char responseStr[RESPONSE_LENGTH] = "550 RMD failed.\r\n";
	return WriteResponse(data->connfd, strlen(responseStr), responseStr);
};

int RNFR_Handler(struct ThreadParam* data) {

};

int RNTO_Handler(struct ThreadParam* data) {

};