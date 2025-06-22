/*
 * semperflash_drv.h
 *
 *  Created on: Jun 22, 2025
 *      Author: Li YunFan
 */

#ifndef INC_SEMPERFLASH_DRV_H_
#define INC_SEMPERFLASH_DRV_H_
#include "stm32n6xx_hal.h"

#define XSPI_TIMEOUT 1000
#define SEMPER_READID_CMD 0x9F;

uint32_t Semper_Read_FlashID(XSPI_HandleTypeDef *hxspi);
uint32_t Semper_Flash_Init(XSPI_HandleTypeDef *hxspi);
uint32_t Semper_8Pins_Mode(XSPI_HandleTypeDef *hxspi);

#endif /* INC_SEMPERFLASH_DRV_H_ */
