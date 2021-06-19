/*
* 최초 작성자: 정회민
* 최초 작성일: 2021.05.25
* 최초 변경일:
* 목적: 서버와 클라이언트를 이용한 갤러그 게임 제작(방어자 클라이언트)
* 개정 이력:	2021.05.25
				2021.05.26
				2021.05.27
				2021.05.28
				2021.05.29
				2021.05.31
				2021.06.01
				2021.06.02
				2021.06.18
				2021.06.19
* 저작권: 정회민
*/

#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <conio.h>
#include <process.h>

#define BUF_SIZE 512
#define MAX_BULLET 40
#define MAX_ENEMY 10

#define KEY_UP		72
#define KEY_DOWN	80
#define KEY_LEFT	75
#define KEY_RIGHT	77
#define KEY_SPACE	32

#define WIDTH 53
#define HEIGHT 30
#define P_LEN 5
#define E_LEN 3

typedef struct BULLET {
	int x;
	int y;
	int visible;
}BULLET;

typedef struct ENEMY {
	int n;
	int x;
	int y;
}ENEMY;

SOCKET clientSocket;

char screen[HEIGHT][WIDTH];

int player_x = 26;
const int player_y = 26;
int score = 0;
int send_buf[BUF_SIZE];

ENEMY enemy[MAX_ENEMY];
int number_of_enemy = 0;
int dead_eney = -1;
BULLET bullet[MAX_BULLET];
number_of_bullet = -1;
int start_flag = 0;

void errorDisplay(char* str);	// 에러 발생 시 종료해주는 함수
void basicSetting(int argc, char* argv[]);	// 서버와의 소켓 연결 요청 처리 함수
unsigned _stdcall receiveInfo(void);	// 서버로부터 종합 정보를 받아서 정보를 구조체 배열에 정리하는 함수
void sendInfo(void);	// 방어자의 위치정보를 보내는 함수
void fireBullet(void);	// 총알의 움직임을 처리하는 함수
int playerActivity(void);	// 방향키 입력을 처리하는 함수
void windowSetting(void);	// 출력 화면 기본 설정하는 함수
void clearWindow(void);	// screen buffer를 공백으로 비우는 함수
unsigned _stdcall drawWindow(void);	// 화면을 그리는 함수
void printPlayer(void);	// screen buffer에 방어자를 그리는 함수
void printEnemy(void);	// screen buffer에 공격자들을 그리는 함수
void printBullet(void);	// screen buffer에 총알들을 그리는 함수
void gotoXY(int x, int y);	// 커서의 이동 처리 함수
void checkCrash(void);	// 총알과 공격자 간의 충돌, 공격자와 방어자간의 충돌을 나타내는 함수
void menu();	// 메뉴 표시 함수

int main(int argc, char* argv[]) {	// 메인 쓰레드 포함 총 3개의 쓰레드로 구성
	windowSetting();
	basicSetting(argc, argv, &clientSocket);
	menu();
	_beginthreadex(NULL, 0, drawWindow, NULL, 0, NULL);
	_beginthreadex(NULL, 0, receiveInfo, NULL, 0, NULL);
	sendInfo(&clientSocket);
	return 0;
}

void errorDisplay(char* str) {
	printf("<ERROR> %s!!!\n", str);
	exit(-1);
}

void basicSetting(int argc, char* argv[]) {
	WSADATA winsockData;
	SOCKADDR_IN serverAddr;

	int error;
	int returnValue;

	// 윈속 DLL:open()
	error = WSAStartup(MAKEWORD(2, 2), &winsockData);
	if (error != 0)
		errorDisplay("소켓 초기화 라이브러리 연결");

	// 소켓 만들기
	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		errorDisplay("socket() error(INVALID_SOCKET)");
	}

	// connect()를 이용해서 서버에 접속 시도...
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	returnValue = connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (returnValue == SOCKET_ERROR) {
		errorDisplay("connect() error(SOCKET_ERROR)");
	}
}

