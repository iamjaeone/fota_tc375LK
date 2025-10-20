/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "bcb.h"
#include "flash_programming.h"


#include "IfxScuWdt.h"
#include "IfxPort_reg.h"

#include <string.h>
#include "Ifx_Assert.h"


/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define BCB_MAGIC_NUMBER 0x4A57



#define BCB_START_ADDRESS (0xAF000000) // First 4KB of D-Flash
#define BCB_MAX_COUNT (DFLASH_SECTOR_SIZE / sizeof(BlockControlBlock))


#define SCU_BASE_ADDRESS 0xF0036000
#define SCU_RESET_STATUS_OFFSET 0x0050
#define SCU_RESETCON_OFFSET 0x0058

#define SCU_RESET_STATUS (*(const volatile uint32*)(SCU_BASE_ADDRESS | SCU_RESET_STATUS_OFFSET))

#define SCU_RESET_STATUS_SMU_MASK (1 << 3)


#define WDT_RELOAD_VALUE 65475 // 10ms @ 6KHz

/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/


typedef union
{
    struct {
        uint32 magicNumber: 16;
        BCB_APPLICATION_TYPE bootApplication: 1; // 0: Application A | 1: Application B
        uint32 updateRequest: 1;
        uint32 isAppAValid: 1;
        uint32 _reserved: 1;
        uint32 isAppBValid: 1;
        uint32 bootAttempt: 4;
        uint32 updateInProgress: 1;
        uint64 reserved: 38;
    } fields;
    uint64 raw;
} BlockControlBlock;  

// 버전정보
// 롤백 - 업데이트 실패


/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

static uint32 g_bcbIndexToWrite;
static BlockControlBlock g_currentBcb;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

static int writeCurrentBcb()
{
    g_currentBcb.fields.magicNumber = BCB_MAGIC_NUMBER;
    if(g_bcbIndexToWrite == 0)
    {
        int result = Flash_Erase(BCB_START_ADDRESS, DFLASH_SECTOR_SIZE);
        uint8 data[DFLASH_SECTOR_SIZE];
        memset(data, 0x00, DFLASH_SECTOR_SIZE);
        result |= Flash_Write(BCB_START_ADDRESS, data, DFLASH_SECTOR_SIZE);
        if(result != 0)
        {
            return result;
        }
    }
    int result = Flash_Write(BCB_START_ADDRESS + (g_bcbIndexToWrite * sizeof(BlockControlBlock)), (uint8*)&g_currentBcb, sizeof(BlockControlBlock));

    if(memcmp((const void*)(BCB_START_ADDRESS + (g_bcbIndexToWrite * sizeof(BlockControlBlock))), (const void*)&g_currentBcb, sizeof(BlockControlBlock)) != 0)
    {
        Flash_Erase(BCB_START_ADDRESS, DFLASH_SECTOR_SIZE);
        Flash_Write(BCB_START_ADDRESS, (uint8*)&g_currentBcb, sizeof(BlockControlBlock));

        while(memcmp((const void*)(BCB_START_ADDRESS), (const void*)&g_currentBcb, sizeof(BlockControlBlock)) != 0)
        {
            P00_IOCR4.B.PC5 = 0x10; // Set P00.5 as output (LED1)
            P00_IOCR4.B.PC6 = 0x10; // Set P00.6 as output (LED2)
            
            P00_OUT.B.P5 = !P00_OUT.B.P5; // LED1 ON
            P00_OUT.B.P6 = !P00_OUT.B.P6; // LED2 ON
            volatile uint32 i = 100000000;
            while(i--);    
            Flash_Erase(BCB_START_ADDRESS, DFLASH_SECTOR_SIZE);
            Flash_Write(BCB_START_ADDRESS, (uint8*)&g_currentBcb, sizeof(BlockControlBlock));
        }
        g_bcbIndexToWrite = 1;
        return 0;
    }

    g_bcbIndexToWrite = (g_bcbIndexToWrite + 1) % BCB_MAX_COUNT;
    return result;
}

