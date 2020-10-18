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
	RNTO
};

enum ClientState {
	NO_USER,
	HAS_USER,
	HAS_PASS
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

struct ThreadParam {
	int connfd = -1;
	int data_connfd = -1;
	int data_listenfd = -1;
	int dataPort;
	char sentence[SENTENCE_LENGTH];
	enum ClientState clientState = NO_USER;
	enum DataConnectionMode dataConnectionMode = NO_CONNECTION;
	struct Request request;
};