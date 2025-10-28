/**
 ******************************************************************************
 * @file    mt25ql512abb.c
 * @modify  MCD Application Team
 * @brief   This file provides the MT25QU02 QSPI drivers.
 ******************************************************************************
 * MT25QU02 action :
 *   Quad IO protocol (QP) and Dual IO protocol (DP) bits of Enhanced Volatile
 *   Configuration Register (EVCR):
 *     QP = 0: Operates in Quad IO protocol (accepts 4-4-4 commands)
 *     QP = 1 and DP = 0: Operates in Dual IO protocol (accepts 2-2-2 commands)
 *     QP = 1 and DP = 1: Operates in Single IO protocol (accepts 1-1-1, 1-1-2,
 *                                               1-2-2, 1-1-4, 1-4-4 commands)
 *   Enter QPI mode by issuing ENABLE_QSPI_CMD(0x35) command from 1-1-1 mode
 *   Exit QPI mode by issuing RESET_QSPI_CMD(0xF5) command from 4-4-4 mode
 *   Enter DPI mode by configuring QP = 1 and DP = 0 in EVCR
 *   Exit DPI mode by configuring QP = 1 and DP = 1 in EVCR
 *
 *   Memory Read commands support STR(Single Transfer Rate) &
 *   DTR(Double Transfer Rate) modes
 *
 *   Dual flash mode is supported: register read and write is double size
 *
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

/* Includes ------------------------------------------------------------------*/
#include "mt25ql512abb.h"

