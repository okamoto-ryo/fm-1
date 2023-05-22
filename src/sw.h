#ifndef _SW_H_
#define _SW_H_

#include <stdint.h>

#define SW_BUF_LEN 4
#define ENC_BUF_LEN 8
#define KEY_BUF_LEN 2

typedef enum {
	TACT1 = 0,
	TACT2,
	ENC_SW,
	NUM_OF_SWITCHES
} sw_t;


typedef enum {
	KEY_C1,
	KEY_CS1,
	KEY_D1,
	KEY_DS1,
	KEY_E1,
	KEY_F1,
	KEY_FS1,
	KEY_G1,
	KEY_GS1,
	KEY_A1,
	KEY_AS1,
	KEY_B1,
	KEY_C2,
	KEY_CS2,
	KEY_D2,
	KEY_DS2,
	KEY_E2,
	NUM_OF_KEYS
} key_t;

void SWInit(void);
uint8_t GetSwState(sw_t num);
uint8_t GetKeyState(key_t num);
int IsAnyKeyOn(void);

#endif
