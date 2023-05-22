#ifndef _fx_h_
#define _fx_h_

#include <stdint.h>

typedef enum {
	FX_TYPE_DLY = 0,
	FX_TYPE_CHO
} fxType_t;

void FXInit(void);
void InputFX(int32_t theInL, int32_t theInR);
void ProcFX(void);
void SetCurFX(fxType_t theFX);
char* GetCurFXName(void);

#endif
