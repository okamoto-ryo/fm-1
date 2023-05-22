#include "disp.h"
#include "stm32f722xx.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>

#define CMD_BUF_LEN 32

uint8_t dispBuf[1056];
uint8_t curDispPage = 0;
uint8_t dmaHandlerState = 0;

extern const uint8_t font1[128][6];

void DispInit(void) {		// Interrupts are not enabled yet
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN
								| RCC_AHB1ENR_GPIOCEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODER10_Msk | GPIO_MODER_MODER12_Msk | 
										GPIO_MODER_MODER13_Msk | GPIO_MODER_MODER14_Msk);
	GPIOB->MODER |= 2U << GPIO_MODER_MODER10_Pos | 2U << GPIO_MODER_MODER12_Pos | 
									1U << GPIO_MODER_MODER13_Pos | 1U << GPIO_MODER_MODER14_Pos;
	GPIOB->AFR[1] |= 5U << GPIO_AFRH_AFRH2_Pos | 5U << GPIO_AFRH_AFRH4_Pos;		// PB10 is SPI2_SCK and PB12 is SPI2_NSS
	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOC->MODER |= 2U << GPIO_MODER_MODER1_Pos;
	GPIOC->AFR[0] |= 5U << GPIO_AFRL_AFRL1_Pos;		// PC1 is SPI2_MOSI
	
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	SPI2->CR1 |= 5U << SPI_CR1_BR_Pos		// Baud rate is fPCLK / 128
						 | SPI_CR1_MSTR
						 | SPI_CR1_CPOL
						 | SPI_CR1_CPHA;
	SPI2->CR2 |= 15U << SPI_CR2_DS_Pos
						 | SPI_CR2_SSOE						// NSS output enable
						 | SPI_CR2_TXDMAEN;
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	DMA1_Stream4->CR |= 0U << DMA_SxCR_CHSEL_Pos
										| 1U << DMA_SxCR_MSIZE_Pos
										| 1U << DMA_SxCR_PSIZE_Pos
										| DMA_SxCR_MINC
										| 1U << DMA_SxCR_DIR_Pos
										| DMA_SxCR_TCIE;
	DMA1_Stream4->NDTR = 66;
	DMA1_Stream4->PAR = (uint32_t)&(SPI2->DR);
	DMA1_Stream4->M0AR = (uint32_t)&dispBuf;
	
	// GPIOB->BSRR |= GPIO_BSRR_BS12;		// CS high	// Done with SPI2 NSS
	GPIOB->BSRR |= GPIO_BSRR_BR14;		// Pull the reset pin low
	Delay(100);
	GPIOB->BSRR |= GPIO_BSRR_BS14;		// Pull the reset pin high
	GPIOB->BSRR |= GPIO_BSRR_BR13;		// A0 (CD) low = command
	// GPIOB->BSRR |= GPIO_BSRR_BR12;		// CS low
	SPI2->CR1 |= SPI_CR1_SPE;
	SPI2->DR = 0x0210;
	Delay(10000);
	SPI2->DR = 0xAFAF;
	Delay(10000);
	GPIOB->BSRR |= GPIO_BSRR_BS13;		// A0 is read at 8th clock rise so don't toggle too early
	// can send data now
	
//	for (uint8_t i = 0; i < 132; i++) {
//		for (uint8_t j = 0; j < 8; j++) {
//			dispBuf[j * 132 + i] = ((i >> j) & 1U) ? 0xFF : 0U;
//			dispBuf[j * 132 + i] = 0xFF;
//			dispBuf[j * 132 + i] = (i == 0 || i == 127) ? 0xFF : 0U;
//		}
//	}
}

void DispOn(void) {
	// Send Display on command to the display
	StartDispDMA();
}

void StartDispDMA(void) {
	GPIOB->BSRR |= GPIO_BSRR_BS13;
	DMA1->HIFCR |= DMA_HIFCR_CTCIF4
							 | DMA_HIFCR_CHTIF4;
	DMA1_Stream4->CR |= DMA_SxCR_EN;
}

void DMA1_Stream4_IRQHandler(void);
void DMA1_Stream4_IRQHandler(void) {
	DMA1_Stream4->CR &= ~DMA_SxCR_EN;
	dmaHandlerState = 0;
	TIM7->CNT = 0;
	TIM7->ARR = 2000;
	TIM7->CR1 |= TIM_CR1_CEN;
	
	DMA1->HIFCR |= DMA_HIFCR_CTCIF4
							 | DMA_HIFCR_CHTIF4;
}

void TIM7_IRQHandler(void);
void TIM7_IRQHandler(void) {
	TIM7->CR1 &= ~TIM_CR1_CEN;
	
	switch (dmaHandlerState) {
		case 0:
			GPIOB->BSRR |= GPIO_BSRR_BR13;
			curDispPage++;
			if (curDispPage > 7) {
				curDispPage = 0;
			}
			SPI2->DR = 0x0210;
			
			TIM7->CNT = 0;
			TIM7->ARR = 2000;
			TIM7->CR1 |= TIM_CR1_CEN;
			dmaHandlerState = 1;
			break;
		case 1:
			SPI2->DR = 0x10B0 | curDispPage;
			
			TIM7->CNT = 0;
			TIM7->ARR = 2000;
			TIM7->CR1 |= TIM_CR1_CEN;
			dmaHandlerState = 2;
			break;
		case 2:
			DMA1_Stream4->M0AR = (uint32_t)&dispBuf + 132 * curDispPage;
			StartDispDMA();
			dmaHandlerState = 0;
			break;
		default:
			dmaHandlerState = 0;
			break;
	}
	
	TIM7->SR &= ~TIM_SR_UIF_Msk;
}

void SetPixel(uint8_t x, uint8_t y) {
	x = 127 - x; y = 63 - y;
	dispBuf[(x & 1U) ? ((y >> 3) * 132 + x - 1) : ((y >> 3) * 132 + x + 1)] |= 1U << (y % 8);
}

void ClearAll(void) {
	for (uint32_t i = 0; i < 1056; i++) {
		dispBuf[i] = 0U;
	}
}

void PutChar(uint8_t x, uint8_t y, char ltr) {
	if (ltr < 128 && x < 21 && y < 8) {
		uint8_t xPx = 1 + 6 * x;
		for (int i = 0; i < 6; i++) {
			dispBuf[((127 - xPx) & 1U) ? ((7 - y) * 132 + (127 - xPx) - 1) : ((7 - y) * 132 + (127 - xPx) + 1)] = font1[(uint8_t)ltr][i];
			xPx++;
		}
	}
}

void DrawString(uint8_t x, uint8_t y, char* str, uint8_t len) {
	for (int i = 0; i < len; i++) {
		PutChar((uint8_t)(x + i), y, str[i]);
	}
}

void DrawValue(uint8_t x, uint8_t y, const char* str, float val) {
	char buf[32];
	sprintf(buf, str, val);
	DrawString(x, y, buf, (uint8_t)strlen(buf));
}
