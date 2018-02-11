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
#define DEFAULT_BUFFER 4096 /*缓冲区大小*/
void elevator_status(int floor, int behavior,int efloor);
int elevator[floor_num + 1] = { 0 };
int behv=0;

/*
*function: 在控制台指定的位置输出字符串

*参数：buf,输出的字符；startX、startY为控制台的X，Y坐标,flag标志位
*/
void printStr(int buf, int startX, int startY, int flag)
{
	HANDLE hd;
	COORD pos;

	pos.X = startX;
	pos.Y = startY;
	hd = GetStdHandle(STD_OUTPUT_HANDLE);   /*获取标准输出的句柄*/
	SetConsoleCursorPosition(hd, pos);      /*设置控制台光标输出的位置*/
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

	/*加载Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("WinSock 初始化失败!\n");
		return 1;
	}

	/*创建Socket*/
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	
	if (sListen == SOCKET_ERROR) {
		printf("socket() 失败: %d\n", WSAGetLastError());
		return 1;
	}

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	port = atoi(argv[1]);    //获取端口值
	local.sin_port = htons(port);

	/*绑定Socket*/
	if (bind(sListen,
		(struct sockaddr*)&local,
		sizeof(local)) == SOCKET_ERROR) {
		printf("bind() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	/*打开监听*/
	listen(sListen, 8);
	ret = 0;
	ret = ioctlsocket(sListen, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。
	if (ret == SOCKET_ERROR){//设置失败。
		printf("设置非阻塞失败\n");
	}
	elevator[0] = 1;
	/*在端口进行监听，一旦有客户机发起连接请示
	就建立与客户机进行通信的线程*/
	while (1) {
		AddrSize = sizeof(client);
		/*监听是否有连接请求*/
		sClient = accept(sListen,
			(struct sockaddr*)&client,
			&AddrSize);
		if (sClient == INVALID_SOCKET) {
			//printf("accept() 失败: %d\n", WSAGetLastError());
			err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK){
				continue;
			}
			break;
		}
		printf("接受客户端连接: %s:%d\n",\
			inet_ntoa(client.sin_addr),\
			ntohs(client.sin_port));
		
		while (TRUE) {
			/*接收来自客户机的消息*/
			memset(Buffer, 0, DEFAULT_BUFFER);
			
			ret = recv(sClient, Buffer, DEFAULT_BUFFER, 0);
			if (ret == 0)
				break;
			else if (ret == SOCKET_ERROR) {
				//printf("recv() 失败: %d\n", WSAGetLastError());
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
			printf("Recv: %c\n", Buffer[0]);    //打印接收到的消息
			elevator_status(elevator[0], behv,efloor);
		}
		/*//创建一个线程去处理
		hThread = CreateThread(NULL, 0, ClientThread,
			(LPVOID)sClient, 0, &dwThread);

		if (hThread == NULL) {
			printf("CreateThread() 失败: %d\n", GetLastError());
			break;
		}*/
		//处理完后关闭
		closesocket(sClient);
	
	}
	closesocket(sListen);
	WSACleanup();    //用完了要清理
	return 0;
}

void elevator_status(int floor, int behavior,int efloor)
{
	int i, j;
	int x = 6, y = 1;
	int min = floor_num;
	system("cls");

	printf("****************************************\n楼层：\t");

	//获取最高楼层floor和最低楼层min
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
	//设定上下行 
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
		printf("\r楼层：\t");
	}
	printf("\n当前楼层:%d\n\t", elevator[0]);
	printf("\n****************************************\n");
}