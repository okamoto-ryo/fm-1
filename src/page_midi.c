#include "page.h"
#include "stm32f722xx.h"
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "disp.h"
#include "midi.h"

void DrawPageMIDI(void) {
	rxBuf_t aBuf = GetRXBuf();
	for (uint32_t i = 0; i < 7; i++) {
		for (uint32_t j = 0; j < 8; j++) {
			char str[3];
			sprintf(str, "%02X", aBuf.ptr[j * 7 + i]);
			DrawString(i * 3, j, str, 3);
		}
	}
	DrawString((aBuf.wIdx % 7) * 3 + 2, aBuf.wIdx / 7, "_", 1);
}
