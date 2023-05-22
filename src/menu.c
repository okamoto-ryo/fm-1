#include "menu.h"
#include "stm32f722xx.h"
#include "system.h"
#include "draw.h"
#include "param.h"
#include "disp.h"
#include "page.h"

enum topMenu {
	TOP_MENU_LOAD,
	TOP_MENU_SAVE,
	TOP_MENU_EXIT,
	TOP_MENU_DEBUG
};

// index,					 titleStr
menuItem_t topMenuItems[] = {
	{TOP_MENU_LOAD,  "LOAD"},
	{TOP_MENU_SAVE,  "SAVE"},
	{TOP_MENU_EXIT,  "EXIT"},
	{TOP_MENU_DEBUG, "DEBUG"}
};

menuItem_t progMenuItems[] = {
	{0, "1"},
	{1, "2"},
	{2, "3"},
	{3, "4"},
	{4, "5"},
	{5, "6"},
	{6, "7"},
	{7, "8"},
};

enum debugMenu {
	DEBUG_MENU_MIDI_MNTR,
	DEBUG_MENU_ERASE_ALL,
	DEBUG_MENU_MEMORY_DUMP,
	DEBUG_MENU_EXIT
};

menuItem_t debugMenuItems[] = {
	{DEBUG_MENU_MIDI_MNTR,	"MIDI IN"},
	{DEBUG_MENU_ERASE_ALL,	"ERASE ALL"},
	{DEBUG_MENU_MEMORY_DUMP,"DUMP"},
	{DEBUG_MENU_EXIT,				"EXIT"}
};

enum menus{
	MENU_TOP,
	MENU_SELECT_MEM,
	MENU_DEBUG
};

// menuTitle,		itemsPtr,				layer, length, cursor
menu_t allMenu[] = {
	{"TOP",				topMenuItems,		0, 		4, 			0},
	{"PROGRAM",		progMenuItems,	1, 		8, 			0},
	{"DEBUG",			debugMenuItems,	1,		4,			0},
};

uint8_t curMenu = MENU_TOP;

//int gMenuSize, gMenuItemSize;

//// Didn't work
//uint8_t GetMenuLength(menuItem_t* menu) {
//	gMenuSize = sizeof(*menu);
//	gMenuItemSize = sizeof(menu[0]);
//	return gMenuSize;
//	// return sizeof(*menu) / sizeof(menuItem_t);
//}

void MenuInit(void) {
	for (uint32_t i = 0; i < sizeof(allMenu) / sizeof(menu_t); i++) {
//		allMenu[i].length = GetMenuLength(allMenu[i].itemsPtr);
	}
}

void DrawMenu(void) {
//	for (int i = 0; i < 8; i++) {
//		DrawSqr((int16_t)(6 + i * 15), 27, 10);
//	}
//	for (int i = 0; i < 8; i++) {
//		DrawVertLine((int16_t)(7 + prgmCursor * 15 + i), 28, 35);
//	}
	
	PutChar(0, allMenu[MENU_TOP].cursor, '>');
	for (uint32_t i = 0; i < 4; i++) {
		DrawString(1, i, topMenuItems[i].titleStr, 5);
	}
	
	if (curMenu > 0) {
		PutChar(8 * allMenu[curMenu].layer, allMenu[curMenu].cursor, '>');
		for (uint8_t i = 0; i < allMenu[curMenu].length; i++) {
			DrawString(9, i, allMenu[curMenu].itemsPtr[i].titleStr, 10);
		}
	}
	
//	for (uint8_t i = 0; i < 128; i++) {
//		PutChar(i % 21, i / 21, (char)i);
//	}
}

void ScrollMenu(int32_t val) {
	int cur = allMenu[curMenu].cursor;
	if (val > 0) {
		cur += val;
		if (cur > allMenu[curMenu].length - 1) {
			cur = allMenu[curMenu].length - 1;
		}
	} else if (val < 0) {
		cur += val;
		if (cur < 0) {
			cur = 0;
		}
	}
	allMenu[curMenu].cursor = (uint8_t)cur;
	
	NVIC_SetPendingIRQ(ViewUpdateVDI_IRQn);
}

void EncPressToMenu(void) {
//	WriteProgram(prgmCursor);
	switch (curMenu) {
		case MENU_TOP:
			switch (allMenu[MENU_TOP].cursor) {
				case TOP_MENU_LOAD:
				case TOP_MENU_SAVE:
					curMenu = MENU_SELECT_MEM;
					break;
				case TOP_MENU_DEBUG:
					curMenu = MENU_DEBUG;
					break;
				case TOP_MENU_EXIT:
					GoToPrevPage();
					break;
			}
			break;
		case MENU_SELECT_MEM:
			switch (allMenu[MENU_TOP].cursor) {
				case TOP_MENU_LOAD:
					LoadProgram(allMenu[MENU_SELECT_MEM].cursor);
					break;
				case TOP_MENU_SAVE:
//					WriteProgram(allMenu[MENU_SELECT_MEM].cursor);
					break;
			}
			curMenu = MENU_TOP;
			GoToPrevPage();
			break;
		case MENU_DEBUG:
			switch (allMenu[curMenu].cursor) {
				case DEBUG_MENU_MIDI_MNTR:
					GoToPage(PAGE_MIDI);
					break;
				case DEBUG_MENU_ERASE_ALL:
					EraseAllPrograms();
					curMenu = MENU_TOP;
					GoToPrevPage();
					break;
				case DEBUG_MENU_MEMORY_DUMP:
					break;
				case DEBUG_MENU_EXIT:
					curMenu = MENU_TOP;
					break;
			}
			break;
	}
}
