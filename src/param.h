#ifndef _PARAM_H_
#define _PARAM_H_

#include <stdint.h>

typedef struct {
	float ratio;
	float index;
	float ratio2;
	float index2;
	float index1Offset;
	uint16_t attack;
	uint16_t decay;
	uint16_t sustain;
	uint16_t release;
	uint16_t ampLFODpt;
	uint16_t pad1;
	float indexLFODepth;
	float indexEnvDepth;
	uint32_t lfo1Period;
	int8_t algorithm;
	int8_t pad2[3];
} param_t;

typedef union {
	uint32_t mem[16];
	param_t param;
} paramUnion_t;

void WriteProgram(int8_t num);
void LoadProgram(int8_t num);
void EraseAllPrograms(void);

#endif
