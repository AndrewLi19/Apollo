/*
 * semperflash.c
 *
 *  Created on: Jun 19, 2025
 *      Author: Li YunFan
 */

#include "../Inc/semperflash_drv.h"

semper_status_t Semper_Flash_Init(SEMPER_HandleTypeDef* flash1,XSPI_HandleTypeDef* hxspi)
{
	flash1->addr_mode = SEMPER_ADDR_3BYTE;
	flash1->interface_mode = SEMPER_1S_MODE;
	flash1->xspi_handler = hxspi;
//	Semper_Reset(flash1); // 复位Flash
//	flash1->interface_mode = SEMPER_1S_MODE;
//	HAL_Delay(1);
	return SEMPER_OK;
}

semper_status_t Semper_Read_FlashID(SEMPER_HandleTypeDef* flash1,uint32_t* flashID)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
    uint8_t rx_buf[3] = {0};
    uint32_t dummy_addr = 0x00000000;
    XSPI_RegularCmdTypeDef  s_command = {0};

    s_command.Instruction = SEMPER_CMD_READID;
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
		s_command.Address = dummy_addr;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		s_command.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		s_command.DummyCycles = 4;
    }
    else
    {
    	return SEMPER_ERROR;
    }
    s_command.DataLength = 3;  // 接收3字节ID数据


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

	uint8_t rd_buffer[30] = {0}; // Buffer to hold read data

    if(flash1->interface_mode == SEMPER_1S_MODE)
    {
		cmd.Instruction = SEMPER_CMD_RDARG;     //RDARG_C_0操作码
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
		if(flash1->addr_mode == SEMPER_ADDR_3BYTE)
		{
			cmd.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
			cmd.Address = reg_addr;
		}
		else if(flash1->addr_mode == SEMPER_ADDR_4BYTE)
		{
			cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
			cmd.Address = (uint32_t) reg_addr;
		}
		else
		{
			return SEMPER_ERROR;
		}
    }
    else if(flash1->interface_mode == SEMPER_8S_MODE)
    {
		cmd.Instruction = 0x6565;
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
		cmd.DummyCycles = 3;
		cmd.Address = (uint32_t) reg_addr;
    }
    else
    {
    	return SEMPER_ERROR;
    }


	cmd.DataLength = 30;
	if(HAL_XSPI_Command(hqspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE)!=HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hqspi, rd_buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE)!=HAL_OK)
		return SEMPER_ERROR;
	*read_data=rd_buffer[0];
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
	Semper_Write_Enable(flash1);

	// Second command: Write Register
	cmd.Instruction = SEMPER_CMD_WRARG;
	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
		if(flash1->addr_mode == SEMPER_ADDR_3BYTE)
		{
			cmd.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
			cmd.Address = reg_addr;
		}
		else if(flash1->addr_mode == SEMPER_ADDR_4BYTE)
		{
			cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
			cmd.Address = (uint32_t) reg_addr;
		}
		else
		{
			return SEMPER_ERROR;
		}
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		cmd.Address = (uint32_t) reg_addr;
	}
	else
	{
		return SEMPER_ERROR;
	}
	
	cmd.Address = reg_addr;
	cmd.DataLength = 1;
	
	if(HAL_XSPI_Command(hqspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	if(HAL_XSPI_Transmit(hqspi, &write_data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;
	return SEMPER_OK;
}

semper_status_t Semper_Read_StatusReg1(SEMPER_HandleTypeDef* flash1, uint8_t* status)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef s_command = {0};

    uint32_t dummy_addr = 0x00000000;

	uint8_t rd_buffer[30] = {0}; // Buffer to hold read data
	
	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		s_command.Instruction = SEMPER_CMD_RDSR1;  // Read Status Register 1 command
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		s_command.DataMode = HAL_XSPI_DATA_1_LINE;
		s_command.DummyCycles = 0;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		s_command.Instruction = 0x0505;  // Read Status Register 1 command in 8S mode
		s_command.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		s_command.DataMode = HAL_XSPI_DATA_8_LINES;
		s_command.DummyCycles = 0;
		
		// For 8S mode, we need to handle address (even if dummy)
		s_command.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		s_command.Address = dummy_addr;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}
	
	s_command.DataLength = 30;

	if(HAL_XSPI_Command(hxspi, &s_command, XSPI_TIMEOUT) != HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hxspi, rd_buffer, XSPI_TIMEOUT) != HAL_OK)
		return SEMPER_ERROR;
	*status = rd_buffer[0];
	return SEMPER_OK;
}


semper_status_t Semper_8Pins_Mode(SEMPER_HandleTypeDef* flash1)
{
	uint8_t cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;
	
	if(cfr5v_data & 0x01) // 检查CFR5V寄存器的第0位
	{
		flash1->interface_mode = SEMPER_8S_MODE;
		return SEMPER_OK; // 如果第0位为1，表示8S模式已启用
	}

	Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, cfr5v_data | 0x01); // 设置CFR5V寄存器的第0位
	flash1->interface_mode = SEMPER_8S_MODE;

	uint8_t new_cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &new_cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;
	return SEMPER_OK;
}

