#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <errno.h>

#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

struct ThreadParam
{
	int connfd;

};

void LoadParam() {

};

bool ArgHandler(int argc, char **argv) {

};

bool SocketInit() {

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