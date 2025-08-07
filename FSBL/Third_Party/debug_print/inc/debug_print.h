/*
 * debugprint.h
 *
 *  Created on: Jan 25, 2025
 *      Author: Li YunFan
 */
#ifndef INC_DEBUGPRINT_H_
#define INC_DEBUGPRINT_H_

#include "stm32n657xx.h"
#include <stdio.h>

#define DEBUGPRINT_RATE 8000000
#define SWO_CODR_ADDR 0xE008A010

void debug_print_init(void);
int _write(int file, char *ptr, int len);
void DWT_Init(void);
uint32_t DWT_GetCycles(void);
int __io_putchar(int);

#endif /* INC_DEBUGPRINT_H_ */
