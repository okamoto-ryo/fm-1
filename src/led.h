#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

typedef enum {
	LED1G = 0,
	LED1R,
	LED2G,
	LED2R,
	NUM_OF_LEDS
} led_t;

void LEDInit(void);
void SetLED(led_t led, uint8_t offOn);

#endif
