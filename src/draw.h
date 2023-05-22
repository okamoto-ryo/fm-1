#ifndef _DRAW_H_
#define _DRAW_H_

#include <stdint.h>



void DrawInit(void);

void DrawVertLine(int16_t x1, int16_t y1, int16_t y2);
void DrawHorizLine(int16_t x1, int16_t x2, int16_t y1);
void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
void DrawSqr(int16_t x, int16_t y, int16_t size);

#endif
