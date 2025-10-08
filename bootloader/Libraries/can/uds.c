/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "uds.h"


/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/



/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/

typedef enum
{
  UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL       = 0x10,
  UDS_SERVICE_ECU_RESET                        = 0x11,
  UDS_SERVICE_SECURITY_ACCESS                  = 0x27,
  UDS_SERVICE_COMMUNICATION_CONTROL            = 0x28,
  UDS_SERVICE_TESTER_PRESENT                   = 0x3E,
  UDS_SERVICE_ACCESS_TIMING_PARAMETER          = 0x83,
  UDS_SERVICE_SECURED_DATA_TRANSMISSION        = 0x84,
  UDS_SERVICE_CONTROL_DTC_SETTING              = 0x85,
  UDS_SERVICE_RESPONSE_ON_EVENT                = 0x86,
  UDS_SERVICE_LINK_CONTROL                     = 0x87,
  UDS_SERVICE_READ_DATA_BY_IDENTIFIER          = 0x22,
  UDS_SERVICE_READ_MEMORY_BY_ADDRESS           = 0x23,
  UDS_SERVICE_READ_SCALING_DATA_BY_IDENTIFIER = 0x24,
  UDS_SERVICE_READ_DATA_BY_PERIODIC_IDENTIFIER = 0x2A,
  UDS_SERVICE_DYNAMICALLY_DEFINE_DATA_IDENTIFIER=0x2C,
  UDS_SERVICE_WRITE_DATA_BY_IDENTIFIER         = 0x2E,
  UDS_SERVICE_WRITE_MEMORY_BY_ADDRESS          = 0x3D,
  UDS_SERVICE_CLEAR_DIAGNOSTIC_INFORMATION     = 0x14,
  UDS_SERVICE_READ_DTC_INFORMATION             = 0x19,
  UDS_SERVICE_INPUT_OUTPUT_CONTROL_BY_IDENTIFIER=0x2F,
  UDS_SERVICE_ROUTINE_CONTROL                  = 0x31,
  UDS_SERVICE_REQUEST_DOWNLOAD                 = 0x34,
  UDS_SERVICE_REQUEST_UPLOAD                   = 0x35,
  UDS_SERVICE_TRANSFER_DATA                    = 0x36,
  UDS_SERVICE_REQUEST_TRANSFER_EXIT            = 0x37,
  UDS_SERVICE_REQUEST_FILE_TRANSFER            = 0x38,
  UDS_SERVICE_TERMINATE_FILE_TRANSFER          = 0x39,
  UDS_SERVICE_REQUEST_FILE_TRANSFER_EXIT       = 0x3A
} UDS_ServiceType;


/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

void UDS_HandleFrame(const uint8* data, uint16 length) {
  UDS_ServiceType serviceType = (UDS_ServiceType) data[0];
  switch (serviceType)
  {
  case UDS_SERVICE_DIAGNOSTIC_SESSION_CONTROL:
  {
    /* 얘는 Application에서 Bootloader로 점프할 때 쓰는 거 */
    break;
  }
  case UDS_SERVICE_REQUEST_DOWNLOAD:
  {
    // ERASE FLASH
    break;
  }
  case UDS_SERVICE_TRANSFER_DATA:
  {
    /* code */
    break;
  }
  case UDS_SERVICE_REQUEST_TRANSFER_EXIT:
  {
    /* code */
    break;
  }

  case UDS_SERVICE_ECU_RESET:
  {
    // RESET
    break;
  }
  default:
    break;
  }
}