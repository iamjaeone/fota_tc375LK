/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "uds.h"

#include "can.h"
#include "cantp.h"
#include "flash_programming.h"

#include <string.h>

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define MAX_UDS_DATA_SIZE 0x3E0      // 0x400(1024) - 0x20(32) = 0x3E0

/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/

typedef enum
{
    UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL = 0x10,
    UDS_SERVICE_ECU_RESET = 0x11,
    UDS_SERVICE_SECURITY_ACCESS = 0x27,
    UDS_SERVICE_COMMUNICATION_CONTROL = 0x28,
    UDS_SERVICE_TESTER_PRESENT = 0x3E,
    UDS_SERVICE_ACCESS_TIMING_PARAMETER = 0x83,
    UDS_SERVICE_SECURED_DATA_TRANSMISSION = 0x84,
    UDS_SERVICE_CONTROL_DTC_SETTING = 0x85,
    UDS_SERVICE_RESPONSE_ON_EVENT = 0x86,
    UDS_SERVICE_LINK_CONTROL = 0x87,
    UDS_SERVICE_READ_DATA_BY_IDENTIFIER = 0x22,
    UDS_SERVICE_READ_MEMORY_BY_ADDRESS = 0x23,
    UDS_SERVICE_READ_SCALING_DATA_BY_IDENTIFIER = 0x24,
    UDS_SERVICE_READ_DATA_BY_PERIODIC_IDENTIFIER = 0x2A,
    UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_IDENTIFIER = 0x2C,
    UDS_SERVICE_WRITE_DATA_BY_IDENTIFIER = 0x2E,
    UDS_SERVICE_WRITE_MEMORY_BY_ADDRESS = 0x3D,
    UDS_SERVICE_CLEAR_DIAGNOSTIC_INFORMATION = 0x14,
    UDS_SERVICE_READ_DTC_INFORMATION = 0x19,
    UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER = 0x2F,
    UDS_SERVICE_ROUTINE_CONTROL = 0x31,
    UDS_SERVICE_REQUEST_DOWNLOAD = 0x34,
    UDS_SERVICE_REQUEST_UPLOAD = 0x35,
    UDS_SERVICE_TRANSFER_DATA = 0x36,
    UDS_SERVICE_REQUEST_TRANSFER_EXIT = 0x37,
    UDS_SERVICE_REQUEST_FILE_TRANSFER = 0x38,
    UDS_SERVICE_TERMINATE_FILE_TRANSFER = 0x39,
    UDS_SERVICE_REQUEST_FILE_TRANSFER_EXIT = 0x3A,

    UDS_SERVICE_REQUEST_DOWNLOAD_POSITIVE_RESPONSE = 0x74
} UDS_ServiceType;

// typedef enum
// {
//     // 0x13	incorrectMessageLengthOrInvalidFormat	This shall sent by the server to client if the message length is wrong
//     // 0x22	conditionsNotCorrect	This shall be returned by server, while in the process of receiving a download of a software or calibration module is ongoing.
//     // 0x31	requestOutOfRange	This NRC shall be returned if:
//     // 1) the specified dataFormatIdentifier is not valid.
//     // 2) The specified addressAndLengthFormatIdentifier is not valid.
//     // 3) The specified memoryAddress/memorySize is not valid.
//     // 0x33	securityAccessDenied	This shall be returned by the server if the server is in locked state & the client requested the 0x34 service. So the client shall unlock the ECU first before 0x34 request.
//     // 0x70	uploadDownloadNotAccepted	If there are some faults for which the server can not perform the software download request.
// } UDS_Negative_ResponseCode;

typedef enum
{
    UDS_ROUTINE_IDENTIFIER_ERASE_FLASH = 0xFF00
} UDS_ROUTINE_IDENTIFIER;


typedef enum
{
    UDS_DOWNLOAD_STATUS_IDLE = 0,
    UDS_DOWNLOAD_STATUS_IN_PROGRESS,
    UDS_DOWNLOAD_STATUS_COMPLETED
} UDS_DownloadStatusType;

