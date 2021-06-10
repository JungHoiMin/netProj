/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.26
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(공격자 클라이언트)
* 개정 이력:	2021.05.26
				2021.05.27
				2021.06.01
* 저작권: 정회민
*/

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#define BUF_SIZE 512

void ErrorDisplay(char* str);
void BasicSetting(int argc, char* argv[]);
void SendInfo(void);
unsigned _stdcall RecvDead(void);

int error;
WSADATA winsockData;
SOCKET clientSocket;
SOCKADDR_IN serverAddr;
int returnValue;
int x, y = 0;
int level = 1;

int main(int argc, char* argv[]) {
	BasicSetting(argc, argv);
	_beginthreadex(NULL, 0, RecvDead, NULL, 0, NULL);
	SendInfo();
	return 0;
}

void ErrorDisplay(char* str) {
	printf("<ERROR> %s!!!\n", str);
	exit(-1);
}

void BasicSetting(int argc, char* argv[]) {
	// 윈속 DLL:open()
	error = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (error != 0)
		ErrorDisplay("소켓 초기화 라이브러리 연결");

	// 소켓 만들기
	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		ErrorDisplay("socket() error(INVALID_SOCKET)");
	}

	// connect() 하무를 이용해서 서버에 접속 시도...
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	returnValue = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (returnValue == SOCKET_ERROR) {
		ErrorDisplay("connect() error(SOCKET_ERROR)");
	}
}

void SendInfo(void) {
	int dx = 0, life_time = 0;
	int buf[BUF_SIZE];
	srand(time(NULL));
	x = rand()*4 % 49 + 1;
	while (1) {
		buf[0] = 2;
		dx = rand() % 3;
		switch (dx) {
		case 0: dx = -1; break;
		case 1: dx = 0; break;
		case 2: dx = 1; break;
		}

		
		if ((life_time % 2) == 1) {
			y++;
			if (y > 27) {
				y = 0;
				level++;
			}
		}

		if (level == 1) {
			buf[1] = x;
			buf[2] = y;
		}
		else if(level == 2){
			x += dx;
			buf[1] = x;
			buf[2] = y;
		}
		else{
			x += (dx * 2);
			buf[1] = x;
			buf[2] = y;
		}

		Sleep(1000 / 60 * 20);
		returnValue = send(clientSocket, &buf, 12, 0);
		life_time++;
	}
}

unsigned _stdcall RecvDead(void) {
	int dead_flag;
	int info;
	while (1) {
		dead_flag = recv(clientSocket, &info, sizeof(int), 0);
		if (dead_flag <= 0) {
			closesocket(clientSocket);
			printf("파괴되었습니다.\n");
			exit(1);
		}
	}
}