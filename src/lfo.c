#include "lfo.h"
#include "led.h"
#include <math.h>

extern const int32_t sinTbl[];

static lfo_t lfo[NUM_OF_GLFO];

void ProcLFO(void) {
	for (uint32_t i = 0; i < NUM_OF_GLFO; i++) {
		while (lfo[i].lfoAccum > 0) {
			lfo[i].index++;
			lfo[i].lev = (int16_t)sinTbl[lfo[i].index];
			
			lfo[i].lfoAccum -= lfo[i].period >> 5;		// 2 * lfo[i].period >> 6
		}
		lfo[i].lfoAccum += 1024;		// 2 * 512
		if (lfo[i].index > 511) {
			lfo[i].index = 0;
			lfo[i].lev = (int16_t)sinTbl[0];
		}
	}
}

lfo_t* GetLFO(uint8_t num) {
	return &lfo[num];
}

void SetLFOFreq(uint16_t ad) {
	float e;
	for (uint32_t i = 0; i < NUM_OF_GLFO; i++) {
		e = 4096.0f * expf((float)(0x0FFF - ad) * 0.0012f);
		lfo[i].period = (uint32_t)e;
	}
}

