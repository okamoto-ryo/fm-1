#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include <stdint.h>

#define EVENT_FIFO_LEN 256

typedef enum {
	E_SW_DOWN,
	E_SW_UP,
	E_KEY_DOWN,
	E_KEY_UP,
	E_ENC,
	E_AD,
	E_EXT_TRIG,
} status_t;

typedef struct {
	status_t status;
	int32_t data1;
	int32_t data2;
	int32_t data3;
} eventPkt_t;

void DispatchEvent(status_t status, int32_t data1, int32_t data2, int32_t data3);

#endif
