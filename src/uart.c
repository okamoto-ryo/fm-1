#include "uart.h"
#include "stm32f722xx.h"


void UARTInit(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODER3_Msk;
	GPIOA->MODER |= 2U << GPIO_MODER_MODER3_Pos;
	GPIOA->AFR[0] |= 7U << GPIO_AFRL_AFRL3_Pos;
	
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	USART2->CR1 |= USART_CR1_RXNEIE
							 | USART_CR1_RE;
	USART2->CR3 |= USART_CR3_OVRDIS
							 | USART_CR3_DMAR;
	USART2->BRR = 1728;
	
	USART2->CR1 |= USART_CR1_UE;
	
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
//	DMA1_Stream5->CR |= 4U << DMA_SxCR_CHSEL_Pos
//										| 0U << DMA_SxCR_MSIZE_Pos
//										| 0U << DMA_SxCR_PSIZE_Pos
//										| DMA_SxCR_MINC
//										| DMA_SxCR_TCIE;
//	DMA1_Stream5->NDTR = USART_BUF_LEN;
//	DMA1_Stream5->PAR = (uint32_t)(&USART2->RDR);
//	DMA1_Stream5->M0AR = (uint32_t)(&rxBuf);
//	
//	DMA1_Stream5->CR |= DMA_SxCR_EN;
}


//void DMA1_Stream5_IRQHandler(void) {
//	DMA1->HIFCR |= DMA_HIFCR_CTCIF5
//							 | DMA_HIFCR_CHTIF5;
//	DMA1_Stream5->M0AR = (uint32_t)(&rxBuf);
//	DMA1_Stream5->CR |= DMA_SxCR_EN;
//}
