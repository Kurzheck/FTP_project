#define SENTENCE_LENGTH 1000
#define PATH_LENGTH 256
#define RESPONSE_LENGTH 256
int listenfd;
int listenPort = 21;
const char rootPath[PATH_LENGTH] = "/tmp";
char serverIP[30] = "127.0.0.1";