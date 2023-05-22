#include "ui.h"
#include "stm32f722xx.h"

uint16_t AdMezzanine(uint16_t ad, uint8_t num) {
	uint16_t ret = ad;
	switch (num) {
		case 0:
			ret = ad;
			break;
		case 1:
			if (ad < 0x0740) {		// Mezzanine is about 10% of full rotation
				ret = 0x0800 * ad / 0x073F;
			} else if (ad < 0x08C0) {
				ret = 0x0800;
			} else {
				ret = 0x0800 + 0x0800 * (ad - 0x08C0) / 0x0740;
			}
			break;
		default:
			ret = ad;
			break;
	}
	return ret;
}
