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

struct Request {
	enum RequestType type;
	char arg[SENTENCE_LENGTH];
};

struct ThreadParam {
	int connfd = -1;
	int datafd = -1;
	char sentence[SENTENCE_LENGTH];
	enum ClientState clientState = NO_USER;
	struct Request request;
};