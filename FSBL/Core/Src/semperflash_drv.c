/*
 * semperflash.c
 *
 *  Created on: Jun 19, 2025
 *      Author: Li YunFan
 */

#include <semperflash_drv.h>

uint32_t Semper_Read_FlashID(XSPI_HandleTypeDef *hxspi)
{
    uint8_t cmd = SEMPER_READID_CMD;
    uint8_t rx_buf[6] = {0};

    XSPI_RegularCmdTypeDef  s_command = {0};

    s_command.Instruction = cmd;
    s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
    s_command.DataMode = HAL_XSPI_DATA_1_LINE;
    s_command.DataLength = 6;  // 接收3字节ID数据

    // 设置延迟周期(根据CFR3V[7:6]配置)
    s_command.DummyCycles = 0;  // 示例值，需根据实际频率配置

    if(HAL_XSPI_Command(hxspi, &s_command, XSPI_TIMEOUT) != HAL_OK)
        return HAL_ERROR;

    if(HAL_XSPI_Receive(hxspi, rx_buf, XSPI_TIMEOUT) != HAL_OK)
        return HAL_ERROR;

    // 解析ID: 制造商ID + 设备ID
    uint16_t manufacturer_id = rx_buf[0];
    uint16_t device_id = (rx_buf[1] << 8) | rx_buf[2];

    return (manufacturer_id << 16) | device_id;
}

uint32_t Semper_Flash_Init(XSPI_HandleTypeDef *hxspi)
{

}