semper_status_t Semper_1Pin_Mode(SEMPER_HandleTypeDef* flash1)
{
	uint8_t cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;

	if(!(cfr5v_data & 0x01)) // 检查CFR5V寄存器的第0位
	{
		flash1->interface_mode = SEMPER_1S_MODE;
		return SEMPER_OK; // 如果第0位为0，表示1S模式已启用
	}

	Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, cfr5v_data & ~0x01); // 清除CFR5V寄存器的第0位
	flash1->interface_mode = SEMPER_1S_MODE;

	uint8_t new_cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &new_cfr5v_data) != SEMPER_OK)
		return SEMPER_ERROR;
	return SEMPER_OK;
}

semper_status_t Semper_Reset(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	cmd.Instruction = SEMPER_CMD_RESET_EN;  // Enable reset command
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
	cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;  // No address for reset command
	cmd.DataMode = HAL_XSPI_DATA_NONE;  // No data for reset command
	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	cmd.Instruction = SEMPER_CMD_RESET;  // Execute reset command
	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Write_Enable(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.Instruction = SEMPER_CMD_WRENB;  // Write Enable command
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.Instruction = 0x0606;
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;  // 8S mode uses 16-bit instruction width
	}
	else
	{
		return SEMPER_ERROR;
	}
	cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;  // No address for write enable command
	cmd.DataMode = HAL_XSPI_DATA_NONE;  // No data for write enable command

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

// semper_status_t Semper_Read_Fast(SEMPER_HandleTypeDef* flash1, uint32_t address, uint8_t* buffer, uint32_t length)
// {
// 	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
// 	XSPI_RegularCmdTypeDef cmd = {0};

// 	if(flash1->interface_mode == SEMPER_1S_MODE)
// 	{
// 		cmd.Instruction = SEMPER_CMD_READ_FAST;  // Fast read command
// 		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
// 		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
// 		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
// 		cmd.DummyCycles = 0;
// 		if(flash1->addr_mode == SEMPER_ADDR_3BYTE)
// 		{
// 			cmd.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
// 		}
// 		else if(flash1->addr_mode == SEMPER_ADDR_4BYTE)
// 		{
// 			cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
// 		}
// 		else
// 		{
// 			return SEMPER_ERROR;
// 		}
// 	}
// 	else if(flash1->interface_mode == SEMPER_8S_MODE)
// 	{
// 		cmd.Instruction = SEMPER_CMD_READ_8S;  // 8S read command
// 		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
// 		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
// 		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
// 		cmd.DummyCycles = 20;  // Adjust dummy cycles for 8S mode
// 		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
// 	}
// 	else
// 	{
// 		return SEMPER_ERROR;
// 	}

// 	cmd.Address = address;

// 	cmd.DataLength = length;

// 	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
// 		return SEMPER_ERROR;

// 	if(HAL_XSPI_Receive(hxspi, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
// 		return SEMPER_ERROR;

// 	return SEMPER_OK;
// }

semper_status_t Semper_Prog_Page(SEMPER_HandleTypeDef* flash1, uint32_t address, uint8_t* data, uint32_t length)
{
	if (Semper_Write_Enable(flash1) != SEMPER_OK)
	{
		return SEMPER_ERROR;
	}
	
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.Instruction = SEMPER_CMD_Prog_Page;  // Page program command
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.Instruction = 0x1212;
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}

	cmd.Address = address;

	cmd.DataLength = length;

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Transmit(hxspi, data, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Poll_RDYBSY(SEMPER_HandleTypeDef* flash1)
{
	uint8_t status = 0;
	do {
		if (Semper_Read_StatusReg1(flash1, &status) != SEMPER_OK)
			return SEMPER_ERROR;
		// while(Semper_Read_StatusReg1(flash1, &status) != SEMPER_OK);
		// Check bit0 (RDYBSY) and error flags
		if (status & 0x01) {
			// Device is busy
			if (status & 0x60) { // Check PRGERR(bit6)/ERSERR(bit5)
				return SEMPER_ERROR;
			}
		}
		HAL_Delay(1); // 1ms polling interval recommended
	} while (status & 0x01); // Wait until RDYBSY=0
	
	return SEMPER_OK;
}

semper_status_t Semper_Erase_Sector(SEMPER_HandleTypeDef* flash1, uint32_t address)
{
	if (Semper_Write_Enable(flash1) != SEMPER_OK)
	{
		return SEMPER_ERROR;
	}
	
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.Instruction = SEMPER_CMD_Erase_Sector;  // Sector erase command
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.Instruction = 0x2121;
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;  // 8S mode uses 16-bit instruction width
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}

	cmd.Address = address;

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Clear_Prog_Err_Flag(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	cmd.Instruction = SEMPER_CMD_CLEAR_PROG_ERR_FLAG;  // Clear programming error flag command
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
	cmd.AddressMode = HAL_XSPI_ADDRESS_NONE;  // No address for this command
	cmd.DataMode = HAL_XSPI_DATA_NONE;  // No data for this command

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Read_Memory_1S(SEMPER_HandleTypeDef* flash1, uint32_t address, uint8_t* buffer, uint32_t length)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	if(flash1->interface_mode == SEMPER_1S_MODE)
	{
		cmd.Instruction = SEMPER_CMD_READ;  // Read command
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		cmd.DataMode = HAL_XSPI_DATA_1_LINE;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
	}
	else
	{
		return SEMPER_ERROR;
	}

	cmd.Address = address;

	cmd.DataLength = length;

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hxspi, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Read_Memory_8S(SEMPER_HandleTypeDef* flash1, uint32_t address, uint8_t* buffer, uint32_t length)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef cmd = {0};

	if(flash1->interface_mode == SEMPER_8S_MODE)
	{
		cmd.Instruction = SEMPER_CMD_READ_8S;  // 8S read command
		cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
		cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		cmd.DataMode = HAL_XSPI_DATA_8_LINES;
		cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		cmd.DummyCycles = 20; // Adjust dummy cycles for 8S mode
	}
	else
	{
		return SEMPER_ERROR;
	}

	cmd.Address = address;

	cmd.DataLength = length;

	if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	if(HAL_XSPI_Receive(hxspi, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		return SEMPER_ERROR;

	return SEMPER_OK;
}

semper_status_t Semper_Read_Memory(SEMPER_HandleTypeDef* flash1, uint32_t address, uint8_t* buffer, uint32_t length)
{
    XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
    XSPI_RegularCmdTypeDef cmd = {0};

    if(flash1->interface_mode == SEMPER_1S_MODE)
    {
        cmd.Instruction = SEMPER_CMD_READ;  // Read command
        cmd.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
        cmd.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
        cmd.DataMode = HAL_XSPI_DATA_1_LINE;
        cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
        // 单线模式不需要额外的空周期
    }
    else if(flash1->interface_mode == SEMPER_8S_MODE)
    {
        cmd.Instruction = SEMPER_CMD_READ_8S;  // 8S read command
        cmd.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
        cmd.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
        cmd.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
        cmd.DataMode = HAL_XSPI_DATA_8_LINES;
        cmd.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
        cmd.DummyCycles = 20; // 8S模式需要的空周期
    }
    else
    {
        return SEMPER_ERROR;
    }

    cmd.Address = address;
    cmd.DataLength = length;

    if(HAL_XSPI_Command(hxspi, &cmd, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return SEMPER_ERROR;

    if(HAL_XSPI_Receive(hxspi, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        return SEMPER_ERROR;

    return SEMPER_OK;
}

semper_status_t Semper_EnableMemoryMappedMode(SEMPER_HandleTypeDef* flash1)
{
	XSPI_HandleTypeDef* hxspi = flash1->xspi_handler;
	XSPI_RegularCmdTypeDef s_command = {0};
	XSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};

	s_command.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
	if (flash1->interface_mode == SEMPER_1S_MODE)
	{
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
		s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
		s_command.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
		s_command.Instruction = SEMPER_CMD_READ; // 1S read command
		s_command.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
		s_command.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS; // 1S mode uses 24-bit address
		s_command.DataMode = HAL_XSPI_DATA_1_LINE;
		s_command.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
		s_command.DummyCycles = 0; // No dummy cycles for 1S mode
  		s_command.DQSMode = HAL_XSPI_DQS_DISABLE;
	}
	else if (flash1->interface_mode == SEMPER_8S_MODE)
	{
		s_command.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
		s_command.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
		s_command.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
		s_command.Instruction = SEMPER_CMD_READ_8S; // 8S read command
		s_command.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
		s_command.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
		s_command.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
		s_command.DataMode = HAL_XSPI_DATA_8_LINES;
		s_command.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
		s_command.DummyCycles = 20; // Adjust dummy cycles for 8S mode
  		s_command.DQSMode = HAL_XSPI_DQS_DISABLE;
	}
	else
	{
		return SEMPER_ERROR; // Unsupported interface mode
	}

	if(HAL_XSPI_Command(hxspi, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return SEMPER_ERROR; // Command execution failed
	}

	s_command.OperationType = HAL_XSPI_OPTYPE_WRITE_CFG;
	s_command.Instruction = 0x1212; // 8S mode page program command
	s_command.DummyCycles = 0; // No dummy cycles for write command
	if(HAL_XSPI_Command(hxspi, &s_command, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return SEMPER_ERROR; // Command execution failed
	}

	s_mem_mapped_cfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;

	if (HAL_XSPI_MemoryMapped(hxspi, &s_mem_mapped_cfg) != HAL_OK)
	{
		return SEMPER_ERROR; // Memory-mapped mode activation failed
	}

	return SEMPER_OK; 
}
