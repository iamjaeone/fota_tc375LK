/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Flash_Programming.h"

#include <string.h>

#include "Ifx_Types.h"
#include "IfxScuWdt.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

#define CPU0_PSPR_ADDR       (0x70100000U)

#define CPU0_CSFR_ADDR       (0xF8810000U)
#define CPU_ICR_OFFSET       (0x0001FE2CU)
#define CPU0_CSFR_ICR        (*(volatile uint32 *)(CPU0_CSFR_ADDR | CPU_ICR_OFFSET))

#define HOST_COMMAND_INTERPRETER_ADDRESS    (0xAF000000U)

#define DMU_ADDRESS          (0xF8040000U)
#define HF_STATUS_OFFSET     (0x00000010U)
#define HF_ERRSR_OFFSET      (0x00000034U)

#define DMU_HF_STATUS        (*(volatile uint32 *)(DMU_ADDRESS | HF_STATUS_OFFSET))
#define DMU_HF_ERRSR         (*(volatile uint32 *)(DMU_ADDRESS | HF_ERRSR_OFFSET))

#define SIZE_OF_ERASE_SECTOR_FUNCTION      (0x00000200U)
#define SIZE_OF_WRITE_PAGE_FUNCTION        (0x00000200U)


/*********************************************************************************************************************/
/*-------------------------------------------------Type Definitions--------------------------------------------------*/
/*********************************************************************************************************************/

typedef enum
{
    FLASH_TYPE_PFLASH0 = 2,
    FLASH_TYPE_PFLASH1 = 3,
    FLASH_TYPE_DFLASH = 0
} FlashType;



/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

uint8 eraseSector(uint32 sectorAddress, uint8 sectorSize, FlashType flashType);
uint8 writePage(uint32 startingAddr, uint32* data, uint32 dataSize, FlashType flashType);

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/



/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

inline static FlashType getFlashType(uint32 address)
{
    if((address >= 0xA0000000) && (address < 0xA02FFFFF))
    {
        return FLASH_TYPE_PFLASH0;
    }

    else if((address >= 0xA0300000) && (address < 0xA05FFFFF))
    {
        return FLASH_TYPE_PFLASH1;
    }
    else if(
        ((address >= 0xAF000000) && (address < 0xAF0FFFFF)) ||    // Data Flash 0 EEPROM (DF0)
        ((address >= 0xAF400000) && (address < 0xAF405FFF))       // Data Flash 0 UCB (DF0)
    )
    {
        return FLASH_TYPE_DFLASH;
    }
    else
    {
        return -1;
    }
}

uint8 Flash_eraseSector(uint32 sectorAddress, uint8 sectorSize)
{
    FlashType flashType = getFlashType(sectorAddress);
    if(flashType == -1) return -1;


    if(flashType == FLASH_TYPE_DFLASH)
    {
        if(sectorAddress % DFLASH_SECTOR_SIZE) return -1;
    }
    else
    {
        if(sectorAddress % PFLASH_SECTOR_SIZE) return -1;
    }

    uint8 (*erase) (uint32 sectorAddress, uint8 sectorSize, FlashType flashType) = eraseSector;

    if(flashType == FLASH_TYPE_PFLASH0)
    {
        memcpy((void*)CPU0_PSPR_ADDR, (const void*)eraseSector, SIZE_OF_ERASE_SECTOR_FUNCTION);
        erase = CPU0_PSPR_ADDR;
    }
    
    return erase(sectorAddress, sectorSize, flashType);
}

uint8 Flash_writePage(uint32 startingAddr, uint32* data, uint32 dataSize)
{
    FlashType flashType = getFlashType(startingAddr);
    if(flashType == -1) return -1;

    if(flashType == FLASH_TYPE_DFLASH)
    {
        if(startingAddr % DFLASH_PAGE_SIZE) return -1;
        if(dataSize % (DFLASH_PAGE_SIZE / 4)) return -1;
    }
    else
    {
        if(startingAddr % PFLASH_PAGE_SIZE) return -1;
        if(dataSize % (PFLASH_PAGE_SIZE / 4)) return -1;
    }


    uint8 (*write) (uint32 startingAddr, uint32* data, uint32 dataSize, FlashType flashType) = writePage;
    if(flashType == FLASH_TYPE_PFLASH0)
    {    
        memcpy((void*)CPU0_PSPR_ADDR, (const void*)writePage, SIZE_OF_WRITE_PAGE_FUNCTION);
        write = CPU0_PSPR_ADDR;
    }

    return write(startingAddr, data, dataSize, flashType);
}

uint8 eraseSector(uint32 sectorAddress, uint8 sectorSize, FlashType flashType)
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

    *eraseCycle1 = sectorAddress;
    *eraseCycle2 = sectorSize;
    *eraseCycle3 = 0x80;
    *eraseCycle4 = 0x50;

    IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);
    __dsync();


    /* Wait until flash is not busy */
    while (DMU_HF_STATUS & (1 << flashType)){}

    // /* Enable global interrupts */
    __enable();
    
    return DMU_HF_ERRSR & 0xFF;
}

uint8 writePage(uint32 startingAddr, uint32* data, uint32 dataSize, FlashType flashType)
{
    __disable();

    uint32 state = 0;
	uint32 dataIndex;
    uint32 page;                                                /* Variable to cycle over all the pages             */
    uint32 offset;                                              /* Variable to cycle over all the words in a page   */

    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();

    
	for (dataIndex = 0; dataIndex < dataSize;)
    {
	    /* Write per Pages */
	    uint32 pageAddr = startingAddr + (dataIndex * 4);
	    
	    /* Enter in page mode */
        volatile uint32 *commandEnterPageAddr = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x5554);
        *commandEnterPageAddr = (flashType == FLASH_TYPE_DFLASH) ? 0x5D : 0x50;

        __dsync();

	    /* Wait until page mode is entered */
        while (DMU_HF_STATUS & (1 << flashType)){}

	    /* Write Page (32 bytes) into the assembly buffer */
        volatile uint32 *commandLoadPage = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x55F0);
        int pageLength = (flashType == FLASH_TYPE_DFLASH) ? DFLASH_PAGE_SIZE : PFLASH_PAGE_SIZE;

	    for(offset = 0; offset < pageLength; offset += 8)     /* Loop over the page length                */
	    {
            *commandLoadPage = data[dataIndex++];
            *(commandLoadPage + 1) = data[dataIndex++];
	    }

        __dsync();

	    /* Write the page */
        volatile uint32 *writePageCycle1 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa50);
        volatile uint32 *writePageCycle2 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaa58);
        volatile uint32 *writePageCycle3 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);
        volatile uint32 *writePageCycle4 = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0xaaa8);

        IfxScuWdt_clearSafetyEndinitInline(endInitSafetyPassword);
        *writePageCycle1 = pageAddr;
        *writePageCycle2 = 0x00;
        *writePageCycle3 = 0xa0;
        *writePageCycle4 = 0xaa;
        IfxScuWdt_setSafetyEndinitInline(endInitSafetyPassword);

        __dsync();
	
	    /* Wait until the page is written in the Program Flash memory */
	    while (DMU_HF_STATUS & (1 << flashType)){}

        if(DMU_HF_ERRSR != 0)
        {
            state |= DMU_HF_ERRSR & 0xFF;
        }
    }


    __enable();

    return state;
}