/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "uds.h"

#include "can.h"
#include "cantp.h"
#include "flash_programming.h"
#include "bcb.h"

#include <string.h>


#include "IfxPort_reg.h"
#include "Bsp.h"

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define UDS_DATA_SIZE_MAX ((CANTP_RX_BUFFER_SIZE - 2) - ((CANTP_RX_BUFFER_SIZE - 2) % PFLASH_PAGE_SIZE) + 2)

#define UDS_P2_TIMEOUT_MS 10000
#define UDS_P2_STAR_TIMEOUT_MS 10000


#define UDS_SESSION_TIMEOUT_MS 15000


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

typedef enum
{
    UDS_NRC_POSITIVE_RESPONSE = 0x00, // The 0x00 is a special and initiative value that can not be used for any NRC. It is this parameter value reserved for several internal code implementation reason.
    UDS_NRC_GENERAL_REJECT = 0x10,    // If there is none of the NRC is available in UDS standard document to meet the needs of the implementation, then you can use this NRC for your ECU.
    UDS_NRC_SERVICE_NOT_SUPPORTED = 0x11, // Suppose the client has sent a request with a Service Identifier. That is not supporting in your ECU by your OEM. Then the server will send the 0x11 NRC response message to the client that it is an unknown Service Identifier.
    UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED = 0x12, // Suppose the client has sent a valid and known Service identifier. But the Sub-function Identifier is unknown or invalid. then the server will send 0x12 Negative Response Code to the client.
    UDS_NRC_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT = 0x13, // If the client has sent any diagnostic request message whose length or the frame format does not match with the prescribed length or the format for that specified service (SID), then the server will send this NRC.
    UDS_NRC_RESPONSE_TOO_LONG = 0x14, // If the client has sent a diagnostic request and the response message length is more than the Transport protocol maximum length, then the server will send this NRC.
    UDS_NRC_BUSY_REPEAT_REQUEST = 0x21, // If the server is too busy with some other operation, then the server will send this NRC to the client. This NRC is supported by each SID.
    UDS_NRC_CONDITIONS_NOT_CORRECT = 0x22, // Before proceeding any service from the client, the server will check for prerequisite conditions are met or not. If not then the server will send this NRC.
    UDS_NRC_REQUEST_SEQUENCE_ERROR = 0x24, // If the client will send the diagnostic message non-sequentially, then the server will send this NRC to the client.
    UDS_NRC_NO_RESPONSE_FROM_SUBNET_COMPONENT = 0x25, // If the Gateway ECU does not receive the response message from that target ECU or server then the Gateway ECU will send this NRC to the client.
    UDS_NRC_FAILURE_PREVENTS_EXECUTION_OF_REQUESTED_ACTION = 0x26, // If the client requested service and at that time a fault occurred in server ECU, this failure condition prevents the server from performing the requested action.
    UDS_NRC_REQUEST_OUT_OF_RANGE = 0x31, // If the server received a client request with a parameter (DID, RID) that is out of range, then the server will send this NRC.
    UDS_NRC_SECURITY_ACCESS_DENIED = 0x33, // The server will send this NRC if the security strategy is not satisfied with the server.
    UDS_NRC_INVALID_KEY = 0x35, // If the server received a security key from the client that is not matching with the server-generated key, then the server will send this NRC.
    UDS_NRC_EXCEED_NUMBER_OF_ATTEMPTS = 0x36, // If a client will send the wrong security key more than the defined counter value, the server will send this NRC to the client.
    UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED = 0x37, // If the client will send the security key before the defined wait time, then the server will send this NRC.
    UDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED = 0x70, // Sometimes due to some fault conditions in the server or server memory, it will not accept any upload or download request from the client.
    UDS_NRC_TRANSFER_DATA_SUSPENDED = 0x71, // During the data transfer, sometimes due to some failure of any kind of fault, the server will not able to write data onto the memory.
    UDS_NRC_GENERAL_PROGRAMMING_FAILURE = 0x72, // If the server detects an error during the erasing or programming of the memory location of permanent memory (e.g. NVM/Flash/EEPROM), then the server will send this NRC.
    UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER = 0x73, // In the Consecutive Frame, if there is any mismatch in Frame index, the server will send this NRC to the client.
    UDS_NRC_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING = 0x78, // This NRC indicates that the request was received, but the server needs more time to execute it.
    UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED_IN_ACTIVE_SESSION = 0x7E, // This NRC will be returned by the server if the requested sub-function Identifier is supported for this SID but not supported in the current active session.
    UDS_NRC_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION = 0x7F, // This NRC will be returned by the server if the requested service identifier is not supported in the current active session.
    UDS_NRC_RPM_TOO_HIGH = 0x81, // Requested action will not be taken because the server prerequisite condition for RPM is higher than the defined value.
    UDS_NRC_RPM_TOO_LOW = 0x82, // Requested action will not be taken because the server prerequisite condition for RPM is lower than the defined value.
    UDS_NRC_ENGINE_IS_RUNNING = 0x83, // An NRC for actuator tests that cannot be performed while the engine is operational.
    UDS_NRC_ENGINE_IS_NOT_RUNNING = 0x84, // An NRC for actuator tests that can only be activated when the engine is in operation.
    UDS_NRC_ENGINE_RUN_TIME_TOO_LOW = 0x85, // The requested action will not be executed because the engine run time is below a pre-programmed limit.
    UDS_NRC_TEMPERATURE_IS_TOO_HIGH = 0x86, // The requested action will not be executed because the current temperature exceeds a pre-programmed maximum threshold.
    UDS_NRC_TEMPERATURE_IS_TOO_LOW = 0x87, // The requested action will not be carried out because the current temperature is below a pre-programmed minimum threshold.
    UDS_NRC_VEHICLE_SPEED_IS_TOO_HIGH = 0x88, // The requested action will not be executed because the current vehicle speed exceeds a pre-programmed maximum threshold.
    UDS_NRC_VEHICLE_SPEED_IS_TOO_LOW = 0x89, // The requested action will not be carried out because the current vehicle speed is below a pre-programmed minimum threshold.
    UDS_NRC_THROTTLE_PEDAL_IS_TOO_HIGH = 0x8A, // The requested action will not be executed because the current TP/APP (Throttle/Pedal Position) is above a pre-programmed maximum threshold.
    UDS_NRC_THROTTLE_PEDAL_IS_TOO_LOW = 0x8B, // The requested action will not be carried out because the current TP/APP (Throttle/Pedal Position) is below a pre-programmed minimum threshold.
    UDS_NRC_TRANSMISSION_RANGE_IS_NOT_IN_NEUTRAL = 0x8C, // The requested action will not be executed because the current transmission range is not in the neutral position.
    UDS_NRC_TRANSMISSION_RANGE_IS_NOT_IN_GEAR = 0x8D, // The requested action will not be executed because the current transmission range is not in a gear position.
    UDS_NRC_BRAKE_SWITCHES_NOT_CLOSED = 0x8F, // Brake Pedal not pressed or not applied. This requirement must be upheld throughout the entire duration of the test.
    UDS_NRC_SHIFTER_LEVER_NOT_IN_PARK = 0x90, // A certain requirement is mandatory before the commencement of specific tests, and it must be adhered to throughout the entire duration of the test.
    UDS_NRC_TORQUE_CONVERTER_CLUTCH_IS_LOCKED = 0x91, // The requested action will not be executed because the TCC (Torque Converter Clutch) status is above a pre-programmed limit or it is locked.
    UDS_NRC_VOLTAGE_IS_TOO_HIGH = 0x92, // The requested action will not be carried out because the current voltage exceeds a pre-programmed maximum threshold.
    UDS_NRC_VOLTAGE_TOO_LOW = 0x93, // The requested action will not be executed because the current voltage is below a pre-programmed minimum threshold.
} UDS_Negative_ResponseCode;

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
    uint64 timer;
} UDS_DownloadSession;

