#include "sw.h"
#include "stm32f722xx.h"
#include "event_handler.h"
#include "ad.h"
#include "timer.h"

static uint8_t swBuf[NUM_OF_SWITCHES][SW_BUF_LEN];
static uint8_t swBufWIdx = 0;
static uint8_t swState[NUM_OF_SWITCHES];
static uint8_t encBuf[2][ENC_BUF_LEN];
static uint8_t encPinState[2];
static uint8_t encBufWIdx = 0;
static uint8_t keyBuf[NUM_OF_KEYS][KEY_BUF_LEN];
static uint8_t keyBufWIdx = 0;
uint8_t keyState[NUM_OF_KEYS];

static const key_t keysOnRow1[5] = {KEY_C1, KEY_E1, KEY_G1, KEY_B1, KEY_D2};
static const key_t keysOnRow2[5] = {KEY_D1, KEY_F1, KEY_A1, KEY_C2, KEY_E2};
static const key_t keysOnRow3[7] = {KEY_CS1, KEY_DS1, KEY_FS1, KEY_GS1, KEY_AS1, KEY_CS2, KEY_DS2};

void SWInit(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN
								| RCC_AHB1ENR_GPIOBEN
								| RCC_AHB1ENR_GPIOCEN
								| RCC_AHB1ENR_GPIODEN;
	
	GPIOA->MODER = GPIOA->MODER & ~GPIO_MODER_MODER15_Msk;
	GPIOB->MODER &= ~(GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk
										| GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER8_Msk
										| GPIO_MODER_MODER9_Msk | GPIO_MODER_MODER15_Msk);
//	GPIOB->MODER |= (1U << GPIO_MODER_MODER3_Pos | 1U << GPIO_MODER_MODER4_Pos | 1U << GPIO_MODER_MODER5_Pos
//										| 1U << GPIO_MODER_MODER6_Pos | 1U << GPIO_MODER_MODER7_Pos | 1U << GPIO_MODER_MODER8_Pos
//										| 1U << GPIO_MODER_MODER9_Pos);
	GPIOC->MODER &= ~(GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER10_Msk
										| GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER12_Msk);
	GPIOC->MODER |= (1U << GPIO_MODER_MODER11_Pos | 1U << GPIO_MODER_MODER12_Pos);
	GPIOD->MODER &= ~(GPIO_MODER_MODER2_Msk);
	GPIOD->MODER |= 1U << GPIO_MODER_MODER2_Pos;
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR15_Msk;
	GPIOA->PUPDR |= 1U << GPIO_PUPDR_PUPDR15_Pos;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR15_Msk;
	GPIOB->PUPDR |= (1U << GPIO_PUPDR_PUPDR3_Pos | 1U << GPIO_PUPDR_PUPDR4_Pos | 1U << GPIO_PUPDR_PUPDR5_Pos
									| 1U << GPIO_PUPDR_PUPDR6_Pos | 1U << GPIO_PUPDR_PUPDR7_Pos | 1U << GPIO_PUPDR_PUPDR8_Pos
									| 1U << GPIO_PUPDR_PUPDR9_Pos);
	GPIOB->PUPDR |= 1U << GPIO_PUPDR_PUPDR15_Pos;
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR6_Msk | GPIO_PUPDR_PUPDR7_Msk | GPIO_PUPDR_PUPDR10_Msk);
	GPIOC->PUPDR |= (1U << GPIO_PUPDR_PUPDR6_Pos | 1U << GPIO_PUPDR_PUPDR7_Pos | 1U << GPIO_PUPDR_PUPDR10_Pos);
}

static void SelectKeyRow(uint8_t curRow) {
	GPIOC->BSRR |= (curRow == 1) ? GPIO_BSRR_BR11 : GPIO_BSRR_BS11;
	GPIOC->BSRR |= (curRow == 2) ? GPIO_BSRR_BR12 : GPIO_BSRR_BS12;
	GPIOD->BSRR |= (curRow == 3) ? GPIO_BSRR_BR2 : GPIO_BSRR_BS2;
}