unsigned _stdcall receiveInfo(void) {
	int i, j;
	int buf[BUF_SIZE];

	for (i = 0; i < MAX_BULLET; i++) {	// 총알버퍼 초기화
		bullet[i].visible = 0;
		bullet[i].x = 0;
		bullet[i].y = 0;
	}

	while (1) {
		recv(clientSocket, &buf, BUF_SIZE, 0);	// 종합 정보 수신
		number_of_enemy = buf[0];	// buf[0] == 공격자 클라이언트의 수
		player_x = buf[1];
		if (buf[2] == 4) {	// buf[2]==4이면 공격자와 방어자 충돌->게임종료
			closesocket(clientSocket);
			printf("종료");
			return 0;
		}

		for (i = 0; i < number_of_enemy; i++) {	// 공격자를 공격자 구조체 배열에 저장
			enemy[i].n = buf[(i + 1) * 3];
			enemy[i].x = buf[(i + 1) * 3 + 1];
			enemy[i].y = buf[(i + 1) * 3 + 2];
		}
	}
}

void sendInfo(void) {
	int dx;
	int returnValue;

	send_buf[2] = -1;	// 파괴된 공격자가 없음을 알림
	while (1) {
		send_buf[0] = 1;	// 방어자 클라이언트임을 알려주는 signal
		dx = 0;
		while (1) {
			dx = playerActivity();	// 방향키 입력을 통해 dx를 받아옴
			if (dx == 0)
				continue;
			break;
		}
		send_buf[1] = dx;
		returnValue = send(clientSocket, &send_buf, 12, 0);
	}
}

void fireBullet(void) {
	int i;

	for (i = 0; i < MAX_BULLET; i++) {
		if ((bullet[i].x == 0) || (bullet[i].y == 0))	// 화면에 나오면 안되는 총알 처리
			continue;
		bullet[i].y--;	// 총알의 움직임 표시
		if (bullet[i].y < 1) {	// 총알이 화면 밖을 나가면 제거
			bullet[i].x = 0;
			bullet[i].y = 0;
			bullet[i].visible = 0;
			number_of_bullet--;
		}
	}
	return 0;
}

// 컨트롤
int playerActivity(void) {
	int nSelect = 0;
	int x = 0, i;

	if (kbhit()) {
		nSelect = getch();
		if (nSelect == 224) {
			nSelect = getch();
			switch (nSelect) {	// 좌우 방향키 입력에 대한 처리
			case KEY_LEFT:
				x = -1;
				break;
			case KEY_RIGHT:
				x = 1;
				break;
			}
		}
		else if (nSelect == KEY_SPACE) {	// SPACE_BAR 입력에 대한 처리
			for (i = 0; i < MAX_BULLET; i++) {
				if ((bullet[i].x == 0) && (bullet[i].y == 0)) {	// 사용중이지 않은 총알 사용
					break;
				}
			}
			number_of_bullet++;
			bullet[i].x = player_x;
			bullet[i].y = 26;
			bullet[i].visible = 1;
		}
	}
	return x;
}

// 게임 화면 구성
void windowSetting(void) {
	CONSOLE_CURSOR_INFO cursor = { 0, };	// 커서에 대한 정보
	cursor.dwSize = 1;
	cursor.bVisible = FALSE;	// 커서를 화면에서 안보이게 설정
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
	system("mode con cols=53 lines=30");	// 화면 크기 설정
}

void clearWindow(void) {
	int i;
	for (i = 0; i < HEIGHT; i++) {
		memset(screen[i], ' ', WIDTH);	// 화면 초기화
		screen[i][WIDTH - 1] = NULL;
	}
}

