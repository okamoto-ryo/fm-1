#include "page.h"
#include "stm32f722xx.h"
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "disp.h"
#include "voice.h"
#include "lfo.h"

void DrawPageLFO(void) {
	lfo_t* lfo = GetLFO(0);
	DrawString(1, 1, "LFO", 4);
	DrawValue(2, 2, "RATE: %.2fHz", (float)(1 << 17) / (float)lfo->period);
	DrawValue(2, 3, "AMP LFO DPT: %.0f", GetAmpLfoDepth());
	DrawValue(2, 4, "IDX LFO DPT: %.0f", GetIndexLFODepth());
	DrawValue(2, 5, "IDX ENV DPT: %.0f", GetIndexEnvDepth());
}
