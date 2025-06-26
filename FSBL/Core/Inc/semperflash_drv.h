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

// #define SEMPER_CMD_PRSSR_4_1   0xXX  // 编程安全硅区域命令（需填充具体值）
// #define SEMPER_CMD_RDSSR_4_0   0xXX  // 读取安全硅区域命令
// #define SEMPER_CMD_ERSSR       0xXX  // 擦除安全硅区域命令（需填充具体值）

/* Status Register 1 (STR1V) 位域定义 */
#define SEMPER_STR1V_RDYBSY    (1 << 0)  /* 设备忙状态标志 */
#define SEMPER_STR1V_WRPGEN    (1 << 1)  /* 写/编程使能状态 */
#define SEMPER_STR1V_ERSERR    (1 << 5)  /* 擦除错误标志 */
#define SEMPER_STR1V_PRGERR    (1 << 6)  /* 编程错误标志 */

/* AutoBoot 配置位 */
#define SEMPER_ATBN_STADR_MASK 0xFFFFF800  /* AutoBoot起始地址掩码（22:0位对齐512B页） */
#define SEMPER_ATBN_STDLY_MASK 0x000001FE  /* AutoBoot启动延迟周期配置 */
#define SEMPER_ATBN_ATBTEN     (1 << 0)    /* AutoBoot使能位 */

/* 读取命令 */
/* SEMPER_CMD_READID 已在前面定义为 0x9F */
// #define SEMPER_CMD_RDIDN_4_0   0xXX  /* Octal模式读取设备ID命令（需填充具体值） */
#define SEMPER_CMD_RSFDP_3_0   0x5A  /* SPI模式读取SFDP参数命令 */
// #define SEMPER_CMD_RSFDP_4_0   0xXX  /* Octal模式读取SFDP参数命令 */
#define SEMPER_CMD_RDUID_0_0   0x4B  /* SPI模式读取唯一ID命令 */

/* 配置寄存器位 */
#define SEMPER_CFR2V_ADDR_LEN  (1 << 7)  /* 地址长度模式（0=3字节，1=4字节） */
#define SEMPER_CFR3V_LATENCY_MASK 0xC0   /* 延迟周期配置掩码（位7:6） */

/* 写操作命令 */
/* SEMPER_CMD_WRENB 已在前面定义为 0x06 */
#define SEMPER_CMD_WRDIS_0_0   0x04  /* 写禁止命令 */
// #define SEMPER_CMD_CLPEF_0_0   0xXX  /* 清除编程/擦除错误标志命令（需填充具体值） */
#define SEMPER_CMD_PRPGE_4_1   0x02  /* 页编程命令 */
// #define SEMPER_CMD_PRPPB_4_0   0xXX  /* 编程持久保护位命令 */

/* 擦除命令 */
#define SEMPER_CMD_ER004_4_0   0x20  /* 4KB扇区擦除命令 */
#define SEMPER_CMD_ER256_4_0   0xD8  /* 256KB扇区擦除命令 */
#define SEMPER_CMD_ERCHP_0_0   0x60  /* 整片擦除命令 */

/* 接口模式配置 */
// #define SEMPER_OCTAL_SDR_MODE  0xXX  /* Octal SDR接口模式配置（需填充具体值） */
// #define SEMPER_OCTAL_DDR_MODE  0xXX  /* Octal DDR接口模式配置 */


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

#endif /* INC_SEMPERFLASH_DRV_H_ */
