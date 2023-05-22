#include "event_handler.h"
#include "system.h"
#include "stm32f722xx.h"
#include "sw.h"
#include "voice.h"
#include "page.h"

static eventPkt_t eventFIFO[EVENT_FIFO_LEN];
static uint8_t eFWIdx = 0;
static uint8_t eFRIdx = 0;		// No overflow as EVENT_FIFO_LEN is 256

static int32_t encAccum = 0;

void DispatchEvent(status_t status, int32_t data1, int32_t data2, int32_t data3) {
	eventPkt_t pkt;
	pkt.status = status;
	pkt.data1 = data1;
	pkt.data2 = data2;
	pkt.data3 = data3;
	
	eventFIFO[eFWIdx++] = pkt;
	NVIC_SetPendingIRQ(EventHandlerVDI_IRQn);
}

void EventHandlerVDI_IRQHandler(void);
void EventHandlerVDI_IRQHandler(void) {
	eventPkt_t pkt;
	uint8_t num = 0;
	if (eFRIdx != eFWIdx) {
		pkt = eventFIFO[eFRIdx++];
		switch (pkt.status) {
			case E_SW_DOWN:
				switch ((sw_t)pkt.data1) {
					case TACT1:
						
						break;
					case TACT2:
						
//						NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
						break;
					case ENC_SW:
						EncPressToPage();
						break;
					default:
						break;
				}
				break;
			case E_KEY_DOWN:
				num = (uint8_t)pkt.data1 + 0x3C;
				NoteOn(num);
				break;
			case E_KEY_UP:
				num = (uint8_t)pkt.data1 + 0x3C;
				NoteOff(num);
				NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
				break;
			case E_ENC:
				encAccum += pkt.data2;
				if (encAccum == 2 || encAccum == -2) {
					EncEventToPage((int32_t)pkt.data2);
					encAccum = 0;
				}
				break;
			case E_AD:
				PageADEventHandler((uint8_t)pkt.data1, (uint16_t)pkt.data2);
				break;
			default:
				break;
		}
	}
	
//	NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
	
	if (eFRIdx != eFWIdx) {
		NVIC_SetPendingIRQ(EventHandlerVDI_IRQn);
	}
}
