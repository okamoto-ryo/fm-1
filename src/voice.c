#include "voice.h"
#include "stm32f722xx.h"
#include "ad.h"
#include "lfo.h"
#include "draw.h"
#include "view.h"
#include "system.h"
#include <math.h>

#define SINE_OSCILLATOR

#define PI 3.14159265358979323846

voice_t voice[NUM_OF_VOICES];

static uint16_t ampLFODpt = 0;

static float filtCoeff = 0.0f;
static float cFreq = 1000.0f;
static float reso = 0.0f;

static float index1Offset = 0.0f;
static float indexLFODepth = 0.0f;
static float indexEnvDepth = 0.0f;

int8_t routeConfig = 0;

extern const int32_t sinTbl[513];
extern const float equalTemp[];

// For scope
uint32_t scopeBufWIdx = 257;
extern int32_t scopeBuf[256];

void VoiceInit(void) {
	for (int i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].num = 0;		// MIDI note no. 0x3C
		voice[i].gate = 0;
		voice[i].eventCnt = 0;
		for (int j = 0; j < NUM_OF_OSCILLATORS; j++) {
			voice[i].osc[j].freq = equalTemp[0];
			voice[i].osc[j].inc = (uint32_t)(16 * voice[i].osc[j].freq / 1250.0f * 65536);
			voice[i].osc[j].tblPtr = 0;
			voice[i].osc[j].amp = 0;
			voice[i].osc[j].ratio = 1.0f;
			voice[i].osc[j].index = 0.0f;
		}
		voice[i].env.a = 0;
		voice[i].env.d = 0;
		voice[i].env.s = 4095;
		voice[i].env.r = 0;
		voice[i].env.envPhase = ENV_OFF;
		voice[i].env.envAccum = 0;
		voice[i].env.lev = 0;
		voice[i].env.levRlsStart = (int16_t)voice[i].env.s;
		voice[i].time = 0;		// increment by 1 every 1ms
	}
	
	filtCoeff = (float)(1.0 - exp(-2 * PI * 5000 / 10000));
	
	GetLFO(0)->period = 0x00008000;
}

static void AssignNote(uint32_t v, uint8_t num) {
	voice[v].num = num;				
	voice[v].gate = 1;
	voice[v].osc[0].freq = equalTemp[num % 12];
	if (num >= 0x3C) {
		voice[v].osc[0].freq *= (float)(1U << (uint32_t)((num - 0x3C) / 12));
	} else {
		voice[v].osc[0].freq /= (float)(1U << (uint32_t)((0x3B - num) / 12 + 1));
	}
	voice[v].osc[0].inc = (uint32_t)(16 * voice[v].osc[0].freq / 1250.0f * 65536);
	
	voice[v].osc[1].freq = voice[v].osc[0].freq * voice[v].osc[1].ratio;
	voice[v].osc[1].inc = (uint32_t)(16 * voice[v].osc[1].freq / 1250.0f * 65536);
	
	voice[v].osc[2].freq = voice[v].osc[1].freq * voice[v].osc[2].ratio;
	voice[v].osc[2].inc = (uint32_t)(16 * voice[v].osc[2].freq / 1250.0f * 65536);
}

