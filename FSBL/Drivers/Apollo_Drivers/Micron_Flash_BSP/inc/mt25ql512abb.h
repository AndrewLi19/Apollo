/**
  ******************************************************************************
  * @file    mt25ql512abb.h
  * @modify  MCD Application Team
  * @brief   This file contains all the description of the
  *          MT25QU02 QSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MT25QU02_H
#define MT25QU02_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mt25ql512abb_conf.h"
#include "stm32n6xx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup MT25QU02
  * @{
  */
/** @defgroup MT25QU02_Exported_Constants MT25QU02 Exported Constants
  * @{
  */

/**
  * @brief  MT25QU02 Size configuration
  */
#define MT25QU02_SECTOR_64K                   (uint32_t)(64 * 1024)        /* 1024 sectors of 64KBytes     */
#define MT25QU02_SUBSECTOR_32K                (uint32_t)(32 * 1024)        /* 2048 subsectors of 32KBytes  */
#define MT25QU02_SUBSECTOR_4K                 (uint32_t)(4  * 1024)        /* 16384 subsectors of 4KBytes  */

#define MT25QU02_FLASH_SIZE                   (uint32_t)(512*1024*1024/8)  /* 512 Mbits => 64MBytes        */
#define MT25QU02_PAGE_SIZE                    (uint32_t)256                /* 262144 pages of 256 Bytes    */

/**
  * @brief  MT25QU02 Timing configuration
  */

#define MT25QU02_BULK_ERASE_MAX_TIME          460000U
#define MT25QU02_SECTOR_ERASE_MAX_TIME        1000U
#define MT25QU02_SUBSECTOR_32K_ERASE_MAX_TIME 1000U
#define MT25QU02_SUBSECTOR_4K_ERASE_MAX_TIME  400U

#define MT25QU02_RESET_MAX_TIME               1000U                 /* when SWreset during erase operation */

#define MT25QU02_AUTOPOLLING_INTERVAL_TIME    0x10U

/**
  * @brief  MT25QU02 Error codes
  */
#define MT25QU02_OK                           (0)
#define MT25QU02_ERROR                        (-1)

/******************************************************************************
  * @brief  MT25QU02 Commands
  ****************************************************************************/
/***** Software RESET Operations *********************************************/
#define MT25QU02_RESET_ENABLE_CMD                     0x66U   /*!< Reset Enable                                            */
#define MT25QU02_RESET_MEMORY_CMD                     0x99U   /*!< Reset Memory                                            */

/***** READ ID Operations ****************************************************/
#define MT25QU02_READ_ID_CMD                          0x9EU   /*!< Read IDentification                                     */
#define MT25QU02_READ_ID_CMD2                         0x9FU   /*!< Read IDentification                                     */
#define MT25QU02_MULTIPLE_IO_READ_ID_CMD              0xAFU   /*!< QPI ID Read                                             */
#define MT25QU02_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AU   /*!< Read Serial Flash Discoverable Parameter                */

/***** READ MEMORY Operations ************************************************/
#define MT25QU02_READ_CMD                             0x03U   /*!< Normal Read 3/4 Byte Address                            */
#define MT25QU02_FAST_READ_CMD                        0x0BU   /*!< Fast Read 3/4 Byte Address                              */
#define MT25QU02_1I2O_FAST_READ_CMD                   0x3BU   /*!< Dual Output Fast Read 3/4 Byte Address                  */
#define MT25QU02_2IO_FAST_READ_CMD                    0xBBU   /*!< Dual Input/Output Fast Read 3/4 Byte Address            */
#define MT25QU02_1I4O_FAST_READ_CMD                   0x6BU   /*!< Quad Output Fast Read 3/4 Byte Address                  */
#define MT25QU02_4IO_FAST_READ_CMD                    0xEBU   /*!< Quad Input/Output Fast Read 3/4 Byte Address            */
#define MT25QU02_FAST_READ_DTR_CMD                    0x0DU   /*!< DTR Fast Read 3/4 Byte Address                          */
#define MT25QU02_1I2O_FAST_READ_DTR_CMD               0x3DU   /*!< DTR Dual Output Fast Read 3/4 Byte Address              */
#define MT25QU02_2IO_FAST_READ_DTR_CMD                0xBDU   /*!< DTR Dual Input/Output Fast Read 3/4 Byte Address        */
#define MT25QU02_1I4O_FAST_READ_DTR_CMD               0x6DU   /*!< DTR Quad Output Fast Read 3/4 Byte Address              */
#define MT25QU02_4IO_FAST_READ_DTR_CMD                0xEDU   /*!< DTR Quad Input/Output Fast Read 3/4 Byte Address        */
#define MT25QU02_4IO_WORD_READ_CMD                    0xE7U   /*!< Quad Input/Output Word Read 3/4 Byte Address            */

