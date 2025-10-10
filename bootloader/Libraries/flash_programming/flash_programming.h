#ifndef FLASH_PROGRAMMING_H
#define FLASH_PROGRAMMING_H

#include "Ifx_Types.h"

#define DFLASH_SECTOR_SIZE  (0x1000U)  // 4KB
#define PFLASH_SECTOR_SIZE   (0x4000U) // 16KB

#define DFLASH_PAGE_SIZE    (0x8U)  // 8 Bytes
#define PFLASH_PAGE_SIZE    (0x20U) // 32 Bytes

boolean isAddressInValidRegion(uint32 address, uint32 size);
uint8 Flash_erase(uint32 startAddress, uint32 eraseSize);
uint8 Flash_write(uint32 startAddr, uint8* data, uint32 dataSize);



#endif /* FLASH_PROGRAMMING_H */