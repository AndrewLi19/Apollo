/*
 * Dev_Inf.c
 *
 */
#include "Dev_Inf.h"
//#include "semperflash_drv.h"
#include "xspi.h"

#define MEMORY_FLASH_SIZE 0x20000000
#define MEMORY_SECTOR_SIZE 0x00001000
#define MEMORY_PAGE_SIZE 0x00001000

/* This structure contains information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo __attribute__((section(".Dev_info"))) /*const*/ StorageInfo = {
#endif
		"STM32N6+MT25QU02", 	 	         // Device Name + version number
		NOR_FLASH,                  		 // Device Type
		0x70000000,                			 // Device Start Address
		MEMORY_FLASH_SIZE,                 	 // Device Size in Bytes
		MEMORY_PAGE_SIZE,                    // Programming Page Size
		0xFF,                                // Initial Content of Erased Memory

		// Specify Size and Address of Sectors (view example below)
		{
				{ (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE),  // Sector Numbers,
				(uint32_t) MEMORY_SECTOR_SIZE },       //Sector Size
				{ 0x00000000, 0x00000000 }
		}
};
