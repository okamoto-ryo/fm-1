#include "page.h"
#include "stm32f722xx.h"
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "disp.h"
#include "voice.h"

void DrawPageStrct(void) {
	enum {
		xOfst = 20,
	};
	
	DrawString(1, 1, "ALGORITHM", 9);
	DrawString(2, 2, "ALGO:", 5);
	
	switch (GetRouteConfig()) {
		case 0:
			DrawSqr(xOfst + 58, 8, 11);
			DrawVertLine(xOfst + 63, 19, 26);
			DrawSqr(xOfst + 58, 26, 11);
			DrawVertLine(xOfst + 63, 37, 44);
			DrawSqr(xOfst + 58, 44, 11);
			break;
		case 1:
			DrawSqr(xOfst + 50, 17, 11);
			DrawSqr(xOfst + 68, 17, 11);
			DrawVertLine(xOfst + 55, 28, 31);
			DrawVertLine(xOfst + 73, 28, 31);
			DrawHorizLine(xOfst + 55, xOfst + 73, 31);
			DrawVertLine(xOfst + 64, 32, 35);
			DrawSqr(xOfst + 59, 35, 11);
			break;
		case 2:
			DrawSqr(xOfst + 50, 17, 11);
			DrawVertLine(xOfst + 55, 28, 35);
			DrawSqr(xOfst + 50, 35, 11);
			DrawSqr(xOfst + 68, 35, 11);
			break;
		case 3:
			DrawSqr(xOfst + 41, 26, 11);
			DrawSqr(xOfst + 59, 26, 11);
			DrawSqr(xOfst + 77, 26, 11);
			break;
		default:
			break;
	}
}
