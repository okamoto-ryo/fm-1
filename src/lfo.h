#ifndef _LFO_H_
#define _LFO_H_

#include <stdint.h>

#define NUM_OF_GLFO 1

typedef struct {
	uint32_t period;
	int32_t lfoAccum;
	uint16_t index;		// 0:511
	int16_t lev;			// -2048:2048
} lfo_t;

void ProcLFO(void);

lfo_t* GetLFO(uint8_t num);
void SetLFOFreq(uint16_t ad);


#endif
