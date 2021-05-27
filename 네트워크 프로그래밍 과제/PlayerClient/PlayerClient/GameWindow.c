#include <Windows.h>
#include <stdio.h>
#include "GameWindow.h"


int player[][5] = {
	{0,0,0,0,0},
	{0,0,1,0,0},
	{0,1,2,1,0},
	{0,3,0,3,0},
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
	system("mode con cols=86 lines=50");	// cols: °¡·Î, lines: ¼¼·Î
}

void PrintPlayer(int x) {
	int i, j;
	const int player_y = 40;
	for (i = 0; i < 5; i++) {
		GotoXY(x, player_y + i);
		for (int j = 0; j < 5; j++) {
			switch (player[i][j]) {
			case 0:
				printf("  ");
				break;
			case 1:
				printf("△");
				break;
			case 2:
				printf("■");
				break;
			case 3:
				printf("○");
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