void NoteOn(uint8_t num) {
	uint32_t i = 0;
	uint32_t maxCnt = 0;
	uint32_t voiceWithMaxCnt = 0;
	while (voice[i].num != num && i < NUM_OF_VOICES) {
		i++;
	}
	if (i < NUM_OF_VOICES) {
		voice[i].env.envPhase = ENV_ATK;
		voice[i].eventCnt = 0;
	} else {
		i = 0;
		while (voice[i].env.envPhase != ENV_OFF && i < NUM_OF_VOICES) {
			i++;
		}
		if (i < NUM_OF_VOICES) {
			AssignNote(i, num);
			voice[i].env.envAccum = 0;
			voice[i].env.lev = 0;
			voice[i].env.envPhase = ENV_ATK;
			voice[i].eventCnt = 0;
		} else {
			for (i = 0; i < NUM_OF_VOICES; i++) {
				if (voice[i].eventCnt > maxCnt) {
					maxCnt = voice[i].eventCnt;
					voiceWithMaxCnt = i;
				}
			}
			AssignNote(voiceWithMaxCnt, num);
			voice[voiceWithMaxCnt].env.envAccum = 0;
			voice[voiceWithMaxCnt].env.lev = 0;
			voice[voiceWithMaxCnt].env.envPhase = ENV_ATK;
			voice[voiceWithMaxCnt].eventCnt = 0;
		}
	}
	for (i = 0; i < NUM_OF_VOICES; i++) {
		if (voice[i].env.envPhase != ENV_OFF) {
			voice[i].eventCnt++;
		}
	}
}

static void VoiceOff(uint32_t v) {		// Deassign note from a voice and make the voice available to new note
	voice[v].env.envPhase = ENV_OFF;
	voice[v].eventCnt = 0;
}

void NoteOff(uint8_t num) {
	uint8_t voiceFound = 0;
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		if ((voice[i].num == num) && !voiceFound) {
			voice[i].gate = 0;
			voice[i].env.levRlsStart = voice[i].env.lev;
			voice[i].env.envPhase = ENV_RLS;
			voice[i].env.envAccum = 0;
			voice[i].time = 0;
			
			voiceFound = 1;
		}
	}
}

#ifdef MOOG_FILTER
void SetMoogCoeff(float normFreq, float resonance) {
	if (normFreq < 0.4f) {
		q = 1.0f - normFreq;
		p = normFreq + 0.8f * normFreq * q;
		f = p + p - 1.0f;
		q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
	}
}
#endif

void SetCutoff(uint16_t ad) {
	cFreq = (float)exp(8.5171931914162374266547336972793 * ad / 4096.0);
	filtCoeff = (float)(1.0 - exp(-2.0 * PI * (double)cFreq / 10000.0));
	#ifdef MOOG_FILTER
	SetMoogCoeff(cFreq / 10000.0f, reso);
	#endif
}

void SetResonance(uint16_t ad) {
	reso = (float)((float)ad / 4096.0f * 1.0f);
	
	#ifdef MOOG_FILTER
	SetMoogCoeff(cFreq / 10000.0f, reso);
	#endif
}

void SetRouteConfigRaw(int8_t num) {
	routeConfig = num;
	ArmDrawRouting();
	TIM5->CNT = 0;
	TIM5->ARR = 2000;
	NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
}

void IncRouteConfig(int32_t i) {
	int32_t num = 0;
	if (routeConfig + i < 0) {
		num = 0;
	} else if (routeConfig + i > 3) {
		num = 3;
	} else {
		num = routeConfig + i;
	}
	SetRouteConfigRaw((int8_t)num);
}

void SetRouteConfig(uint16_t ad) {
	SetRouteConfigRaw(ad >> 10);
}

int8_t GetRouteConfig(void) {
	return routeConfig;
}

float GetRatio(void) {
	return voice[0].osc[1].ratio;
}

void SetRatioRaw(float rat) {
	for (int i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[1].ratio = rat;
		voice[i].osc[1].freq = voice[i].osc[0].freq * rat;
		voice[i].osc[1].inc = (uint32_t)(16 * voice[i].osc[1].freq / 1250.0f * 65536);
		if (routeConfig == 0) {		// In line
			voice[i].osc[2].freq = voice[i].osc[1].freq * voice[i].osc[2].ratio;
			voice[i].osc[2].inc = (uint32_t)(16 * voice[i].osc[2].freq / 1250.0f * 65536);
		}
	}
}

