/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/

#include "can.h"

#include "cantp.h"
#include "IfxCan_Can.h"
#include <string.h>

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define RX_BUFFER_SIZE 64

#define MAXIMUM_CAN_DATA_PAYLOAD    2                       /* Define maximum classical CAN payload in 4-byte words */

#define ISR_PRIORITY_CAN_TX 0xB
#define ISR_PRIORITY_CAN_RX 0xA

/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/

typedef struct
{
    IfxCan_Can_Config canConfig;                            /* CAN module configuration structure                   */
    IfxCan_Can canModule;                                   /* CAN module handle                                    */
    IfxCan_Can_Node canSrcNode;                             /* CAN source node handle data structure                */
    IfxCan_Can_Node canDstNode;                             /* CAN destination node handle data structure           */
    IfxCan_Can_NodeConfig canNodeConfig;                    /* CAN node configuration structure                     */
    IfxCan_Filter canFilter;                                /* CAN filter configuration structure                   */
    IfxCan_Message txMsg;                                   /* Transmitted CAN message structure                    */
    IfxCan_Message rxMsg;                                   /* Received CAN message structure                       */
    uint8 txData[8];                                        /* Transmitted CAN data array                           */
    uint8 rxData[8];                                        /* Received CAN data array                              */
} McmcanType;


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

McmcanType                  g_mcmcan;                       /* Global MCMCAN configuration and control structure    */

CanMessage g_can_rxBuffer[RX_BUFFER_SIZE];
uint32 g_can_rxBufferHead;
uint32 g_can_rxBufferTail;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

static int Can_RecvMsg(void);

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

/* Default CAN Tx Handler */
IFX_INTERRUPT(Can_TxIsrHandler, 0, ISR_PRIORITY_CAN_TX);
void Can_TxIsrHandler(void)
{
    /* Clear the "Transmission Completed" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canSrcNode.node, IfxCan_Interrupt_transmissionCompleted);
    P00_OUT.B.P6 = !P00_OUT.B.P6;  /* Toggle LED2 to indicate message transmission */
}

