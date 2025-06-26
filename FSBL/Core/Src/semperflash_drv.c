/*
 * semperflash.c
 *
 *  Created on: Jun 19, 2025
 *      Author: Li YunFan
 */

#include <semperflash_drv.h>

semper_status_t Semper_Flash_Init(SEMPER_HandleTypeDef* flash1,XSPI_HandleTypeDef* hxspi)
{
	flash1->addr_mode = SEMPER_ADDR_3BYTE;
	flash1->interface_mode = SEMPER_1S_MODE;
	flash1->xspi_handler = hxspi;
	return SEMPER_OK;
}

semper_status_t Semper_Read_FlashID(SEMPER_HandleTypeDef* flash1,uint32_t* flashID)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
    uint8_t rx_buf[30] = {0};
    uint32_t dummy_addr = 0x00000000;
    XSPI_RegularCmdTypeDef  s_command = {0};

    s_command.Instruction = SEMPER_CMD_READID;
    if(flash1->interface_mode == SEMPER_1S_MODE)
    {
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		s_command.DataMode = HAL_XSPI_DATA_1_LINE;
	    // 设置延迟周期(根据CFR3V[7:6]配置)
	    s_command.DummyCycles = 0;  // 示例值，需根据实际频率配置
    }
    else if(flash1->interface_mode == SEMPER_8S_MODE)
    {
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		s_command.DataMode = HAL_XSPI_DATA_8_LINES;
		s_command.Address = dummy_addr;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		s_command.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		s_command.DummyCycles = 4;
    }
    else
    {
    	return SEMPER_ERROR;
    }
    s_command.DataLength = 30;  // 接收3字节ID数据


    if(HAL_XSPI_Command(hxspi, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return SEMPER_ERROR;

    if(HAL_XSPI_Receive(hxspi, rx_buf, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return SEMPER_ERROR;
    }

	// 解析ID: 制造商ID + 设备ID
	uint16_t manufacturer_id = rx_buf[0];
	uint16_t device_id = (rx_buf[1] << 8) | rx_buf[2];

	*flashID = (manufacturer_id << 16) | device_id;
	return SEMPER_OK;
}

semper_status_t Semper_Read_Reg(SEMPER_HandleTypeDef* flash1, uint32_t reg_addr, uint8_t* read_data)
{
	XSPI_HandleTypeDef* hqspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};
	cmd.Instruction = SEMPER_CMD_RDARG;     //RDARG_C_0操作码
    if(flash1->interface_mode == SEMPER_1S_MODE)
    {
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
    }
    else if(flash1->interface_mode == SEMPER_8S_MODE)
    {
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
		cmd.DummyCycles = 4;
    }
    else
    {
    	return SEMPER_ERROR;
    }

	cmd.Address = reg_addr;   //CFR5V寄存器地址（需根据实际地址表确认）
	if(flash1->addr_mode == SEMPER_ADDR_3BYTE)
	{
		cmd.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
	}
	else if(flash1->addr_mode == SEMPER_ADDR_4BYTE)
	{
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}

	cmd.DataLength = 1;
	if(HAL_XSPI_Command(hqspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE)!=HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hqspi, read_data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE)!=HAL_OK)
		return SEMPER_ERROR;
	return SEMPER_OK;
}


semper_status_t Semper_Enter_4Byte_Address_Mode(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};
	
	cmd.Instruction = SEMPER_CMD_EN4B;  // Ensure this is defined in your header file
	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	
	/* Update address mode in the flash structure */
	flash1->addr_mode = SEMPER_ADDR_4BYTE;
	
	return SEMPER_OK;
}

semper_status_t Semper_Exit_4Byte_Address_Mode(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};
	
	cmd.Instruction = SEMPER_CMD_EX4B;  // Ensure this is defined in your header file
	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	
	/* Update address mode in the flash structure */
	flash1->addr_mode = SEMPER_ADDR_3BYTE;
	
	return SEMPER_OK;
}

semper_status_t Semper_Write_Reg(SEMPER_HandleTypeDef* flash1, uint32_t reg_addr, uint8_t write_data)
{
	XSPI_HandleTypeDef* hqspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};
	
	// First command: Write Enable
	cmd.Instruction = SEMPER_CMD_WRENB;
	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
	}
	else
	{
		return SEMPER_ERROR;
	}
	cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;
	cmd.DataMode = HAL_XSPI_DATA_NONE;
	
	if(HAL_XSPI_Command(hqspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	// Second command: Write Register
	cmd.Instruction = SEMPER_CMD_WRARG;
	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
	}
	else
	{
		return SEMPER_ERROR;
	}
	
	cmd.Address = reg_addr;
	if(flash1->addr_mode == SEMPER_ADDR_3BYTE)
	{
		cmd.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
	}
	else if(flash1->addr_mode == SEMPER_ADDR_4BYTE)
	{
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}
	
	cmd.DataLength = 1;
	if(HAL_XSPI_Command(hqspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	if(HAL_XSPI_Transmit(hqspi, &write_data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	uint8_t readbuffer;
	HAL_XSPI_Receive(hqspi, &readbuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE);
	return SEMPER_OK;
}

semper_status_t Semper_Read_StatusReg1(SEMPER_HandleTypeDef* flash1, uint8_t* status)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef s_command = {0};

	s_command.Instruction = SEMPER_CMD_RDSR1;
	
	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		s_command.DataMode = HAL_XSPI_DATA_1_LINE;
		s_command.DummyCycles = 0;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		s_command.DataMode = HAL_XSPI_DATA_8_LINES;
		s_command.DummyCycles = 4;
		
		// For 8S mode, we need to handle address (even if dummy)
		s_command.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		s_command.Address = 0x00000000;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}
	
	s_command.DataLength = 1;

	if(HAL_XSPI_Command(hxspi, &s_command, XSPI_TIMEOUT) != HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hxspi, status, XSPI_TIMEOUT) != HAL_OK)
		return SEMPER_ERROR;
		
	return SEMPER_OK;
}


semper_status_t Semper_8Pins_Mode(SEMPER_HandleTypeDef* flash1)
{
	uint8_t cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;
	
	if(cfr5v_data & 0x01) // 检查CFR5V寄存器的第0位
	{
		return SEMPER_OK; // 如果第0位为1，表示8S模式已启用
	}
	
//	Semper_Enter_4Byte_Address_Mode(flash1);
//	flash1->addr_mode = SEMPER_ADDR_4BYTE;
	
	Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, cfr5v_data | 0x01); // 设置CFR5V寄存器的第0位
	flash1->interface_mode = SEMPER_8S_MODE;

//	uint8_t status=0;
//	Semper_Read_StatusReg1(flash1,&status);
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;
	return SEMPER_OK;
}
