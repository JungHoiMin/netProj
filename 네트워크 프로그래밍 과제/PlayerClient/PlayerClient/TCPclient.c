/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.25
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(네트워크 프로그래밍)
* 개정 이력:
* 저작권: 정회민
*/
#include <WinSock2.h>
#include <stdio.h>
#include <process.h>
#include "GameWindow.h"
#include "Control.h"

#define BUF_SIZE 512

void ErrorDisplay(char* str);
int Recvn(SOCKET s, char* buf, int len, int flags);
unsigned _stdcall ReceiveInfo(void);
void SendInfo(void);
void BasicSetting(int argc, char* argv[]);

int error;
WSADATA winsockData;
SOCKET clientSocket;
SOCKADDR_IN serverAddr;
int returnValue;

int main(int argc, char* argv[]) {
	BasicSetting(argc, argv);
	_beginthreadex(NULL, 0, ReceiveInfo, 0, 0, NULL);
	SendInfo();
	return 0;
}

void ErrorDisplay(char* str) {
	printf("<ERROR> %s!!!\n", str);
	exit(-1);
}

int Recvn(SOCKET s, int* buf, int len, int flags) {
	int received;
	int* ptr = buf;
	int left = len;
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}
	return (len - left);
}

void BasicSetting(int argc, char* argv[]) {
	WindowSetting();
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

unsigned _stdcall ReceiveInfo(void) {
	int i;
	int number_of_enemy = 0;
	int player_fire = 0;
	int buf[BUF_SIZE];
	int player_x = 39;
	while (1) {
		PrintPlayer(player_x);
		
		player_fire = 0;
		recv(clientSocket, &buf, 12, 0);
		number_of_enemy = buf[0];
		player_x = buf[1];
		player_fire = buf[2];
		//printf("%d, %d입니다.\n", buf[0], buf[1]);
		
		// 적군 코딩
	}
}

void SendInfo(void) {
	int flag_fire, dx;
	int buf[BUF_SIZE];
	while (1) {
		buf[0] = 1;
		flag_fire = 1;
		dx = 1;
		while (1) {
			dx = PlayerActivity();
			if (dx == 1)
				continue;
			else {
				if (dx == -11) {	// 총알 발사
					flag_fire = 1;
					dx = 1;
				}
				break;
			}
		}
		
		buf[1] = dx;
		buf[2] = flag_fire;

		returnValue = send(clientSocket, &buf, 12, 0);
	}
}