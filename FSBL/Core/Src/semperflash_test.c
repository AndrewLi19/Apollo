/*
 * semperflash_test.c
 *
 *  Created on: Jun 28, 2025
 *      Author: DELL
 */

#include "semperflash_test.h"

void Semper_Test_Init(SEMPER_HandleTypeDef* flash1,XSPI_HandleTypeDef* hxspi)
{
    flash1->addr_mode = SEMPER_ADDR_3BYTE;
#ifdef SEMPER_8S_TEST
    flash1->interface_mode = SEMPER_8S_MODE; // 使用8S模式进行测试
#else
    flash1->interface_mode = SEMPER_1S_MODE; // 默认使用1S
#endif
	flash1->xspi_handler = hxspi;
}


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
    uint8_t read_data[128] = {0}; // 读取数据缓冲区
    uint32_t read_addr = 0x00010000; // 假设从地址0x00001000开始读取

#ifdef SEMPER_8S_TEST
    if (Semper_Read_Memory(flash1, read_addr, read_data, 64) == SEMPER_OK) {
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
#else
    if (Semper_Read_Memory_1S(flash1, read_addr, read_data, 64) == SEMPER_OK) {
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
#endif
}

void Semper_Write_Reg_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t cfr2v_data = 0;
	if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR2V, &cfr2v_data) != SEMPER_OK)
    {
        printf("Failed to read CFR2V register.\n");
        return;
    }


	if(Semper_Write_Reg(flash1, SEMPER_ADDR_CFR2V, 0x08)!= SEMPER_OK)
    {
        printf("Failed to write CFR2V register.\n");
        return;
    }

    // 读取CFR2V寄存器以验证写入
    uint8_t new_cfr2v_data = 0;
    if(Semper_Read_Reg(flash1, SEMPER_ADDR_CFR2V, &new_cfr2v_data) != SEMPER_OK)
    {
        printf("Failed to read CFR2V register after write.\n");
        return;
    }

    if (new_cfr2v_data == 0x08) {
        // 成功写入CFR2V寄存器
        printf("CFR2V Register written successfully.\n");
    } else {
        // 写入CFR2V寄存器失败
        printf("Failed to write CFR2V Register.\n");
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

//    if (Semper_Erase_Sector(flash1, write_addr) != SEMPER_OK) {
//        printf("Failed to erase sector before programming page.\n");
//        return;
//    }
    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot program page.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return;
    }

    uint8_t read_data0[256] = {0}; // 读取数据缓冲区
    if (Semper_Read_Memory(flash1, write_addr, read_data0, sizeof(read_data0)) != SEMPER_OK) {
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
        if (Semper_Read_Memory(flash1, write_addr, read_data, 256) == SEMPER_OK) {
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
    if (Semper_Read_Memory(flash1, write_addr, read_data_after_erase, sizeof(read_data_after_erase)) == SEMPER_OK) {
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
    uint32_t erase_addr = 0x00010000; // 假设要擦除的扇区地址
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
        if (Semper_Read_Memory(flash1, erase_addr, read_data, sizeof(read_data)) == SEMPER_OK) {
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


void Semper_Switch_Mode_Test(SEMPER_HandleTypeDef* flash1)
{
    // 先设置为1S模式
//    flash1->interface_mode = SEMPER_1S_MODE;
    flash1->addr_mode = SEMPER_ADDR_3BYTE;

    uint8_t crf5v_data = 0;
    if (Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &crf5v_data) != SEMPER_OK) {
        printf("Failed to read CFR5V register.\n");
        return;
    }
//	Semper_Write_Reg(flash1, SEMPER_ADDR_CFR5V, crf5v_data | 0x01); // 设置CFR5V寄存器的第0位

    Semper_8Pins_Mode(flash1); // 切换到8S模式
    flash1->interface_mode = SEMPER_8S_MODE;

    uint32_t flash_id = 0;
    if (Semper_Read_FlashID(flash1, &flash_id) != SEMPER_OK) {
        printf("Failed to read Flash ID in 8S mode.\n");
        return;
    }
    printf("Flash ID in 8S mode: 0x%08X\n", flash_id);
//
//    uint8_t newcrf5v_data = 0;
//    if (Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &newcrf5v_data) != SEMPER_OK) {
//        printf("Failed to read CFR5V register in 8S mode.\n");
//        return;
//    }
    
    // if (newcrf5v_data == crf5v_data) {
    //     printf("CFR5V register data remains unchanged after switching to 8S mode.\n");
    // } else {
    //     printf("CFR5V register data changed after switching to 8S mode.\n");
    // }

    // Semper_1Pin_Mode(flash1); // 切换回1S模式
    // flash1->interface_mode = SEMPER_1S_MODE;

    // uint8_t crf5v_data_after = 0;
    // if (Semper_Read_Reg(flash1, SEMPER_ADDR_CFR5V, &crf5v_data_after) != SEMPER_OK) {
    //     printf("Failed to read CFR5V register in 1S mode.\n");
    //     return;
    // }

    // if (crf5v_data_after == crf5v_data) {
    //     printf("CFR5V register data remains unchanged after switching back to 1S mode.\n");
    // } else {
    //     printf("CFR5V register data changed after switching back to 1S mode.\n");
    // }
}

void Semper_Read_ID_Test(SEMPER_HandleTypeDef* flash1)
{
    uint32_t flash_id = 0;
    if (Semper_Read_FlashID(flash1, &flash_id) == SEMPER_OK) {
        // 成功读取Flash ID
        printf("Flash ID: 0x%08X\n", flash_id);
    } else {
        // 读取Flash ID失败
        printf("Failed to read Flash ID.\n");
    }
}

void Semper_Read_Memory_8S_Test(SEMPER_HandleTypeDef* flash1)
{
    uint8_t read_data[128] = {0}; // 读取数据缓冲区
    uint32_t read_addr = 0x00010000; // 假设从地址0x00001000开始读取
    uint8_t write_data[128] = {0}; // 假设要写入128字节数据
    for (int i = 0; i < sizeof(write_data); i++) {
        write_data[i] = i; // 填充数据
    }
    if(Semper_Erase_Sector(flash1, read_addr) != SEMPER_OK)
    {
        printf("Failed to erase sector before reading in 8S mode.\n");
        return;
    }
    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot read back data before reading in 8S mode.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return;
    }
    if (Semper_Prog_Page(flash1, read_addr, write_data, sizeof(write_data)) == SEMPER_OK) {
        // 成功编程页面
        printf("Page programmed successfully at address 0x%08X.\n", read_addr);
    } else {
        printf("Page programming failed at address 0x%08X.\n", read_addr);
        return;
    }
    if(Semper_8Pins_Mode(flash1) != SEMPER_OK)
    {
        printf("Failed to switch to 8S mode.\n");
        return;
    }
    if (Semper_Read_Memory_8S(flash1, read_addr, read_data, sizeof(read_data)) == SEMPER_OK) {
        // 成功读取数据
        printf("Read Memory Data from 0x%08X in 8S mode:\n", read_addr);
        for (int i = 0; i < sizeof(read_data); i++) {
            printf("0x%02X ", read_data[i]);
            if ((i + 1) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    } else {
        // 读取数据失败
        printf("Failed to read memory data in 8S mode.\n");
    }
}
    
void Semper_Memory_Mapped_Mode_Test(SEMPER_HandleTypeDef* flash1)
{
    if(Semper_Erase_Sector(flash1, 0x00010000) != SEMPER_OK)
    {
        printf("Failed to erase sector before enabling Memory Mapped Mode.\n");
        return;
    }

    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot read back data before enabling Memory Mapped Mode.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return;
    }
    uint8_t write_data[64]; // 假设要写入64字节数据
    for (int i = 0; i < sizeof(write_data); i++) {
        write_data[i] = i; // 填充数据
    }
    uint32_t write_addr = 0x00010000; // 假设从地址0x00001000开始写入

    if (Semper_Prog_Page(flash1, write_addr, write_data, sizeof(write_data)) != SEMPER_OK) {
        printf("Failed to program page before enabling Memory Mapped Mode.\n");
        return SEMPER_ERROR;
    }

    if(Semper_Poll_RDYBSY(flash1) != SEMPER_OK)
    {
        printf("Device is busy, cannot read back data before enabling Memory Mapped Mode.\n");
        Semper_Clear_Prog_Err_Flag(flash1); // 清除编程错误标志
        return SEMPER_ERROR;
    }

    uint8_t read_data_indirect[64] = {0}; // 读取数据缓冲区
    if (Semper_Read_Memory(flash1, write_addr, read_data_indirect, sizeof(read_data_indirect)) != SEMPER_OK) {
        printf("Failed to read memory data before enabling Memory Mapped Mode.\n");
        return SEMPER_ERROR;
    }
    printf("Data before enabling Memory Mapped Mode:\n");
    for (int i = 0; i < sizeof(read_data_indirect); i++)
    {
        printf("0x%02X ", read_data_indirect[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    if (Semper_EnableMemoryMappedMode(flash1) == SEMPER_OK) {
        printf("Memory Mapped Mode enabled successfully.\n");
    } else {
        printf("Failed to enable Memory Mapped Mode.\n");
        return SEMPER_ERROR;
    }

    // 读取内存映射模式下的数据
    uint8_t read_data[64] = {0}; // 假设读取64字节数据
    uint32_t read_addr = 0x70010000;

    printf("Read Memory Data in Memory Mapped Mode from 0x%08X:\n", read_addr);
    for (int i = 0; i < sizeof(read_data); i++) {
        printf("0x%02X ", read_data[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
    return SEMPER_OK;
}
