#include "fx.h"
#include "da.h"

#define DLY_BUF_LEN 10000	// 0.25 sec

int fxOutL = 0;
int fxOutR = 0;

int32_t curFX = FX_TYPE_DLY;
int32_t delayTime = 10000;	// 0.25 sec
int32_t delayFbk = 256;	// 0-1024 (100%)
int32_t fxLev = 512;	// 0-1024

int32_t delayBufL[DLY_BUF_LEN];
// int32_t delayBufR[DLY_BUF_LEN];
int32_t delayBufPtr = 0;

void FXInit(void) {
//	for (int i = 0; i < DLY_BUF_LEN; i++) {
//		delayBufL[i] = 0;
//		delayBufR[i] = 0;
//	}
}

void InputFX(int32_t theInL, int32_t theInR) {
	delayBufL[delayBufPtr] = theInL;
	// delayBufR[delayBufPtr] = theInR;
	delayBufPtr++;
	if (delayBufPtr >= DLY_BUF_LEN) {
		delayBufPtr = 0;
	}
	
	fxOutL = theInL; fxOutR = theInR;
	ProcFX();
	
	int outL = theInL + (fxOutL * fxLev) / 1024 + 2048;
	int outR = theInR + (fxOutR * fxLev) / 1024 + 2048;
	if (outL < 0) {
		outL = 0;
	} else if (outL >= 4096) {
		outL = 4095;
	}
	if (outR < 0) {
		outR = 0;
	} else if (outR >= 4096) {
		outR = 4095;
	}
	DAOut((uint32_t)outL, (uint32_t)outR);
}

void ProcFX(void) {	// 40kHz
	switch (curFX) {
		case FX_TYPE_DLY:
			{
				int ptrToRead = delayBufPtr - delayTime;
				if (ptrToRead < 0) {
					ptrToRead += DLY_BUF_LEN;
				}
				fxOutL = delayBufL[ptrToRead];
				// fxOutR = delayBufR[ptrToRead];
				fxOutR = delayBufL[ptrToRead];
				delayBufL[delayBufPtr] = (fxOutR * delayFbk) / 1024;
				// delayBufR[delayBufPtr] = (fxOutL * delayFbk) / 1024;
			}
			break;
		case FX_TYPE_CHO:
			break;
		default:
			break;
	}
}

void SetCurFX(fxType_t theFX) {
	curFX = theFX;
}

char* GetCurFXName(void) {
	switch (curFX) {
		case FX_TYPE_DLY:
			return "DELAY";
		case FX_TYPE_CHO:
			return "CHORUS";
		default:
			return "";
	}
}
