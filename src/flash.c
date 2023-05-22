#include "flash.h"
#include "stm32f722xx.h"


static void UnlockFlash(void) {
	if (FLASH->CR & FLASH_CR_LOCK) {
		FLASH->KEYR = KEY1;
		FLASH->KEYR = KEY2;	// Unlock flash write
	}
}

void FlashInit(void) {
	UnlockFlash();
	
#ifdef ERASE_IN_INIT
	EraseSector7();
#endif
	
	FLASH->CR |= FLASH_CR_ERRIE;
	
	if (FLASH->SR & FLASH_SR_ERSERR) {
		FLASH->SR |= FLASH_SR_ERSERR;
	}
	if (FLASH->SR & FLASH_SR_PGPERR) {
		FLASH->SR |= FLASH_SR_PGPERR;
	}
	if (FLASH->SR & FLASH_SR_PGAERR) {
		FLASH->SR |= FLASH_SR_PGAERR;
	}
	if (FLASH->SR & FLASH_SR_WRPERR) {
		FLASH->SR |= FLASH_SR_WRPERR;
	}
	if (FLASH->SR & FLASH_SR_OPERR) {
		FLASH->SR |= FLASH_SR_OPERR;
	}
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR |= FLASH_SR_EOP;
	}
}

void EraseSector7(void) {
//	int timeoutCnt = 0x044AA200;		// 72 million
	while (FLASH->SR & FLASH_SR_BSY_Msk) {
//		if (--timeoutCnt < 0) {
//			return;
//		}
	}
	UnlockFlash();
	
	FLASH->CR &= FLASH_CR_PSIZE_Msk;
	FLASH->CR |= 2U << FLASH_CR_PSIZE_Pos;		// 32-bit write
	FLASH->CR &= ~FLASH_CR_SNB_Msk;
	FLASH->CR |= 7U << FLASH_CR_SNB_Pos;	// Sector 7
	FLASH->CR |= FLASH_CR_SER;
	FLASH->CR |= FLASH_CR_STRT;
	
//	timeoutCnt = 0x044AA200;		// 72 million
	while (FLASH->SR & FLASH_SR_BSY_Msk) {
//		if (--timeoutCnt < 0) {
//			return;
//		}
	}
	FLASH->CR &= ~FLASH_CR_SER;
	FLASH->CR |= FLASH_CR_LOCK;
}

int WriteWord(uint32_t offset, uint32_t data) {
	uint32_t* ptr = (uint32_t*)(SECTOR7_BASE + offset);
	
	while (FLASH->SR & FLASH_SR_BSY_Msk) {
	}
	if (FLASH->SR & FLASH_SR_ERSERR) {
		FLASH->SR |= FLASH_SR_ERSERR;
	}
	if (FLASH->SR & FLASH_SR_PGPERR) {
		FLASH->SR |= FLASH_SR_PGPERR;
	}
	if (FLASH->SR & FLASH_SR_PGAERR) {
		FLASH->SR |= FLASH_SR_PGAERR;
	}
	if (FLASH->SR & FLASH_SR_WRPERR) {
		FLASH->SR |= FLASH_SR_WRPERR;
	}
	if (FLASH->SR & FLASH_SR_OPERR) {
		FLASH->SR |= FLASH_SR_OPERR;
	}
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR |= FLASH_SR_EOP;
	}
	
	UnlockFlash();	// Unlocking unlocked CR causes fault!
	
	FLASH->CR &= ~FLASH_CR_PSIZE_Msk;
	FLASH->CR |= 2U << FLASH_CR_PSIZE_Pos;
	FLASH->CR |= FLASH_CR_PG;
	// Something goes wrong and the written area is erased immediately after the write. 
	// Just locking before the write solved this.
	FLASH->CR |= FLASH_CR_LOCK;
	
	*ptr = data;		// if stopped here, the write will be successful
	
	while (FLASH->SR & FLASH_SR_BSY_Msk) {
	}
	//	FLASH->CR &= ~FLASH_CR_PG;		// flash_cr is locked
	
	return 1;
}
