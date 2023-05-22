#include "page.h"
#include "stm32f722xx.h"
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "disp.h"
#include "voice.h"

void DrawPageOsc(void) {
	DrawString(1, 1, "OSC", 4);
	DrawValue(2, 2, "1 RATIO: %2.2f", GetRatio());
	DrawValue(2, 3, "1 INDEX: %.0f", GetIndex());
	DrawValue(2, 4, "2 RATIO: %2.2f", GetRatio2());
	DrawValue(2, 5, "2 INDEX: %.0f", GetIndex2());
}
