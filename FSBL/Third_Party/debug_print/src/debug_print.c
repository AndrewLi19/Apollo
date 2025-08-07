/*
 * debug_print.c
 *
 *  Created on: Mar 6, 2025
 *      Author: DELL
 */
#include "debug_print.h"

void debug_print_init(void)
{
	*(__IO uint32_t*)(SWO_CODR_ADDR) = ((SystemCoreClock / DEBUGPRINT_RATE) - 1); // SWO_CODR
}

int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}

void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    __DSB();
    __ISB();
}

uint32_t DWT_GetCycles(void) {
    return DWT->CYCCNT;
}

int __io_putchar(int ch)
{
  ITM_SendChar(ch);
  return ch;
}

