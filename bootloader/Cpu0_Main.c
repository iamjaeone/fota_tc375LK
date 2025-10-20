#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "can.h"
#include "bcb.h"
#include "uds.h"

#include "flash_programming.h"
#include <string.h>


IFX_ALIGN(4) IfxCpu_syncEvent cpuSyncEvent = 0;


void core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    // BCB_ClearBcb();

    BCB_BootLoaderInit();
    Can_Init(BD_500K, CAN_NODE0);
    Can_SetFilterRange(0x7E0, 0x7EF);
    UDS_ResponseDiagnosticSessionControl();

    while(1)
    {
        Can_MainFunction();
        CANTP_MainFunction();
        UDS_MainFunction();
    }
}
