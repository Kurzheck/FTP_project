#ifndef UTIL
#define UTIL

#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "data_structure.h"

int SetRequest(struct ThreadParam* data) {
	// char sentence[SENTENCE_LENGTH] = strcmp(data->sentence);
	char sentence[SENTENCE_LENGTH] = {0};
	strcpy(sentence, data->sentence);
	printf("sentence: %s\n", sentence);
	fflush(stdout);
	int cmdLength = -1;
	char* argPtr;
	for (int i = 0; i < strlen(sentence); i++) {
		if (sentence[i] != ' ' &&
		sentence[i] != '\n' &&
		sentence[i] != '\r' &&
		sentence[i] != '\t') {
			sentence[i] = toupper(sentence[i]);
			continue;
		}
		cmdLength = i;
		sentence[i] = '\0';
		argPtr = &(sentence[i+1]);
		break;
	}
	if (cmdLength < 0) {
		cmdLength = strlen(sentence);
	}
	printf("cmd length = %d\n", cmdLength);
	if (cmdLength > 4) {
		printf("command too long.\n");
		return 0;
	}
	memset(data->request.arg, 0, SENTENCE_LENGTH);
	// command has argument
	if (sentence[cmdLength + 1] != '\0') {
		strcpy(data->request.arg, argPtr);
	}
	for (int i = strlen(argPtr)-1; i > 0; i--)
	{
		if (argPtr[i] == '\n'
		|| argPtr[i] == ' '
		|| argPtr[i] == '\r'
		|| argPtr[i] == '\t')
		{
			data->request.arg[i] = '\0';
			continue;
		}
		break;
	}
	printf("arg = %s\n", data->request.arg);

	if (cmdLength == 3) {
		switch (sentence[0])
		{
		case 'M':
			if (strcmp(sentence, "MKD") == 0)
				data->request.type = MKD;
			else
				return 0;
			break;
		case 'C':
			if (strcmp(sentence, "CWD") == 0)
				data->request.type = CWD;
			else
				return 0;
			break;
		case 'P':
			if (strcmp(sentence, "PWD") == 0)
				data->request.type = PWD;
			else
				return 0;
			break;
		case 'R':
			if (strcmp(sentence, "RMD") == 0)
				data->request.type = RMD;
			else
				return 0;
			break;
		default:
			return 0;
			break;
		}
	}
	else if (cmdLength == 4) {
		switch (sentence[0])
		{
		case 'U':
			if (strcmp(sentence, "USER") == 0)
				data->request.type = USER;
			else
				return 0;
			break;
		case 'Q':
			if (strcmp(sentence, "QUIT") == 0)
				data->request.type = QUIT;
			else
				return 0;
			break;
		case 'T':
			if (strcmp(sentence, "TYPE") == 0)
				data->request.type = TYPE;
			else
				return 0;
			break;
		case 'L':
			if (strcmp(sentence, "LIST") == 0)
				data->request.type = LIST;
			else
				return 0;
			break;
		case 'R':
			if (strcmp(sentence, "RNFR") == 0)
				data->request.type = RNFR;
			else if (strcmp(sentence, "RNTO") == 0)
				data->request.type = RNTO;
			else if (strcmp(sentence, "RETR") == 0)
				data->request.type = RETR;
			else
				return 0;
			break;
		case 'P':
			if (strcmp(sentence, "PASS") == 0)
				data->request.type = PASS;
			else if (strcmp(sentence, "PASV") == 0)
				data->request.type = PASV;
			else if (strcmp(sentence, "PORT") == 0)
				data->request.type = PORT;
			else
				return 0;
			break;
		case 'S':
			if (strcmp(sentence, "SYST") == 0)
				data->request.type = SYST;
			else if (strcmp(sentence, "STOR") == 0)
				data->request.type = STOR;
			else
				return 0;
			break;
		default:
			return 0;
			break;
		}
	}
	return 1;
};

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
				sentence[p-1] = '\0'; // TODO ?? delete this line?
				break;
			}
		}
	}
	return 1;
};

int WriteResponse(int fd, int len, const char* sentence) {
	int p = 0;
	printf("enter writeresponse loop\n");
	fflush(stdout);
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
	printf("loop ends\n");
	fflush(stdout);
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
	char ipStr[30]; // = serverIP;
	strcpy(ipStr, serverIP);

	for (int i = 0; i < 30; i++) {
		if (ipStr[i] == '.') {
			ipStr[i] = ',';
		}
	}

	char* returnStr = (char*)malloc(30 * sizeof(char));
	sprintf(returnStr, "%s,%d,%d", ipStr, p1, p2);
	return returnStr;
};

