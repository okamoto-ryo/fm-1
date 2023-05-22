#include "midi.h"
#include "voice.h"
#include "uart.h"
#include "system.h"
#include "stm32f722xx.h"

uint8_t rxBuf[USART_BUF_LEN];
static uint32_t rxBufWIdx = 0;
static uint32_t rxBufRIdx = 0;

midiStat_t curStat;
parserStage_t parserStage;
midi_t msg;

void USART2_IRQHandler(void);
void USART2_IRQHandler(void) {
	rxBuf[rxBufWIdx++] = (uint8_t)USART2->RDR;
	if (rxBufWIdx >= USART_BUF_LEN) {
		rxBufWIdx = 0;
	}

	NVIC_SetPendingIRQ(ParseUSARTRX_VDI_IRQn);
}

void ParseUSARTRX_VDI_IRQHandler(void);
void ParseUSARTRX_VDI_IRQHandler(void) {
	while (rxBufRIdx != rxBufWIdx) {
		uint8_t dat = rxBuf[rxBufRIdx++];
		if (rxBufRIdx >= USART_BUF_LEN) {
			rxBufRIdx = 0;
		}
		
		if (dat & 0x80) {		// It's a status byte
			switch (dat & 0xF0) {
				case 0x80:
					curStat = STAT_NOTEOFF;
					break;
				case 0x90:
					curStat = STAT_NOTEON;
					break;
				case 0xB0:
					curStat = STAT_CC;
					break;
				case 0xD0:
					curStat = STAT_CH_PRESS;
					break;
				default:
					curStat = STAT_UNKNOWN;
					break;
			}
			parserStage = PARSE_DATA1;
		} else {
			switch (parserStage) {
				case PARSE_DATA1:
					msg.data1 = dat;
					switch (curStat) {
						case STAT_CH_PRESS:
//							SetIndexOffset(msg.data1 << 4U);
							break;
						default:
							break;
					}
					parserStage = PARSE_DATA2;
					break;
				case PARSE_DATA2:
					msg.data2 = dat;
					switch (curStat) {
						case STAT_NOTEOFF:
							NoteOff(msg.data1);
							break;
						case STAT_NOTEON:
							if (msg.data2 == 0) {
								NoteOff(msg.data1);
							} else {
								NoteOn(msg.data1);
							}
							break;
						case STAT_CC:
							switch (msg.data1) {
								case 0x01:
									SetIndexLFODepth(msg.data2 << 5U);
									break;
								default:
									break;
							}
							break;
						default:
							break;
					}
					parserStage = PARSE_DATA1;
					break;
				default:
					break;
			}
		}
	}
	
	NVIC_ClearPendingIRQ(ParseUSARTRX_VDI_IRQn);
	
	NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
}

rxBuf_t GetRXBuf(void) {
	rxBuf_t ret;
	ret.ptr = rxBuf;
	ret.wIdx = rxBufWIdx;
	return ret;
}