void SetRatio(uint16_t ad) {
	float rat = 1.0f;
	if (ad < 0x0800) {
		rat /= 1 + ((0x0800 - ad) >> 8);		// [1:9] in step by 1
	} else {
		rat *= 1 + ((ad - 0x0800) >> 8);		// [1:9] in step by 1
	}
	
	SetRatioRaw(rat);
}

float GetRatio2(void) {
	return voice[0].osc[2].ratio;
}

void SetRatio2Raw(float rat) {
	for (int i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[2].ratio = rat;
		if (routeConfig == 0) {
			voice[i].osc[2].freq = voice[i].osc[1].freq * rat;
		} else {
			voice[i].osc[2].freq = voice[i].osc[0].freq * rat;
		}
		voice[i].osc[2].inc = (uint32_t)(16 * voice[i].osc[2].freq / 1250.0f * 65536);
	}
}

void SetRatio2(uint16_t ad) {
	float rat = 1.0f;
	if (ad < 0x0800) {
		rat /= 1 + ((0x0800 - ad) >> 8);		// [1:9] in step by 1
	} else {
		rat *= 1 + ((ad - 0x0800) >> 8);		// [1:9] in step by 1
	}
	
	SetRatio2Raw(rat);
}

float GetIndex(void) {
	return voice[0].osc[1].index;
}

void SetIndexRaw(float ind) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[1].index = ind;
	}
}

void SetIndex(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[1].index = (float)ad * 3;
	}
}

float GetIndex2(void) {
	return voice[0].osc[2].index;
}

void SetIndex2Raw(float ind) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[2].index = ind;
	}
}

void SetIndex2(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].osc[2].index = (float)ad * 3;
	}
}

float GetIndexOffset(void) {
	return index1Offset;
}

void SetIndexOffsetRaw(float ofs) {
	index1Offset = ofs;
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		if (voice[i].osc[1].index + ofs > (float)4096 * 3) {
			voice[i].osc[1].index = 4096 * 3 - ofs;
		}
	}
}

void SetIndexOffset(uint16_t ad) {
	index1Offset = (float)ad * 3;
	SetIndexOffsetRaw(index1Offset);
}

void SetAttack(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].env.a = ad & 0x0FFF;
	}
}

void SetDecay(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].env.d = ad & 0x0FFF;
	}
}

void SetSustain(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].env.s = ad & 0x0FFF;
	}
}

void SetRelease(uint16_t ad) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].env.r = ad & 0x0FFF;
	}
}

uint16_t GetAmpLfoDepth(void) {
	return ampLFODpt;
}

void SetAmpLfoDepth(uint16_t ad) {
	ampLFODpt = ad & 0x0FFF;
}

float GetIndexLFODepth(void) {
	return indexLFODepth;
}

void SetIndexLFODepthRaw(float depth) {
	indexLFODepth = depth;
}

void SetIndexLFODepth(uint16_t ad) {
	indexLFODepth = (float)(ad & 0x0FFF) * 2.0f;		// Not as strong as "index" knob
}

float GetIndexEnvDepth(void) {
	return indexEnvDepth;
}

void SetIndexEnvDepthRaw(float depth) {
	indexEnvDepth = depth;
}

void SetIndexEnvDepth(uint16_t ad) {
	indexEnvDepth = (float)(ad & 0x0FFF) * 2.0f;
}

