/*
 * semperflash_test.h
 *
 *  Created on: Jun 28, 2025
 *      Author: DELL
 */

#ifndef INC_SEMPERFLASH_TEST_H_
#define INC_SEMPERFLASH_TEST_H_

#include "semperflash_drv.h"

void Semper_Read_Reg_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Write_Reg_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Read_Memory_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Write_Enable_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Prog_Page_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Clear_Prog_Err_Flag_Test(SEMPER_HandleTypeDef* flash1);
void Semper_Erase_Sector_Test(SEMPER_HandleTypeDef* flash1);

#endif /* INC_SEMPERFLASH_TEST_H_ */
