/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "flash_programming.h"

#include <string.h>

#include "Ifx_Types.h"
#include "IfxScuWdt.h"

#include "IfxCpu.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

#define CPU0_PSPR_ADDR (0x70100000U)

#define CPU0_CSFR_ADDR (0xF8810000U)
#define CPU_ICR_OFFSET (0x0001FE2CU)
#define CPU0_CSFR_ICR (*(volatile uint32 *)(CPU0_CSFR_ADDR | CPU_ICR_OFFSET))

#define HOST_COMMAND_INTERPRETER_ADDRESS (0xAF000000U)

#define COMMAND_LOAD_PAGE_UPPER (*(volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x55F0))
#define COMMAND_LOAD_PAGE_LOWER (*(volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x55F4))

#define DMU_ADDRESS (0xF8040000U)
#define HF_STATUS_OFFSET (0x00000010U)
#define HF_ERRSR_OFFSET (0x00000034U)

#define DMU_HF_STATUS (*(volatile uint32 *)(DMU_ADDRESS | HF_STATUS_OFFSET))
#define DMU_HF_ERRSR (*(volatile uint32 *)(DMU_ADDRESS | HF_ERRSR_OFFSET))

#define SIZE_OF_ERASE_SECTOR_FUNCTION (0x1000U)
#define SIZE_OF_WRITE_PAGE_FUNCTION (0x1000U)

/*********************************************************************************************************************/
/*-------------------------------------------------Type Definitions--------------------------------------------------*/
/*********************************************************************************************************************/

typedef enum
{
    FLASH_TYPE_PFLASH0 = 2,
    FLASH_TYPE_PFLASH1 = 3,
    FLASH_TYPE_DFLASH = 0
} FlashType;

typedef struct
{
    uint32 startAddress;
    uint32 endAddress;
} MemoryRegion;

const MemoryRegion VALID_MEMORY_REGIONS[] = {
    {0xA0020000, 0xA0300000}    // Application A: 3MB - 128KB
    , {0xA0300000, 0xA0600000}              // Application B: 3MB
    , {0xAF000000, 0xAF100000}              // Data Flash 0: 1MB
};

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

uint8 eraseSector(uint32 sectorAddress, uint8 sectorSize, FlashType flashType);
uint8 writePage(uint32 startAddr, const uint8 *data, uint32 dataSize, FlashType flashType);

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

inline boolean isAddressInValidRegion(uint32 address, uint32 size)
{
    for (size_t i = 0; i < sizeof(VALID_MEMORY_REGIONS) / sizeof(VALID_MEMORY_REGIONS[0]); i++)
    {
        if (address >= VALID_MEMORY_REGIONS[i].startAddress &&
            (address + size) <= VALID_MEMORY_REGIONS[i].endAddress)
        {
            return TRUE;
        }
    }
    return FALSE;
}

inline static FlashType getFlashType(uint32 address)
{
    if ((address >= 0xA0000000) && (address < 0xA0300000))
    {
        return FLASH_TYPE_PFLASH0;
    }

    else if ((address >= 0xA0300000) && (address < 0xA0600000))
    {
        return FLASH_TYPE_PFLASH1;
    }
    else if (
        ((address >= 0xAF000000) && (address < 0xAF100000)) || // Data Flash 0 EEPROM (DF0)
        ((address >= 0xAF400000) && (address < 0xAF406000))    // Data Flash 0 UCB (DF0)
    )
    {
        return FLASH_TYPE_DFLASH;
    }
    else
    {
        return -1;
    }
}

uint8 Flash_Erase(uint32 startAddress, uint32 eraseSize)
{
    FlashType flashType = getFlashType(startAddress);
    if (flashType == -1)
        return -1;

    uint32 sectorSize = (flashType == FLASH_TYPE_DFLASH) ? DFLASH_SECTOR_SIZE : PFLASH_SECTOR_SIZE;

    startAddress -= startAddress % sectorSize;
    eraseSize = (eraseSize + sectorSize - 1) / sectorSize;

    uint8 (*erase)(uint32 startAddress, uint8 eraseSectorCount, FlashType flashType) = eraseSector;

    if (flashType == FLASH_TYPE_PFLASH0)
    {
        memcpy((void *)CPU0_PSPR_ADDR, (const void *)eraseSector, SIZE_OF_ERASE_SECTOR_FUNCTION);
        erase = CPU0_PSPR_ADDR;
    }

    return erase(startAddress, eraseSize, flashType);
}

