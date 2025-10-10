/*
 * semperflash_test.h
 *
 *  Created on: Jun 28, 2025
 *      Author: DELL
 */

#ifndef INC_SEMPERFLASH_TEST_H_
#define INC_SEMPERFLASH_TEST_H_

//#define SEMPER_8S_TEST

#include "../../Drivers/Apollo_Drivers/Semper_Flash_BSP/Inc/semperflash_drv.h"

void Semper_Test_Init(SEMPER_HandleTypeDef* flash1, XSPI_HandleTypeDef* hxspi);
void Semper_Read_Reg_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Write_Reg_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Read_Memory_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Write_Enable_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Prog_Page_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Clear_Prog_Err_Flag_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Erase_Sector_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Switch_Mode_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Read_ID_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Memory_Mapped_Mode_Test(SEMPER_HandleTypeDef* flash1);
#endif /* INC_SEMPERFLASH_TEST_H_ */
