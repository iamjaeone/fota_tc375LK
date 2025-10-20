#include "abswap.h"
#include "flash_programming.h"
#include "bcb.h"
#include "Ifx_Assert.h"

#define HOST_COMMAND_SEQUENCE_ADDRESS 0xAF000000
#define UCB_SWAP_IDENTIFIER 0x17

#define DMU_BASE_ADDRESS 0xF8040000
#define DMU_HF_PROTECT_OFFSET 0x000001C
#define DMU_HF_CONFIRM1_OFFSET 0x0000024
#define DMU_HF_PROCONTP_OFFSET 0x0000084

#define SCU_BASE_ADDRESS 0xF0036000
#define SCU_STMEM1_OFFSET 0x184

#define UCB_SWAP_ORIG_ADDRESS 0xAF402E00

#define UCB_SWAP_MAKRER_STANDARD 0x00000055
#define UCB_SWAP_MAKRER_ALTERNATE 0x000000AA

#define UCB_OTP_ORIG_ADDRESS 0xAF404000 // + 0x200 * n for UCB_OTP_ORIGn

#define UCB_OTP_PROCONTP_ADDER 0x1E8
#define UCB_OTP_CONFIRMATION_ADDER 0x1F0

#define UCB_CONFIRMATION_CODE 0x57B5327F

static void disableUCBWriteProtection()
{
    volatile uint32 *commandAddress = (uint32 *)(HOST_COMMAND_SEQUENCE_ADDRESS | 0x553C);
    *commandAddress = UCB_SWAP_IDENTIFIER;
    *commandAddress = 0; // PW 0
    *commandAddress = 0; // PW 1
    *commandAddress = 0; // PW 2
    *commandAddress = 0; // PW 3
    *commandAddress = 0; // PW 4
    *commandAddress = 0; // PW 5
    *commandAddress = 0; // PW 6
    *commandAddress = 0; // PW 7

    volatile uint32 *dmuHfProtect = (uint32 *)(DMU_BASE_ADDRESS | DMU_HF_PROTECT_OFFSET);
    while (!(*dmuHfProtect & (1 << 5)))
    {
    }
}

static void enableWriteProtection()
{
    volatile uint32 *commandAddress = (uint32 *)(HOST_COMMAND_SEQUENCE_ADDRESS | 0x5554);
    *commandAddress = 0xF5;
}

void restoreSwapConfig()
{
    /* UCB OTP Programming UCB_OTP_SWAPEN Enable*/
    uint8 ucbOtpData[512];
    memset(ucbOtpData, 0x0, 0x1E8);
    uint32 *procontp = (uint32 *)&ucbOtpData[UCB_OTP_PROCONTP_ADDER];
    *procontp = 0;
    uint32 *confirmation = (uint32 *)&ucbOtpData[UCB_OTP_CONFIRMATION_ADDER];
    *confirmation = 0x43211234;
    *(confirmation + 1) = 0x0;

    Flash_Erase(UCB_OTP_ORIG_ADDRESS, 0x1000);
    Flash_Write(UCB_OTP_ORIG_ADDRESS, ucbOtpData, sizeof(ucbOtpData));
    BCB_SystemReset();

    /* UCB Swap Configuration to ORIG0(0xAF40 2E00) to Standard */
    uint32 ucbSwapConfig[4] = {
        UCB_SWAP_MAKRER_STANDARD, // MarkerL: Swap to Standard or Alternate
        0xAF402E00,               // MarkerH: Address of UCB MarkerL
        UCB_CONFIRMATION_CODE,    // ConfirmationL: Confirmation Code
        0xAF402E08                // ConfirmationH: Address of ConfirmationL
    };

    disableUCBWriteProtection();
    uint32 ucbSwapPassword[] = {
        0x0, // PW 0
        0x0, // PW 1
        0x0, // PW 2
        0x0, // PW 3
        0x0, // PW 4
        0x0, // PW 5
        0x0, // PW 6
        0x0  // PW 7
    };

    uint32 ucbSwapConfirmation[2] = {
        UCB_CONFIRMATION_CODE, // Confirmation: Confirmation Code
        0x0                    // Reserved
    };

    uint8 buffer[0xE00];
    memcpy(buffer, (void *)0xAF402000, sizeof(buffer));
    Flash_Erase(0xAF402000, 0x1000);
    Flash_Write(0xAF402000, buffer, sizeof(buffer));
    Flash_Write(UCB_SWAP_ORIG_ADDRESS | 0x2F00, (uint8 *)ucbSwapPassword, sizeof(ucbSwapPassword));
    Flash_Write(UCB_SWAP_ORIG_ADDRESS | 0x2FF0, (uint8 *)ucbSwapConfirmation, sizeof(ucbSwapConfirmation));
    Flash_Write(UCB_SWAP_ORIG_ADDRESS, (uint8 *)ucbSwapConfig, sizeof(ucbSwapConfig));
    enableWriteProtection();

    BCB_SystemReset();
    return;
}

