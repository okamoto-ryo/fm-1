#ifndef _SYSTEM_H_
#define _SYSTEM_H_

void ClockInit(void);

#define EventHandlerVDI_IRQn SDMMC1_IRQn
#define EventHandlerVDI_IRQHandler SDMMC1_IRQHandler

#define ViewUpdateVDI_IRQn CAN1_RX0_IRQn
#define ViewUpdateVDI_IRQHandler CAN1_RX0_IRQHandler

#define ParseUSARTRX_VDI_IRQn CAN1_RX1_IRQn
#define ParseUSARTRX_VDI_IRQHandler CAN1_RX1_IRQHandler

#endif
