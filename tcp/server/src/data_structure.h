#include "global.h"
enum RequestType
{
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

struct Request
{
    enum RequestType type;
    char arg[SENTENCE_LENGTH];
};

struct ThreadParam
{
	int connfd;
	char sentence[SENTENCE_LENGTH];
    struct Request request;
};