/***** READ MEMORY Operations with 4-Byte Address ****************************/
#define MT25QU02_4_BYTE_ADDR_READ_CMD                 0x13U   /*!< Normal Read 4 Byte address                              */
#define MT25QU02_4_BYTE_ADDR_FAST_READ_CMD            0x0CU   /*!< Fast Read 4 Byte address                                */
#define MT25QU02_4_BYTE_ADDR_1I2O_FAST_READ_CMD       0x3CU   /*!< Dual Output Fast Read 4 Byte address                    */
#define MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_CMD        0xBCU   /*!< Dual Input/Output Fast Read 4 Byte address              */
#define MT25QU02_4_BYTE_ADDR_1I4O_FAST_READ_CMD       0x6CU   /*!< Quad Output Fast Read 4 Byte address                    */
#define MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_CMD        0xECU   /*!< Quad Input/Output Fast Read 4 Byte address              */
#define MT25QU02_4_BYTE_ADDR_FAST_READ_DTR_CMD        0x0EU   /*!< DTR Fast Read 4 Byte Address                            */
#define MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD    0xBEU   /*!< DTR Dual Input/Output Fast Read 4 Byte address          */
#define MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD    0xEEU   /*!< DTR Quad Input/Output Fast Read 4 Byte address          */

/***** WRITE Operations ******************************************************/
#define MT25QU02_WRITE_ENABLE_CMD                     0x06U   /*!< Write Enable                                            */
#define MT25QU02_WRITE_DISABLE_CMD                    0x04U   /*!< Write Disable                                           */

/***** READ REGISTER Operations **********************************************/
#define MT25QU02_READ_STATUS_REG_CMD                  0x05U   /*!< Read Status Register                                    */
#define MT25QU02_READ_FLAG_STATUS_REG_CMD             0x70U   /*!< Read Flag Status Register                               */
#define MT25QU02_READ_NONVOL_CFG_REG_CMD              0xB5U   /*!< Read NonVolatile Configuration Register                 */
#define MT25QU02_READ_VOL_CFG_REG_CMD                 0x85U   /*!< Read Volatile Configuration Register                    */
#define MT25QU02_READ_ENHANCED_VOL_CFG_REG_CMD        0x65U   /*!< Read Enhanced Volatile Configuration Register           */
#define MT25QU02_READ_EXTENDED_ADDR_REG_CMD           0xC8U   /*!< Read Extended Address Register                          */
#define MT25QU02_READ_GEN_PURP_REG_CMD                0x96U   /*!< Read Extended Address Register                          */

/***** WRITE REGISTER Operations *********************************************/
#define MT25QU02_WRITE_STATUS_REG_CMD                 0x01U   /*!< Write Status Register                                   */
#define MT25QU02_WRITE_NONVOL_CFG_REG_CMD             0xB1U   /*!< Write NonVolatile Configuration Register                */
#define MT25QU02_WRITE_VOL_CFG_REG_CMD                0x81U   /*!< Write Volatile Configuration Register                   */
#define MT25QU02_WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61U   /*!< Write Enhanced Volatile Configuration Register          */
#define MT25QU02_WRITE_EXTENDED_ADDR_REG_CMD          0xC5U   /*!< Write Extended Address Register                         */

/***** CLEAR FLAG STATUS REGISTER Operations *********************************/
#define MT25QU02_CLEAR_FLAG_STATUS_REG_CMD            0x50U   /*!< Read Flag Status Register                               */

/***** PROGRAM Operations ****************************************************/
#define MT25QU02_PAGE_PROG_CMD                        0x02U   /*!< Page Program 3/4 Byte Address                           */
#define MT25QU02_DUAL_INPUT_FAST_PROG_CMD             0xA2U   /*!< Dual Input Fast Program 3/4 Byte Address                */
#define MT25QU02_EXTENDED_DUAL_INPUT_FAST_PROG_CMD    0xD2U   /*!< Extended Dual Input Fast Program 3/4 Byte Address       */
#define MT25QU02_QUAD_INPUT_FAST_PROG_CMD             0x32U   /*!< Quad Input Fast Program 3/4 Byte Address                */
#define MT25QU02_EXTENDED_QUAD_INPUT_FAST_PROG_CMD    0x38U   /*!< Extended Quad Input Fast Program 3/4 Byte Address       */

