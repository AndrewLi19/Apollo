#include "semperflash_drv.h"
#include "main.h"
#include "gpio.h"
#include "xspi.h"

#define LOADER_OK	0x1
#define LOADER_FAIL	0x0

extern void SystemClock_Config(void);
extern XSPI_HandleTypeDef hxspi1;

// Global SemperFlash handle - initialized once in Init() function
static SEMPER_HandleTypeDef flash1;

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1	: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Init(void) {

	*(uint32_t*)0xE000EDF0=0xA05F0000; //enable interrupts in debug


	SystemInit();

	SCB->VTOR = 0x341C0000 | 0x200;

	__set_PRIMASK(0); //enable interrupts

	HAL_Init();

    SystemClock_Config();

//    MX_GPIO_Init();

	__HAL_RCC_XSPI1_FORCE_RESET();  //completely reset peripheral
    __HAL_RCC_XSPI1_RELEASE_RESET();
	// Initialize XSPI1
	MX_XSPI1_Init();

	// Initialize SemperFlash driver using global flash1
	if (Semper_Flash_Init(&flash1, &hxspi1) != SEMPER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

//	HAL_Delay(100);
	if (Semper_EnableMemoryMappedMode(&flash1) != SEMPER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	__set_PRIMASK(1); //disable interrupts
//	Read(0x90000000, 4, (uint8_t*)&flash1); // Test read
	return LOADER_OK;
}

//int Read(uint32_t Address, uint32_t Size, uint8_t *Buffer)
//{
//	__set_PRIMASK(0); //enable interrupts

//	MX_XSPI1_Init();
//	Semper_Flash_Init(&flash1, &hxspi1);

//	if(HAL_XSPI_Abort(&hxspi1) != HAL_OK)
//	{
//		__set_PRIMASK(1); //disable interrupts
//		return LOADER_FAIL;
//	}

// 	if (Semper_Read_Memory(&flash1, (Address & (0x0fffffff)), Buffer, Size) != SEMPER_OK)
// 	{
// //		__set_PRIMASK(1); //disable interrupts
// 		return LOADER_FAIL;
// 	}
//	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

//	if (Semper_EnableMemoryMappedMode(&flash1) != SEMPER_OK)
//	{
//		return LOADER_FAIL;
//	}

//	memcpy(Buffer, (uint8_t*)Address, Size);
//	__set_PRIMASK(1); //disable interrupts
//	return LOADER_OK;
//}


/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1		: Operation succeeded
 * @retval  LOADER_FAIL = 0	: Operation failed
 */
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer) {

	__set_PRIMASK(0); //enable interrupts

	if(HAL_XSPI_Abort(&hxspi1) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}


	if (Semper_Prog_Page(&flash1, (Address & (0x0fffffff)),(uint8_t*) buffer,Size) != SEMPER_OK)
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

	if(HAL_XSPI_Abort(&hxspi1) != HAL_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}


	if (Semper_Erase_Sector(&flash1, EraseStartAddress, EraseEndAddress) != SEMPER_OK)
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

	// __set_PRIMASK(0); //enable interrupts

	// if(HAL_XSPI_Abort(&hxspi1) != HAL_OK)
	// {
	// 	__set_PRIMASK(1); //disable interrupts
	// 	return LOADER_FAIL;
	// }


	// if (CSP_QSPI_Erase_Chip() != HAL_OK)
	// {
	// 	 __set_PRIMASK(1); //disable interrupts
	// 	return LOADER_FAIL;
	// }

	// __set_PRIMASK(1); //disable interrupts
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

	if (Semper_EnableMemoryMappedMode(&flash1) != SEMPER_OK)
	{
		__set_PRIMASK(1); //disable interrupts
		return LOADER_FAIL;
	}

	checksum = CheckSum((uint32_t) MemoryAddr + (missalignement & 0xf),
			Size - ((missalignement >> 16) & 0xF), InitVal);
	while (Size > VerifiedData) {
		if (*(uint8_t*) MemoryAddr++
				!= *((uint8_t*) RAMBufferAddr + VerifiedData)){
			__set_PRIMASK(1); //disable interrupts
			return ((checksum << 32) + (MemoryAddr + VerifiedData));
		}
		VerifiedData++;
	}

	__set_PRIMASK(1); //disable interrupts
	return (checksum << 32);
}
