/*
 * semperflash_drv.h
 *
 *  Created on: Jun 22, 2025
 *      Author: Li YunFan
 */

#ifndef APOLLO_DRIVERS_SEMPER_FLASH_BSP_INC_SEMPERFLASH_DRV_H_
#define APOLLO_DRIVERS_SEMPER_FLASH_BSP_INC_SEMPERFLASH_DRV_H_
#include "stm32n6xx_hal.h"

#define XSPI_TIMEOUT 1000
#define SEMPER_CMD_READID 0x9F
#define SEMPER_CMD_RDARG 0x65
#define SEMPER_CMD_WRARG 0x71
#define SEMPER_CMD_WRENB 0x06
#define SEMPER_ADDR_CFR1V 0x00800002
#define SEMPER_ADDR_CFR2V 0x00800003
#define SEMPER_ADDR_CFR3V 0x00800004
#define SEMPER_ADDR_CFR4V 0x00800005
#define SEMPER_ADDR_CFR5V 0x00800006
#define SEMPER_CMD_RDSR1 0x05
#define SEMPER_CMD_EN4B 0xB7  // 进入4字节地址模式命令
#define SEMPER_CMD_EX4B 0xB8  // 退出4字节地址模式命令
#define SEMPER_CMD_READ 0x13
#define SEMPER_CMD_READ_FAST 0x0B
#define SEMPER_CMD_READ_8S 0xECEC
#define SEMPER_CMD_RESET_EN 0x66
#define SEMPER_CMD_RESET 0x99
#define SEMPER_CMD_Prog_Page 0x12
#define SEMPER_CMD_Erase_Sector 0x21
#define SEMPER_CMD_CLEAR_PROG_ERR_FLAG 0x82

typedef enum {
    SEMPER_1S_MODE = 0x01,
    SEMPER_8S_MODE = 0x02,
    SEMPER_8D_MODE = 0x03
} SEMPER_Interface_Mode;

typedef enum {
	SEMPER_ADDR_3BYTE = 0x00,
	SEMPER_ADDR_4BYTE = 0x01,
} SEMPER_Addr_Mode;

typedef struct{
	XSPI_HandleTypeDef* xspi_handler;
	SEMPER_Addr_Mode  addr_mode;
	SEMPER_Interface_Mode interface_mode;
} SEMPER_HandleTypeDef;

typedef enum {
	SEMPER_OK = 0x00,
	SEMPER_ERROR = 0x01,
} semper_status_t;

semper_status_t Semper_Read_FlashID(SEMPER_HandleTypeDef*,uint32_t*);
semper_status_t Semper_Flash_Init(SEMPER_HandleTypeDef*,XSPI_HandleTypeDef*);
semper_status_t Semper_8Pins_Mode(SEMPER_HandleTypeDef*);
semper_status_t Semper_1Pin_Mode(SEMPER_HandleTypeDef*);
semper_status_t Semper_Read_Memory_1S(SEMPER_HandleTypeDef*, uint32_t, uint8_t*, uint32_t);
semper_status_t Semper_Read_Fast(SEMPER_HandleTypeDef*, uint32_t, uint8_t*, uint32_t);
semper_status_t Semper_Prog_Page(SEMPER_HandleTypeDef*, uint32_t, uint8_t*, uint32_t);
semper_status_t Semper_Reset(SEMPER_HandleTypeDef*);
semper_status_t Semper_Read_Reg(SEMPER_HandleTypeDef*, uint32_t, uint8_t*);
semper_status_t Semper_Read_StatusReg1(SEMPER_HandleTypeDef*, uint8_t*);
semper_status_t Semper_Write_Reg(SEMPER_HandleTypeDef*, uint32_t, uint8_t);
semper_status_t Semper_Enter_4Byte_Address_Mode(SEMPER_HandleTypeDef*);
semper_status_t Semper_Exit_4Byte_Address_Mode(SEMPER_HandleTypeDef*);
semper_status_t Semper_Poll_RDYBSY(SEMPER_HandleTypeDef*);
semper_status_t Semper_Write_Enable(SEMPER_HandleTypeDef*);
semper_status_t Semper_Erase_Sector(SEMPER_HandleTypeDef*, uint32_t);
semper_status_t Semper_Clear_Prog_Err_Flag(SEMPER_HandleTypeDef*);
semper_status_t Semper_Read_Memory_8S(SEMPER_HandleTypeDef*, uint32_t, uint8_t*, uint32_t);
semper_status_t Semper_Read_Memory(SEMPER_HandleTypeDef*, uint32_t, uint8_t*, uint32_t);
semper_status_t Semper_EnableMemoryMappedMode(SEMPER_HandleTypeDef*);
#endif /* APOLLO_DRIVERS_SEMPER_FLASH_BSP_INC_SEMPERFLASH_DRV_H_ */
