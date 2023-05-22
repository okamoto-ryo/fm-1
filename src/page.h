#ifndef _PAGE_H_
#define _PAGE_H_

#include "voice.h"
#include <stdint.h>

typedef enum {
	PAGE_STRUCTURE,
	PAGE_OSC,
	PAGE_ENV,
	PAGE_LFO,
	PAGE_MENU,
	PAGE_MIDI,
	NUM_OF_PAGES
} page_t;

void PageADEventHandler(uint8_t num, uint16_t ad);
void ScrollPage(int val);
page_t GetCurPage(void);
void EncEventToPage(int32_t val);
void EncPressToPage(void);
void GoToPrevPage(void);
void GoToPage(page_t page);

void DrawPages(void);
void DrawPageStrct(void);
void DrawPageOsc(void);
void DrawPageEnv(void);
void DrawPageLFO(void);
void DrawPageMIDI(void);

#endif
