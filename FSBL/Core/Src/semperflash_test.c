/*
 * semperflash_test.c
 *
 *  Created on: Jun 28, 2025
 *      Author: DELL
 */

#include "semperflash_test.h"

void Semper_Read_Reg_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t reg_data = 0;
    uint32_t reg_addr = SEMPER_ADDR_CFR5V; // 读取CFR5V寄存器

    if (Semper_Read_Reg(flash1, reg_addr, &reg_data) == SEMPER_OK) {
        // 成功读取寄存器数据
        printf("CFR5V Register Data: 0x%02X\n", reg_data);
    } else {
        // 读取寄存器失败
        printf("Failed to read CFR5V register.\n");
    }
}

void Semper_Read_Memory_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t read_data[256] = {0}; // 读取数据缓冲区
    uint32_t read_addr = 0x00010000; // 假设从地址0x00001000开始读取

    if (Semper_Read_Memory_1S(flash1, read_addr, read_data, sizeof(read_data)) == SEMPER_OK) {
        // 成功读取数据
        printf("Read Memory Data from 0x%08X:\n", read_addr);
        for (int i = 0; i < sizeof(read_data); i++) {
            printf("0x%02X ", read_data[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    } else {
        // 读取数据失败
        printf("Failed to read memory data.\n");
    }
}

void Semper_Write_Reg_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t cfr5v_data = 0;
//    flash1->interface_mode = SEMPER_8S_MODE; // 设置为8S模式
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cfr5v_data) != SEMPER_OK)
    {
        printf("Failed to read CFR5V register.\n");
        return;
    }

	if(!(cfr5v_data & 0x01)) // 检查CFR5V寄存器的第0位
	{
	    Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, cfr5v_data | 0x01); // 设置CFR5V寄存器的第0位
	}
		
	flash1->interface_mode = SEMPER_8S_MODE;

	uint8_t new_cfr5v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &new_cfr5v_data) != SEMPER_OK)
    {
        printf("Failed to read CFR5V register after write.\n");
        return;
    }

    if (new_cfr5v_data & 0x01) {
        // 成功设置8S模式
        printf("CFR5V Register set to 8S mode successfully.\n");
    } else {
        // 设置8S模式失败
        printf("Failed to set CFR5V Register to 8S mode.\n");
    }

    if(Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, new_cfr5v_data & ~0x01) != SEMPER_OK)
    {
        printf("Failed to clear CFR5V register.\n");
        return;
    }

    flash1->interface_mode = SEMPER_1S_MODE;

    uint8_t cleared_cfr5v_data = 0;
    if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &cleared_cfr5v_data) != SEMPER_OK)
    {
        printf("Failed to read CFR5V register after clearing.\n");
        return;
    }
}

void Semper_Write_Enable_Test(SEMPER_HandleTypeDef* flash1)
{
    if (Semper_Write_Enable(flash1) == SEMPER_OK) {
    	uint8_t status = 0;
        if(Semper_Read_StatusReg1(flash1, &status) == SEMPER_OK)
        {
            // 成功使能写操作
            printf("Write Enable command executed successfully.\n");
            printf("Status Register 1: 0x%02X\n", status);
        } else {
            // 读取状态寄存器失败
            printf("Failed to read Status Register 1 after Write Enable command.\n");
        }
    } else {
        // 使能写操作失败
        printf("Failed to execute Write Enable command.\n");
    }
}

void Semper_Prog_Page_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t write_data[256]; // 假设要写入256字节数据
    for (int i = 0; i < sizeof(write_data); i++) {
        write_data[i] = i; // 填充数据
    }
    
    uint32_t write_addr = 0x00010000; // 假设从地址0x00001000开始写入

    flash1->addr_mode = SEMPER_ADDR_4BYTE;

    if (Semper_Erase_Sector(flash1, write_addr) != SEMPER_OK) {
        printf("Failed to erase sector before programming page.\n");
        return;
    }
    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot program page.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return;
    }

    uint8_t read_data0[256] = {0}; // 读取数据缓冲区
    if (Semper_Read_Memory_1S(flash1, write_addr, read_data0, sizeof(read_data0)) != SEMPER_OK) {
        printf("Failed to read memory data before programming page.\n");
        return;
    }
    
    if (Semper_Prog_Page(flash1, write_addr, write_data, 256) == SEMPER_OK) {
        // 成功编程页面
        printf("Page programmed successfully at address 0x%08X.\n", write_addr);
        
        // 验证写入的数据
        uint8_t read_data[256] = {0};
        if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
        {
            printf("Device is busy, cannot read back data.\n");
            Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
            return;
        }
        if (Semper_Read_Memory_1S(flash1, write_addr, read_data, 256) == SEMPER_OK) {
            printf("Verification of written data:\n");
            for (int i = 0; i < 64; i++) {
                printf("0x%02X ", read_data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
            printf("\n");
        } 
        else {
            printf("Failed to read back the programmed data.\n");
        }
    } else {
        printf("Page programming failed at address 0x%08X.\n", write_addr);
    }

    if(Semper_Erase_Sector(flash1, write_addr) != SEMPER_OK)
    {
        printf("Failed to erase sector after programming page.\n");
        return;
    }
    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot read back data after erasing sector.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return;
    }
    // 再次读取数据以验证擦除结果
    uint8_t read_data_after_erase[256] = {0};
    if (Semper_Read_Memory_1S(flash1, write_addr, read_data_after_erase, sizeof(read_data_after_erase)) == SEMPER_OK) {
        printf("Data after sector erase:\n");
        for (int i = 0; i < sizeof(read_data_after_erase); i++) {
            printf("0x%02X ", read_data_after_erase[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    } else {
        printf("Failed to read data after sector erase.\n");
    }
}

void Semper_Clear_Prog_Err_Flag_Test(SEMPER_HandleTypeDef* flash1)
{
    if (Semper_Clear_Prog_Err_Flag(flash1) == SEMPER_OK) {
        uint8_t status = 0;
        if(Semper_Read_StatusReg1(flash1, status) == SEMPER_OK)
        {
            printf("Programming error flag cleared successfully.\n");
        } else {
            printf("Failed to read status register after clearing programming error flag.\n");
        }
    } else {
        printf("Failed to clear programming error flag.\n");
    }
}

void Semper_Erase_Sector_Test(SEMPER_HandleTypeDef* flash1)
{
    uint32_t erase_addr = 0x00001000; // 假设要擦除的扇区地址
    flash1->addr_mode = SEMPER_ADDR_4BYTE;
    if (Semper_Erase_Sector(flash1, erase_addr) == SEMPER_OK) {
        printf("Sector erased successfully at address 0x%08X.\n", erase_addr);
        
        uint8_t status = 0;
        if (Semper_Poll_RDYBSY(flash1) != SEMPER_OK) {
            printf("Device is busy, cannot read back data.\n");
            Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
            return;
        }
        // 验证擦除结果
        uint8_t read_data[256] = {0};
        if (Semper_Read_Memory_1S(flash1, erase_addr, read_data, sizeof(read_data)) == SEMPER_OK) {
            printf("Data after sector erase:\n");
            for (int i = 0; i < sizeof(read_data); i++) {
                printf("0x%02X ", read_data[i]);
                if ((i + 1) % 16 == 0) {
                    printf("\n");
                }
            }
            printf("\n");
        } else {
            printf("Failed to read data after sector erase.\n");
        }
    } else {
        printf("Sector erase failed at address 0x%08X.\n", erase_addr);
    }
}