uint8 Flash_Write(uint32 startAddr, const uint8 *data, uint32 dataSize)
{
    FlashType flashType = getFlashType(startAddr);
    if (flashType == -1)
        return -1;

    uint8 (*write)(uint32 startAddr, const uint8 *data, uint32 dataSize, FlashType flashType) = writePage;
    if (flashType == FLASH_TYPE_PFLASH0)
    {
        memcpy((void *)CPU0_PSPR_ADDR, (const void *)writePage, SIZE_OF_WRITE_PAGE_FUNCTION);
        write = CPU0_PSPR_ADDR;
    }

    return write(startAddr, data, dataSize, flashType);
}

uint8 eraseSector(uint32 startAddress, uint8 eraseSectorCount, FlashType flashType)
{
    /* Disable global interrupts */
    __disable();

    /* Erase sector */
    volatile uint32 *eraseCycle1 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa50);
    volatile uint32 *eraseCycle2 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa58);
    volatile uint32 *eraseCycle3 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);
    volatile uint32 *eraseCycle4 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);

    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();
    IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);

    *eraseCycle1 = startAddress;
    *eraseCycle2 = eraseSectorCount;
    *eraseCycle3 = 0x80;
    *eraseCycle4 = 0x50;

    IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);
    __dsync();

    /* Wait until flash is not busy */
    while (DMU_HF_STATUS & (1 << flashType))
    {
    }

    // /* Enable global interrupts */
    __enable();

    return DMU_HF_ERRSR & 0xFF;
}

uint8 writePage(uint32 startAddr, const uint8 *data, uint32 dataSize, FlashType flashType)
{
    __disable();

    uint32 dataIndex = 0;
    uint32 state = 0;

    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();

    int pageLength = (flashType == FLASH_TYPE_DFLASH) ? DFLASH_PAGE_SIZE : PFLASH_PAGE_SIZE;

    while (dataIndex < dataSize)
    {
        uint32 editAddr = startAddr + dataIndex;
    
        uint32 offsetInPage = editAddr % pageLength;
        uint32 pageAddrAligned = editAddr - offsetInPage;

        uint8 pageBuffer[PFLASH_PAGE_SIZE];
        
        for(int i = 0; i < pageLength; i++)
        {
            pageBuffer[i] = 0xFF;
        }

        uint32 bytesToWrite = ((pageLength - offsetInPage) < (dataSize - dataIndex)) ? (pageLength - offsetInPage) : (dataSize - dataIndex);

        while(bytesToWrite--)
        {
            pageBuffer[offsetInPage++] = data[dataIndex++];
        }

        /* Enter in page mode */
        volatile uint32 *commandEnterPageAddr = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x5554);
        *commandEnterPageAddr = (flashType == FLASH_TYPE_DFLASH) ? 0x5D : 0x50;

        __dsync();

        /* Wait until page mode is entered */
        while (DMU_HF_STATUS & (1 << flashType)) {}


        /* Load the page buffer */
        for(int pageOffset = 0; pageOffset < pageLength; pageOffset += 8)
        {
            COMMAND_LOAD_PAGE_UPPER = (uint32) (pageBuffer[pageOffset])
                                            | (pageBuffer[pageOffset + 1] << 8)
                                            | (pageBuffer[pageOffset + 2] << 16)
                                            | (pageBuffer[pageOffset + 3] << 24);
            COMMAND_LOAD_PAGE_LOWER = (uint32) (pageBuffer[pageOffset + 4])
                                            | (pageBuffer[pageOffset + 5] << 8)
                                            | (pageBuffer[pageOffset + 6] << 16)
                                            | (pageBuffer[pageOffset + 7] << 24);
        }

        __dsync();

        /* Write the page */
        volatile uint32 *writePageCycle1 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa50);
        volatile uint32 *writePageCycle2 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa58);
        volatile uint32 *writePageCycle3 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);
        volatile uint32 *writePageCycle4 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);

        IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);
        *writePageCycle1 = pageAddrAligned;
        *writePageCycle2 = 0x00;
        *writePageCycle3 = 0xa0;
        *writePageCycle4 = 0xaa;
        IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);

        __dsync();

        /* Wait until the page is written in the Program Flash memory */
        while (DMU_HF_STATUS & (1 << flashType)) {}

        if (DMU_HF_ERRSR != 0)
        {
            state |= DMU_HF_ERRSR & 0xFF;
            break;
        }
    }

    __enable();

    return state;
}