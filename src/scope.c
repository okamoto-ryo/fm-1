#include "scope.h"
#include "disp.h"
#include "stm32f722xx.h"

int32_t scopeBuf[256];
extern uint32_t scopeBufWIdx;

void DrawScope(void) {
	for (int i = 0; i < 128; i++) {
		static int valBefore = 0;
		int val = (scopeBuf[i] + 2048) >> 6;
//		if (val - valBefore > 0) {
//			for (int v = valBefore; v < val; v++) {
//				if (v < ((val + valBefore) >> 1)) {
//					SetPixel((uint8_t)i - 1, (uint8_t)(64 - v));
//				} else {
//					SetPixel((uint8_t)i, (uint8_t)(64 - v));
//				}
//			}
//		} else {
//			for (int v = val; v < valBefore; v++) {
//				if (v > ((val + valBefore) >> 1)) {
//					SetPixel((uint8_t)i - 1, (uint8_t)(64 - v));
//				} else {
//					SetPixel((uint8_t)i, (uint8_t)(64 - v));
//				}
//			}
//		}
		if (i > 0) {
			if (val - valBefore > 1) {
				for (int v = valBefore + 1; v < val; v++) {
					SetPixel((uint8_t)i, (uint8_t)(64 - v));
				}
			} else if (valBefore - val > 1) {
				for (int v = val + 1; v < valBefore; v++) {
					SetPixel((uint8_t)i, (uint8_t)(64 - v));
				}
			}
		}
		SetPixel((uint8_t)i, (uint8_t)(64 - val));
		
		valBefore = val;
	}
}

void TIM5_IRQHandler(void);
void TIM5_IRQHandler(void) {
	scopeBufWIdx = 0;
	TIM5->SR &= ~TIM_SR_UIF;
}