static void applicationReset()
{
    __disable();
    IfxScuWdt_clearSafetyEndinit(IfxScuWdt_getSafetyWatchdogPassword());
    volatile uint32* resetCon = (uint32*)(SCU_BASE_ADDRESS + SCU_RESETCON_OFFSET);
    *resetCon = (*resetCon & ~(0x3 << 6)) | (1 << 7); // bit[7:6] = 0b10: Application Reset
    while(1);
}

static void jumpToApplication()
{
    void (*appEntry)(void) = (void*) (g_currentBcb.fields.bootApplication == BCB_APPLICATION_TYPE_A ? BCB_APPLICATION_A_START_ADDRESS : BCB_APPLICATION_B_START_ADDRESS);

    __disable();
    /* Add CAN Interrupts Disable */

    IfxScuWdt_changeCpuWatchdogReload(IfxScuWdt_getCpuWatchdogPassword(), WDT_RELOAD_VALUE);
    IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_enableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    appEntry();
}

static const BlockControlBlock* initiateGlobalVariables()
{
    IFX_ASSERT(IFX_VERBOSE_LEVEL_FATAL, sizeof(BlockControlBlock) == 8);

    g_bcbIndexToWrite = -1;

    for(uint32 bcbIndex = 0; bcbIndex < BCB_MAX_COUNT; bcbIndex++)
    {
        const BlockControlBlock* lastBcb = (BlockControlBlock*)(BCB_START_ADDRESS + bcbIndex * sizeof(BlockControlBlock));
        if(lastBcb->fields.magicNumber != BCB_MAGIC_NUMBER)
        {
            break;
        }
        g_bcbIndexToWrite = bcbIndex;
    }

    if(g_bcbIndexToWrite == -1)
    {
        // No valid BCB found, Run Application A by default
        g_bcbIndexToWrite = 0;

        g_currentBcb.fields.bootApplication = BCB_APPLICATION_TYPE_A;
        g_currentBcb.fields.isAppAValid = 0;
        g_currentBcb.fields.isAppBValid = 0;
        g_currentBcb.fields.updateRequest = 0;
        g_currentBcb.fields.bootAttempt = 1;
        g_currentBcb.fields.updateInProgress = 0;

        writeCurrentBcb();
        BCB_HardReset();
    }
    if(g_bcbIndexToWrite >= BCB_MAX_COUNT)
    {
        g_bcbIndexToWrite = BCB_MAX_COUNT - 1;
    }

    const BlockControlBlock* lastBcb = (BlockControlBlock*)(BCB_START_ADDRESS + (g_bcbIndexToWrite * sizeof(BlockControlBlock)));
    memcpy(&g_currentBcb, lastBcb, sizeof(BlockControlBlock));
    
    g_bcbIndexToWrite = (g_bcbIndexToWrite + 1) % BCB_MAX_COUNT;


    return lastBcb;
}

void BCB_UpdateRollbackHandler()
{
    g_currentBcb.fields.updateInProgress = 0;
    g_currentBcb.fields.updateRequest = 0;
    g_currentBcb.fields.bootAttempt = 1;
    writeCurrentBcb();
    jumpToApplication();
}

