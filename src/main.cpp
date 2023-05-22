extern "C" {
	#include "stm32f722xx.h"
	#include "system.h"
	#include "timer.h"
	#include "sw.h"
	#include "led.h"
	#include "ad.h"
	#include "da.h"
	#include "disp.h"
	#include "uart.h"
	#include "voice.h"
	#include "lfo.h"
	#include "view.h"
	#include "draw.h"
	#include "flash.h"
	#include "menu.h"
}

int main(void) {
	ClockInit();
	TimerInit();
	SWInit();
	LEDInit();
	ADInit();
	DAInit();
	UARTInit();
	DispInit();
	VoiceInit();
	FlashInit();
	MenuInit();
	
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 4);
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 3);
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(DMA2_Stream0_IRQn, 5);
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	NVIC_SetPriority(EventHandlerVDI_IRQn, 6);
	NVIC_EnableIRQ(EventHandlerVDI_IRQn);
	NVIC_SetPriority(DMA1_Stream4_IRQn, 3);
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	NVIC_SetPriority(TIM7_IRQn, 3);
	NVIC_EnableIRQ(TIM7_IRQn);
	NVIC_SetPriority(ViewUpdateVDI_IRQn, 8);
	NVIC_EnableIRQ(ViewUpdateVDI_IRQn);
	NVIC_SetPriority(TIM5_IRQn, 9);
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_SetPriority(ParseUSARTRX_VDI_IRQn, 5);
	NVIC_EnableIRQ(ParseUSARTRX_VDI_IRQn);
	
	
	DispOn();
	
	while (1) {
		
	}
}

