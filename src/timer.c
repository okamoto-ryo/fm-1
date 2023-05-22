#include "timer.h"
#include "stm32f722xx.h"

void TimerInit(void) {
	// TIM4 for DAC
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CR2 |= 2U << TIM_CR2_MMS_Pos;		// Output Update event to TRGO
	TIM4->DIER |= TIM_DIER_UIE;
	TIM4->ARR = 2700;		// TIM4 interrupt freq is 108M / 2700 = 40k
	
	TIM4->CR1 |= TIM_CR1_CEN;
	
	// TIM3 for SW and ADC
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->PSC = 9;
	TIM3->ARR = 3600;	// TIM3 interrupt freq is 108M / (9 + 1) / 3600 = 3k
	
	TIM3->CR1 |= TIM_CR1_CEN;
	
	// TIM10 for envelope
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	TIM10->DIER |= TIM_DIER_UIE;
	TIM10->PSC = 9;
	TIM10->ARR = 10800;	// TIM10 interrupt freq is 108M / (9 + 1) / 10800 = 1k
	
	TIM10->CR1 |= TIM_CR1_CEN;
	
	// TIM6 for delay function
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->PSC = 53;		// TIM6 counter rate is 108M / (53 + 1) = 2MHz
	TIM6->ARR = 65535;
	
	// TIM7 for display cycle
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->PSC = 53;		// TIM7 counter rate is 54M / (53 + 1) = 1MHz
	TIM7->ARR = 10000;
	
	// TIM5 for scope update
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	TIM5->DIER |= TIM_DIER_UIE;
	TIM5->PSC = 53999;
	TIM5->ARR = 1000;		// TIM5 interrupt freq is 108M / (53999 + 1) / 50 = 40
	TIM5->CR1 |= TIM_CR1_CEN;
}

void Delay(uint16_t us) {
	TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM6->CNT = 0;
	TIM6->CR1 |= TIM_CR1_CEN;
	while (TIM6->CNT <= us) {
	}
	TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM6->CNT = 0;
}