typedef struct
{
    UDS_DownloadStatusType status;
    boolean isErased;
    uint32 targetAddress;
    uint32 totalSize;
    uint32 receivedSize;
} UDS_DownloadSession;




/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

UDS_DownloadSession g_uds_download_session;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

static void handleRequestDownloadService(const uint8 *data, uint16 length);
static void handleRoutineControlService(const uint8 *data, uint16 length);
static void handleTransferDataService(const uint8 *data, uint16 length);
static void handleRequestTransferExitService(const uint8 *data, uint16 length);
static void handleEcuResetService(const uint8 *data, uint16 length);

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

void UDS_HandleFrame(const uint8 *data, uint16 length)
{
    UDS_ServiceType serviceId = (UDS_ServiceType)data[0];
    switch (serviceId)
    {
    case UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:
    {
        /* 얘는 Application에서 Bootloader로 점프할 때 쓰는 거라 Bootloader에서는 필요없음 */
        // Bootloader Flag 설정하고 Reset
        break;
    }
    case UDS_SERVICE_REQUEST_DOWNLOAD:
    {
        handleRequestDownloadService(data, length);
        break;
    }
    case UDS_SERVICE_ROUTINE_CONTROL:
    {
        /* ERASE FLASH */
        handleRoutineControlService(data, length);
        break;
    }
    case UDS_SERVICE_TRANSFER_DATA:
    {
        handleTransferDataService(data, length);
        break;
    }
    case UDS_SERVICE_REQUEST_TRANSFER_EXIT:
    {
        handleRequestTransferExitService(data, length);
        break;
    }

    case UDS_SERVICE_ECU_RESET:
    {
        handleEcuResetService(data, length);
        break;
    }
    default:
        break;
    }
}

static void responseRequestDownload(uint16 maxNumberOfBlockLength)
{
    // Negative 추가할지?

    uint8 data[8];
    data[0] = UDS_SERVICE_REQUEST_DOWNLOAD_POSITIVE_RESPONSE; // Positive Response
    data[1] = (maxNumberOfBlockLength > 0xFF) ? 2 : 1;
    if (data[1] == 1)
    {
        data[2] = (uint8)maxNumberOfBlockLength;
        CANTP_sendCanTPMessage(data, 3);
    }
    else
    {
        data[2] = (uint8)((maxNumberOfBlockLength >> 8) & 0xFF);
        data[3] = (uint8)(maxNumberOfBlockLength & 0xFF);
        CANTP_sendCanTPMessage(data, 4);
    }
}

static void handleRequestDownloadService(const uint8 *data, uint16 length)
{
    // 양식 나중에 확인하기, 특히 Identifier
    // https://piembsystech.com/request-download-0x34-service-uds-protocol/#
    uint8 dataFormatIdentifier = data[1];
    uint8 addressLengthIdentifier = data[2] & 0x0F;
    uint8 memorySizeLengthIdentifier = (data[2] >> 4) & 0x0F;
    uint32 memoryAddress = 0;
    uint32 memorySize = 0;

    for (uint8 i = 0; i < addressLengthIdentifier; i++)
    {
        memoryAddress = (memoryAddress << 8) | data[3 + i];
    }

    for (uint8 i = 0; i < memorySizeLengthIdentifier; i++)
    {
        memorySize = (memorySize << 8) | data[3 + addressLengthIdentifier + i];
    }

    if (!isAddressInValidRegion(memoryAddress, memorySize))
    {
        // Negative Response: Out of Memory Range
        return;
    }

    responseRequestDownload(MAX_UDS_DATA_SIZE);
    g_uds_download_session.status = UDS_DOWNLOAD_STATUS_IN_PROGRESS;
    g_uds_download_session.targetAddress = memoryAddress;
    g_uds_download_session.totalSize = memorySize;
    g_uds_download_session.receivedSize = 0;
    g_uds_download_session.isErased = FALSE;
}

