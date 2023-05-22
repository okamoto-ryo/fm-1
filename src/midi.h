#ifndef _MIDI_H_
#define _MIDI_H_

#include <stdint.h>

typedef enum {
	STAT_NOTEOFF = 0x80,
	STAT_NOTEON = 0x90,
	STAT_CC = 0xB0,
	STAT_CH_PRESS = 0xD0,
	STAT_UNKNOWN
} midiStat_t;

typedef enum {
	CC_MOD
} cc_t;

typedef enum {
	PARSE_STATUS,
	PARSE_DATA1,
	PARSE_DATA2
} parserStage_t;

typedef struct {
	uint8_t stat;
	uint8_t data1;
	uint8_t data2;
} midi_t;

typedef struct {
	uint8_t* ptr;
	uint32_t wIdx;
} rxBuf_t;

rxBuf_t GetRXBuf(void);

#endif
