#include "led.h"
#include "stm32f722xx.h"

void LEDInit(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN
								| RCC_AHB1ENR_GPIOCEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER12_Msk);
	GPIOA->MODER |= 1U << GPIO_MODER_MODER8_Pos | 1U << GPIO_MODER_MODER11_Pos | 1U << GPIO_MODER_MODER12_Pos;
	GPIOC->MODER &= ~GPIO_MODER_MODER9_Msk;
	GPIOC->MODER |= 1U << GPIO_MODER_MODER9_Pos;
	
	for (int i = 0; i < NUM_OF_LEDS; i++) {
		SetLED((led_t)i, 0);
	}
}

void SetLED(led_t led, uint8_t offOn) {
	switch (led) {
		case LED1G:
			GPIOA->BSRR |= offOn ? GPIO_BSRR_BS8 : GPIO_BSRR_BR8;
			break;
		case LED1R:
			GPIOC->BSRR |= offOn ? GPIO_BSRR_BS9 : GPIO_BSRR_BR9;
			break;
		case LED2G:
			GPIOA->BSRR |= offOn ? GPIO_BSRR_BS12 : GPIO_BSRR_BR12;
			break;
		case LED2R:
			GPIOA->BSRR |= offOn ? GPIO_BSRR_BS11 : GPIO_BSRR_BR11;
			break;
		default:
			break;
	}
}
