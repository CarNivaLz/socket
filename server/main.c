#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib , "ws2_32.lib")


#define up	1
#define down 2
#define stop 0
#define inside 1
#define outdide 0

#define floor_num 7
#define DEFAULT_BUFFER 4096 /*��������С*/
void elevator_status(int floor, int behavior,int efloor);
int elevator[floor_num + 1] = { 0 };
int behv=0;

/*
*function: �ڿ���ָ̨����λ������ַ���

*������buf,������ַ���startX��startYΪ����̨��X��Y����,flag��־λ
*/
void printStr(int buf, int startX, int startY, int flag)
{
	HANDLE hd;
	COORD pos;

	pos.X = startX;
	pos.Y = startY;
	hd = GetStdHandle(STD_OUTPUT_HANDLE);   /*��ȡ��׼����ľ��*/
	SetConsoleCursorPosition(hd, pos);      /*���ÿ���̨��������λ��*/
	if (flag == 1) {
		printf(" [%d] ", buf);
	}
	else {
		printf(" %d ", buf);
	}

}

int main(int argc, char **argv)
{
	WSADATA        wsd;
	HANDLE        hThread;
	DWORD        dwThread;
	SOCKET        sListen, sClient;
	int            AddrSize;
	unsigned long ul = 1,err;
	
	int ret,efloor=1,floor=1,n;
	unsigned short port;

	struct sockaddr_in local, client;

	char Buffer[DEFAULT_BUFFER];

	if (argc < 2) {
		printf("Usage:%s Port\n", argv[0]);
		return -1;
	}

	/*����Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("WinSock ��ʼ��ʧ��!\n");
		return 1;
	}

	/*����Socket*/
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	
	if (sListen == SOCKET_ERROR) {
		printf("socket() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	port = atoi(argv[1]);    //��ȡ�˿�ֵ
	local.sin_port = htons(port);

	/*��Socket*/
	if (bind(sListen,
		(struct sockaddr*)&local,
		sizeof(local)) == SOCKET_ERROR) {
		printf("bind() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}
	/*�򿪼���*/
	listen(sListen, 8);
	ret = 0;
	ret = ioctlsocket(sListen, FIONBIO, (unsigned long *)&ul);//���óɷ�����ģʽ��
	if (ret == SOCKET_ERROR){//����ʧ�ܡ�
		printf("���÷�����ʧ��\n");
	}
	elevator[0] = 1;
	/*�ڶ˿ڽ��м�����һ���пͻ�������������ʾ
	�ͽ�����ͻ�������ͨ�ŵ��߳�*/
	while (1) {
		AddrSize = sizeof(client);
		/*�����Ƿ�����������*/
		sClient = accept(sListen,
			(struct sockaddr*)&client,
			&AddrSize);
		if (sClient == INVALID_SOCKET) {
			//printf("accept() ʧ��: %d\n", WSAGetLastError());
			err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK){
				continue;
			}
			break;
		}
		printf("���ܿͻ�������: %s:%d\n",\
			inet_ntoa(client.sin_addr),\
			ntohs(client.sin_port));
		
		while (TRUE) {
			/*�������Կͻ�������Ϣ*/
			memset(Buffer, 0, DEFAULT_BUFFER);
			
			ret = recv(sClient, Buffer, DEFAULT_BUFFER, 0);
			if (ret == 0)
				break;
			else if (ret == SOCKET_ERROR) {
				//printf("recv() ʧ��: %d\n", WSAGetLastError());
				err = WSAGetLastError();

				if (err != WSAEWOULDBLOCK) {
					break;
				}
				
			}
			Buffer[3] = Buffer[1];
			Buffer[1] = '\0';
			n = atoi(&Buffer[0]);
			if (behv == 0)
				behv = n;

			n = atoi(&Buffer[3]);
			if(n != 0)
				elevator[n]++;
			floor = n;
			printf("Recv: %c\n", Buffer[0]);    //��ӡ���յ�����Ϣ
			elevator_status(elevator[0], behv,efloor);
		}
		/*//����һ���߳�ȥ����
		hThread = CreateThread(NULL, 0, ClientThread,
			(LPVOID)sClient, 0, &dwThread);

		if (hThread == NULL) {
			printf("CreateThread() ʧ��: %d\n", GetLastError());
			break;
		}*/
		//�������ر�
		closesocket(sClient);
	
	}
	closesocket(sListen);
	WSACleanup();    //������Ҫ����
	return 0;
}

void elevator_status(int floor, int behavior,int efloor)
{
	int i, j;
	int x = 6, y = 1;
	int min = floor_num;
	system("cls");

	printf("****************************************\n¥�㣺\t");

	//��ȡ���¥��floor�����¥��min
	for (i = 1; i <= floor_num; i++) {
		if (elevator[i] >= 1) {
			if (i > floor)
				floor = i;
			else if (i < min)
				min = i;
		}
		else
			floor = min;
	}
	j = 0;
	for (i = 1; i <= floor_num; i++) {
		if (elevator[i] == 0)
			j++;
	}
	if (j == 7)
		floor = elevator[0];
	//�趨������ 
	if (elevator[0] > floor) {
		behavior = down;
		elevator[0]--;
	}
		
	else if (elevator[0] < floor) {
		behavior = up;
		elevator[0]++;
	}
	else
		behavior = stop;


	if (behavior == up) {
		i = elevator[0];
			for (j = 1; j <= floor_num; j++) {
				if (i == j) {
					//printf("[%d]  ", j);
					printStr(j, x, y, 1);
					printf("    ");
					elevator[0] = j;
					elevator[i] = 0;

				} 
				else {
					printStr(j, x, y, 0);
					printf("    ");
					//printf("%d  ", j);
				}
				x = x + 5;
			}
			printf("\r");
			Sleep(3000);
		}
	
	else if (behavior == down) {
			i = elevator[0];
			for (j = 1; j <= floor_num; j++) {
				if (i == j) {
					//printf("[%d]  ", j);
					printStr(j, x, y, 1);
					printf("    ");
					elevator[0] = j;
					elevator[i] = 0;
					//break;
				}
				else {
					printStr(j, x, y, 0);
					printf("    ");
					//printf("%d  ", j);
				}
				x = x + 5;
			}
			printf("\r");
			Sleep(3000);
	}
	else {
		i = elevator[0];
		for (j = 1; j <= floor_num; j++) {
			if (i == j) {
				printf("[%d]  ", j);
				elevator[0] = j;
			}
			else
				printf("%d  ", j);
		}
		printf("\r¥�㣺\t");
	}
	printf("\n��ǰ¥��:%d\n\t", elevator[0]);
	printf("\n****************************************\n");
}