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

void errorDisplay(char* str);
void basicSetting(int argc, char* argv[]);
unsigned _stdcall receiveInfo(void);
void sendInfo(void);
void fireBullet(void);
int playerActivity(void);
void windowSetting(void);
void clearWindow(void);
unsigned _stdcall drawWindow(void);
void printPlayer(void);
void printEnemy(void);
void printBullet(void);
void gotoXY(int x, int y);
void checkCrash(void);
void menu();

int main(int argc, char* argv[]) {

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

	// connect() 하무를 이용해서 서버에 접속 시도...
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
	for (i = 0; i < MAX_BULLET; i++) {
		bullet[i].visible = 0;
		bullet[i].x = 0;
		bullet[i].y = 0;
	}

	while (1) {
		recv(clientSocket, &buf, BUF_SIZE, 0);
		number_of_enemy = buf[0];
		player_x = buf[1];
		if (buf[2] == 4) {
			closesocket(clientSocket);
			printf("종료");
			return 0;
		}

		for (i = 0; i < number_of_enemy; i++) {
			enemy[i].n = buf[(i + 1) * 3];
			enemy[i].x = buf[(i + 1) * 3 + 1];
			enemy[i].y = buf[(i + 1) * 3 + 2];
		}
	}
}

void sendInfo(void) {
	int dx;
	int returnValue;

	while (1) {
		send_buf[0] = 1;
		dx = 0;
		while (1) {
			dx = playerActivity();
			if (dx == 0)
				continue;
			break;
		}
		send_buf[1] = dx;
		send_buf[2] = dead_eney;
		returnValue = send(clientSocket, &send_buf, 12, 0);
		if (dead_eney != -1) {
			dead_eney = -1;
		}
	}
}

void fireBullet(void) {
	int i;

	for (i = 0; i < MAX_BULLET; i++) {
		if ((bullet[i].x == 0) || (bullet[i].y == 0))
			continue;
		bullet[i].y--;
		if (bullet[i].y < 1) {
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
	int x = 0, i, n = -1;
	if (kbhit()) {
		nSelect = getch();

		if (nSelect == 224) {
			nSelect = getch();

			switch (nSelect) {
			case KEY_LEFT:
				x = -1;
				break;
			case KEY_RIGHT:
				x = 1;
				break;
			}
		}
		else if (nSelect == KEY_SPACE) {
			for (i = 0; i < MAX_BULLET; i++) {
				if ((bullet[i].x == 0) && (bullet[i].y == 0)) {
					n = i;
					break;
				}
			}
			number_of_bullet++;

			bullet[n].x = player_x;
			bullet[n].y = 26;
			bullet[n].visible = 1;
		}
	}

	return x;
}

// 게임 화면 구성
void windowSetting(void) {
	CONSOLE_CURSOR_INFO cursor = { 0, };
	cursor.dwSize = 1;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
	system("mode con cols=53 lines=30");
}

void clearWindow(void) {
	int i;
	for (i = 0; i < HEIGHT; i++) {
		memset(screen[i], ' ', WIDTH);
		screen[i][WIDTH - 1] = NULL;
	}
}

unsigned _stdcall drawWindow(void) {
	int i;

	while (1) {
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
		printf("score: %d", score);
		Sleep(1000 / 144);
	}
}

void printPlayer(void) {
	int i;
	char paint_player[P_LEN + 1] = "_=^=_";

	for (i = 0; i < P_LEN; i++) {
		screen[player_y][player_x - P_LEN / 2 + i] = paint_player[i];
	}
}

void printEnemy(void) {
	int i, j;
	char paint_enemy[E_LEN + 1] = "&.&";

	for (i = 0; i < number_of_enemy; i++) {
		for (j = 0; j < E_LEN; j++) {
			screen[enemy[i].y][enemy[i].x - E_LEN / 2 + j] = paint_enemy[j];
		}
	}

}

void printBullet(void) {
	int i;
	char paint_bullet = '?';

	for (i = 0; i < number_of_bullet + 1; i++) {
		if (bullet[i].visible == 1) {
			screen[bullet[i].y][bullet[i].x] = paint_bullet;
		}
	}
}

void checkCrash(void) {
	int i, j;

	for (i = 0; i < number_of_enemy; i++) {
		if (((player_y == enemy[i].y - 1) || (player_y == enemy[i].y))
			&&
			((player_x - 4 <= enemy[i].x - 1) && (player_x + 4 >= enemy[i].x + 1))) {
			printf("종료");
			exit(1);
		}
		for (j = 0; j < MAX_BULLET; j++) {
			if (bullet[j].visible == 1) {
				if (((bullet[j].y == enemy[i].y + 1) || (bullet[j].y == enemy[j].y)) &&
					((bullet[j].x > enemy[i].x - 2) && (bullet[j].x < enemy[j].x + 2))) {
					dead_eney = enemy[i].n;
					send_buf[1] = 0;
					send_buf[2] = dead_eney;
					send(clientSocket, &send_buf, 12, 0);
					if (dead_eney != -1) {
						dead_eney = -1;
					}
					score++;
					bullet[j].visible = 0;
					break;
				}
			}
		}
	}
}

void gotoXY(int x, int y) {
	COORD Pos;
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
			if (nSelect == KEY_SPACE) {
				start_flag = 111;
				send_buf[0] = 1;
				send_buf[1] = 0;
				send_buf[2] = dead_eney;
				send(clientSocket, &start_flag, sizeof(start_flag), 0);
				send(clientSocket, &send_buf, 12, 0);
				break;
			}
		}
	}
}  