void TIM4_IRQHandler(void);
void TIM4_IRQHandler(void) {
	// Render all the voices
	int32_t p = 0;
	int32_t p1 = 0;
	int32_t ampLev = 0x0800;
	int32_t mixOut = 0;
	int32_t ampOut = 0;
	float indexLFOOffset = 0.0f;
	float indexEnvOffset = 0.0f;
	
	#ifdef SIMPLE_FILTER
	static int32_t out2;
	static int32_t out3;
	static int32_t out4;
	static int32_t filtAcm1 = 0;
	static int32_t filtAcm2 = 0;
	static int32_t filtAcm3 = 0;
	static int32_t filtAcm4 = 0;
	#endif
	
	GPIOA->BSRR |= GPIO_BSRR_BS8;
	
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		// For a table of 512 samples, increment of 8 generates 1.25kHz
		
		for (uint32_t j = 0; j < NUM_OF_OSCILLATORS; j++) {
			voice[i].osc[j].tblPtr += voice[i].osc[j].inc;
			if (voice[i].osc[j].tblPtr >= (512 << 16)) {
				voice[i].osc[j].tblPtr -= (512 << 16);
			}
		}
		
		// Linear interpolation
		//	uint32_t out = (uint32_t)(sinTbl[voice[0].tblPtr >> 16] + (((voice[0].tblPtr & 0xFFFF) * (sinTbl[(voice[0].tblPtr >> 16) + 1] - sinTbl[voice[0].tblPtr >> 16])) >> 16));
		
		// No interpolation (About S/N ratio, refer to p.442 of Musical Applications of Microprocessors pdf)
		if (voice[i].env.lev) {
			#ifdef SINE_OSCILLATOR
			// ampOut += sinTbl[voice[i].osc[j].tblPtr >> 16];		// sine
				
			switch (routeConfig) {
				case 0:
					p1 = ((int32_t)voice[i].osc[1].tblPtr + (int32_t)((float)sinTbl[voice[i].osc[2].tblPtr >> 16] * voice[i].osc[2].index)) >> 16;
					
					while (p1 < 0) {
						p1 += 512;
					}
					while (p1 > 511) {
						p1 -= 512;
					}
					
					indexLFOOffset = (float)GetLFO(0)->lev * indexLFODepth / 4096.0f;
					indexEnvOffset = (float)voice[i].env.lev * indexEnvDepth / 4096.0f;
					p = ((int32_t)voice[i].osc[0].tblPtr + (int32_t)((float)sinTbl[p1] * (voice[i].osc[1].index + index1Offset + indexLFOOffset + indexEnvOffset))) >> 16;
					while (p < 0) {
						p += 512;
					}
					while (p > 511) {
						p -= 512;
					}
					
					mixOut = sinTbl[p];
					break;
				case 1:
					indexLFOOffset = (float)GetLFO(0)->lev * indexLFODepth / 4096.0f;
					indexEnvOffset = (float)voice[i].env.lev * indexEnvDepth / 4096.0f;
					p = ((int32_t)voice[i].osc[0].tblPtr + (int32_t)((float)sinTbl[(int32_t)voice[i].osc[1].tblPtr >> 16] * (voice[i].osc[1].index + index1Offset + indexLFOOffset + indexEnvOffset)) + (int32_t)((float)sinTbl[(int32_t)voice[i].osc[2].tblPtr >> 16] * (voice[i].osc[2].index + indexLFOOffset + indexEnvOffset))) >> 16;
					while (p < 0) {
						p += 512;
					}
					while (p > 511) {
						p -= 512;
					}
					
					mixOut = sinTbl[p];
					break;
				case 2:
					indexLFOOffset = (float)GetLFO(0)->lev * indexLFODepth / 4096.0f;
					indexEnvOffset = (float)voice[i].env.lev * indexEnvDepth / 4096.0f;
					p = ((int32_t)voice[i].osc[0].tblPtr + (int32_t)((float)sinTbl[(int32_t)voice[i].osc[1].tblPtr >> 16] * (voice[i].osc[1].index + index1Offset + indexLFOOffset + indexEnvOffset))) >> 16;
					while (p < 0) {
						p += 512;
					}
					while (p > 511) {
						p -= 512;
					}
					
					mixOut = sinTbl[p] >> 1;
					mixOut += (int32_t)((float)sinTbl[(int32_t)voice[i].osc[2].tblPtr >> 16] * voice[i].osc[2].index / 24576.0f);
					break;
				case 3:
					mixOut = (int32_t)((float)sinTbl[(int32_t)voice[i].osc[0].tblPtr >> 16] / 3.0f);
					mixOut += (int32_t)((float)sinTbl[(int32_t)voice[i].osc[1].tblPtr >> 16] * voice[i].osc[1].index / 36864.0f);
					mixOut += (int32_t)((float)sinTbl[(int32_t)voice[i].osc[2].tblPtr >> 16] * voice[i].osc[2].index / 36864.0f);
					break;
				default:
					break;
			}
			
			if (i == 0 && scopeBufWIdx < 256) {
				scopeBuf[scopeBufWIdx++] = mixOut;
			}
			if (scopeBufWIdx == 256) {
				NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
				scopeBufWIdx++;
			}
			
			ampOut += (mixOut * voice[i].env.lev) >> 12;
			
			#endif
			
			#ifdef SAW_OSCILLATOR
			ampOut += (voice[i].osc[j].tblPtr >> 13) - 2047;		// saw
			#endif
		}
		
		
		
	}
	
	ampLev += ((GetLFO(0)->lev) * ampLFODpt) >> 12;
	DAC1->DHR12R1 = (uint32_t)((ampOut * ampLev >> 12) + (2048 << 3)) >> 3;
	GPIOA->BSRR |= GPIO_BSRR_BR8;
	TIM4->SR &= ~TIM_SR_UIF;
}