void TIM3_IRQHandler(void);
void TIM3_IRQHandler(void) {
	static uint8_t curRow = 1;
	static uint8_t swDrvState = 0;
	static uint8_t encPinStateBefore[2];
	static int rotState = 0;
	
	switch (swDrvState) {
		case 0:
			SelectKeyRow(curRow);
			swDrvState = 1;
			break;
		case 1:
			swBuf[TACT1][swBufWIdx] = (GPIOA->IDR & GPIO_IDR_ID15_Msk) ? 0U : 1U;
			swBuf[TACT2][swBufWIdx] = (GPIOC->IDR & GPIO_IDR_ID10_Msk) ? 0U : 1U;
			swBuf[ENC_SW][swBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID15_Msk) ? 0U : 1U;
			
			for (uint8_t i = 0; i < NUM_OF_SWITCHES; i++) {
				if (swState[i] == 0) {
					if (swBuf[i][0] + swBuf[i][1] + swBuf[i][2] + swBuf[i][3] == 4) {
						swState[i] = 1;
						DispatchEvent(E_SW_DOWN, i, 0, 0);
					}
				} else {
					if (swBuf[i][0] + swBuf[i][1] + swBuf[i][2] + swBuf[i][3] == 0) {
						swState[i] = 0;
						DispatchEvent(E_SW_UP, i, 0, 0);
					}
				}
			}
			
			swBufWIdx++;
			if (swBufWIdx >= SW_BUF_LEN) {
				swBufWIdx = 0;
			}
			
			encBuf[0][encBufWIdx] = (GPIOC->IDR & GPIO_IDR_ID6_Msk) ? 0U : 1U;		// ENC pin A
			encBuf[1][encBufWIdx] = (GPIOC->IDR & GPIO_IDR_ID7_Msk) ? 0U : 1U;		// ENC pin B
			
			for (uint8_t i = 0; i < 2; i++) {
				int sum = 0;
				encPinStateBefore[i] = encPinState[i];
				if (encPinState[i] == 0) {
					for (int j = 0; j < ENC_BUF_LEN; j++) {
						sum += encBuf[i][j];
					}
					if (sum == ENC_BUF_LEN) {
						encPinState[i] = 1;
					}
				} else {
					for (int j = 0; j < ENC_BUF_LEN; j++) {
						sum += encBuf[i][j];
					}
					if (sum == 0) {
						encPinStateBefore[i] = 1;
						encPinState[i] = 0;
					}
				}
			}
			
			#if 0
			if (encPinStateBefore[1] == 1 && encPinState[1] == 0) {
				if (encPinState[0] == 0) {
					DispatchEvent(E_ENC, 0, 1, 0);
				}
			#if 0
				else {
					DispatchEvent(E_ENC, 0, 1, 0);
				}
			#else
			} else if (encPinStateBefore[0] == 1 && encPinState[0] == 0) {
				if (encPinState[1] == 0) {
					DispatchEvent(E_ENC, 0, -1, 0);
				}
			#endif
			}
			#endif
			
			
			
			if (encPinStateBefore[0] == 0 && encPinStateBefore[1] == 0) {
				if (encPinState[0] == 1 && encPinState[1] == 0) {
					rotState = 1;
				} else if (encPinState[0] == 0 && encPinState[1] == 1) {
					rotState = 2;
				}
			} else if (encPinStateBefore[0] == 1 && encPinStateBefore[1] == 1) {
				if (encPinState[0] == 1 && encPinState[1] == 0) {
					rotState = 3;
				} else if (encPinState[0] == 0 && encPinState[1] == 1) {
					rotState = 4;
				}
			}
			
			if (rotState) {
				switch (rotState) {
					case 1:
						if (encPinState[0] == 1 && encPinState[1] == 1) {
							DispatchEvent(E_ENC, 0, 1, 0);
							
							rotState = 0;
						} else if (encPinState[0] == 0 && encPinState[1] == 0) {
							rotState = 0;
						}
						break;
					case 2:
						if (encPinState[0] == 1 && encPinState[1] == 1) {
							DispatchEvent(E_ENC, 0, -1, 0);
							
							rotState = 0;
						} else if (encPinState[0] == 0 && encPinState[1] == 0) {
							rotState = 0;
						}
						break;
					case 3:
						if (encPinState[0] == 0 && encPinState[1] == 0) {
							DispatchEvent(E_ENC, 0, -1, 0);
							
							rotState = 0;
						} else if (encPinState[0] == 1 && encPinState[1] == 1) {
							rotState = 0;
						}
						break;
					case 4:
						if (encPinState[0] == 0 && encPinState[1] == 0) {
							DispatchEvent(E_ENC, 0, 1, 0);
							
							rotState = 0;
						} else if (encPinState[0] == 1 && encPinState[1] == 1) {
							rotState = 0;
						}
						break;
					default:
						break;
				}
			}
			
			encBufWIdx++;
			if (encBufWIdx >= ENC_BUF_LEN) {
				encBufWIdx = 0;
			}
			
			switch (curRow) {
				case 1:
					keyBuf[KEY_C1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID3_Msk) ? 0U : 1U;
					keyBuf[KEY_E1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID4_Msk) ? 0U : 1U;
					keyBuf[KEY_G1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID5_Msk) ? 0U : 1U;
					keyBuf[KEY_B1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID6_Msk) ? 0U : 1U;
					keyBuf[KEY_D2][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID8_Msk) ? 0U : 1U;
				
					for (int i = 0; i < 5; i++) {
						if (keyState[keysOnRow1[i]] == 0) {
							if (keyBuf[keysOnRow1[i]][0] + keyBuf[keysOnRow1[i]][1] == 2) {
								keyState[keysOnRow1[i]] = 1;
								DispatchEvent(E_KEY_DOWN, keysOnRow1[i], 0, 0);
							}
						} else {
							if (keyBuf[keysOnRow1[i]][0] + keyBuf[keysOnRow1[i]][1] == 0) {
								keyState[keysOnRow1[i]] = 0;
								DispatchEvent(E_KEY_UP, keysOnRow1[i], 0, 0);
							}
						}
					}
					break;
				case 2:
					keyBuf[KEY_D1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID3_Msk) ? 0U : 1U;
					keyBuf[KEY_F1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID4_Msk) ? 0U : 1U;
					keyBuf[KEY_A1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID5_Msk) ? 0U : 1U;
					keyBuf[KEY_C2][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID7_Msk) ? 0U : 1U;
					keyBuf[KEY_E2][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID9_Msk) ? 0U : 1U;
					for (int i = 0; i < 5; i++) {
						if (keyState[keysOnRow2[i]] == 0) {
							if (keyBuf[keysOnRow2[i]][0] + keyBuf[keysOnRow2[i]][1] == 2) {
								keyState[keysOnRow2[i]] = 1;
								DispatchEvent(E_KEY_DOWN, keysOnRow2[i], 0, 0);
							}
						} else {
							if (keyBuf[keysOnRow2[i]][0] + keyBuf[keysOnRow2[i]][1] == 0) {
								keyState[keysOnRow2[i]] = 0;
								DispatchEvent(E_KEY_UP, keysOnRow2[i], 0, 0);
							}
						}
					}
					break;
				case 3:
					keyBuf[KEY_CS1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID3_Msk) ? 0U : 1U;
					keyBuf[KEY_DS1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID4_Msk) ? 0U : 1U;
					keyBuf[KEY_FS1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID5_Msk) ? 0U : 1U;
					keyBuf[KEY_GS1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID6_Msk) ? 0U : 1U;
					keyBuf[KEY_AS1][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID7_Msk) ? 0U : 1U;
					keyBuf[KEY_CS2][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID8_Msk) ? 0U : 1U;
					keyBuf[KEY_DS2][keyBufWIdx] = (GPIOB->IDR & GPIO_IDR_ID9_Msk) ? 0U : 1U;
					for (int i = 0; i < 7; i++) {
						if (keyState[keysOnRow3[i]] == 0) {
							if (keyBuf[keysOnRow3[i]][0] + keyBuf[keysOnRow3[i]][1] == 2) {
								keyState[keysOnRow3[i]] = 1;
								DispatchEvent(E_KEY_DOWN, keysOnRow3[i], 0, 0);
							}
						} else {
							if (keyBuf[keysOnRow3[i]][0] + keyBuf[keysOnRow3[i]][1] == 0) {
								keyState[keysOnRow3[i]] = 0;
								DispatchEvent(E_KEY_UP, keysOnRow3[i], 0, 0);
							}
						}
					}
					break;
				default:
					curRow = 1;
					break;
			}
			
			curRow++;
			if (curRow > 3) {
				curRow = 1;
				keyBufWIdx++;
				if (keyBufWIdx >= KEY_BUF_LEN) {
					keyBufWIdx = 0;
				}
			}
		
			swDrvState = 0;
			break;
		default:
			swDrvState = 0;
			break;
	}
	
	static uint8_t cnt = 0;
	cnt++;
	if (cnt >= 20) {
		ADStart();		// Every 10ms (100Hz)
		cnt = 0;
	}
	
	TIM3->SR &= ~TIM_SR_UIF;
}

uint8_t GetSwState(sw_t num) {
	return swState[num];
}

uint8_t GetKeyState(key_t num) {
	return keyState[num];
}

int IsAnyKeyOn(void) {
	for (int i = 0; i < NUM_OF_KEYS; i++) {
		if (keyState[i] == 1) {
			return 1;
		}
	}
	return 0;
}
