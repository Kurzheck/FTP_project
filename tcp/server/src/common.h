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
#include "command_handlers.h"

bool ArgHandler(int argc, char **argv) {
	if (argc == 1) {
		return 1;
	}
	else if (argc == 3) {
		if (strcmp(arg[1], "-port") == 0) {
			int port = atoi(argv[2]);
			if (port >= 0 && port < 65535) {
				listenPort = port;
				return 1;
			}
		}
		else if (strcmp(arg[1], "-root") == 0) {
			strcpy(rootPath, argv[2]);
			return 1;
		}
	}
	else if (argc == 5) {
		if (strcmp(argv[1], "-port") == 0 && strcmp(argv[3], "-root") == 0) {
			int port = atoi(argv[2]);
			if (port >= 0 && port < 65535 && !argv[4]) {
				listenPort = port;
				strcpy(rootPath, argv[4]);
				return 1;
			}
		}
		else if (strcmp(argv[1], "-root") == 0 && strcmp(argv[3], "-port") == 0) {
			int port = atoi(argv[4]);
			if (port >= 0 && port < 65535 && !argv[2]) {
				listenPort = port;
				strcpy(rootPath, argv[2]);
				return 1;
			}
		}
	}
	return 0;
};

bool SocketInit() {
	struct sockaddr_in addr;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}
	
	// set ip and port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(listenPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// bind socket with ip and port
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}
	
	// max client number
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}
};

void* EstablishConnection(void* params)
{
	struct ThreadParam* connectionData = (struct ThreadParam*)params;
	Login(connectionData);
	HandleCommand(connectionData);
}

struct Request SetRequest(char* sentence) {
	// TODO
};

void Login(struct ThreadParam* data) {
	int connfd = data->connfd;
	char* sentence = data->sentence;

	// USER
	while (1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		data->request = SetRequest(sentence);
		if (data->request.type == USER) {
			USER_Handler(data);
			break;
		}
		else {
			printf("Error user(): %s(%d)\n", strerror(errno), errno);
			char responseStr[RESPONSE_LENGTH] = "530 needs to login.\r\n";
			WriteResponse(data->connfd, strlen(responseStr), responseStr);
			continue;
		}
	}

	// PASS
	while (1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		data->request = SetRequest(sentence);
		if (data->request.type == PASS) {
			PASS_Handler(data);
			break;
		}
		else {
			printf("Error pass(): %s(%d)\n", strerror(errno), errno);
			char responseStr[RESPONSE_LENGTH] = "530 needs password.\r\n";
			WriteResponse(data->connfd, strlen(responseStr), responseStr);
			continue;
		}
	}
};

void HandleCommand(struct ThreadParam* data) {
	int connfd = data->connfd;
	char* sentence = data->sentence;

	while(1) {
		ReadRequest(connfd, SENTENCE_LENGTH, sentence);
		data->request = SetRequest(sentence);
		switch (data->request.type) {
			case RETR:	
				RETR_Handler(data);
				break;
			case STOR:
				STOR_Handler(data);
				break;
			case QUIT:
				QUIT_Handler(data);
				free(data);
				phtread_exit(0);
				break;
			case SYST:
				SYST_Handler(data);
				break;
			case TYPE:
				TYPE_Handler(data);
				break;
			case PORT:
				PORT_Handler(data);
				break;
			case PASV:
				PASV_Handler(data);
				break;
			case MKD:
				MKD_Handler(data);
				break;
			case CWD:
				CWD_Handler(data);
				break;
			case PWD:
				PWD_Handler(data);
				break;
			case LIST:
				LIST_Handler(data);
				break;
			case RMD:
				RMD_Handler(data);
				break;
			case RNFR:
				RNFR_Handler(data);
				break;
			case RNTO:
				RNTO_Handler(data);
				break;
			default:
				INVALID_Handler(data);
				break;
		}
	}
};
/*
int main(int argc, char **argv) {
	int listenfd, connfd;		//����socket������socket��һ���������������ݴ���
	struct sockaddr_in addr;
	char sentence[8192];
	int p;
	int len;

	//����socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Error socket(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//���ñ�����ip��port
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = 6789;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	//����"0.0.0.0"

	//��������ip��port��socket��
	if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		printf("Error bind(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//��ʼ����socket
	if (listen(listenfd, 10) == -1) {
		printf("Error listen(): %s(%d)\n", strerror(errno), errno);
		return 1;
	}

	//����������������
	while (1) {
		//�ȴ�client������ -- ��������
		if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
			printf("Error accept(): %s(%d)\n", strerror(errno), errno);
			continue;
		}
		
		//ե��socket����������
		p = 0;
		while (1) {
			int n = read(connfd, sentence + p, 8191 - p);
			if (n < 0) {
				printf("Error read(): %s(%d)\n", strerror(errno), errno);
				close(connfd);
				continue;
			} else if (n == 0) {
				break;
			} else {
				p += n;
				if (sentence[p - 1] == '\n') {
					break;
				}
			}
		}
		//socket���յ����ַ�������������'\0'
		sentence[p - 1] = '\0';
		len = p - 1;
		
		//�ַ�������
		for (p = 0; p < len; p++) {
			sentence[p] = toupper(sentence[p]);
		}

		//�����ַ�����socket
 		p = 0;
		while (p < len) {
			int n = write(connfd, sentence + p, len + 1 - p);
			if (n < 0) {
				printf("Error write(): %s(%d)\n", strerror(errno), errno);
				return 1;
	 		} else {
				p += n;
			}			
		}

		close(connfd);
	}

	close(listenfd);
}
*/