void setSwapConfiguration(uint32 ucbSwapIndexToWrite, boolean swapToAlternate)
{
    uint32 ucbSwapConfig[4] = {
        (swapToAlternate ? UCB_SWAP_MAKRER_ALTERNATE : UCB_SWAP_MAKRER_STANDARD), // MarkerL: Swap to Standard or Alternate
        0xAF402E00 + (0x10 * ucbSwapIndexToWrite),                                // MarkerH: Address of UCB MarkerL
        UCB_CONFIRMATION_CODE,                                                    // ConfirmationL: Confirmation Code
        0xAF402E08 + (0x10 * ucbSwapIndexToWrite)                                 // ConfirmationH: Address of ConfirmationL
    };

    disableUCBWriteProtection();
    if (ucbSwapIndexToWrite == 0)
    {
        uint32 ucbSwapPassword[] = {
            0x0, // PW 0
            0x0, // PW 1
            0x0, // PW 2
            0x0, // PW 3
            0x0, // PW 4
            0x0, // PW 5
            0x0, // PW 6
            0x0  // PW 7
        };

        uint32 ucbSwapConfirmation[2] = {
            UCB_CONFIRMATION_CODE, // Confirmation: Confirmation Code
            0x0                    // Reserved
        };

        uint8 buffer[0xE00];
        memcpy(buffer, (void *)0xAF402000, sizeof(buffer));
        Flash_Erase(0xAF402000, 0x1000);
        Flash_Write(0xAF402000, buffer, sizeof(buffer));
        Flash_Write(UCB_SWAP_ORIG_ADDRESS | 0x2F00, (uint8 *)ucbSwapPassword, sizeof(ucbSwapPassword));
        Flash_Write(UCB_SWAP_ORIG_ADDRESS | 0x2FF0, (uint8 *)ucbSwapConfirmation, sizeof(ucbSwapConfirmation));
    }

    Flash_Write(UCB_SWAP_ORIG_ADDRESS + (0x10 * ucbSwapIndexToWrite), (uint8 *)ucbSwapConfig, sizeof(ucbSwapConfig));
    enableWriteProtection();

    BCB_SystemReset();
    return;
}

void writeSwapConfig(uint32 index)
{
    uint32 ucbSwapConfig[4] = {
        UCB_SWAP_MAKRER_STANDARD,  // MarkerL: Swap to Standard or Alternate
        0xAF402E00 + (0x10 * index), // MarkerH: Address of UCB MarkerL
        UCB_CONFIRMATION_CODE,       // ConfirmationL: Confirmation Code
        0xAF402E08 + (0x10 * index)  // ConfirmationH: Address of ConfirmationL
    };


    uint32 ucbSwapPassword[] = {
        0x0, // PW 0
        0x0, // PW 1
        0x0, // PW 2
        0x0, // PW 3
        0x0, // PW 4
        0x0, // PW 5
        0x0, // PW 6
        0x0  // PW 7
    };

    uint32 ucbSwapConfirmation[2] = {
        0x43211234,             // Unlocked
        0x0                     // Reserved
    };

    disableUCBWriteProtection();
    Flash_Write(UCB_SWAP_ORIG_ADDRESS + (0x10 * index), (uint8 *)ucbSwapConfig, sizeof(ucbSwapConfig));
    enableWriteProtection();

    BCB_SystemReset();
    return;
}

