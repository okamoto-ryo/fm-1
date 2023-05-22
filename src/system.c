#include "system.h"
#include "stm32f722xx.h"

void ClockInit(void) {
	RCC->CR &= ~RCC_CR_PLLON_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLM_Msk);
	RCC->PLLCFGR |= 216 << RCC_PLLCFGR_PLLN_Pos		// PLL VCO output is 2 * 216 = 432MHz
								| 12 << RCC_PLLCFGR_PLLM_Pos;		// PLL VCO input is 2MHz
	// PLLP is 2 (0b00), so PLL out to SYSCLK is 432 / 2 = 216MHz
	
	RCC->CFGR |= 4U << RCC_CFGR_PPRE2_Pos			// APB2 is 108MHz
						 | 5U << RCC_CFGR_PPRE1_Pos;		// APB1 is 54MHz
	
	RCC->CR |= RCC_CR_HSEON
					 | RCC_CR_PLLON;
	while (!((RCC->CR & RCC_CR_HSERDY_Msk) && (RCC->CR & RCC_CR_PLLRDY_Msk))) {
	}
	
	FLASH->ACR = FLASH_ACR_LATENCY_7WS;
	while ((FLASH->ACR & FLASH_ACR_LATENCY_Msk) != FLASH_ACR_LATENCY_7WS) {
	}
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL) {
	}
	
	RCC->CR &= ~RCC_CR_HSION;
	
	SystemCoreClockUpdate();
}