/***** PROGRAM Operations with 4-Byte Address ********************************/
#define MT25QU02_4_BYTE_ADDR_PAGE_PROG_CMD            0x12U   /*!< Page Program 4 Byte Address                             */
#define MT25QU02_4_BYTE_ADDR_QUAD_INPUT_FAST_PROG_CMD 0x34U   /*!< Quad Input Fast Program 4 Byte Address                  */
#define MT25QU02_4_BYTE_ADDR_QUAD_INPUT_EXTENDED_FAST_PROG_CMD 0x3EU /*!< Quad Input Extended Fast Program 4 Byte Address  */

/***** ERASE Operations ******************************************************/
#define MT25QU02_SUBSECTOR_ERASE_32K_CMD              0x52U   /*!< SubSector Erase 32KB 3/4 Byte Address                   */
#define MT25QU02_SUBSECTOR_ERASE_4K_CMD               0x20U   /*!< SubSector Erase 4KB 3/4 Byte Address                    */
#define MT25QU02_SECTOR_ERASE_64K_CMD                 0xD8U   /*!< Sector Erase 64KB 3/4 Byte Address                      */
#define MT25QU02_BULK_ERASE_CMD                       0xC4U   /*!< Bulk Erase                                              */

/***** ERASE Operations with 4-Byte Address **********************************/
#define MT25QU02_4_BYTE_ADDR_SECTOR_ERASE_64K_CMD     0xDCU   /*!< Sector Erase 64KB 4 Byte address                        */
#define MT25QU02_4_BYTE_ADDR_SUBSECTOR_ERASE_4K_CMD   0x21U   /*!< SubSector Erase 4KB 4 Byte address                      */
#define MT25QU02_4_BYTE_ADDR_SUBSECTOR_ERASE_32K_CMD  0x5CU   /*!< SubSector Erase 32KB 4 Byte address                     */

/***** SUSPEND/RESUME Operations *********************************************/
#define MT25QU02_PROG_ERASE_SUSPEND_CMD               0x75U   /*!< Program/Erase suspend                                   */
#define MT25QU02_PROG_ERASE_RESUME_CMD                0x7AU   /*!< Program/Erase resume                                    */

/***** ONE-TIME PROGRAMMABLE (OTP) Operations ********************************/
#define MT25QU02_READ_OTP_ARRAY_CMD                   0x48U   /*!< Read OTP Array 3/4 Byte Address                         */
#define MT25QU02_PROGRAM_OTP_ARRAY_CMD                0x42U   /*!< Program OTP Array 3/4 Byte Address                      */

/***** 4-BYTE ADDRESS MODE Operations ****************************************/
#define MT25QU02_ENTER_4_BYTE_ADDR_MODE_CMD           0xB7U   /*!< Enter 4-Byte mode (3/4 Byte address commands)           */
#define MT25QU02_EXIT_4_BYTE_ADDR_MODE_CMD            0xE9U   /*!< Exit 4-Byte mode (3/4 Byte address commands)            */

/***** QUAD PROTOCOL Operations **********************************************/
#define MT25QU02_ENABLE_QSPI_CMD                      0x35U   /*!< Enable QPI; Enter QPI                                   */
#define MT25QU02_RESET_QSPI_CMD                       0xF5U   /*!< Reset QPI; Exit QPI                                     */

/***** DEEP POWER-DOWN Operations ********************************************/
#define MT25QU02_ENTER_DEEP_POWER_DOWN_CMD            0xB9U   /*!< Enter deep power down                                   */
#define MT25QU02_RELEASE_FROM_DEEP_POWER_DOWN_CMD     0xABU   /*!< Release from Deep Power down                            */