/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

UDS_DownloadSession g_udsDownloadSession;
BCB_APPLICATION_TYPE g_currentApplication = 2;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

static void responseNegative(UDS_ServiceType serviceId, UDS_Negative_ResponseCode nrc);
static void handleRequestDownloadService(const uint8 *data, uint16 length);
static void handleRoutineControlService(const uint8 *data, uint16 length);
static void handleTransferDataService(const uint8 *data, uint16 length);
static void handleRequestTransferExitService(const uint8 *data, uint16 length);
static void handleEcuResetService(const uint8 *data, uint16 length);
static void handleDiagnosticSessionControlService(const uint8 *data, uint16 length);

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

void UDS_MainFunction(void)
{
    #ifdef __BOOTLOADER__
    if(g_udsDownloadSession.timer <= IfxStm_get(&MODULE_STM0))
    {   
        BCB_UpdateRollbackHandler();
    }
    #endif
}

inline static void updateSessionTimer(void)
{
    sint32 timerTicks = IfxStm_getTicksFromMilliseconds(&MODULE_STM0, UDS_SESSION_TIMEOUT_MS);
    g_udsDownloadSession.timer = IfxStm_get(&MODULE_STM0) + timerTicks;
}

void UDS_HandleFrame(const uint8 *data, uint16 length)
{
    updateSessionTimer();
    UDS_ServiceType serviceId = (UDS_ServiceType)data[0];
    switch (serviceId)
    {
    case UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:
    {
#ifdef __BOOTLOADER__
        if(data[1] == 0x02)
        {
            UDS_ResponseDiagnosticSessionControl();
        }
        else
        {
            responseNegative(UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
        }
#else
        handleDiagnosticSessionControlService(data, length);
#endif
        break;
    }
#ifdef __BOOTLOADER__
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
#endif
    default:
        responseNegative(serviceId, UDS_NRC_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION);
        break;
    }
}

void UDS_ResponseDiagnosticSessionControl(void)
{
    uint8 response[6];
    response[0] = UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL + 0x40;         // Positive Response
    response[1] = 0x02;                                                  // Echo SubFunction
    response[2] = (UDS_P2_TIMEOUT_MS >> 8) & 0xFF;                       // P2 Server Timeout: 1000ms
    response[3] = UDS_P2_TIMEOUT_MS & 0xFF;                              // P2 Server Timeout: 1000ms
    response[4] = (UDS_P2_STAR_TIMEOUT_MS >> 8) & 0xFF;                  // P2* Server Timeout: 5000ms
    response[5] = UDS_P2_STAR_TIMEOUT_MS & 0xFF;                         // P2* Server Timeout: 5000ms
    CANTP_SendCanTPMessage(response, 6);
    updateSessionTimer();
}

void handleDiagnosticSessionControlService(const uint8 *data, uint16 length)
{
    uint8 subFunction = data[1];
    switch(subFunction)
    {
        case 0x02: // Programming Session
        {
            // System Reset
            BCB_RequestUpdate();
            break;
        }
        default:
        {
            responseNegative(UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
            break;
        }
    }
}


static void responseNegative(UDS_ServiceType serviceId, UDS_Negative_ResponseCode nrc)
{
    g_udsDownloadSession.status = UDS_DOWNLOAD_STATUS_IDLE;
    uint8 response[3];
    response[0] = 0x7F;
    response[1] = serviceId;
    response[2] = nrc;
    CANTP_SendCanTPMessage(response, 3);
}

static void routineControlEraseFlash(const uint8 *data)
{
    // Flash Erase
    uint32 address = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
    uint32 size = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];

    if(g_currentApplication == 2)
    {
        g_currentApplication = BCB_GetCurrentBootApplication();
    
        if(g_currentApplication == BCB_APPLICATION_TYPE_A && address == BCB_APPLICATION_A_START_ADDRESS)
        {
            responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
            return;
        }
        else if(g_currentApplication == BCB_APPLICATION_TYPE_B && address == BCB_APPLICATION_B_START_ADDRESS)
        {
            responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
            return;
        }
    }

    
    // Valid Memory Region Check
    if (!isAddressInValidRegion(address, size))
    {
        responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }



    // Erase
    int result = Flash_Erase(address, size);

    if (result != 0)
    {
        // Negative Response: Erase Failed
        responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
        return;
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

    uint8 subFunction = data[1];

    switch (routineIdentifier)
    {
    case UDS_ROUTINE_IDENTIFIER_ERASE_FLASH:
    {
        if (g_udsDownloadSession.status != UDS_DOWNLOAD_STATUS_IDLE)
        {
            // Negative Response: Download Not In Progress
            responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_CONDITIONS_NOT_CORRECT);
            return;
        }

        routineControlEraseFlash(data);
        break;
    }
    default:
        responseNegative(UDS_SERVICE_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
        break;
    }
}

static void responseRequestDownload(uint16 maxNumberOfBlockLength)
{
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
    if (g_udsDownloadSession.status != UDS_DOWNLOAD_STATUS_IDLE)
    {
        responseNegative(UDS_SERVICE_REQUEST_DOWNLOAD, UDS_NRC_CONDITIONS_NOT_CORRECT);
        return;
    }

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
        responseNegative(UDS_SERVICE_REQUEST_DOWNLOAD, UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }

    g_udsDownloadSession.targetAddress = memoryAddress;
    g_udsDownloadSession.totalSize = memorySize;
    g_udsDownloadSession.receivedSize = 0;
    g_udsDownloadSession.expectedBlockSequence = 1;

    g_udsDownloadSession.status = UDS_DOWNLOAD_STATUS_IN_PROGRESS;
    responseRequestDownload(UDS_DATA_SIZE_MAX);
}

static void handleTransferDataService(const uint8 *data, uint16 length)
{
    if (g_udsDownloadSession.status != UDS_DOWNLOAD_STATUS_IN_PROGRESS)
    {
        responseNegative(UDS_SERVICE_TRANSFER_DATA, UDS_NRC_CONDITIONS_NOT_CORRECT);
        return;
    }

    if (g_udsDownloadSession.expectedBlockSequence != data[1])
    {
        responseNegative(UDS_SERVICE_TRANSFER_DATA, UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER);
        return;
    }

    /* Send Response Pending */
    // responseNegative(UDS_SERVICE_TRANSFER_DATA, UDS_NRC_REQUEST_CORRECTLY_RECEIVED_RESPONSE_PENDING);

    // Process the received data block
    uint16 dataSize = length - 2; // Subtracting service ID and block sequence counter

    uint32 startAddress = g_udsDownloadSession.targetAddress + g_udsDownloadSession.receivedSize;
    int status = Flash_Write(startAddress, &data[2], dataSize);
    if (status != 0)
    {
        // Negative Response: Write Failed
        responseNegative(UDS_SERVICE_TRANSFER_DATA, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
        return;
    }

    g_udsDownloadSession.receivedSize += dataSize;
    g_udsDownloadSession.expectedBlockSequence++;

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_TRANSFER_DATA + 0x40; // Positive Response
    response[1] = data[1];                          // Echo back the block sequence counter
    CANTP_SendCanTPMessage(response, 2);
}

static void handleRequestTransferExitService(const uint8 *data, uint16 length)
{
    if (g_udsDownloadSession.status != UDS_DOWNLOAD_STATUS_IN_PROGRESS)
    {
        responseNegative(UDS_SERVICE_REQUEST_TRANSFER_EXIT, UDS_NRC_CONDITIONS_NOT_CORRECT);
        return;
    }

    if (g_udsDownloadSession.receivedSize != g_udsDownloadSession.totalSize)
    {
        // Negative Response: Download Not Completed
        responseNegative(UDS_SERVICE_REQUEST_TRANSFER_EXIT, UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }
        
    // Reset download session
    g_udsDownloadSession.status = UDS_DOWNLOAD_STATUS_IDLE;

    // Send positive response
    uint8 response[8];
    response[0] = UDS_SERVICE_REQUEST_TRANSFER_EXIT + 0x40; // Positive Response
    CANTP_SendCanTPMessage(response, 1);
}

static void handleEcuResetService(const uint8 *data, uint16 length)
{
    uint8 resetType = data[1]; // 0x01: Hard Reset, 0x02: Key Off On Reset, 0x03: Soft Reset
    // Send positive response
    switch (resetType)
    {
    case 0x01:
    {
        uint8 response[2];
        response[0] = UDS_SERVICE_ECU_RESET + 0x40; // Positive Response
        response[1] = resetType;
        CANTP_SendCanTPMessage(response, 2);

        /* Check Message Sent -> CAN 공부 후에 구현 */
        volatile int wait = 10000;
        while (wait--)
            ;

            
        BCB_CommitUpdate(!g_currentApplication);

        BCB_HardReset();
        break;
    }
    default:
        responseNegative(UDS_SERVICE_ECU_RESET, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
        break;
    }
}
