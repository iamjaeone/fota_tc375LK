#ifndef FLASH_PROGRAMMING_H_
#define FLASH_PROGRAMMING_H_

#include "Ifx_Types.h"
/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

uint8 Flash_eraseSector(uint32 sectorAddress, uint8 sectorSize);
void Flash_writePage(uint32 startingAddr, uint32* data, uint32 dataSize);

#endif /* FLASH_PROGRAMMING_H_ */
