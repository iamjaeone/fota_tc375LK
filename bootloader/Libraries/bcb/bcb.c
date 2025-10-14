/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "bcb.h"
#include "flash_programming.h"
#include "crc16.h"
#include "IfxScuWdt.h"
#include "IfxPort_reg.h"

#include <string.h>
#include <assert.h>

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define BCB_MAGIC_NUMBER 0x4A57

#define BCB_NEXT_BOOT_APPLICATION_A 0
#define BCB_NEXT_BOOT_APPLICATION_B 1

#define BCB_APPLICATION_A_START_ADDRESS 0xA0020000
#define BCB_APPLICATION_B_START_ADDRESS 0xA0300000

#define BCB_START_ADDRESS (0xAF100000 - DFLASH_SECTOR_SIZE) // last 4KB of DFLASH
#define BCB_MAX_COUNT (DFLASH_SECTOR_SIZE / sizeof(BlockControlBlock))


#define SCU_BASE_ADDRESS 0xF0036000
#define SCU_RESET_STATUS_OFFSET 0x0050

#define SCU_RESET_STATUS (*(const volatile uint32*)(SCU_BASE_ADDRESS | SCU_RESET_STATUS_OFFSET))

#define SCU_RESET_STATUS_SMU_MASK (1 << 3)

/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/

typedef union
{
    struct {
        uint32 magicNumber: 16;
        uint32 nextBoot: 1; // 0: Application A | 1: Application B
        uint32 updateRequest: 1;
        uint32 isAppAValid: 1;
        uint32 isAppBValid: 1;
        uint32 bootAttempt: 4;
        uint32 crc: 16;
        uint32 reserved: 24;
    } fields;
    uint64 raw;
} BlockControlBlock;


/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

static g_currentBcbIndex;
static BlockControlBlock g_currentBcb;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

inline static uint16 calculateBcbCrc(const BlockControlBlock* bcb)
{
    uint8 data[3];
    data[0] = (uint8)(bcb->fields.magicNumber >> 8 & 0xFF);
    data[1] = (uint8)(bcb->fields.magicNumber & 0xFF);
    data[2] = (uint8)(bcb->fields.nextBoot << 7)
                | (bcb->fields.updateRequest << 6)
                | (bcb->fields.isAppAValid << 5)
                | (bcb->fields.isAppBValid << 4)
                | (bcb->fields.bootAttempt & 0x0F);
    return crc16(data, sizeof(data));
}


void BCB_Init()
{
    assert(sizeof(BlockControlBlock) == 8);

    g_currentBcbIndex = -1;
    const BlockControlBlock* lastBcb = (BlockControlBlock*) BCB_START_ADDRESS;

    while(g_currentBcbIndex < BCB_MAX_COUNT - 1 && lastBcb->fields.magicNumber == BCB_MAGIC_NUMBER)
    {
        g_currentBcbIndex++;
        lastBcb++;
    }
    if(lastBcb->fields.magicNumber == BCB_MAGIC_NUMBER) g_currentBcbIndex++;

    if(g_currentBcbIndex != -1)
    {
        while(g_currentBcbIndex >= 0 && lastBcb->fields.crc != calculateBcbCrc(lastBcb))
        {
            g_currentBcbIndex--;
            lastBcb--;
        }

        if(lastBcb->fields.crc != calculateBcbCrc(lastBcb))
        {
            g_currentBcbIndex = -1;
        }
    }

    memcpy(&g_currentBcb, lastBcb, sizeof(BlockControlBlock));

    if(g_currentBcbIndex == -1 || g_currentBcbIndex == BCB_MAX_COUNT - 1)
    {
        Flash_Erase(BCB_START_ADDRESS, DFLASH_SECTOR_SIZE);
    }

    if(g_currentBcbIndex == -1)
    {
        g_currentBcb.fields.magicNumber = BCB_MAGIC_NUMBER;
        g_currentBcb.fields.nextBoot = BCB_NEXT_BOOT_APPLICATION_A;
        g_currentBcb.fields.updateRequest = 1;
        g_currentBcb.fields.isAppAValid = 0;
        g_currentBcb.fields.isAppBValid = 0;
        g_currentBcb.fields.bootAttempt = 1;
        g_currentBcb.fields.crc = calculateBcbCrc(&g_currentBcb);
        g_currentBcb.fields.reserved = 0;

        Flash_Write(BCB_START_ADDRESS, (uint8*)&g_currentBcb, sizeof(BlockControlBlock));
        // TODO -- System Reset
    }

    if(lastBcb->fields.updateRequest)
    {
        // Handle update request
        // Send Response to Tester: UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL
        return;
    }


    if(SCU_RESET_STATUS & SCU_RESET_STATUS_SMU_MASK)
    {
        if(g_currentBcb.fields.bootAttempt >= 3)
        {
            if(
                (lastBcb->fields.nextBoot == BCB_NEXT_BOOT_APPLICATION_A && g_currentBcb.fields.isAppBValid)
                || (lastBcb->fields.nextBoot == BCB_NEXT_BOOT_APPLICATION_B && !g_currentBcb.fields.isAppBValid)
            )
            {
                g_currentBcb.fields.updateRequest = 1;
                g_currentBcbIndex = (g_currentBcbIndex + 1) % BCB_MAX_COUNT;
                Flash_Write(BCB_START_ADDRESS + (g_currentBcbIndex + 1) * sizeof(BlockControlBlock), (uint8*)&g_currentBcb, sizeof(BlockControlBlock));

            }
            else
            {
                g_currentBcb.fields.isAppBValid = 0;
            }
            g_currentBcb.fields.bootAttempt = 1;
        }
    }
    else
    {
        g_currentBcb.fields.bootAttempt++;

    }
}




void BCB_JumpToApplication(boolean isAppA)
{
    uint32 appStartAddress = isAppA ? BCB_APPLICATION_A_START_ADDRESS : BCB_APPLICATION_B_START_ADDRESS;

    void (*appEntry)(void) = (void*) appStartAddress;
    
    
    __disable();
    /* Add CAN Interrupts Disable */
    
    P00_OUT.B.P5 = 1;
    P00_OUT.B.P6 = 1;

    IfxScuWdt_changeCpuWatchdogReload(IfxScuWdt_getCpuWatchdogPassword(), 0x0);
    IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_enableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    appEntry();
}
