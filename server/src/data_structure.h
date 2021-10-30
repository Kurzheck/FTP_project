#ifndef DATA_STRUCTURE
#define DATA_STRUCTURE
#include <netinet/in.h>
#include "global.h"
enum RequestType {
	USER,
	PASS,
	RETR,
	STOR,
	QUIT,
	SYST,
	TYPE,
	PORT,
	PASV,
	MKD,
	CWD,
	PWD,
	LIST,
	RMD,
	RNFR,
	RNTO,
	REST
};

enum ClientState {
	NO_USER,
	HAS_USER,
	HAS_PASS,
	HAS_RNFR
};

enum DataConnectionMode {
	NO_CONNECTION,
	PASV_MODE,
	PORT_MODE
};

struct Request {
	enum RequestType type;
	char arg[SENTENCE_LENGTH];
};

struct ClientAddr {
	char IP[30]; // = {0};
	int port; // = -1;
};

struct ThreadParam {
	int connfd; // for command
	int datafd; // for data transfer
	int listenfd; // for pasv
	int dataPort;
	char sentence[SENTENCE_LENGTH];
	char currDir[PATH_LENGTH]; // relative to rootPath
	char RNFRPath[PATH_LENGTH]; // store RNFR path
	enum ClientState clientState; // = NO_USER;
	enum DataConnectionMode dataConnectionMode; // = NO_CONNECTION;
	struct Request request;
	struct ClientAddr clientAddr;
	int readPos; // file read position
	struct sockaddr_in dataAddr;
};

#endif