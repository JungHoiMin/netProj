#include <Windows.h>
#include <stdio.h>
#include "GameWindow.h"


int player[][5] = {
	{0,0,0,0,0},
	{0,0,1,0,0},
	{0,2,3,4,0},
	{0,5,0,5,0},
	{0,0,0,0,0}
};

void CursorInvisible() {
	CONSOLE_CURSOR_INFO cursor = { 0, };
	cursor.dwSize = 1;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void WindowSetting() {
	CursorInvisible();
	system("mode con cols=86 lines=50");	// cols: 가로, lines: 세로
}

void PrintPlayer(int x) {
	int i, j;
	const int y = 40;
	for (i = 0; i < 5; i++) {
		GotoXY(x, y + i);
		for (int j = 0; j < 5; j++) {
			switch (player[i][j]) {
			case 0:
				printf("  ");
				break;
			case 1:
				printf("△");
				break;
			case 2:
				printf("◁");
				break;
			case 3:
				printf("■");
				break;
			case 4:
				printf("▷");
				break;
			case 5:
				printf("＠");
				break;
			}
		}
		printf("\n");
	}
}


void GotoXY(int x, int y) {
	COORD Pos;
	Pos.X = x;
	Pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}