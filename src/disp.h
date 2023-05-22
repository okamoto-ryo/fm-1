#ifndef _DISP_H_
#define _DISP_H_

#include <stdint.h>

void DispInit(void);
void DispOn(void);
void StartDispDMA(void);
void StartDispCmd(void);
void SetPixel(uint8_t x, uint8_t y);
void ClearAll(void);
void PutChar(uint8_t x, uint8_t y, char ltr);
void DrawString(uint8_t x, uint8_t y, char* str, uint8_t len);
void DrawValue(uint8_t x, uint8_t y, const char* str, float val);

#endif
