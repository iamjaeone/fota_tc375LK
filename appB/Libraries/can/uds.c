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

#define UDS_DATA_SIZE_MAX ((CANTP_RX_BUFFER_SIZE - 2) - ((CANTP_RX_BUFFER_SIZE - 2) % PFLASH_PAGE_SIZE) + 2)

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
    UDS_SERVICE_REQUEST_FILE_TRANSFER_EXIT = 0x3A
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
    UDS_DOWNLOAD_STATUS_IDLE,
    UDS_DOWNLOAD_STATUS_IN_PROGRESS
} UDS_DownloadStatusType;

typedef struct
{
    UDS_DownloadStatusType status;
    uint32 targetAddress;
    uint32 totalSize;
    uint32 receivedSize;
    uint8 expectedBlockSequence;
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
    case UDS_SERVICE_ROUTINE_CONTROL:
    {
        /* ERASE FLASH */
        handleRoutineControlService(data, length);
        break;
    }
    case UDS_SERVICE_REQUEST_DOWNLOAD:
    {
        handleRequestDownloadService(data, length);
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

static void routineControlEraseFlash(const uint8 *data)
{
    // Flash Erase
    uint32 address = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    uint32 size = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];

    if (!isAddressInValidRegion(address, size)) // Valid Region Check
    {
        // Negative Response: Out of Memory Range
        return;
    }
    // Erase
    int result = Flash_Erase(address, size);

    if (!result)
    {
        // Negative Response: Erase Failed
    }

    uint8 response[5];
    response[0] = UDS_SERVICE_ROUTINE_CONTROL + 0x40;
    response[1] = data[1]; // Echo RoutineControlType
    response[2] = data[2]; // RoutineIdentifier
    response[3] = data[3]; // RoutineIdentifier
    response[4] = 0x00;    // RoutineStatus: Success

    CANTP_SendCanTPMessage(response, 5);
}

static void handleRoutineControlService(const uint8 *data, uint16 length)
{
    UDS_ROUTINE_IDENTIFIER routineIdentifier = (data[2] << 8) | data[3];

     /*
      *   Sub-function: StartRoutine(0x01) 말고 나중에 더 추가할지 말지
      *   - 0x02: StopRoutine
      *   - 0x03: RequestRoutineResults
      */ 
    uint8 subFunction = data[1];
    
    switch (routineIdentifier)
    {
    case UDS_ROUTINE_IDENTIFIER_ERASE_FLASH:
    {
        if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_IDLE)
        {
            // Negative Response: Download Not In Progress
            return;
        }

        routineControlEraseFlash(data);
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
    data[0] = UDS_SERVICE_REQUEST_DOWNLOAD + 0x40; // Positive Response
    data[1] = (maxNumberOfBlockLength > 0xFF) ? 2 : 1;
    if (data[1] == 1)
    {
        data[2] = (uint8)maxNumberOfBlockLength;
        CANTP_SendCanTPMessage(data, 3);
    }
    else
    {
        data[2] = (uint8)((maxNumberOfBlockLength >> 8) & 0xFF);
        data[3] = (uint8)(maxNumberOfBlockLength & 0xFF);
        CANTP_SendCanTPMessage(data, 4);
    }
}

static void handleRequestDownloadService(const uint8 *data, uint16 length)
{
    if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_IDLE)
    {
        // Negative Response: Download Already In Progress
        return;
    }
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

    g_uds_download_session.targetAddress = memoryAddress;
    g_uds_download_session.totalSize = memorySize;
    g_uds_download_session.receivedSize = 0;
    g_uds_download_session.expectedBlockSequence = 1;

    g_uds_download_session.status = UDS_DOWNLOAD_STATUS_IN_PROGRESS;
    responseRequestDownload(UDS_DATA_SIZE_MAX);
}

static void handleTransferDataService(const uint8 *data, uint16 length)
{
    if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_IN_PROGRESS)
    {
        // Negative Response: Download Not In Progress
        return;
    }

    if (g_uds_download_session.expectedBlockSequence != data[1])
    {
        // Negative Response: Invalid Sequence
        return;
    }


    // Process the received data block
    uint16 dataSize = length - 2; // Subtracting service ID and block sequence counter

    uint32 startAddress = g_uds_download_session.targetAddress + g_uds_download_session.receivedSize;
    int status = Flash_Write(startAddress, &data[2], dataSize);
    if (status != 0)
    {
        // Negative Response: Flash Write Failed
        return;
    }

    g_uds_download_session.receivedSize += dataSize;
    g_uds_download_session.expectedBlockSequence++;

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_TRANSFER_DATA + 0x40; // Positive Response
    response[1] = data[1];                          // Echo back the block sequence counter
    CANTP_SendCanTPMessage(response, 2);
}

static void handleRequestTransferExitService(const uint8 *data, uint16 length)
{
    if (g_uds_download_session.status != UDS_DOWNLOAD_STATUS_IN_PROGRESS)
    {
        // Negative Response: Download Not In Progress
        return;
    }

    if (g_uds_download_session.receivedSize != g_uds_download_session.totalSize)
    {
        // Negative Response: Download Not Completed
        return;
    }

    /* Verify Checksum 추가할지 말지*/

    // Reset download session
    g_uds_download_session.status = UDS_DOWNLOAD_STATUS_IDLE;

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_REQUEST_TRANSFER_EXIT + 0x40; // Positive Response
    CANTP_SendCanTPMessage(response, 1);
}

static void handleEcuResetService(const uint8 *data, uint16 length)
{
    uint8 resetType = data[1]; // 0x01: Hard Reset, 0x02: Key Off On Reset, 0x03: Soft Reset
    // Reset the ECU

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_ECU_RESET + 0x40; // Positive Response
    response[1] = resetType;
    CANTP_SendCanTPMessage(response, 2);

    /* Check Message Sent -> CAN 공부 후에 구현 */

    // TODO -- Reset the system here
}