/***** ADVANCED SECTOR PROTECTION Operations *********************************/
#define MT25QU02_READ_SECTOR_PROTECTION_CMD           0x2DU   /*!< Read Sector Protection                                  */
#define MT25QU02_PROGRAM_SECTOR_PROTECTION_CMD        0x2CU   /*!< Program Sector Protection                               */
#define MT25QU02_READ_VOL_LOCK_BITS_CMD               0xE8U   /*!< Read Volatile Lock Bits 3/4 Byte Address                */
#define MT25QU02_WRITE_VOL_LOCK_BITS_CMD              0xE5U   /*!< Write Volatile Lock Bits 3/4 Byte Address               */
#define MT25QU02_READ_NONVOL_LOCK_BITS_CMD            0xE2U   /*!< Read NonVolatile Lock Bits 4 Byte Address               */
#define MT25QU02_WRITE_NONVOL_LOCK_BITS_CMD           0xE3U   /*!< Write NonVolatile Lock Bits 4 Byte Address              */
#define MT25QU02_ERASE_NONVOL_LOCK_BITS_CMD           0xE4U   /*!< Erase NonVolatile Lock Bits                             */
#define MT25QU02_READ_GLOBAL_FREEZE_BIT_CMD           0xA7U   /*!< Read Global Freeze Bit                                  */
#define MT25QU02_WRITE_GLOBAL_FREEZE_BIT_CMD          0xA6U   /*!< Write Global Freeze Bit                                 */
#define MT25QU02_READ_PASSWORD_REGISTER_CMD           0x27U   /*!< Read Password                                           */
#define MT25QU02_WRITE_PASSWORD_REGISTER_CMD          0x28U   /*!< Write Password                                          */
#define MT25QU02_PASSWORD_UNLOCK_CMD                  0x29U   /*!< Unlock Password                                         */

/***** ADVANCED SECTOR PROTECTION Operations with 4-Byte Address *************/
#define MT25QU02_4_BYTE_ADDR_READ_VOL_LOCK_BITS_CMD   0xE0U   /*!< Read Volatile Lock Bits 4 Byte Address                  */
#define MT25QU02_4_BYTE_ADDR_WRITE_VOL_LOCK_BITS_CMD  0xE1U   /*!< Write Volatile Lock Bits 4 Byte Address                 */

/***** ADVANCED FUNCTION INTERFACE Operations ********************************/
#define MT25QU02_INTERFACE_ACTIVATION_CMD             0x9BU   /*!< Interface Activation                                    */
#define MT25QU02_CYCLIC_REDUNDANCY_CHECK_CMD          0x27U   /*!< Cyclic Redundancy Check                                 */


/******************************************************************************
  * @brief  MT25QU02 Registers
  ****************************************************************************/
/* Status Register */
#define MT25QU02_SR_WIP                               0x01U   /*!< Write in progress                                       */
#define MT25QU02_SR_WEN                               0x02U   /*!< Write enable latch                                      */
#define MT25QU02_SR_BLOCKPR                           0x5CU   /*!< Block protected against program and erase operations    */
#define MT25QU02_SR_TB                                0x20U   /*!< Top/Bottom bit to configure the block protect area      */
#define MT25QU02_SR_SRWD                              0x80U   /*!< Status register write enable/disable                    */

/* Flag Status Register */
#define MT25QU02_FSR_4BYTE                            0x01U   /*!< 3-Bytes or 4-Bytes addressing                           */
#define MT25QU02_FSR_PRE                              0x02U   /*!< Failure or Protection Error                             */
#define MT25QU02_FSR_P_SUSPEND                        0x04U   /*!< Program Suspend                                         */
#define MT25QU02_FSR_P_FAIL                           0x10U   /*!< Program Failure                                         */
#define MT25QU02_FSR_E_FAIL                           0x20U   /*!< Erase Failure                                           */
#define MT25QU02_FSR_E_SUSPEND                        0x40U   /*!< Erase Suspend                                           */
#define MT25QU02_FSR_READY                            0x80U   /*!< Busy or Ready                                           */

/* Extended Address Register */
#define MT25QU02_EAR_128MBS                           0x03U   /*!< A[25:24] bits; 128Mb segment selection                  */

/* Nonvolatile Configuration Register */
#define MT25QU02_NVCR_4BYTE                         0x0001U   /*!< 3-Bytes or 4-Bytes addressing                           */
#define MT25QU02_NVCR_128MBS                        0x0002U   /*!< Highest or Lowest default 128Mb segment                 */
#define MT25QU02_NVCR_DPI                           0x0004U   /*!< Dual IO command input mode                              */
#define MT25QU02_NVCR_QPI                           0x0008U   /*!< Quad IO command input mode                              */
#define MT25QU02_NVCR_RH                            0x0010U   /*!< Enable #HOLD or #RESET on DQ3                           */
#define MT25QU02_NVCR_DTR                           0x0020U   /*!< Double Transfer Rate Protocol                           */
#define MT25QU02_NVCR_ODS                           0x01C0U   /*!< Output driver strength                                  */
#define MT25QU02_NVCR_XIP                           0x0E00U   /*!< XIP mode at power-on reset                              */
#define MT25QU02_NVCR_DC                            0xF000U   /*!< Dummy Clock Cycles setting for FAST READ commands       */