int ParseIPPort(struct ClientAddr* addr, char str[]) {
	// "192,168,0,105,255,254"
	strcpy(addr->IP, str);
	int comma[5];
	int j = 0;
	for (int i = 0; i < strlen(str); i++) {
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

int ReadFile(struct ThreadParam* data, const char* filePath) {
	FILE* file = fopen(filePath, "w");
	if (!file) {
		return 0;
	}
	char dataBuffer[BUFFER_SIZE] = {0};
	// int total = 0;
	// int readLen;
	while (1) {
		int readLen = read(data->datafd, dataBuffer, BUFFER_SIZE);
		if (readLen < 0) {
			free(file);
			return 0;
		}
		if (readLen == 0) {
			break;
		}
		fwrite(dataBuffer, sizeof(char), readLen, file);
	}
	free(file);
	return 1;
};

int WriteFile(struct ThreadParam* data, const char* filePath) {
	int fd = open(filePath, O_RDONLY);
	if (fd < 0)
	{
		printf("file open error\n");
		return 0;
	}
	lseek(fd, data->readPos, SEEK_SET);
	char dataBuffer[BUFFER_SIZE] = {0};
	int readLen;
	while (1)
	{
		readLen = read(fd, dataBuffer, BUFFER_SIZE);
		if (readLen == 0)
		{
			break;
		}
		write(data->datafd, dataBuffer, readLen);
	}
	// TODO close xxx


	data->readPos = 0;
	/*
	FILE* file = fopen(filePath, "r");
	if (!file) {
		return 0;
	}
	char dataBuffer[BUFFER_SIZE] = {0};
	while (!feof(file)) {
		int readLen = fread(dataBuffer, sizeof(char), BUFFER_SIZE, file);
		int total = 0;
		int writeLen;
		while ((writeLen = write(data->datafd, dataBuffer + total, readLen - total)) > 0) {
			total += writeLen;
		}
	}
	free(file);
	return 1;
	*/
};

int AbsPath(char* dst, const char* root, const char* cwd, char* param) {
	// root = /tmp; cwd = /file/text;
	// param = ./lala/dada  relative
	// param = /doc/news  absolute
	// param = ../../mimimi  relative
	if (!dst || !root || !cwd || !param) {
		printf("get absolute path failed.\n");
		return 0;
	}

	char tmp[PATH_LENGTH];
	strcpy(tmp, root);
	// remove '/' at root end
	char* tmp_end = tmp + strlen(tmp);
	if (*(tmp_end - 1) == '/') {
		tmp_end--;
		*(tmp_end) = '\0';
	}
	// now tmp is root absolute
	if (param[0] != '/') { // param is relative
		if (cwd[0] != '/') {
			*(tmp_end) = '/';
			tmp_end++;
		}
		strcpy(tmp_end, cwd);
		tmp_end += strlen(cwd);
		if (*(tmp_end - 1) == '/') {
			tmp_end--;
			*(tmp_end) = '\0';
		}
		// now tmp is cwd absolute
	}
	// ready to concat param
	char* param_end = param + strlen(param);
	char* p = param;
	while (p < param_end) {
		char* slashPos = strchr(p, '/');
		if (!slashPos) {
			slashPos = param_end;
		}
		*slashPos = '\0';
		if (strcmp(p, ".") == 0 || slashPos == param) {
			p = slashPos + 1;
			continue;
		}
		if (*p == '.' && *(p+1) == '.' && *(p+2) == '.') { // invalid
			return 0;
		}
		if (*p == '.' && *(p+1) == '.') {
			while (tmp_end > tmp && *(tmp_end - 1) != '/') {
				tmp_end--;
			}
			if (tmp_end == tmp) {
				return 0;
			}
			tmp_end--;
			*tmp_end = '\0';
			p = slashPos + 1;
			continue;
		}
		if (*(tmp_end-1) != '/') {
			*tmp_end = '/';
			tmp_end++;
		}
		strcpy(tmp_end, p);
		tmp_end += slashPos - p;
		p = slashPos + 1;
	}

	strcpy(dst, tmp);
	if (dst[strlen(dst) - 1] == '/') {
		dst[strlen(dst) - 1] = '\0';
	}
	printf("abs = %s\n", dst);
	return 1;
}

int MakeDir(struct ThreadParam* data) {
	char filePath[PATH_LENGTH] = {0};
	if (!AbsPath(filePath, rootPath, data->currDir, data->request.arg)) {
		return 0;
	}
	if (mkdir(filePath, 0) == 0) {
		return 1;
	}
	return 0;
};

int ChangeDir(struct ThreadParam* data) {
	char filePath[PATH_LENGTH] = {0};
	if (!AbsPath(filePath, "/", data->currDir, data->request.arg)) {
		return 0;
	}
	struct stat s = {0};
	stat(filePath, &s);
	// no such dir
	if (!(s.st_mode & S_IFREG)) {
		return 0;
	}
	strcpy(data->currDir, filePath);
	return 1;
};

int ListDir(const char *file, const char *dir)
{
	char command[PATH_LENGTH];
	sprintf(command, "ls %s -lh", dir);
	FILE *ls_output = popen(command, "r");
	FILE *f = fopen(file, "w");

	char buf[PATH_LENGTH];
	while (1)
	{
		int num = fread(buf, 1, PATH_LENGTH, ls_output);
		if (!num)
		{
			break;
		}
		fwrite(buf, 1, num, f);
	}

	fclose(f);
	fclose(ls_output);
	return 1;
}

int RemoveDir(struct ThreadParam* data) {
	// TODO
};

#endif