#include <string.h>

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "can.h"
#include "uds.h"

IFX_ALIGN(4) IfxCpu_syncEvent cpuSyncEvent = 0;

void core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    Can_Init(BD_500K, CAN_NODE2);

    UDS_ResponseDiagnosticSessionControl();


    P00_IOCR4.B.PC5 = 0x10;
    P00_IOCR4.B.PC6 = 0x10;
    
    P00_OUT.B.P5 = 0;
    P00_OUT.B.P6 = 0;

    while (1)
    {
        if (g_can_rxBufferHead != g_can_rxBufferTail)
        {
            Can_HandleMessage();
        }
    }
}