// Process envelopes and lfos (every 1ms)
void TIM1_UP_TIM10_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void) {
	for (uint32_t i = 0; i < NUM_OF_VOICES; i++) {
		voice[i].time++;
		switch (voice[i].env.envPhase) {
			case ENV_ATK:
				if (voice[i].env.a == 0) {
					voice[i].env.lev = 0x0FFF;
					voice[i].time = 0;
					voice[i].env.envPhase = ENV_DCY;
				} else {
					while (voice[i].env.envAccum > 0) {
						voice[i].env.envAccum -= 2 * voice[i].env.a;
						voice[i].env.lev++;
					}
					voice[i].env.envAccum += 2 * 0x0FFF;
					if (voice[i].env.lev >= 0x0FFF) {
						voice[i].env.lev = 0x0FFF;
						voice[i].time = 0;
						voice[i].env.envAccum = 0;
						voice[i].env.envPhase = ENV_DCY;
					}
				}
				break;
			case ENV_DCY:
				if (voice[i].env.d == 0) {
					voice[i].env.lev = (int16_t)(voice[i].env.s & 0x0FFF);
					voice[i].time = 0;
					voice[i].env.envPhase = ENV_SUS;
				} else {
					while (voice[i].env.envAccum > 0) {
						voice[i].env.envAccum -= 2 * voice[i].env.d;
						voice[i].env.lev--;
					}
					voice[i].env.envAccum += 2 * (0x0FFF - voice[i].env.s);
					if (voice[i].env.lev <= voice[i].env.s) {
						voice[i].env.lev = (int16_t)(voice[i].env.s & 0x0FFF);
						voice[i].env.envPhase = ENV_SUS;
					}
				}
				break;
			case ENV_SUS:
				break;
			case ENV_RLS:
				if (voice[i].env.lev > 0) {
					if (voice[i].env.r == 0) {
						voice[i].env.lev = 0;
						VoiceOff(i);	// Deassign note from the voice
					} else {
						while (voice[i].env.envAccum > 0) {
							voice[i].env.envAccum -= 2 * voice[i].env.r;
							voice[i].env.lev--;
						}
						voice[i].env.envAccum += 2 * voice[i].env.levRlsStart;
						if (voice[i].env.lev <= 0) {
							voice[i].env.lev = 0;
							VoiceOff(i);	// Deassign note from the voice
						}
					}
				}
				break;
			case ENV_OFF:
				break;
		}
	}
	
	ProcLFO();
	
	TIM10->SR &= ~TIM_SR_UIF_Msk;
}

env_t* GetEnv(uint8_t num) {
	return &voice[num].env;
}

