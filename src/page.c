#include "page.h"
#include "stm32f722xx.h"
#include "system.h"
#include "led.h"
#include "voice.h"
#include "lfo.h"
#include "menu.h"

static void (*potMtrx[4][4])(uint16_t ad) = {
	{ SetRouteConfig,	0,							0,								0									},
	{ SetRatio,			SetIndex,				SetRatio2,				SetIndex2					},
	{ SetAttack,		SetDecay,				SetSustain,				SetRelease				},
	{ SetLFOFreq,		SetAmpLfoDepth,	SetIndexLFODepth,	SetIndexEnvDepth 	},
};

page_t curPage = PAGE_OSC;
page_t pageToGoBack = PAGE_OSC;

void PageADEventHandler(uint8_t num, uint16_t ad) {
	if (*potMtrx[curPage][num]) {
		(*potMtrx[curPage][num])(ad);
		NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
	}
}

void ScrollPage(int val) {
	int pageInt = (int)curPage;
	pageInt += val;
	if (pageInt < 0) {
		pageInt = 0;
	} else if (pageInt > 3) {
		pageInt = 3;
	}
	curPage = (page_t)pageInt;
	
	switch (curPage) {
		case PAGE_OSC:
			break;
		case PAGE_ENV:
			break;
		case PAGE_LFO:
			break;
		default:
			break;
	}
	
	NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
}

page_t GetCurPage(void) {
	return curPage;
}

void EncEventToPage(int32_t val) {
	switch (curPage) {
		case PAGE_MENU:
			ScrollMenu(val);
			break;
		default:
			//IncRouteConfig(val);
			ScrollPage(val);
			break;
	}
}

void EncPressToPage(void) {
	switch (curPage) {
		case PAGE_MENU:
			EncPressToMenu();
//			curPage = pageToGoBack;
			NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
			break;
		case PAGE_MIDI:
			pageToGoBack = PAGE_OSC;
			curPage = PAGE_MENU;
			NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
			break;
		default:
			pageToGoBack = curPage;
			curPage = PAGE_MENU;
			NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
			break;
	}
}

void GoToPrevPage(void) {
	curPage = pageToGoBack;
}

void GoToPage(page_t page) {
	curPage = page;
}

void DrawPages(void) {
	switch (curPage) {
		case PAGE_STRUCTURE:
			DrawPageStrct();
			break;
		case PAGE_OSC:
			DrawPageOsc();
			break;
		case PAGE_ENV:
			DrawPageEnv();
			break;
		case PAGE_LFO:
			DrawPageLFO();
			break;
		case PAGE_MIDI:
			DrawPageMIDI();
			break;
		default:
			break;
	}
}