MT25QU02_HandleTypeDef MT25QU02_Dev;
extern XSPI_HandleTypeDef hxspi2;
XSPI_HandleTypeDef* MT25QU02_Ctx = &hxspi2;
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup MT25QU02 MT25QU02
  * @{
  */

/** @defgroup MT25QU02_Exported_Functions MT25QU02 Exported Functions
  * @{
  */

/**
 * @brief  Initialize the MT25QU02 handler.
 * @param  xspi_handler XSPI handle
 * @retval MT25QU02 handle
 */
MT25QU02_HandleTypeDef MT25QU02_Handler_Init(XSPI_HandleTypeDef* xspi_handler)
{
  MT25QU02_Dev.xspi_handler = xspi_handler;
  MT25QU02_Dev.addr_mode = MT25QU02_3BYTES_SIZE;
  MT25QU02_Dev.interface_mode = MT25QU02_SPI_MODE;
  MT25QU02_Dev.transfer_rate = MT25QU02_STR_TRANSFER;
  MT25QU02_Dev.wrap_length = MT25QU02_BURST_READ_WRAP_16;
  MT25QU02_Dev.output_driver_strength = MT25QU02_EVCR_ODS_30;
  MT25QU02_Dev.dual_flash = MT25QU02_DUALFLASH_DISABLE;
  return MT25QU02_Dev;
}

void MT25QU02_Update(MT25QU02_HandleTypeDef *Dev)
{
  MT25QU02_Dev = *Dev;
}

MT25QU02_HandleTypeDef MT25QU02_GetHandle(void)
{
  return MT25QU02_Dev;
}

int32_t MT25QU02_Init(void)
{
  MT25QU02_Handler_Init(MT25QU02_Ctx);
  return MT25QU02_OK;
}

// /**
//   * @brief  Get Flash information
//   * @param  pInfo pointer to information structure
//   * @retval error status
//   */
// int32_t MT25QU02_GetFlashInfo(MT25QU02_Info_t *pInfo)
// {
//   /* Configure the structure with the memory configuration */prog
//   pInfo->FlashSize              = MT25QU02_FLASH_SIZE;
//   pInfo->EraseSectorSize        = MT25QU02_SECTOR_64K;
//   pInfo->EraseSectorsNumber     = (MT25QU02_FLASH_SIZE/MT25QU02_SECTOR_64K);
//   pInfo->EraseSubSectorSize     = MT25QU02_SUBSECTOR_32K;
//   pInfo->EraseSubSectorNumber   = (MT25QU02_FLASH_SIZE/MT25QU02_SUBSECTOR_32K);
//   pInfo->EraseSubSector1Size    = MT25QU02_SUBSECTOR_4K;
//   pInfo->EraseSubSector1Number  = (MT25QU02_FLASH_SIZE/MT25QU02_SUBSECTOR_4K);
//   pInfo->ProgPageSize           = MT25QU02_PAGE_SIZE;
//   pInfo->ProgPagesNumber        = (MT25QU02_FLASH_SIZE/MT25QU02_PAGE_SIZE);

//   return MT25QU02_OK;
// };

/**
  * @brief  Polling WIP(Write In Progress) bit become to 0
  *         SPI/DPI/QPI; 1-0-1/2-0-2/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  DualFlash Dual flash mode state
  * @retval error status
  */
int32_t MT25QU02_AutoPollingMemReady()
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_DualFlash_t DualFlash = MT25QU02_Dev.dual_flash;

  XSPI_RegularCmdTypeDef s_command = {0};
  XSPI_AutoPollingTypeDef s_config = {0};

  /* Configure automatic polling mode to wait for memory ready (XSPI layout) */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_READ_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  /* At least one dummy cycle is required when using dual or quad mode to receive data */
  s_command.DummyCycles        = (Mode == MT25QU02_QPI_MODE) ? 2U : (Mode == MT25QU02_DPI_MODE) ? 4U : 0U;
  s_command.DataMode           = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_DATA_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_DATA_2_LINES :
                                 HAL_XSPI_DATA_1_LINE;
  s_command.DataLength         = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? 2U : 1U;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;
  /* Alternate bytes fields explicit defaults */
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;

  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  // uint8_t status = 0;
  // if(HAL_XSPI_Receive(Ctx, &status, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  // {
  //   return MT25QU02_ERROR;
  // }
  /* Configure polling criteria: wait for WIP bit == 0 */
  s_config.MatchValue    = 0;
  s_config.MatchMask     = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? ((MT25QU02_SR_WIP << 8) | MT25QU02_SR_WIP) : MT25QU02_SR_WIP;
  s_config.MatchMode     = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime  = MT25QU02_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(Ctx, &s_config, 300000) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

/* Read/Write Array Commands (3/4 Byte Address Command Set) *********************/
/**
  * @brief  Reads an amount of data from the QSPI memory on STR mode.
  *         SPI/DPI/QPI; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4/2-2-2/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read
  * @retval QSPI memory status
  */
int32_t MT25QU02_ReadSTR(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;

  /* zero-init to avoid uninitialized fields */
  XSPI_RegularCmdTypeDef s_command = {0};

  /* Common default XSPI fields */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;
  

  /* Initialize the read command (per-mode adjustments follow) */
  s_command.AddressWidth = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.Address = ReadAddr;
  s_command.DataLength = Size;

  switch(Mode)
  {
  case MT25QU02_SPI_1I2O_MODE :          /* 1-1-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_1I2O_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_1I2O_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_2IO_MODE :           /* 1-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_1I4O_MODE :          /* 1-1-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_1I4O_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_1I4O_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_4IO_MODE :           /* 1-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 10;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_DPI_MODE :               /* 2-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_2_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 10;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
  default :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_1_LINE;
    break;
  }

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

/**
  * @brief  Reads an amount of data from the QSPI memory on DTR mode.
  *         SPI/DPI/QPI; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4/2-2-2/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start addressS
  * @param  Size Size of data to read
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MT25QU02_ReadDTR(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;

  XSPI_RegularCmdTypeDef s_command = {0};

  /* Common default XSPI fields */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_ENABLE;

  /* Initialize the read command */
  s_command.AddressWidth = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.Address = ReadAddr;
  s_command.DataLength = Size;

  switch(Mode)
  {
  case MT25QU02_SPI_1I2O_MODE :          /* 1-1-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MT25QU02_1I2O_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 6;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_2IO_MODE :           /* 1-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 6;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_1I4O_MODE :          /* 1-1-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MT25QU02_1I4O_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 6;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_4IO_MODE :           /* 1-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_DPI_MODE :               /* 2-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_2_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 6;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
  default :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_FAST_READ_DTR_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 6;
    s_command.DataMode        = HAL_XSPI_DATA_1_LINE;
    break;
  }

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  /* Reception of the data */
  if (HAL_XSPI_Receive(Ctx, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  *         SPI/DPI/QPI; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4/2-2-2/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ MT25QU02_PAGE_SIZE
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MT25QU02_PageProgram(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;

  /* zero-init to avoid uninitialized fields */
  XSPI_RegularCmdTypeDef s_command = {0};

  /* Common default XSPI fields */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;
  

  /* Initialize the program command (XSPI layout) */
  s_command.AddressWidth = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.Address = WriteAddr;
  s_command.DataLength = Size;

  switch(Mode)
  {
  case MT25QU02_SPI_1I2O_MODE :              /* 1-1-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MT25QU02_DUAL_INPUT_FAST_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_2IO_MODE :               /* 1-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = MT25QU02_EXTENDED_DUAL_INPUT_FAST_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_1I4O_MODE :              /* 1-1-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_QUAD_INPUT_FAST_PROG_CMD : MT25QU02_4_BYTE_ADDR_QUAD_INPUT_FAST_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_4IO_MODE :               /* 1-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_EXTENDED_QUAD_INPUT_FAST_PROG_CMD : MT25QU02_4_BYTE_ADDR_QUAD_INPUT_EXTENDED_FAST_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_DPI_MODE :                   /* 2-2-2 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_2_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_EXTENDED_DUAL_INPUT_FAST_PROG_CMD : MT25QU02_4_BYTE_ADDR_PAGE_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_QPI_MODE :                   /* 4-4-4 commands */
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_EXTENDED_QUAD_INPUT_FAST_PROG_CMD : MT25QU02_4_BYTE_ADDR_QUAD_INPUT_EXTENDED_FAST_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_MODE :                   /* 1-1-1 commands, Power on H/W default setting */
  default :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_PAGE_PROG_CMD : MT25QU02_4_BYTE_ADDR_PAGE_PROG_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 0;
    s_command.DataMode        = HAL_XSPI_DATA_1_LINE;
    break;
  }

  /* Configure the command */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  /* Transmission of the data */
  if (HAL_XSPI_Transmit(Ctx, pData, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         MT25QU02 support 4K, 32K, 64K size block erase commands.
  *         SPI/DPI/QPI; 1-1-0/2-2-0/4-4-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Block size to erase
  * @retval QSPI memory status
  */
int32_t MT25QU02_BlockErase(uint32_t BlockAddress, MT25QU02_Erase_t BlockSize)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;
  int32_t ret = MT25QU02_OK;

  /* zero-init to avoid uninitialized fields */
  XSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the erase instruction based on block size and address size */
  switch(BlockSize)
  {
  case MT25QU02_ERASE_32K :
    s_command.Instruction = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_SUBSECTOR_ERASE_32K_CMD : MT25QU02_4_BYTE_ADDR_SUBSECTOR_ERASE_32K_CMD;
    break;

  case MT25QU02_ERASE_64K :
    s_command.Instruction = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_SECTOR_ERASE_64K_CMD : MT25QU02_4_BYTE_ADDR_SECTOR_ERASE_64K_CMD;
    break;

  case MT25QU02_ERASE_4K :
  default :
    s_command.Instruction = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_SUBSECTOR_ERASE_4K_CMD : MT25QU02_4_BYTE_ADDR_SUBSECTOR_ERASE_4K_CMD;
    break;
  }

  /* XSPI-specific command initialization (defaults + per-mode fields) */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.AddressMode        = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_ADDRESS_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_ADDRESS_2_LINES :
                                 HAL_XSPI_ADDRESS_1_LINE;
  s_command.AddressWidth       = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.Address           = BlockAddress;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = HAL_XSPI_DATA_NONE;
  s_command.DataLength        = 0;
  s_command.DataDTRMode       = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    ret = MT25QU02_ERROR;
  }

  return ret;
}

 /**
   * @brief  Die erase.
   *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
   * @param  Ctx Component object pointer
   * @param  Mode Interface mode
   * @retval error status
   */
 int32_t MT25QU02_DieErase(uint32_t DieAddress)
 {
   XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
   MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
   MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;
   int32_t ret = MT25QU02_OK;

   /* zero-init to avoid uninitialized fields */
   XSPI_RegularCmdTypeDef s_command = {0};

   /* Initialize the bulk erase command (XSPI layout) */
   s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
   s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
   s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
   s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
   s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                  (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                  HAL_XSPI_INSTRUCTION_1_LINE;
   s_command.Instruction        = MT25QU02_BULK_ERASE_CMD;
   s_command.AddressMode        = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_ADDRESS_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_ADDRESS_2_LINES :
                                 HAL_XSPI_ADDRESS_1_LINE;
   s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
   s_command.AddressWidth       = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
   s_command.Address            = DieAddress;
   s_command.AlternateBytes     = 0;
   s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
   s_command.AlternateBytesWidth= 0;
   s_command.AlternateBytesDTRMode = 0;
   s_command.DummyCycles        = 0;
   s_command.DataMode           = HAL_XSPI_DATA_NONE;
   s_command.DataLength        = 0;
   s_command.DataDTRMode       = HAL_XSPI_DATA_DTR_DISABLE;
   s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

   /* Send the command */
   if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     ret = MT25QU02_ERROR;
   }

   return ret;
 }

/**
  * @brief  Enable memory mapped mode for the QSPI memory on STR mode.
  *         SPI/DPI/QPI; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4/2-2-2/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @retval QSPI memory status
  */
int32_t MT25QU02_EnableMemoryMappedModeSTR()
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;

  XSPI_RegularCmdTypeDef      s_command = {0};
  XSPI_MemoryMappedTypeDef    s_mem_mapped_cfg = {0};

  /* Per-file common defaults */
  s_command.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.AddressWidth = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  /* Configure instruction/address/data per interface mode (STR) */
  switch(Mode)
  {
  case MT25QU02_SPI_1I2O_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_1I2O_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_1I2O_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_2IO_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_1I4O_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_1I4O_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_1I4O_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_4IO_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 10;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_DPI_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_2_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_QPI_MODE :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles     = 10;
    s_command.DataMode        = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_MODE :
  default :
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction     = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_FAST_READ_CMD : MT25QU02_4_BYTE_ADDR_FAST_READ_CMD;
    s_command.AddressMode     = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles     = 8;
    s_command.DataMode        = HAL_XSPI_DATA_1_LINE;
    break;
  }

  if(HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

	s_command.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
	s_command.Instruction = MT25QU02_PAGE_PROG_CMD;
	s_command.DummyCycles = 0;

	if(HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return MT25QU02_ERROR; // Command execution failed
	}

  s_mem_mapped_cfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_XSPI_MemoryMapped(Ctx, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

/**
  * @brief  Enable memory mapped mode for the QSPI memory on DTR mode.
  *         SPI/DPI/QPI; 1-1-1/1-1-2/1-2-2/1-1-4/1-4-4/2-2-2/4-4-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  AddressSize Address size
  * @note   Address size is forced to 3 Bytes when the 4 Bytes address size
  *         command is not available for the specified interface mode
  * @retval QSPI memory status
  */
int32_t MT25QU02_EnableMemoryMappedModeDTR()
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_AddressSize_t AddressSize = MT25QU02_Dev.addr_mode;

  XSPI_RegularCmdTypeDef      s_command = {0};
  XSPI_MemoryMappedTypeDef    s_mem_mapped_cfg = {0};

  /* Per-file common defaults */
  s_command.OperationType      = HAL_XSPI_OPTYPE_READ_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.AddressWidth = (AddressSize == MT25QU02_3BYTES_SIZE) ? HAL_XSPI_ADDRESS_24_BITS : HAL_XSPI_ADDRESS_32_BITS;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_ENABLE;

  switch(Mode)
  {
  case MT25QU02_SPI_1I2O_MODE :          /* 1-1-2 read commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MT25QU02_1I2O_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles        = 6;
    s_command.DataMode           = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_2IO_MODE :           /* 1-2-2 read commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles        = 6;
    s_command.DataMode           = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_SPI_1I4O_MODE :          /* 1-1-4 read commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = MT25QU02_1I4O_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles        = 6;
    s_command.DataMode           = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_4IO_MODE :           /* 1-4-4 read commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles        = 8;
    s_command.DataMode           = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_DPI_MODE :               /* 2-2-2 commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_2_LINES;
    s_command.Instruction        = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_2IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_2IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_2_LINES;
    s_command.DummyCycles        = 6;
    s_command.DataMode           = HAL_XSPI_DATA_2_LINES;
    break;

  case MT25QU02_QPI_MODE :               /* 4-4-4 commands */
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_4_LINES;
    s_command.Instruction        = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_4IO_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_4IO_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_4_LINES;
    s_command.DummyCycles        = 8;
    s_command.DataMode           = HAL_XSPI_DATA_4_LINES;
    break;

  case MT25QU02_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
  default :
    s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.Instruction        = (AddressSize == MT25QU02_3BYTES_SIZE) ? MT25QU02_FAST_READ_DTR_CMD : MT25QU02_4_BYTE_ADDR_FAST_READ_DTR_CMD;
    s_command.AddressMode        = HAL_XSPI_ADDRESS_1_LINE;
    s_command.DummyCycles        = 6;
    s_command.DataMode           = HAL_XSPI_DATA_1_LINE;
    break;
  }

  if(HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

	s_command.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
	s_command.Instruction = MT25QU02_PAGE_PROG_CMD;
	s_command.DummyCycles = 0;

	if(HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return MT25QU02_ERROR; // Command execution failed
	}

  s_mem_mapped_cfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;

  if (HAL_XSPI_MemoryMapped(Ctx, &s_mem_mapped_cfg) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}


/* Register/Setting Commands **************************************************/
/**
  * @brief  This function send a Write Enable and wait it is effective.
  *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  DualFlash Dual flash mode state
  * @retval error status
  */
int32_t MT25QU02_WriteEnable(void)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_DualFlash_t DualFlash = MT25QU02_Dev.dual_flash;

  /* zero-init to avoid uninitialized fields */
  XSPI_RegularCmdTypeDef     s_command = {0};
  XSPI_AutoPollingTypeDef    s_config = {0};

  /* Initialize the write enable command (XSPI layout) defaults */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_WRITE_ENABLE_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AddressWidth       = HAL_XSPI_ADDRESS_24_BITS;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = HAL_XSPI_DATA_NONE;
  s_command.DataLength         = 0;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;
  

  /* Send the command */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  /* Reuse s_command to configure READ STATUS REGISTER (XSPI layout) */
  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_READ_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  /* At least one dummy cycle may be required in multi-line modes (align with AutoPollingMemReady) */
  s_command.DummyCycles        = (Mode == MT25QU02_QPI_MODE) ? 2U : (Mode == MT25QU02_DPI_MODE) ? 4U : 0U;
  s_command.DataMode           = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_DATA_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_DATA_2_LINES :
                                 HAL_XSPI_DATA_1_LINE;
  s_command.DataLength         = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? 2U : 1U;
  s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;

  /* Configure the READ STATUS REGISTER command (needed by AutoPolling) */
  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }


  /* Configure automatic polling mode to wait for write enabling (read status reg) */
  s_config.MatchValue      = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? ((MT25QU02_SR_WEN << 8) | MT25QU02_SR_WEN) : MT25QU02_SR_WEN;
  s_config.MatchMask       = s_config.MatchValue;
  s_config.MatchMode       = HAL_XSPI_MATCH_MODE_AND;
  s_config.IntervalTime    = MT25QU02_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop   = HAL_XSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_XSPI_AutoPolling(Ctx, &s_config, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  return MT25QU02_OK;
}

// /**
//   * @brief  This function reset the (WEN) Write Enable Latch bit.
//   *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface mode
//   * @retval error status
//   */
// int32_t MT25QU02_WriteDisable(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode)
// {
//   QSPI_CommandTypeDef     s_command;

//   /* Initialize the read ID command */
//   s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//   s_command.Instruction       = MT25QU02_WRITE_DISABLE_CMD;
//   s_command.AddressMode       = QSPI_ADDRESS_NONE;
//   s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//   s_command.DummyCycles       = 0;
//   s_command.DataMode          = QSPI_DATA_NONE;
//   s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//   s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//   s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//   /* Configure the command */
//   if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//   {
//     return MT25QU02_ERROR;
//   }

//   return MT25QU02_OK;
// }

// /**
//   * @brief  Read Flash Status register value
//   *         SPI/DPI/QPI; 1-0-1/2-0-2/4-0-4
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface mode
//   * @param  DualFlash Dual flash mode state
//   * @param  Value Status register value pointer
//   * @retval error status
//   */
// int32_t MT25QU02_ReadStatusRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value)
// {
//     QSPI_CommandTypeDef s_command;

//     /* Initialize the reading of status register */
//     s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//     s_command.Instruction       = MT25QU02_READ_STATUS_REG_CMD;
//     s_command.AddressMode       = QSPI_ADDRESS_NONE;
//     s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//     s_command.DummyCycles       = 0;
//     s_command.DataMode          = (Mode == MT25QU02_QPI_MODE) ? QSPI_DATA_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_DATA_2_LINES : QSPI_DATA_1_LINE;
//     s_command.NbData            = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? 2U : 1U;
//     s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//     s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//     s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//     /* Configure the command */
//     if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     {
//       return MT25QU02_ERROR;
//     }

//     /* Reception of the data */
//     if (HAL_QSPI_Receive(Ctx, Value, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     {
//       return MT25QU02_ERROR;
//     }

//     return MT25QU02_OK;
// }

// /**
//   * @brief  Read Flash enhanced volatile configuration register value
//   *         SPI/DPI/QPI; 1-0-1/2-0-2/4-0-4
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface mode
//   * @param  DualFlash Dual flash mode state
//   * @param  Value Status register value pointer
//   * @retval error status
//   */
// int32_t MT25QU02_ReadEnhancedVolCfgRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value)
// {
//     QSPI_CommandTypeDef s_command;

//     /* Initialize the reading of configuration register */
//     s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//     s_command.Instruction       = MT25QU02_READ_ENHANCED_VOL_CFG_REG_CMD;
//     s_command.AddressMode       = QSPI_ADDRESS_NONE;
//     s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//     s_command.DummyCycles       = 0;
//     s_command.DataMode          = (Mode == MT25QU02_QPI_MODE) ? QSPI_DATA_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_DATA_2_LINES : QSPI_DATA_1_LINE;
//     s_command.NbData            = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? 2U : 1U;
//     s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//     s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//     s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//     /* Configure the command */
//     if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     {
//       return MT25QU02_ERROR;
//     }

//     /* Reception of the data */
//     if (HAL_QSPI_Receive(Ctx, Value, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//     {
//       return MT25QU02_ERROR;
//     }

//     return MT25QU02_OK;
// }

// /**
//   * @brief  Write enhanced volatile configuration register.
//   *         SPI/DPI/QPI; 1-0-1/2-0-2/4-0-4
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface mode
//   * @param  DualFlash Dual flash mode state
//   * @param  Value Value for write to enhanced configuration register.
//   * @retval QSPI memory status
//   */
// int32_t MT25QU02_WriteEnhancedVolCfgRegister(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode, MT25QU02_DualFlash_t DualFlash, uint8_t *Value)
// {
//   QSPI_CommandTypeDef s_command;

//   /* Update the configuration register with new value */
//   s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//   s_command.Instruction       = MT25QU02_WRITE_ENHANCED_VOL_CFG_REG_CMD;
//   s_command.AddressMode       = QSPI_ADDRESS_NONE;
//   s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//   s_command.DummyCycles       = 0;
//   s_command.DataMode          = (Mode == MT25QU02_QPI_MODE) ? QSPI_DATA_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_DATA_2_LINES : QSPI_DATA_1_LINE;
//   s_command.NbData            = (DualFlash == MT25QU02_DUALFLASH_ENABLE) ? 2U : 1U;
//   s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//   s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//   s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//   /* Configure the write volatile configuration register command */
//   if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//   {
//     return MT25QU02_ERROR;
//   }

//   /* Transmission of the data */
//   if (HAL_QSPI_Transmit(Ctx, Value, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//   {
//     return MT25QU02_ERROR;
//   }

//   return MT25QU02_OK;
// }

 /**
   * @brief  This function put QSPI memory in QPI mode (Quad I/O) from SPI mode.
   *         SPI -> QPI; 1-x-x -> 4-4-4
   *         SPI; 1-0-0
   * @param  Ctx Component object pointer
   * @retval error status
   */
 int32_t MT25QU02_EnterQPIMode(void)
 {
   XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
   MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
   XSPI_RegularCmdTypeDef s_command = {0};

   /* QSPI memory is supposed to be in SPI mode, so SEND CMD on 1 LINE */
   s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
   s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
   s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
   s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
   /* Ensure instruction issued on single line (from SPI mode) */
   s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_1_LINE;
   s_command.Instruction        = MT25QU02_ENABLE_QSPI_CMD;
   s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
   s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
   s_command.AlternateBytes     = 0;
   s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
   s_command.AlternateBytesWidth= 0;
   s_command.AlternateBytesDTRMode = 0;
   s_command.DummyCycles        = 0;
   s_command.DataMode           = HAL_XSPI_DATA_NONE;
   s_command.DataLength        = 0;
   s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
   s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

   /* Send the command */
   if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }

   /* Update local mode to QPI if needed (caller may call MT25QU02_Update) */
   MT25QU02_HandleTypeDef pDev = MT25QU02_GetHandle();
   if(&pDev != NULL)
    {
      pDev.interface_mode = MT25QU02_QPI_MODE;
    }
    MT25QU02_Update(&pDev);
   return MT25QU02_OK;
 }

 /**
   * @brief  This function put QSPI memory in SPI mode (Single I/O) from QPI mode.
   *         QPI -> SPI; 4-4-4 -> 1-x-x
   *         QPI; 4-0-0
   * @param  Ctx Component object pointer
   * @retval error status
   */
 int32_t MT25QU02_ExitQPIMode(void)
 {
   XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
   MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
   XSPI_RegularCmdTypeDef s_command = {0};

   /* Send RESET_QSPI_CMD in QPI mode -> instruction on 4 lines */
   s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
   s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
   s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
   s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
   s_command.InstructionMode    = HAL_XSPI_INSTRUCTION_4_LINES;
   s_command.Instruction        = MT25QU02_RESET_QSPI_CMD;
   s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
   s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
   s_command.AlternateBytes     = 0;
   s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
   s_command.AlternateBytesWidth= 0;
   s_command.AlternateBytesDTRMode = 0;
   s_command.DummyCycles        = 0;
   s_command.DataMode           = HAL_XSPI_DATA_NONE;
   s_command.DataLength        = 0;
   s_command.DataDTRMode        = HAL_XSPI_DATA_DTR_DISABLE;
   s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

   /* Send the command */
   if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }

    /* Update local mode to SPI if needed (caller may call MT25QU02_Update) */
    MT25QU02_HandleTypeDef pDev = MT25QU02_GetHandle();
    if(&pDev != NULL)
    {
      pDev.interface_mode = MT25QU02_SPI_MODE;
    }
    MT25QU02_Update(&pDev);
   return MT25QU02_OK;
 }

/**
  * @brief  Flash enter 4 Byte address mode. Effect 3/4 address byte commands only.
  *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @retval error status
  */
int32_t MT25QU02_Enter4BytesAddressMode(void)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  XSPI_RegularCmdTypeDef s_command = {0};

  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_ENTER_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = HAL_XSPI_DATA_NONE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  MT25QU02_Dev.addr_mode = MT25QU02_4BYTES_SIZE;

  return MT25QU02_OK;
}

/**
  * @brief  Flash exit 4 Byte address mode. Effect 3/4 address byte commands only.
  *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
  * @param  Component object pointer
  * @retval error status
  */
int32_t MT25QU02_Exit4BytesAddressMode(void)
{
  XSPI_HandleTypeDef *Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  XSPI_RegularCmdTypeDef s_command = {0};

  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_EXIT_4_BYTE_ADDR_MODE_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = HAL_XSPI_DATA_NONE;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return MT25QU02_ERROR;
  }

  MT25QU02_Dev.addr_mode = MT25QU02_3BYTES_SIZE;

  return MT25QU02_OK;
}

/* ID Commands ****************************************************************/
/**
  * @brief  Read Flash 3 Byte IDs.
  *         Manufacturer ID, Memory type, Memory density
  *         SPI/DPI/QPI; 1-0-1/2-0-2/4-0-4
  * @param  Ctx Component object pointer
  * @param  Mode Interface mode
  * @param  ID 3 bytes IDs pointer
  * @param  DualFlash Dual flash mode state
  * @retval error status
  */
int32_t MT25QU02_ReadID(uint8_t *ID)
{
  XSPI_HandleTypeDef* Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;
  MT25QU02_DualFlash_t DualFlash = MT25QU02_Dev.dual_flash;

  /* zero-init to avoid uninitialized fields and set common defaults (align other functions) */
  XSPI_RegularCmdTypeDef s_command = {0};

  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = ((Mode == MT25QU02_QPI_MODE) || (Mode == MT25QU02_DPI_MODE)) ? MT25QU02_MULTIPLE_IO_READ_ID_CMD : MT25QU02_READ_ID_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_DATA_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_DATA_2_LINES :
                                 HAL_XSPI_DATA_1_LINE;
  s_command.DataLength        = 3;
  s_command.DataDTRMode       = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

   /* Configure the command */
   if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }
 
   /* Reception of the data */
   if (HAL_XSPI_Receive(Ctx, ID, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }
 
   return MT25QU02_OK;
}

// /* Reset Commands *************************************************************/
// /**
//   * @brief  Flash reset enable command
//   *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface select
//   * @retval error status
//   */
// int32_t MT25QU02_ResetEnable(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode)
// {
//   QSPI_CommandTypeDef s_command;

//   /* Initialize the reset enable command */
//   s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//   s_command.Instruction       = MT25QU02_RESET_ENABLE_CMD;
//   s_command.AddressMode       = QSPI_ADDRESS_NONE;
//   s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//   s_command.DummyCycles       = 0;
//   s_command.DataMode          = QSPI_DATA_NONE;
//   s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//   s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//   s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//   /* Send the command */
//   if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//   {
//     return MT25QU02_ERROR;
//   }

//   return MT25QU02_OK;
// }

// /**
//   * @brief  Flash reset memory command
//   *         SPI/DPI/QPI; 1-0-0/2-0-0/4-0-0
//   * @param  Ctx Component object pointer
//   * @param  Mode Interface select
//   * @retval error status
//   */
// int32_t MT25QU02_ResetMemory(QSPI_HandleTypeDef *Ctx, MT25QU02_Interface_t Mode)
// {
//   QSPI_CommandTypeDef s_command;

//   /* Initialize the reset enable command */
//   s_command.InstructionMode   = (Mode == MT25QU02_QPI_MODE) ? QSPI_INSTRUCTION_4_LINES : (Mode == MT25QU02_DPI_MODE) ? QSPI_INSTRUCTION_2_LINES : QSPI_INSTRUCTION_1_LINE;
//   s_command.Instruction       = MT25QU02_RESET_MEMORY_CMD;
//   s_command.AddressMode       = QSPI_ADDRESS_NONE;
//   s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//   s_command.DummyCycles       = 0;
//   s_command.DataMode          = QSPI_DATA_NONE;
//   s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//   s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//   s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//   /* Send the command */
//   if (HAL_QSPI_Command(Ctx, &s_command, HAL_QSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//   {
//     return MT25QU02_ERROR;
//   }

//   return MT25QU02_OK;
// }
int32_t MT25QU02_ReadFlagStatusRegister(uint8_t *Value)
{
  XSPI_HandleTypeDef* Ctx = MT25QU02_Dev.xspi_handler;
  MT25QU02_Interface_t Mode = MT25QU02_Dev.interface_mode;

  /* zero-init to avoid uninitialized fields and set common defaults (align other functions) */
  XSPI_RegularCmdTypeDef s_command = {0};

  s_command.OperationType      = HAL_XSPI_OPTYPE_COMMON_CFG;
  s_command.IOSelect           = HAL_XSPI_SELECT_IO_3_0;
  s_command.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionMode    = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_INSTRUCTION_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_INSTRUCTION_2_LINES :
                                 HAL_XSPI_INSTRUCTION_1_LINE;
  s_command.Instruction        = MT25QU02_READ_FLAG_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytes     = 0;
  s_command.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
  s_command.AlternateBytesWidth= 0;
  s_command.AlternateBytesDTRMode = 0;
  s_command.DummyCycles        = 0;
  s_command.DataMode           = (Mode == MT25QU02_QPI_MODE) ? HAL_XSPI_DATA_4_LINES :
                                 (Mode == MT25QU02_DPI_MODE) ? HAL_XSPI_DATA_2_LINES :
                                 HAL_XSPI_DATA_1_LINE;
  s_command.DataLength        = 1;
  s_command.DataDTRMode       = HAL_XSPI_DATA_DTR_DISABLE;
  s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

   /* Configure the command */
   if (HAL_XSPI_Command(Ctx, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }
 
   /* Reception of the data */
   if (HAL_XSPI_Receive(Ctx, Value, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
   {
     return MT25QU02_ERROR;
   }
 
   return MT25QU02_OK;
}
/**
  * @}
  */

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
