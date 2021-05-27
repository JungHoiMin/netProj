#include <stdio.h>
#include <conio.h>
#include "Control.h"

int PlayerActivity() {
	int nSelect = 0;
	int x = 0;
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
			x = -11;
		}
	}

	return x;
}