//#include "semperflash_drv.h"
#include "mt25ql512abb.h"
#include "main.h"
#include "gpio.h"
#include "xspi.h"
#include <stdbool.h>

#define LOADER_OK	0x1
#define LOADER_FAIL	0x0

extern void SystemClock_Config(void);
extern XSPI_HandleTypeDef hxspi2;
extern uint32_t FirstInit;
// Global SemperFlash handle - initialized once in Init() function
//static SEMPER_HandleTypeDef flash1;

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1	: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Init(void){
	helper();
//	uint32_t buffer = *(uint32_t*) 0x70000000;
//	HAL_XSPI_Abort(&hxspi2) != HAL_OK;
//	helper();
//	uint32_t buffer2 = *(uint32_t*) 0x70000000;
}
int helper(void) {
	*(uint32_t*)0xE000EDF0=0xA05F0000; //enable interrupts in debug
	if(FirstInit != 1234)
	{
		SystemInit();
		SCB->VTOR = 0x34180600;

		__set_PRIMASK(0); //enable interrupts

		HAL_Init();

		SystemClock_Config();

		MX_GPIO_Init();

		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

		MX_XSPI2_Init();
		FirstInit = 1234;
	}


	// Initialize SemperFlash driver using global flash1
	if (MT25QU02_Init() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	if(HAL_XSPI_Abort(&hxspi2) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
//	MT25QU02_Enter4BytesAddressMode();
	if (MT25QU02_EnableMemoryMappedModeSTR() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	return LOADER_OK;
}


/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1		: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer) {
//	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	const uint32_t MAX_PROG_SIZE = 4;
	uint32_t current_size, current_addr;
	uint8_t* current_buffer;

	__set_PRIMASK(0); //enable interrupts

	if(HAL_XSPI_Abort(&hxspi2) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	current_addr = Address & 0x0fffffff;
	current_buffer = buffer;
	
	// Process the data in chunks of MAX_PROG_SIZE
	while(Size > 0)
	{
		// Determine the current chunk size
		current_size = (Size > MAX_PROG_SIZE) ? MAX_PROG_SIZE : Size;

		// Program the current chunk
		if (MT25QU02_WriteEnable() != MT25QU02_OK)
		{
			__set_PRIMASK(1); //disable interrupts
			return LOADER_FAIL;
		}
		if (MT25QU02_PageProgram(current_buffer, current_addr, current_size) != MT25QU02_OK)
		{
			__set_PRIMASK(1); //disable interrupts
			return LOADER_FAIL;
		}

		if(MT25QU02_AutoPollingMemReady() != MT25QU02_OK)
		{
			__set_PRIMASK(1); //disable interrupts
			return LOADER_FAIL;
		}
		// Update pointers and remaining size
//		HAL_XSPI_Abort(&hxspi2);
		current_addr += current_size;
		current_buffer += current_size;
		Size -= current_size;
	}

	if (MT25QU02_EnableMemoryMappedModeSTR() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	__set_PRIMASK(1); //disable interrupts
	return LOADER_OK;
}

/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  LOADER_OK = 1		: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress) {

	__set_PRIMASK(0); //enable interrupts

	if(HAL_XSPI_Abort(&hxspi2) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	if (MT25QU02_Erase_Sector(EraseStartAddress & (0x0fffffff), (EraseEndAddress & (0x0fffffff))) != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	if (MT25QU02_EnableMemoryMappedModeSTR() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	__set_PRIMASK(1); //disable interrupts
	return LOADER_OK;
}

/**
 * Description :
 * Mass erase of external flash area
 * Optional command - delete in case usage of mass erase is not planed
 * Inputs    :
 *      none
 * outputs   :
 *     none
 * Note: Optional for all types of device
 */
int MassErase(void) {
	__set_PRIMASK(0); //enable interrupts
	if(HAL_XSPI_Abort(&hxspi2) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	uint32_t Die1Address = 0x00000000;
	uint32_t Die2Address = 0x04000000;
	uint32_t Die3Address = 0x08000000;
	uint32_t Die4Address = 0x0C000000;

	if (ut_erase_die(Die1Address) != LOADER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	if (ut_erase_die(Die2Address) != LOADER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	if (ut_erase_die(Die3Address) != LOADER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	if (ut_erase_die(Die4Address) != LOADER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}
	__set_PRIMASK(1); //disable interrupts
	return LOADER_OK;
}

int ut_erase_die(uint32_t DieAddress)
{
	if(MT25QU02_WriteEnable() != MT25QU02_OK)
	{
		return LOADER_FAIL;
	}
	if(MT25QU02_DieErase(DieAddress) != MT25QU02_OK)
	{
		return LOADER_FAIL;
	}
	if(MT25QU02_AutoPollingMemReady() != MT25QU02_OK)
	{
		return LOADER_FAIL;
	}
	return LOADER_OK;
}

/**
 * Description :
 * Calculates checksum value of the memory zone
 * Inputs    :
 *      StartAddress  : Flash start address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Checksum value
 * Note: Optional for all types of device
 */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal) {
	uint8_t missalignementAddress = StartAddress % 4;
	uint8_t missalignementSize = Size;
	int cnt;
	uint32_t Val;

	StartAddress -= StartAddress % 4;
	Size += (Size % 4 == 0) ? 0 : 4 - (Size % 4);

	for (cnt = 0; cnt < Size; cnt += 4) {
		Val = *(uint32_t*) StartAddress;
		if (missalignementAddress) {
			switch (missalignementAddress) {
			case 1:
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 1;
				break;
			case 2:
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 2;
				break;
			case 3:
				InitVal += (uint8_t) (Val >> 24 & 0xff);
				missalignementAddress -= 3;
				break;
			}
		} else if ((Size - missalignementSize) % 4 && (Size - cnt) <= 4) {
			switch (Size - missalignementSize) {
			case 1:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				InitVal += (uint8_t) (Val >> 16 & 0xff);
				missalignementSize -= 1;
				break;
			case 2:
				InitVal += (uint8_t) Val;
				InitVal += (uint8_t) (Val >> 8 & 0xff);
				missalignementSize -= 2;
				break;
			case 3:
				InitVal += (uint8_t) Val;
				missalignementSize -= 3;
				break;
			}
		} else {
			InitVal += (uint8_t) Val;
			InitVal += (uint8_t) (Val >> 8 & 0xff);
			InitVal += (uint8_t) (Val >> 16 & 0xff);
			InitVal += (uint8_t) (Val >> 24 & 0xff);
		}
		StartAddress += 4;
	}

	return (InitVal);
}

/**
 * Description :
 * Verify flash memory with RAM buffer and calculates checksum value of
 * the programmed memory
 * Inputs    :
 *      FlashAddr     : Flash address
 *      RAMBufferAddr : RAM buffer address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Operation failed (address of failure)
 *     R1             : Checksum value
 * Note: Optional for all types of device
 */
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size,uint32_t missalignement){

	__set_PRIMASK(0); //enable interrupts
	uint32_t VerifiedData = 0, InitVal = 0;
	uint64_t checksum;
	Size *= 4;
	if(HAL_XSPI_Abort(&hxspi2) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	if (MT25QU02_AutoPollingMemReady() != MT25QU02_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	checksum = CheckSum((uint32_t) MemoryAddr + (missalignement & 0xf),
			Size - ((missalignement >> 16) & 0xF), InitVal);
	while (Size > VerifiedData) {
		uint32_t current_addr = MemoryAddr;
		if (*(uint8_t*) current_addr != *((uint8_t*) RAMBufferAddr + VerifiedData)){
		     return ((checksum << 32) + current_addr); // 返回真实的失败地址
		}
		MemoryAddr++;
		VerifiedData++;

//		if (*(uint8_t*) MemoryAddr++
//				!= *((uint8_t*) RAMBufferAddr + VerifiedData)){
//			__set_PRIMASK(1); //disable interrupts
//			return ((checksum << 32) + (MemoryAddr + VerifiedData));
//		}
		VerifiedData++;
	}

	__set_PRIMASK(1); //disable interrupts
	return (checksum << 32);
}