/* Default CAN Rx Handler */
IFX_INTERRUPT(Can_RxIsrHandler, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler (void)
{
    Can_RecvMsg();
}

/* Function to initialize MCMCAN module and nodes related for this application use case */
void Can_Init(CAN_BAUDRATES ls_baudrate, CAN_NODE CAN_Node)
{
    /* wake up transceiver (node 0) */
    IfxPort_setPinModeOutput(&MODULE_P20,6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    MODULE_P20.OUT.B.P6 = 0;

    IfxCan_Can_initModuleConfig(&g_mcmcan.canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan.canModule, &g_mcmcan.canConfig);
    IfxCan_Can_initNodeConfig(&g_mcmcan.canNodeConfig, &g_mcmcan.canModule);

    switch (ls_baudrate)
    {
        case BD_NOUSE:
            g_mcmcan.canNodeConfig.busLoopbackEnabled = TRUE;
            break;
        case BD_500K:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 500000;
            break;
        case BD_1M:
            g_mcmcan.canNodeConfig.baudRate.baudrate = 1000000;
            break;
    }

    g_mcmcan.canNodeConfig.busLoopbackEnabled = FALSE;

    if (CAN_Node == CAN_NODE0) { /* CAN Node 0 for lite kit */
        g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_0;
        const IfxCan_Can_Pins pins =
        {
                &IfxCan_TXD00_P20_8_OUT, IfxPort_OutputMode_pushPull, /* TX Pin for lite kit (can node 0) */
                &IfxCan_RXD00B_P20_7_IN, IfxPort_InputMode_pullUp, /* RX Pin for lite kit (can node 0) */
                IfxPort_PadDriver_cmosAutomotiveSpeed1
        };
        g_mcmcan.canNodeConfig.pins = &pins;
    } else if (CAN_Node == CAN_NODE2) { /* CAN Node 2 for mikrobus */
        g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_2;
        const IfxCan_Can_Pins pins =
        {
                &IfxCan_TXD02_P15_0_OUT, IfxPort_OutputMode_pushPull, /* TX Pin for mikrobus (can node 2) */
                &IfxCan_RXD02A_P15_1_IN, IfxPort_InputMode_pullUp, /* RX Pin for mikrobus (can node 2) */
                IfxPort_PadDriver_cmosAutomotiveSpeed1
        };
        g_mcmcan.canNodeConfig.pins = &pins;
    }

    g_mcmcan.canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    g_mcmcan.canNodeConfig.interruptConfig.transmissionCompletedEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.traco.priority = ISR_PRIORITY_CAN_TX;
    g_mcmcan.canNodeConfig.interruptConfig.traco.interruptLine = IfxCan_InterruptLine_0;
    g_mcmcan.canNodeConfig.interruptConfig.traco.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canSrcNode, &g_mcmcan.canNodeConfig);

    /* Reception handling configuration */
    g_mcmcan.canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_sharedFifo0;
    g_mcmcan.canNodeConfig.rxConfig.rxBufferDataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0Size = 15;

    /* General filter configuration */
    g_mcmcan.canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_standard;
    g_mcmcan.canNodeConfig.filterConfig.standardListSize = 8;
    g_mcmcan.canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
    g_mcmcan.canNodeConfig.filterConfig.rejectRemoteFramesWithStandardId = TRUE;


    /* Interrupt configuration */
    g_mcmcan.canNodeConfig.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.priority = ISR_PRIORITY_CAN_RX;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_1;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;
    IfxCan_Can_initNode(&g_mcmcan.canDstNode, &g_mcmcan.canNodeConfig);

    /* Rx filter configuration (default: all messages accepted) */
    Can_SetFilterRange(0x0, 0x7FF);

    g_can_rxBufferHead = 0;
    g_can_rxBufferTail = 0;
}

void Can_SetFilterRange(uint32 start, uint32 end)
{
    g_mcmcan.canFilter.number = 0;
    g_mcmcan.canFilter.type = IfxCan_FilterType_range;
    g_mcmcan.canFilter.elementConfiguration = IfxCan_FilterElementConfiguration_storeInRxFifo0;
    g_mcmcan.canFilter.id1 = start;
    g_mcmcan.canFilter.id2 = end;
    IfxCan_Can_setStandardFilter(&g_mcmcan.canDstNode, &g_mcmcan.canFilter);
}

void Can_SendMsg(const CanMessage *msg)
{
    /* Initialization of the TX message with the default configuration */
    IfxCan_Can_initMessage(&g_mcmcan.txMsg);

    g_mcmcan.txMsg.messageId = msg->id;
    g_mcmcan.txMsg.dataLengthCode = msg->dlc;

    /* Define the content of the data to be transmitted */
    for (int i = 0; i < 8; i++) {
        g_mcmcan.txData[i] = msg->data[i];
    }

    /* Send the CAN message with the previously defined TX message content */
    while( IfxCan_Status_notSentBusy ==
           IfxCan_Can_sendMessage(&g_mcmcan.canSrcNode, &g_mcmcan.txMsg, (uint32 *)&g_mcmcan.txData[0]) )
    {
    }
}

static int Can_RecvMsg(void)
{
    int err = 0;
    /* Clear the "RX FIFO 0 new message" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.canDstNode.node, IfxCan_Interrupt_rxFifo0NewMessage);

    /* Received message content should be updated with the data stored in the RX FIFO 0 */
    g_mcmcan.rxMsg.readFromRxFifo0 = TRUE;
    g_mcmcan.rxMsg.readFromRxFifo1 = FALSE;

    /* Read the received CAN message */
    IfxCan_Can_readMessage(&g_mcmcan.canDstNode,
                           &g_mcmcan.rxMsg,
                           (uint32*) &g_mcmcan.rxData);


                           
    g_can_rxBuffer[g_can_rxBufferHead].id = (uint16) g_mcmcan.rxMsg.messageId;
    g_can_rxBuffer[g_can_rxBufferHead].dlc = g_mcmcan.rxMsg.dataLengthCode;
    memcpy(g_can_rxBuffer[g_can_rxBufferHead].data, g_mcmcan.rxData, 8);

    g_can_rxBufferHead = (g_can_rxBufferHead + 1) % RX_BUFFER_SIZE;

    return err;
}

void Can_HandleMessage(void)
{
    CanMessage *rxMsg = &g_can_rxBuffer[g_can_rxBufferTail];
    g_can_rxBufferTail = (g_can_rxBufferTail + 1) % RX_BUFFER_SIZE;

    if(
        (rxMsg->id == 0x7E0)
    )
    {
        CANTP_HandleCanMessage(rxMsg);
    }

}