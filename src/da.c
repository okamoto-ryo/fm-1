#include "da.h"
#include "stm32f722xx.h"

void DAInit(void) {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER |= GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk;
	
	DAC1->CR |= 5U << DAC_CR_TSEL2_Pos			// Triggered by TIM4 TRGO
						| DAC_CR_TEN2
						| 5U << DAC_CR_TSEL1_Pos
						| DAC_CR_TEN1;
	
	DAC1->CR |= DAC_CR_EN2 | DAC_CR_EN1;
	
}