unsigned _stdcall drawWindow(void) {
	int i;
	
	while (1) {	// 화면 그리기
		fireBullet();
		clearWindow();
		printPlayer();
		printEnemy();
		printBullet();
		checkCrash();
		for (i = 0; i < HEIGHT; i++) {
			gotoXY(0, i);
			printf("%s", screen[i]);
		}
		gotoXY(2, i - 2);
		printf("score: %d", score);	// 점수 표시
		Sleep((float)(1000 / 144));	// 144프레임(1초에 144번 출력)
	}
}

void printPlayer(void) {
	int i;
	char paint_player[P_LEN + 1] = "_=^=_";	// 방어자

	for (i = 0; i < P_LEN; i++) {
		screen[player_y][player_x - P_LEN / 2 + i] = paint_player[i];
	}
}

void printEnemy(void) {
	int i, j;
	char paint_enemy[E_LEN + 1] = "&.&";	// 공격자

	for (i = 0; i < number_of_enemy; i++) {
		for (j = 0; j < E_LEN; j++) {
			screen[enemy[i].y][enemy[i].x - E_LEN / 2 + j] = paint_enemy[j];
		}
	}

}

void printBullet(void) {
	int i;
	char paint_bullet = '?';	// 총알

	for (i = 0; i < number_of_bullet + 1; i++) {
		if (bullet[i].visible == 1) {
			screen[bullet[i].y][bullet[i].x] = paint_bullet;
		}
	}
}

void checkCrash(void) {
	int i, j;
	
	for (i = 0; i < number_of_enemy; i++) {
		if (((player_y == enemy[i].y - 1) || (player_y == enemy[i].y))	// 공격자와 방어자의 충돌 처리
			&&
			((player_x - 4 <= enemy[i].x - 1) && (player_x + 4 >= enemy[i].x + 1))) {
			printf("종료");
			closesocket(clientSocket);
			exit(1);
		}
		for (j = 0; j < MAX_BULLET; j++) {
			if (bullet[j].visible == 1) {
				if (((bullet[j].y == enemy[i].y + 1) || (bullet[j].y == enemy[i].y)) &&		// 공격자와 총알과의 충돌 처리
					((bullet[j].x > enemy[i].x - 2) && (bullet[j].x < enemy[i].x + 2))) {	// 충돌 시 파괴된 공격자의 정보 송신
					dead_eney = enemy[i].n;
					send_buf[1] = 0;
					send_buf[2] = dead_eney;
					send(clientSocket, &send_buf, 12, 0);
					if (dead_eney != -1) {
						dead_eney = -1;
						send_buf[2] = dead_eney;
					}
					send(clientSocket, &send_buf, 12, 0);
					score++;
					bullet[j].visible = 0;
					break;
				}
			}
		}
	}
}

void gotoXY(int x, int y) {
	COORD Pos;	// 커서에 좌표를 주어 이동
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void menu() {
	int i;
	int nSelect = 0;
	char playGame[11] = "Game Start";
	char info[19] = "PRESS SPACE BAR!!!";
	char myinfo[24] = "2017156038 Jung Hoe Min";

	clearWindow();

	for (i = 0; i < 10; i++) {
		screen[14][26 - 10 / 2 + i] = playGame[i];
	}
	for (i = 0; i < 18; i++) {
		screen[16][26 - 18 / 2 + i] = info[i];
	} 
	for (i = 0; i < 23; i++) {
		screen[18][26 - 23 / 2 + i] = myinfo[i]; 
	}

	for (i = 0; i < HEIGHT; i++) {
		gotoXY(0, i);
		printf("%s", screen[i]);
	}

	while (1) {
		if (kbhit()) {
			nSelect = getch();
			if (nSelect == KEY_SPACE) {	// SPACE BAR를 입력 시 게임 시작
				start_flag = 111;
				send_buf[0] = 1;
				send_buf[1] = 0;
				send_buf[2] = -1;
				send(clientSocket, &start_flag, sizeof(start_flag), 0);	// 게임 시작 flag를 서버에 전송
				send(clientSocket, &send_buf, 12, 0);
				break;
			}
		}
	}
}  