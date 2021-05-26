#include <winsock2.h>
#include <stdio.h>
#include "Control.h"
#include "GameWindow.h"

int main() {
	WindowSetting();
	int x =39;
	int dx = 0;
	while (1) {
		PrintPlayer(x);
		dx = PlayerActivity();
		switch (dx) {
		case 1:dx = 0; break;
		case 2:dx = -1; break;
		case 3:dx = 1; break;
		}
		x += dx;
		if (x < MIN_MOVE) {
			x = MIN_MOVE;
		}
		else if (x > MAX_MOVE) {
			x = MAX_MOVE;
		}
	}
}