/* Volatile Configuration Register */
#define MT25QU02_VCR_WRAP                             0x03U   /*!< Wrap                                                    */
#define MT25QU02_VCR_XIP                              0x08U   /*!< XIP                                                     */
#define MT25QU02_VCR_DC                               0xF0U   /*!< Dummy Clock Cycles setting for FAST READ commands       */

/* Enhanced Volatile Configuration Register */
#define MT25QU02_EVCR_ODS                             0x07U   /*!< Output driver strength                                  */
#define MT25QU02_EVCR_RH                              0x10U   /*!< Enable #HOLD or #RESET on DQ3                           */
#define MT25QU02_EVCR_DTR                             0x20U   /*!< Double Transfer Rate Protocol                           */
#define MT25QU02_EVCR_DPI                             0x40U   /*!< Dual IO command input mode                              */
#define MT25QU02_EVCR_QPI                             0x80U   /*!< Quad IO command input mode                              */

/* Sector Protection Security Register */
#define MT25QU02_SPSR_SPL                           0x0002U   /*!< Sector Protection lock with or without password         */
#define MT25QU02_SPSR_PPL                           0x0004U   /*!< Password Protection lock                                */

/* Global Freeze Register */
#define MT25QU02_GFR_GFB                              0x01U   /*!< Global Freeze bit                                       */

/* Volatile Lock Bit Security Register */
#define MT25QU02_VLBSR_SWL                            0x01U   /*!< Sector write lock                                       */
#define MT25QU02_VLBSR_SLD                            0x02U   /*!< Sector lock down                                        */

/**
  * @}
  */

/** @defgroup MT25QU02_Exported_Types MT25QU02 Exported Types
  * @{
  */
typedef struct {
  uint32_t FlashSize;                        /*!< Size of the flash                             */
  uint32_t EraseSectorSize;                  /*!< Size of sectors for the erase operation       */
  uint32_t EraseSectorsNumber;               /*!< Number of sectors for the erase operation     */
  uint32_t EraseSubSectorSize;               /*!< Size of subsector for the erase operation     */
  uint32_t EraseSubSectorNumber;             /*!< Number of subsector for the erase operation   */
  uint32_t EraseSubSector1Size;              /*!< Size of subsector 1 for the erase operation   */
  uint32_t EraseSubSector1Number;            /*!< Number of subsector 1 for the erase operation */
  uint32_t ProgPageSize;                     /*!< Size of pages for the program operation       */
  uint32_t ProgPagesNumber;                  /*!< Number of pages for the program operation     */
} MT25QU02_Info_t;

typedef enum {
  MT25QU02_SPI_MODE = 0,                 /*!< 1-1-1 commands, Power on H/W default setting  */
  MT25QU02_SPI_1I2O_MODE,                /*!< 1-1-2 commands                                */
  MT25QU02_SPI_2IO_MODE,                 /*!< 1-2-2 commands                                */
  MT25QU02_SPI_1I4O_MODE,                /*!< 1-1-4 commands                                */
  MT25QU02_SPI_4IO_MODE,                 /*!< 1-4-4 commands                                */
  MT25QU02_DPI_MODE,                     /*!< 2-2-2 commands                                */
  MT25QU02_QPI_MODE                      /*!< 4-4-4 commands                                */
} MT25QU02_Interface_t;

typedef enum {
  MT25QU02_STR_TRANSFER = 0,             /*!< Single Transfer Rate                          */
  MT25QU02_DTR_TRANSFER                  /*!< Double Transfer Rate                          */
} MT25QU02_Transfer_t;

// typedef enum {
//   MT25QU02_DUALFLASH_DISABLE = QSPI_DUALFLASH_DISABLE, /*!<  Single flash mode              */
//   MT25QU02_DUALFLASH_ENABLE = QSPI_DUALFLASH_ENABLE    /*!<  Dual flash mode                */
// } MT25QU02_DualFlash_t;