static void routineControlEraseFlash(const uint8 *data)
{
    // Flash Erase
    uint32 address = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    uint32 size = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];

    if (isAddressInValidRegion(address, size))
    {
        // Erase
        int result = Flash_erase(address, size);

        if (result == 0)
        {
            uint8 response[8];
            response[0] = UDS_SERVICE_ROUTINE_CONTROL + 0x40;
            response[1] = 0x01;         // Sub-function: StartRoutine 말고 나중에 더 추가할지 말지
            response[2] = data[2];      // RoutineIdentifier
            response[3] = data[3];      // RoutineIdentifier
            response[4] = 0x00;         // RoutineStatus: Success
            CANTP_sendCanTPMessage(response, 5);
        }
        else
        {
            // Negative Response: Erase Failed
        }
    }
    else
    {
        // Negative Response: Out of Memory Range
    }

    g_uds_download_session.isErased = TRUE;
}

static void handleRoutineControlService(const uint8 *data, uint16 length)
{
    UDS_ROUTINE_IDENTIFIER routineIdentifier = (data[2] << 8) | data[3];

    switch (routineIdentifier)
    {
    case UDS_ROUTINE_IDENTIFIER_ERASE_FLASH:
    {
        routineControlEraseFlash(data);
        break;
    }
    default:
        break;
    }
}

static void handleTransferDataService(const uint8 *data, uint16 length)
{
    if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_IN_PROGRESS)
    {
        // Negative Response: Download Not In Progress
        return;
    }

    if (!g_uds_download_session.isErased)
    {
        // Negative Response: Flash Not Erased
        return;
    }

    uint8 blockSequenceCounter = data[1];
    uint8 currentSequence = (g_uds_download_session.receivedSize / 7) % 0xFF + 1;

    if(blockSequenceCounter != currentSequence)
    {
        // Negative Response: Invalid Sequence
        return;
    }

    // Process the received data block
    uint8 dataSize = length - 2; // Subtracting service ID and block sequence counter
    int status = Flash_write(g_uds_download_session.targetAddress + g_uds_download_session.receivedSize, &data[2], dataSize);
    if(status != 0)
    {
        // Negative Response: Flash Write Failed
        return;
    }
    g_uds_download_session.receivedSize += dataSize;

    if (g_uds_download_session.receivedSize >= g_uds_download_session.totalSize)
    {
        // Download complete
        g_uds_download_session.status = UDS_DOWNLOAD_STATUS_COMPLETED;
    }

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_TRANSFER_DATA + 0x40; // Positive Response
    response[1] = blockSequenceCounter;
    CANTP_sendCanTPMessage(response, 2);
}

static void handleRequestTransferExitService(const uint8 *data, uint16 length)
{
    if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_COMPLETED)
    {
        // Negative Response: Download Not Completed
        return;
    }

    /* Verify Checksum 추가할지 말지*/

    
    // Reset download session
    g_uds_download_session.status = UDS_DOWNLOAD_STATUS_IDLE;
    g_uds_download_session.isErased = FALSE;
    g_uds_download_session.targetAddress = 0;
    g_uds_download_session.totalSize = 0;
    g_uds_download_session.receivedSize = 0;

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_REQUEST_TRANSFER_EXIT + 0x40; // Positive Response
    CANTP_sendCanTPMessage(response, 1);
}

static void handleEcuResetService(const uint8 *data, uint16 length)
{
    uint8 resetType = data[1]; // 0x01: Hard Reset, 0x02: Key Off On Reset, 0x03: Soft Reset
    // Reset the ECU
    // This is a placeholder implementation. Actual reset mechanism will depend on the hardware.
    // For example, it might involve writing to a specific register or triggering a watchdog timer.

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_ECU_RESET + 0x40; // Positive Response
    response[1] = resetType;

    CANTP_sendCanTPMessage(response, 2);
    
    // TODO -- Reset the system here

}