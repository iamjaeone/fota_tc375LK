/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "uds.h"

#include "can.h"
#include "cantp.h"

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

/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

static void handleDiagnosticSessionControlService(const uint8 *data, uint16 length);

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
        handleDiagnosticSessionControlService(data, length);
        break;
    }
    default:
        break;
    }
}

void handleDiagnosticSessionControlService(const uint8 *data, uint16 length)
{
    uint8 subFunction = data[1];
    
    switch(subFunction)
    {
        case 0x02: // Programming Session
        {
            // System Reset
            IfxScuWdt_changeCpuWatchdogReload(IfxScuWdt_getCpuWatchdogPassword(), 0xFFFF);
            while (1) {}
            break;
        }

    }
}