typedef enum {
  MT25QU02_ERASE_4K = 0,                 /*!< 4K size Sector erase                          */
  MT25QU02_ERASE_32K,                    /*!< 32K size Block erase                          */
  MT25QU02_ERASE_64K,                    /*!< 64K size Block erase                          */
  MT25QU02_ERASE_BULK                    /*!< Whole bulk erase                              */
} MT25QU02_Erase_t;

typedef enum {
  MT25QU02_BURST_READ_WRAP_16 = 0,       /*!< 16 bytes boundary aligned                     */
  MT25QU02_BURST_READ_WRAP_32,           /*!< 32 bytes boundary aligned                     */
  MT25QU02_BURST_READ_WRAP_64,           /*!< 64 bytes boundary aligned                     */
  MT25QU02_BURST_READ_WRAP_NONE          /*!< Disable wrap function */
} MT25QU02_WrapLength_t;

typedef enum {
  MT25QU02_EVCR_ODS_90 = 1,              /*!< Output driver strength 90 ohms                */
  MT25QU02_EVCR_ODS_45 = 3,              /*!< Output driver strength 45 ohms                */
  MT25QU02_EVCR_ODS_20 = 5,              /*!< Output driver strength 20 ohms                */
  MT25QU02_EVCR_ODS_30 = 7               /*!< Output driver strength 30 ohms (default)      */
} MT25QU02_ODS_t;

typedef enum {
  MT25QU02_3BYTES_SIZE = 0,              /*!< 3 Bytes address mode                          */
  MT25QU02_4BYTES_SIZE                   /*!< 4 Bytes address mode                          */
} MT25QU02_AddressSize_t;

typedef enum {
  MT25QU02_DUALFLASH_ENABLE = 0,       /*!<  Dual flash mode                               */
  MT25QU02_DUALFLASH_DISABLE           /*!<  Single flash mode                             */
} MT25QU02_DualFlash_t;

typedef struct{
	XSPI_HandleTypeDef* xspi_handler;
	MT25QU02_AddressSize_t  addr_mode;
  MT25QU02_Interface_t    interface_mode;
  MT25QU02_Transfer_t     transfer_rate;
  MT25QU02_WrapLength_t   wrap_length;
  MT25QU02_Info_t         flash_info;
  MT25QU02_ODS_t          output_driver_strength;
  MT25QU02_DualFlash_t    dual_flash;
} MT25QU02_HandleTypeDef;

/**
  * @}
  */

/** @defgroup MT25QU02_Exported_Functions MT25QU02 Exported Functions
  * @{
  */

/* MT25QU02_HandleTypeDef functions ******************************************************/
MT25QU02_HandleTypeDef MT25QU02_Handler_Init(XSPI_HandleTypeDef* xspi_handler);
void MT25QU02_Update(MT25QU02_HandleTypeDef *Dev);
MT25QU02_HandleTypeDef MT25QU02_GetHandle(void); 
int32_t MT25QU02_Init(void);
// /* Function by commands combined */
// int32_t MT25QU02_GetFlashInfo(MT25QU02_Info_t *pInfo);
int32_t MT25QU02_AutoPollingMemReady();

// /* Read/Write Array Commands **************************************************/
int32_t MT25QU02_ReadSTR(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MT25QU02_ReadDTR(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t MT25QU02_PageProgram(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t MT25QU02_BlockErase(uint32_t BlockAddress, MT25QU02_Erase_t BlockSize);
int32_t MT25QU02_DieErase(uint32_t DieAddress);
int32_t MT25QU02_EnableMemoryMappedModeSTR();
int32_t MT25QU02_EnableMemoryMappedModeDTR();

// /* Register/Setting Commands **************************************************/
int32_t MT25QU02_WriteEnable(void);
int32_t MT25QU02_ReadFlagStatusRegister(uint8_t *Value);
// int32_t MT25QU02_WriteDisable(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode);
// int32_t MT25QU02_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value);
// int32_t MT25QU02_ReadEnhancedVolCfgRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value);
// int32_t MT25QU02_WriteEnhancedVolCfgRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value);
int32_t MT25QU02_EnterQPIMode();
int32_t MT25QU02_ExitQPIMode();
int32_t MT25QU02_Enter4BytesAddressMode();
int32_t MT25QU02_Exit4BytesAddressMode();

/* ID/Security Commands *******************************************************/
int32_t MT25QU02_ReadID(uint8_t *ID);

// /* Reset Commands *************************************************************/
// int32_t MT25QU02_ResetEnable(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode);
// int32_t MT25QU02_ResetMemory(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* MT25QU02_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
