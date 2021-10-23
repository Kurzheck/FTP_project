#ifndef GLOBAL
#define GLOBAL

#define SENTENCE_LENGTH 1000
#define PATH_LENGTH 200
#define RESPONSE_LENGTH 256
#define BUFFER_SIZE 2048
int listenfd;
int listenPort = 21;
char rootPath[PATH_LENGTH] = "/tmp";
char serverIP[30] = "127.0.0.1";

#endif