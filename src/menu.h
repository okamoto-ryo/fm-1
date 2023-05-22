#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>





typedef struct {
	uint8_t index;
	char titleStr[10];
} menuItem_t;

typedef struct {
	char menuTitle[10];
	menuItem_t* itemsPtr;
	int8_t layer;
	uint8_t length;
	uint8_t cursor;
} menu_t;

void MenuInit(void);
void DrawMenu(void);
void ScrollMenu(int32_t val);
void EncPressToMenu(void);

#endif
