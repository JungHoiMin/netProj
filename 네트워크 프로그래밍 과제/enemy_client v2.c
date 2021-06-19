/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.26
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(공격자 클라이언트)
* 개정 이력:	2021.05.26
				2021.05.27
				2021.06.01
				2021.06.18
				2021.06.19
* 저작권: 정회민
*/

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#define BUF_SIZE 512
#define MIN_MOVE 3
#define MAX_MOVE 49

void ErrorDisplay(char* str);	// 에러 발생 시 종료해주는 함수
void BasicSetting(int argc, char* argv[]);	// 서버와의 소켓 연결 요청 처리 함수
void SendInfo(void);	// 공격자의 위치정보를 보내는 함수
unsigned _stdcall RecvDead(void);	// 서버로부터의 수신을 통해 게임 시작과 파괴되어 소켓 연결 종료를 처리하는 함수

int error;
WSADATA winsockData;
SOCKET clientSocket;
SOCKADDR_IN serverAddr;
int returnValue;
int x, y = 0;
int level = 1;
int start_flag = 0;

int main(int argc, char* argv[]) {	// 메인 쓰레드 포함 총 2개의 쓰레드로 구성
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
	int dx = 0, dy = 1;
	int life_time = 0;	// 공격자가 방어자보다 밑으로가면 다시 위에서 시작
	int buf[BUF_SIZE];
	int start;
	float sleep_time =0.0;
	float real_time;
	srand(time(NULL));
	x = rand() * 4 % 49 + 1;	// random으로 x좌표 설정
	while(1){
		buf[0] = 2;
		dx = rand() % 3;
		switch (dx) {
		case 0: dx = -1; break;
		case 1: dx = 0; break;
		case 2: dx = 1; break;
		}

		if (level > 1) {	// level에 따른 난이도 증가
			dx *= 2;
		}

		x += dx;
		// 공격자의 위치에 대한 예외처리
		if (x < MIN_MOVE) {
			x = MIN_MOVE;
		}
		else if (x > MAX_MOVE) {
			x = MAX_MOVE;
		}

		if ((life_time % 2) == 1) {	// 2번의 좌표 전송 당 한 번만 내려옴
			if (level > 2) {	// level에 따른 난이도 조정
				dy = rand() % 4;
				switch (dy) {
				case 0: dy = -1; break;
				case 1: dy = 1; break;
				case 2: dy = 1; break;
				case 3: dy = 1; break;
				}
			}
			y += dy;
			if (y > 27) {	// 공격자가 방어자보다 아래로 갈 경우 일정 시간(0.5초) 후 다시 맨 위로 이동
				y = 0;
				level++;
				Sleep(500);
			}
			else if (y < 0) {
				y = 0;
			}
		}

		buf[1] = x;
		buf[2] = y;

		if (start_flag == 111) {	// 시작 플래그를 받으면 시간에 따라 속도가 빨라진다.
			sleep_time = (float)(1000 / 60);
			real_time = (float)(clock() - start) / CLOCKS_PER_SEC;
			sleep_time = (float)sleep_time * (float)(20 - real_time);
		}

		if (sleep_time < 120) {	// 속도가 너무 빠라지는 것을 방지하기 위해 일정 속도가 지나면 유지
			sleep_time = 120;
		}

		Sleep(sleep_time);
		send(clientSocket, &buf, 12, 0);
		printf("%d, %d, %d, %.3f\n", buf[0], buf[1], buf[2], sleep_time);
		life_time++;
		while (start_flag != 111) {	// 시작 플래그 시 그때부터 시간을 잰다.
			start = clock();
		}
	} 
}

unsigned _stdcall RecvDead(void) {
	int dead_flag;
	int info;
	printf("Game Loding...\n");
	while (start_flag != 111) {	// 시작 플래그를 받기 전까지 무한 대기
		recv(clientSocket, &start_flag, 4, 0);
	}
	printf("Game Start!!!\n");
	while (1) {
		dead_flag = recv(clientSocket, &info, sizeof(int), 0);	// 파괴 플래그를 받으면 소켓 종료 및 클라이언트 종료
		if (dead_flag <= 0) {
			closesocket(clientSocket);
			printf("파괴되었습니다.\n");
			exit(1);
		}
	}
}