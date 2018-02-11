#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib , "ws2_32.lib")

#define BUFSIZE 4096 /*��������С*/

int main(int argc, char *argv[])
{
	WSADATA wsd;
	SOCKET sClient;
	char Buffer[BUFSIZE];
	int ret,n;
	struct sockaddr_in server;
	unsigned short port;
	struct hostent *host = NULL;

	if (argc < 3) {
		printf("Usage:%s IP Port\n", argv[0]);
		return -1;
	}


	/*����Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("Winsock    ��ʼ��ʧ��!\n");
		return 1;
	}

	/*����Socket*/
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET) {
		printf("socket() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}
	/*ָ����������ַ*/
	server.sin_family = AF_INET;
	port = atoi(argv[2]);
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(argv[1]);

	if (server.sin_addr.s_addr == INADDR_NONE) {
		host = gethostbyname(argv[1]);    //����ĵ�ַ������������
		if (host == NULL) {
			printf("�޷���������˵�ַ: %s\n", argv[1]);
			return 1;
		}
		CopyMemory(&server.sin_addr,
			host->h_addr_list[0],
			host->h_length);
	}
	/*���������������*/
	if (connect(sClient, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR) {
		printf("connect() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}

	/*���͡�������Ϣ*/

	for (;;) {
		//�ӱ�׼�����ȡҪ���͵�����
		//gets_s(Buffer,BUFSIZE);
		printf("�����������У�(���У�1��/���У�2������¥�㣨1~7��\n���磺17���ر��ϵ�7¥��:");
		scanf("%s", Buffer);
		Buffer[2] = '\0';
		//�������������Ϣ
		ret = send(sClient, Buffer, strlen(Buffer), 0);
		if (ret == 0) {
			break;
		}
		else if (ret == SOCKET_ERROR) {
			printf("send() ʧ��: %d\n", WSAGetLastError());
			break;
		}
		printf("Send %d bytes\n", ret);
	
	}
	//�����ˣ��ر�socket���(�ļ�������)
	closesocket(sClient);
	WSACleanup();    //����
	return 0;
}

