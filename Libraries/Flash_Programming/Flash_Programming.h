
#ifndef FLASH_PROGRAMMING_H_
#define FLASH_PROGRAMMING_H_

#include "Ifx_Types.h"


#define PFLASH_PAGE_SIZE 32  // 0x20
#define DFLASH_PAGE_SIZE 8

#define PFLASH_SECTOR_SIZE (PFLASH_PAGE_SIZE * 512)  // 0x4000
#define DFLASH_SECTOR_SIZE (DFLASH_PAGE_SIZE * 512)  // 0x1000


/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

uint8 Flash_eraseSector(uint32 sectorAddress, uint8 sectorSize);
uint8 Flash_writePage(uint32 startingAddr, uint32* data, uint32 dataSize);

#endif /* FLASH_PROGRAMMING_H_ */