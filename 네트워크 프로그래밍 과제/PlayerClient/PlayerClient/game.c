#include <winsock2.h>
#include <stdio.h>
#include "Control.h"
#include "GameWindow.h"

int main() {
	WindowSetting();
	int x =39;
	while (1) {
		PrintPlayer(x);
		x += PlayerActivity();

		if (x < MIN_MOVE) {
			x = MIN_MOVE;
		}
		else if (x > MAX_MOVE) {
			x = MAX_MOVE;
		}
	}
}