void BCB_BootLoaderInit()
{
    IFX_ASSERT(IFX_VERBOSE_LEVEL_FATAL, sizeof(BlockControlBlock) == 8);

    const BlockControlBlock* lastBcb = initiateGlobalVariables();

    if(lastBcb->fields.updateRequest)
    {
        if(lastBcb->fields.updateInProgress)
        {
            BCB_UpdateRollbackHandler();
            return;
        }

        g_currentBcb.fields.updateInProgress = 1;
        writeCurrentBcb();
        return;
    }

    if(lastBcb->fields.bootApplication == BCB_APPLICATION_TYPE_A && lastBcb->fields.isAppAValid && lastBcb->fields.bootAttempt == 0)
    {
        jumpToApplication();
        return;
    }
    else if(lastBcb->fields.bootApplication == BCB_APPLICATION_TYPE_B && lastBcb->fields.isAppBValid && lastBcb->fields.bootAttempt == 0)
    {
        jumpToApplication();
        return;
    }


    if(SCU_RESET_STATUS & SCU_RESET_STATUS_SMU_MASK)
    {
        if(lastBcb->fields.bootAttempt >= 3)
        {
            // Invalid
            if(lastBcb->fields.bootApplication == BCB_APPLICATION_TYPE_A)
            {
                g_currentBcb.fields.isAppAValid = 0;
            }
            else
            {
                g_currentBcb.fields.isAppBValid = 0;
            }

            if(lastBcb->fields.isAppAValid)
            {
                g_currentBcb.fields.bootApplication = BCB_APPLICATION_TYPE_A;
            }
            else if(lastBcb->fields.isAppBValid)
            {
                g_currentBcb.fields.bootApplication = BCB_APPLICATION_TYPE_B;
            }
            else
            {
                P00_IOCR4.B.PC5 = 0x10; // P00.5 as output (LED1)
                P00_IOCR4.B.PC6 = 0x10; // P00.6 as output (LED2)
                while(1)
                {
                    P00_OUT.B.P5 = !P00_OUT.B.P5;    // LED1 toggle
                    P00_OUT.B.P6 = !P00_OUT.B.P6;    // LED2 toggle
                    for(volatile uint32 i = 0; i < 1000000; i++);
                }
                return;
            }
            
            g_currentBcb.fields.bootAttempt = 1;
            writeCurrentBcb();
            jumpToApplication();
            return;
        }
        else
        {
            g_currentBcb.fields.bootAttempt += 1;
            writeCurrentBcb();
            jumpToApplication();
            return;
        }
    }
    else
    {
        // Power On Reset or Other Reset
        g_currentBcb.fields.bootAttempt = 1;
        writeCurrentBcb();
        jumpToApplication();
        return;
    }
}

void BCB_ApplicationInit()
{
    IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    const BlockControlBlock* lastBcb = initiateGlobalVariables();
    if(lastBcb->fields.bootAttempt == 0) return;

    if(lastBcb->fields.bootApplication == BCB_APPLICATION_TYPE_A)
    {
        g_currentBcb.fields.isAppAValid = 1;
    }
    else
    {
        g_currentBcb.fields.isAppBValid = 1;
    }
    g_currentBcb.fields.updateRequest = 0;
    g_currentBcb.fields.bootAttempt = 0;
    IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    writeCurrentBcb();
}

void BCB_RequestUpdate()
{
    g_currentBcb.fields.updateRequest = 1;
    g_currentBcb.fields.updateInProgress = 0;
    writeCurrentBcb();
    BCB_HardReset();
}

void BCB_CommitUpdate(uint32 nextBoot)
{
    if(nextBoot == BCB_APPLICATION_TYPE_A)
    {
        g_currentBcb.fields.isAppAValid = 0;
    }
    else
    {
        g_currentBcb.fields.isAppBValid = 0;
    }
    g_currentBcb.fields.bootApplication = nextBoot;
    g_currentBcb.fields.updateRequest = 0;
    g_currentBcb.fields.bootAttempt = 1;
    g_currentBcb.fields.updateInProgress = 0;
    writeCurrentBcb();
}

void BCB_ClearBcb()
{
    uint8 data[DFLASH_SECTOR_SIZE];
    memset(data, 0x00, DFLASH_SECTOR_SIZE);
    int result = Flash_Erase(0xAF000000, DFLASH_SECTOR_SIZE);
    result = Flash_Write(0xAF000000, data, DFLASH_SECTOR_SIZE);
}

void BCB_HardReset()
{
    __disable();
    IfxScuWdt_clearSafetyEndinit(IfxScuWdt_getSafetyWatchdogPassword());
    volatile uint32* resetCon = (uint32*)(SCU_BASE_ADDRESS + SCU_RESETCON_OFFSET);
    *resetCon = (*resetCon & ~(0x3 << 6)) | (1 << 6); // bit[7:6] = 0b01: System Reset
    while(1);
}

BCB_APPLICATION_TYPE BCB_GetCurrentBootApplication()
{
    return g_currentBcb.fields.bootApplication;
}