void eraseSwapConfig()
{
    uint8 buffer[0x1000];
    memcpy(buffer, (void *)0xAF402000, sizeof(buffer));
    uint32* marker = (uint32*)&buffer[0xE00];
    *marker = 0x000000AA;
    disableUCBWriteProtection();
    Flash_Erase(0xAF402000, 0x1000);
    Flash_Write(0xAF402000, buffer, sizeof(buffer));
    enableWriteProtection();
    return;
}

void eraseLastSwapConfig()
{
    uint8 buffer[0x8];
    memset(buffer, 0xFF, sizeof(buffer));
    disableUCBWriteProtection();
    Flash_Write(0xAF402EF8, buffer, sizeof(buffer));
    enableWriteProtection();
}

void ABSwap_InitializeUCBSwap()
{
    volatile uint32 *dmuHfConfirm1 = (uint32 *)(DMU_BASE_ADDRESS + DMU_HF_CONFIRM1_OFFSET);

    // Check if UCB Swap is already configured
    if ((((*dmuHfConfirm1) >> 14) & 0x3) != 0x2) // bit[15:14] (00: UNREAD | 01: UNLOCKED | 10: CONFIRMED | 11: ERRORED)
    {
        /* UCB Swap Configuration to ORIG0(0xAF40 2E00) to Standard */
        setSwapConfiguration(0, FALSE);
        return;
    }

    // Check if UCB Swap Enable is already set
    volatile uint32 *dmuHfProcontp = (uint32 *)(DMU_BASE_ADDRESS + DMU_HF_PROCONTP_OFFSET);
    if (((*dmuHfProcontp >> 16) & 0x3) != 0x3)
    {
        /* UCB OTP Programming UCB_OTP_SWAPEN Enable*/
        uint8 ucbOtpData[512];
        memset(ucbOtpData, 0x0, 0x1E8);
        uint32 *procontp = (uint32 *)&ucbOtpData[UCB_OTP_PROCONTP_ADDER];
        *procontp = 0 | (1 << 16) | (1 << 17);
        uint32 *confirmation = (uint32 *)&ucbOtpData[UCB_OTP_CONFIRMATION_ADDER];
        *confirmation = UCB_CONFIRMATION_CODE;
        *(confirmation + 1) = 0x0;

        Flash_Erase(UCB_OTP_ORIG_ADDRESS, 0x1000);
        Flash_Write(UCB_OTP_ORIG_ADDRESS, ucbOtpData, sizeof(ucbOtpData));
        BCB_SystemReset();
        return;
    }
}

void ABSwap_Swap()
{
    volatile uint32 *SCU_STMEM1 = (uint32 *)(SCU_BASE_ADDRESS + SCU_STMEM1_OFFSET);
    uint8 swapCfg = ((*SCU_STMEM1 >> 16) & 0x3); // bit[17:16] (00: No Swap | 01: Standard | 10: Alternate | 11: Reserved)
    IFX_ASSERT(IFX_VERBOSE_LEVEL_FATAL, swapCfg == 0x1 || swapCfg == 0x2);

    boolean isSwapOrig = !((*SCU_STMEM1 >> 18) & 0x1);       // bit[18]
    uint32 ucbSwapIndexToWrite = (*SCU_STMEM1 >> 19) & 0x1F; // bit[23:19]
    ucbSwapIndexToWrite /= 2;
    ucbSwapIndexToWrite += 1;
    ucbSwapIndexToWrite %= 16;

    IFX_ASSERT(IFX_VERBOSE_LEVEL_FATAL, isSwapOrig);
    setSwapConfiguration(ucbSwapIndexToWrite, swapCfg == 0x1);
}