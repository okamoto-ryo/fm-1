#include "view.h"
#include "stm32f722xx.h"
#include "system.h"
#include "sw.h"
#include "led.h"
#include "page.h"
#include "disp.h"
#include "draw.h"
#include "voice.h"
#include "scope.h"
#include "menu.h"

uint8_t viewState = 0;

static void DrawRouting(void) {
	switch (GetRouteConfig()) {
		case 0:
			DrawSqr(58, 8, 11);
			DrawVertLine(63, 19, 26);
			DrawSqr(58, 26, 11);
			DrawVertLine(63, 37, 44);
			DrawSqr(58, 44, 11);
			break;
		case 1:
			DrawSqr(50, 17, 11);
			DrawSqr(68, 17, 11);
			DrawVertLine(55, 28, 31);
			DrawVertLine(73, 28, 31);
			DrawHorizLine(55, 73, 31);
			DrawVertLine(64, 32, 35);
			DrawSqr(59, 35, 11);
			break;
		case 2:
			DrawSqr(50, 17, 11);
			DrawVertLine(55, 28, 35);
			DrawSqr(50, 35, 11);
			DrawSqr(68, 35, 11);
			break;
		case 3:
			DrawSqr(41, 26, 11);
			DrawSqr(59, 26, 11);
			DrawSqr(77, 26, 11);
			break;
		default:
			break;
	}
}

void ArmDrawRouting(void) {
	viewState = 0;
}

void ViewUpdateVDI_IRQHandler(void);
void ViewUpdateVDI_IRQHandler(void) {
	#ifdef DISP_PAGES
	
	env_t* pEnv;
	ClearAll();
	
	switch (GetCurPage()) {
		case PAGE_OSC:
			DrawLine(63, 31, 73, 0);
			DrawLine(63, 31, 127, 21);
			DrawLine(63, 31, 127, 41);
			DrawLine(63, 31, 73, 63);
			DrawLine(63, 31, 53, 63);
			DrawLine(63, 31, 0, 41);
			DrawLine(63, 31, 0, 21);
			DrawLine(63, 31, 53, 0);
			
			break;
		case PAGE_ENV:
			pEnv = GetEnv(0);
			DrawLine(0, 63, pEnv->a >> 7, 0);
			DrawLine(pEnv->a >> 7, 0, (pEnv->a >> 7) + (pEnv->d >> 7), 63 - (pEnv->s >> 6));
			DrawLine((pEnv->a >> 7) + (pEnv->d >> 7), 63 - (pEnv->s >> 6), 127 - (pEnv->r >> 7), 63 - (pEnv->s >> 6));
			DrawLine(127 - (pEnv->r >> 7), 63 - (pEnv->s >> 6), 127, 63);
			break;
		default:
			
			break;
	}
	#endif
	
	ClearAll();
	if (GetCurPage() == PAGE_MENU) {
		DrawMenu();
	} else {
		#if 1
		switch (IsAnyKeyOn()) {
			case 0:
				DrawPages();
				break;
			case 1:
				DrawScope();
				break;
		}
		#else
		DrawPages();
		#endif
	}

//	DrawLine(0, 0, 127, 63);
//	DrawLine(0, 63, 63, 0);
//	DrawLine(63, 63, 127, 0);
	
	NVIC_ClearPendingIRQ(ViewUpdateVDI_IRQn);
}

