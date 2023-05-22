#include "ad.h"
#include "stm32f722xx.h"
#include "event_handler.h"
#include <stdlib.h>

#include "led.h" // debug

static uint16_t adRaw[4 * AD_BUF_LEN];
uint16_t adFilt[4];
uint16_t adOld[4];
uint8_t adIssuedLast[4];
int32_t adProced[4];

void ADInit(void) {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN
								| RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	
	GPIOA->MODER |= GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk;
	GPIOB->MODER |= GPIO_MODER_MODER0_Msk | GPIO_MODER_MODER1_Msk;
	
	ADC1->CR2 &= ~ADC_CR2_ADON;
	ADC1->CR1 |= ADC_CR1_SCAN;
	ADC1->CR2 |= ADC_CR2_DMA
						 | ADC_CR2_DDS;		// This must be set to make ADC issue continuous DMA requests
	ADC1->SMPR2 |= 3U << ADC_SMPR2_SMP6_Pos | 3U << ADC_SMPR2_SMP7_Pos
							 | 3U << ADC_SMPR2_SMP8_Pos | 3U << ADC_SMPR2_SMP9_Pos;
	ADC1->SQR1 |= 3U << ADC_SQR1_L_Pos;
	ADC1->SQR3 |= 9U << ADC_SQR3_SQ4_Pos | 8U << ADC_SQR3_SQ3_Pos
							| 7U << ADC_SQR3_SQ2_Pos | 6U << ADC_SQR3_SQ1_Pos;
	
	DMA2_Stream0->CR &= ~DMA_SxCR_EN;
	while (DMA2_Stream0->CR & DMA_SxCR_EN_Msk) {}
	DMA2_Stream0->CR |= DMA_SxCR_CIRC
										| 0U << DMA_SxCR_CHSEL_Pos
										| 1U << DMA_SxCR_MSIZE_Pos
										| 1U << DMA_SxCR_PSIZE_Pos
										| DMA_SxCR_MINC
										| DMA_SxCR_TCIE;
	DMA2_Stream0->NDTR = 16U;
	DMA2_Stream0->PAR = (uint32_t)&(ADC1->DR);
	DMA2_Stream0->M0AR = (uint32_t)&(adRaw);
	
	
	ADC1->CR2 |= ADC_CR2_ADON;
		
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < AD_BUF_LEN; j++) {
			adRaw[4 * i + j] = 0;
		}
		adFilt[i] = 0;
		adOld[i] = 0;
		adIssuedLast[i] = 0;
	}
			
}

void ADStart(void) {
	DMA2_Stream0->CR |= DMA_SxCR_EN;
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

inline static int32_t adProc(uint16_t val) {
	if (val < AD_LO) {
		return 0UL;
	} else if (val < AD_HI) {
		return (val - AD_LO) * 0x0FFF / (AD_HI - AD_LO);
	} else {
		return 0x0FFF;
	}
}

void DMA2_Stream0_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void) {
	for (int32_t i = 0; i < 4; i++) {
		adFilt[i] = (adRaw[i] + adRaw[i + 4] + adRaw[i + 8] + adRaw[i + 12]) >> 2;
		if (abs(adFilt[i] - adOld[i]) > AD_THRESHOLD) {
			adProced[i] = adProc(adFilt[i]);
			DispatchEvent(E_AD, i, adProc(adFilt[i]), 0);
			adOld[i] = adFilt[i];
			adIssuedLast[i] = 1;
		} /* else if (adIssuedLast[i]) {
			adProced[i] = adProc(adFilt[i]);
			DispatchEvent(E_AD, i, adProc(adFilt[i]), 0);
			adOld[i] = adFilt[i];
			adIssuedLast[i] = 0;
		} */
	}
	DMA2->LIFCR |= DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0;
}
