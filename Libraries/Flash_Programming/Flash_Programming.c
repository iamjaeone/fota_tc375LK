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
#define CPU_ICR_OFFSET       (0x1FE2CU)
#define CPU0_CSFR_ICR        (*(volatile uint32 *)(CPU0_CSFR_ADDR | CPU_ICR_OFFSET))

#define HOST_COMMAND_INTERPRETER_ADDRESS    (0xAF000000U)


#define FLASH_TYPE_PFLASH0   (2)
#define FLASH_TYPE_PFLASH1   (3)
#define FLASH_TYPE_DFLASH    (0)


#define DMU_ADDRESS          (0xF8040000U)
#define HF_STATUS_OFFSET     (0x0000010U)
#define HF_ERRSR_OFFSET      (0x0000034U)

#define DMU_HF_STATUS      (*(volatile uint32 *)(DMU_ADDRESS | HF_STATUS_OFFSET))
#define DMU_HF_ERRSR       (*(volatile uint32 *)(DMU_ADDRESS | HF_ERRSR_OFFSET))


#define SIZE_OF_ERASE_SECTOR_FUNCTION (0x00000200U)
#define SIZE_OF_WRITE_PFLASH_FUNCTION (0x00000200U)



#define PFLASH_PAGE_LENGTH_BYTES 32
#define DFLASH_PAGE_LENGTH_BYTES 8


/*********************************************************************************************************************/
/*-------------------------------------------------Type Definitions--------------------------------------------------*/
/*********************************************************************************************************************/

typedef union
{
    uint8 (*eraseSector) (uint32 sectorAddress, uint8 sectorSize);
    void (*writePage) (uint32 startingAddr, uint32* data, uint32 dataSize);
} Function;


/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

uint8 eraseSector(uint32 sectorAddress, uint8 sectorSize);
void writePFLASH(uint32 startingAddr, uint32* data, uint32 dataSize);

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

Function g_command;

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/


uint8 Flash_eraseSector(uint32 sectorAddress, uint8 sectorSize)
{
    memcpy((void*)CPU0_PSPR_ADDR, (const void*)eraseSector, SIZE_OF_ERASE_SECTOR_FUNCTION);
    g_command.eraseSector = CPU0_PSPR_ADDR;
    int status = g_command.eraseSector(sectorAddress, sectorSize);
    return status;
}

void Flash_writePage(uint32 startingAddr, uint32* data, uint32 dataSize)
{
    memcpy((void*)CPU0_PSPR_ADDR, (const void*)writePFLASH, SIZE_OF_WRITE_PFLASH_FUNCTION);
    g_command.writePage = CPU0_PSPR_ADDR;
    g_command.writePage(startingAddr, data, dataSize);
}


uint8 eraseSector(uint32 sectorAddress, uint8 sectorSize)
{
    /* Disable global interrupts */
    __disable();

    int flashType;
    if(
        (sectorAddress >= 0x80000000 && sectorAddress < 0x802FFFFF)
        || (sectorAddress >= 0xA0000000 && sectorAddress < 0xA02FFFFF)
    )
    {
        flashType = FLASH_TYPE_PFLASH0;
    }
    else if(
        (sectorAddress >= 0x80300000 && sectorAddress < 0x805FFFFF)
        || (sectorAddress >= 0xA0300000 && sectorAddress < 0xA05FFFFF)
    )
    {
        flashType = FLASH_TYPE_PFLASH1;
    }
    else
    {
        flashType = FLASH_TYPE_DFLASH;
    }



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

    /* Return errors */
    uint8 status = 0;
    if(DMU_HF_ERRSR != 0) status = DMU_HF_ERRSR & 0xFF;


    // /* Enable global interrupts */
    __enable();
    // while((*cpu0Icr & (1 << 15)) == 0) {}
    return status;
}

void writePFLASH(uint32 startingAddr, uint32* data, uint32 dataSize)
{
    __disable();

    int flashType, commandEnterPage;
    if(
        (startingAddr >= 0x80000000 && startingAddr < 0x802FFFFF)
        || (startingAddr >= 0xA0000000 && startingAddr < 0xA02FFFFF)
    )
    {
        flashType = FLASH_TYPE_PFLASH0;
        commandEnterPage = 0x50;
    }
    else if(
        (startingAddr >= 0x80300000 && startingAddr < 0x805FFFFF)
        || (startingAddr >= 0xA0300000 && startingAddr < 0xA05FFFFF)
    )
    {
        flashType = FLASH_TYPE_PFLASH1;
        commandEnterPage = 0x50;
    }
    else
    {
        flashType = FLASH_TYPE_DFLASH;
        commandEnterPage = 0x5D;
    }




	uint32 dataIndex;
    uint32 page;                                                /* Variable to cycle over all the pages             */
    uint32 offset;                                              /* Variable to cycle over all the words in a page   */

    /* Get the current password of the Safety WatchDog module */
    uint16 endInitSafetyPassword = IfxScuWdt_getSafetyWatchdogPasswordInline();

    
	for (dataIndex = 0; dataIndex < dataSize; )
    {
	    /* Write per Pages */
	    // dataIndex * 4Bytes(uint32)
	    uint32 pageAddr = startingAddr + (dataIndex * 4);
	    
	    /* Enter in page mode */
        volatile uint32 *commandEnterPageAddr = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x5554);
        
        *commandEnterPageAddr = commandEnterPage;

        __dsync();

	    /* Wait until page mode is entered */
        while (DMU_HF_STATUS & (1 << flashType)){}

	    /* Write Page (32 bytes) into the assembly buffer */
        volatile uint32 *commandLoadPage = (volatile uint32 *)(HOST_COMMAND_INTERPRETER_ADDRESS | 0x55F0);
	    for(offset = 0; offset < PFLASH_PAGE_LENGTH_BYTES; offset += 0x8)     /* Loop over the page length                */
	    {
		    uint32 word = (dataIndex  < dataSize) ? data[dataIndex++] : 0xFFFFFFFF;
            /* Write into the assembly buffer */
            *commandLoadPage = word;
            word = (dataIndex  < dataSize) ? data[dataIndex++] : 0xFFFFFFFF;
            /* Write into the assembly buffer */
            *(commandLoadPage + 1) = word;
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
    }


    __enable();
}