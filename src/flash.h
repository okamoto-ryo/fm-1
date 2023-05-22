#ifndef _FLASH_H_
#define _FLASH_H_

#include <stdint.h>

#define KEY1 (0x45670123)
#define KEY2 (0xCDEF89AB)
#define SECTOR7_BASE (0x08060000)


void FlashInit(void);
void EraseSector7(void);
int WriteWord(uint32_t offset, uint32_t data);

#endif
