#include <stdio.h>
#include <conio.h>
#include "Control.h"

int PlayerActivity() {
	int nSelect = 0;
	int x = 1;
	if (kbhit()) {
		nSelect = getch();
		if (nSelect == 224) {
			nSelect = getch();
			switch (nSelect) {
			case KEY_LEFT:
				x = 2;
				break;
			case KEY_RIGHT:
				x = 3;
				break;
			case KEY_SPACE:
				x = -11;
				break;
			}

		}
	}
	
	return x;
}