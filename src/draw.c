#include "draw.h"
#include "disp.h"

#include <stdlib.h>

uint8_t penColor = 0;

static void DrawPixel(int x, int y) {
	switch (penColor) {
		case 0:
			if (x >= 0 && x < 128 && y >= 0 && y < 64) {
				SetPixel((uint8_t)x, (uint8_t)y);
			}
			break;
		default:
			break;
	}
}
	
void DrawVertLine(int16_t x1, int16_t y1, int16_t y2) {
	if (y1 < y2) {
		for (int16_t y = y1; y <= y2; y++) {
			DrawPixel(x1, y);
		}
	} else {
		for (int16_t y = y2; y <= y1; y++) {
			DrawPixel(x1, y);
		}
	}
}

void DrawHorizLine(int16_t x1, int16_t x2, int16_t y1) {
	if (x1 < x2) {
		for (int16_t x = x1; x <= x2; x++) {
			DrawPixel(x, y1);
		}
	} else {
		for (int16_t x = x2; x <= x1; x++) {
			DrawPixel(x, y1);
		}
	}
}

void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	int16_t xs, ys, xe, ye;
	int16_t dx, dy;
	int16_t D;
	int16_t inc;
	int16_t x, y;
	
	if (x1 == x2) {
		DrawVertLine(x1, y1, y2);
	} else {
		if (y1 == y2) {
			DrawHorizLine(x1, x2, y1);
		} else {
			if (abs(x1 - x2) > abs(y1 - y2)) {
				if (x1 < x2) {
					xs = x1; ys = y1; xe = x2; ye = y2;
				} else {
					xs = x2; ys = y2; xe = x1; ye = y1;
				}
				dx = xe - xs; dy = ye - ys;
				
				y = ys;
				if (dy > 0) {
					inc = 1;
				} else {
					dy = -dy;
					inc = -1;
				}
				D = 2 * dy - dx;
				for (x = xs; x <= xe; x++) {
					DrawPixel(x, y);
					if (D > 0) {
						y += inc;
						D -= 2 * dx;
					}
					D += 2 * dy;
				}
			} else {
				if (y1 < y2) {
					xs = x1; ys = y1; xe = x2; ye = y2;
				} else {
					xs = x2; ys = y2; xe = x1; ye = y1;
				}
				dx = xe - xs; dy = ye - ys;
				
				x = xs;
				if (dx > 0) {
					inc = 1;
				} else {
					dx = -dx;
					inc = -1;
				}
				D = 2 * dx - dy;
				for (y = ys; y <= ye; y++) {
					DrawPixel(x, y);
					if (D > 0) {
						x += inc;
						D -= 2 * dy;
					}
					D += 2 * dx;
				}
			}
		}
	}
}

void DrawSqr(int16_t x, int16_t y, int16_t size) {
	DrawLine(x, y, x + size - 1, y);
	DrawLine(x + size - 1, y, x + size - 1, y + size - 1);
	DrawLine(x + size - 1, y + size - 1, x, y + size - 1);
	DrawLine(x, y + size - 1, x, y);
}
