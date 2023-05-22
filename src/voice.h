#ifndef _VOICE_H_
#define _VOICE_H_

#include <stdint.h>

#define NUM_OF_VOICES 6
#define NUM_OF_OSCILLATORS 3

/*
	routeConfig
	0			1				2				3
	
	2						
	|
	1			1 2			1
	|			|/			|
	0			0				0	2			0 1 2
	
*/

typedef enum {
	ENV_ATK,
	ENV_DCY,
	ENV_SUS,
	ENV_RLS,
	ENV_OFF
} envPhase_t;

typedef struct {
	float freq;
	uint32_t inc;
	uint32_t tblPtr;	// Shifted left by 16 bits
	uint32_t amp;
	float ratio;
	float index;
} osc_t;

typedef struct {
	uint16_t a;
	uint16_t d;
	uint16_t s;
	uint16_t r;
	int16_t lev;
	int16_t levRlsStart;
	envPhase_t envPhase;
	int32_t envAccum;
} env_t;

typedef struct {
	osc_t osc[NUM_OF_OSCILLATORS];
	uint8_t num;
	uint8_t gate;
	uint16_t eventCnt;
	uint16_t time;
	env_t env;
} voice_t;
	

void VoiceInit(void);
void NoteOn(uint8_t num);
void NoteOff(uint8_t num);
void SetCutoff(uint16_t ad);
void SetResonance(uint16_t ad);

void SetRouteConfigRaw(int8_t num);
void IncRouteConfig(int32_t i);
void SetRouteConfig(uint16_t ad);
int8_t GetRouteConfig(void);
float GetRatio(void);
void SetRatioRaw(float rat);
void SetRatio(uint16_t ad);
float GetRatio2(void);
void SetRatio2Raw(float rat);
void SetRatio2(uint16_t ad);
float GetIndex(void);
void SetIndexRaw(float ind);
void SetIndex(uint16_t ad);
float GetIndex2(void);
void SetIndex2Raw(float ind);
void SetIndex2(uint16_t ad);
float GetIndexOffset(void);
void SetIndexOffsetRaw(float ofs);
void SetIndexOffset(uint16_t ad);


void SetAttack(uint16_t ad);
void SetDecay(uint16_t ad);
void SetSustain(uint16_t ad);
void SetRelease(uint16_t ad);

uint16_t GetAmpLfoDepth(void);
void SetAmpLfoDepth(uint16_t ad);
float GetIndexLFODepth(void);
void SetIndexLFODepthRaw(float depth);
void SetIndexLFODepth(uint16_t ad);
float GetIndexEnvDepth(void);
void SetIndexEnvDepthRaw(float depth);
void SetIndexEnvDepth(uint16_t ad);

env_t* GetEnv(uint8_t num);

#endif
