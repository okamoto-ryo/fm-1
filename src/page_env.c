#include "page.h"
#include "stm32f722xx.h"
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "disp.h"
#include "voice.h"

void DrawPageEnv(void) {
	env_t* pEnv;
	pEnv = GetEnv(0);
	DrawLine(0, 63, pEnv->a >> 7, 0);
	DrawLine(pEnv->a >> 7, 0, (pEnv->a >> 7) + (pEnv->d >> 7), 63 - (pEnv->s >> 6));
	DrawLine((pEnv->a >> 7) + (pEnv->d >> 7), 63 - (pEnv->s >> 6), 127 - (pEnv->r >> 7), 63 - (pEnv->s >> 6));
	DrawLine(127 - (pEnv->r >> 7), 63 - (pEnv->s >> 6), 127, 63);
}
