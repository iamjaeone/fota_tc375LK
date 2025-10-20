/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "cantp.h"

#include "Ifx_Types.h"
#include "uds.h"
#include "Bsp.h"

#include <string.h>

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define CANTP_PADDING_BYTE 0x55
#define FLOW_CONTROL_SEPARATION_TIME_MS 10
#define CANTP_TIMEOUT_MS 1000


/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/
typedef enum
{
    CANTP_SESSION_STATE_IDLE = 0,
    CANTP_SESSION_STATE_RECEIVING,
    CANTP_SESSION_STATE_SENDING
} CANTP_SessionState;

typedef struct
{
    CANTP_SessionState state;

    uint16 rxTotalSize;                   // 받아야 할 전체 데이터의 크기
    uint8 rxConsecutiveSeqNum;            // 다음에 받아야 할 Consecutive Frame의 순서 번호
    uint8 rxBuffer[CANTP_RX_BUFFER_SIZE]; // 수신된 데이터를 저장할 버퍼
    uint16 rxBufferCounter;               // 지금까지 받은 데이터의 크기

    uint16 txTotalSize;        // 보내야 할 전체 데이터의 크기
    uint16 txBufferCounter;    // 지금까지 보낸 데이터의 크기
    uint8 *txBuffer;           // 송신할 데이터를 저장할 버퍼
    uint8 txConsecutiveSeqNum; // 다음에 보낼 Consecutive Sequence Number

    uint64 timer;
} CANTP_Session;

typedef enum
{
    CANTP_FRAME_TYPE_SINGLE_FRAME = 0x0,
    CANTP_FRAME_TYPE_FIRST_FRAME = 0x1,
    CANTP_FRAME_TYPE_CONSECUTIVE_FRAME = 0x2,
    CANTP_FRAME_TYPE_FLOW_CONTROL = 0x3
} CANTP_FrameType;

typedef struct
{
    CANTP_FrameType frameType;
    uint8 payloadLength;
    uint8 data[7];
} CANTP_SingleFrame;

typedef struct
{
    CANTP_FrameType frameType;
    uint16 payloadLength;
    uint8 data[6];
} CANTP_FirstFrame;

typedef struct
{
    CANTP_FrameType frameType;
    uint8 sequenceNumber;
    uint8 data[7];
} CANTP_ConsecutiveFrame;

typedef enum
{
    CANTP_FLOW_STATUS_CONTINUE = 0,
    CANTP_FLOW_STATUS_WAIT = 1,
    CANTP_FLOW_STATUS_OVERFLOW = 2
} CANTP_FlowStatus;

typedef struct
{
    CANTP_FrameType frameType;
    CANTP_FlowStatus flowStatus;
    uint8 blockSize;
    uint8 separationTime;
} CANTP_FlowControlFrame;

/*********************************************************************************************************************/
/*------------------------------------------------Global Variables---------------------------------------------------*/
/*********************************************************************************************************************/

CANTP_Session g_cantpSession;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

static void handleSingleFrame(const CANTP_SingleFrame *frame);
static void handleFirstFrame(const CANTP_FirstFrame *frame);
static void sendFlowControl(CANTP_FlowStatus flowStatus, uint8 blockSize, uint8 separationTime);
static void handleConsecutiveFrame(const CANTP_ConsecutiveFrame *frame);
static void handleFlowControl(const CANTP_FlowControlFrame *frame);

/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

void CANTP_MainFunction()
{
    if(g_cantpSession.state == CANTP_SESSION_STATE_IDLE)
    {
        return;
    }

    if(g_cantpSession.timer <= IfxStm_get(&MODULE_STM0))
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;   
    }
}

inline static void updateTimer()
{
    sint32 timerTicks = IfxStm_getTicksFromMilliseconds(&MODULE_STM0, CANTP_TIMEOUT_MS);
    g_cantpSession.timer = IfxStm_get(&MODULE_STM0) + timerTicks;
}

void CANTP_HandleCanMessage(const CanMessage *canMsg)
{
    CANTP_FrameType frameType = (CANTP_FrameType)(canMsg->data[0] >> 4);

    switch (frameType)
    {
    case CANTP_FRAME_TYPE_SINGLE_FRAME:
    {
        if (g_cantpSession.state != CANTP_SESSION_STATE_IDLE)
        {
            g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
            return;
        }

        CANTP_SingleFrame singleFrame;
        singleFrame.frameType = frameType;
        singleFrame.payloadLength = canMsg->data[0] & 0x0F;
        memcpy(singleFrame.data, canMsg->data + 1, singleFrame.payloadLength);

        handleSingleFrame(&singleFrame);
        break;
    }

    case CANTP_FRAME_TYPE_FIRST_FRAME:
    {
        if (g_cantpSession.state != CANTP_SESSION_STATE_IDLE)
        {
            g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
            return;
        }

        CANTP_FirstFrame firstFrame;
        firstFrame.frameType = frameType;
        firstFrame.payloadLength = (canMsg->data[0] & 0x0F) << 8 | canMsg->data[1];
        memcpy(firstFrame.data, canMsg->data + 2, 6);

        handleFirstFrame(&firstFrame);
        break;
    }
    case CANTP_FRAME_TYPE_CONSECUTIVE_FRAME:
    {
        if (g_cantpSession.state != CANTP_SESSION_STATE_RECEIVING)
        {
            g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
            return;
        }

        CANTP_ConsecutiveFrame consecutiveFrame;
        consecutiveFrame.frameType = frameType;
        consecutiveFrame.sequenceNumber = canMsg->data[0] & 0x0F;
        memcpy(consecutiveFrame.data, canMsg->data + 1, 7);

        handleConsecutiveFrame(&consecutiveFrame);
        break;
    }
    case CANTP_FRAME_TYPE_FLOW_CONTROL:
    {
        if (g_cantpSession.state != CANTP_SESSION_STATE_SENDING)
        {
            g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
            return;
        }

        CANTP_FlowControlFrame flowControlFrame;
        flowControlFrame.frameType = frameType;
        flowControlFrame.flowStatus = canMsg->data[0] & 0x0F;
        flowControlFrame.blockSize = canMsg->data[1];
        flowControlFrame.separationTime = canMsg->data[2];

        // Consecutive 송신
        handleFlowControl(&flowControlFrame);
        break;
    }
    default:
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        break;
    }
}

void CANTP_SendCanTPMessage(uint8 *data, uint16 length)
{
    if (length <= 7)
    {
        // Single Frame
        CanMessage canMsg;
        canMsg.id = RESPONSE_CANTP_ID;
        canMsg.dlc = length + 1;
        canMsg.data[0] = (CANTP_FRAME_TYPE_SINGLE_FRAME << 4) | (length & 0x0F);
        memcpy(canMsg.data + 1, data, length);
        Can_SendMsg(&canMsg);
        return;
    }

    // First Frame
    g_cantpSession.state = CANTP_SESSION_STATE_SENDING;
    g_cantpSession.txBuffer = data;
    g_cantpSession.txTotalSize = length;

    CanMessage canMsg;
    canMsg.id = RESPONSE_CANTP_ID;
    canMsg.dlc = 8;
    canMsg.data[0] = (CANTP_FRAME_TYPE_FIRST_FRAME << 4) | ((length >> 8) & 0x0F);
    canMsg.data[1] = length & 0xFF;
    memcpy(canMsg.data + 2, data, 6);
    g_cantpSession.txBufferCounter = 6;
    g_cantpSession.txConsecutiveSeqNum = 1;

    Can_SendMsg(&canMsg);
    updateTimer();
}

static void handleSingleFrame(const CANTP_SingleFrame *frame)
{
    if(g_cantpSession.state != CANTP_SESSION_STATE_IDLE)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }
    
    UDS_HandleFrame(frame->data, frame->payloadLength);
}

static void handleFirstFrame(const CANTP_FirstFrame *frame)
{
    if(g_cantpSession.state != CANTP_SESSION_STATE_IDLE)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    if (frame->payloadLength > CANTP_RX_BUFFER_SIZE)
    {
        sendFlowControl(CANTP_FLOW_STATUS_OVERFLOW, 0, 0);
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    g_cantpSession.rxTotalSize = frame->payloadLength;

    memcpy(g_cantpSession.rxBuffer, frame->data, 6);
    g_cantpSession.rxBufferCounter = 6;

    g_cantpSession.rxConsecutiveSeqNum = 1;
    
    g_cantpSession.state = CANTP_SESSION_STATE_RECEIVING;
    sendFlowControl(CANTP_FLOW_STATUS_CONTINUE, FLOW_CONTROL_BLOCK_SIZE, FLOW_CONTROL_SEPARATION_TIME_MS);
}

static void sendFlowControl(CANTP_FlowStatus flowStatus, uint8 blockSize, uint8 separationTime)
{
    CANTP_FlowControlFrame fcFrame;
    fcFrame.frameType = CANTP_FRAME_TYPE_FLOW_CONTROL;
    fcFrame.flowStatus = flowStatus;
    fcFrame.blockSize = blockSize;
    fcFrame.separationTime = separationTime;

    CanMessage canMsg;
    canMsg.id = RESPONSE_CANTP_ID;
    canMsg.dlc = 8;
    canMsg.data[0] = (fcFrame.frameType << 4) | (fcFrame.flowStatus & 0x0F);
    canMsg.data[1] = fcFrame.blockSize;
    canMsg.data[2] = fcFrame.separationTime;
    memset(canMsg.data + 3, CANTP_PADDING_BYTE, 5);

    Can_SendMsg(&canMsg);
    
    updateTimer();
}

static void handleConsecutiveFrame(const CANTP_ConsecutiveFrame *frame)
{

    if(g_cantpSession.state != CANTP_SESSION_STATE_RECEIVING)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    // 시퀀스 번호 불일치
    if (frame->sequenceNumber != g_cantpSession.rxConsecutiveSeqNum)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    uint16 bytesToCopy = g_cantpSession.rxTotalSize - g_cantpSession.rxBufferCounter;
    if (bytesToCopy > 7)
    {
        bytesToCopy = 7;
    }

    memcpy(g_cantpSession.rxBuffer + g_cantpSession.rxBufferCounter, frame->data, bytesToCopy);
    g_cantpSession.rxBufferCounter += bytesToCopy;

    if (g_cantpSession.rxBufferCounter >= g_cantpSession.rxTotalSize)
    {
        // 모든 데이터 수신 완료
        UDS_HandleFrame(g_cantpSession.rxBuffer, g_cantpSession.rxTotalSize);
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }


    if ((FLOW_CONTROL_BLOCK_SIZE > 0) && (g_cantpSession.rxConsecutiveSeqNum % FLOW_CONTROL_BLOCK_SIZE) == 0)
    {
        sendFlowControl(CANTP_FLOW_STATUS_CONTINUE, FLOW_CONTROL_BLOCK_SIZE, FLOW_CONTROL_SEPARATION_TIME_MS);
    }
    else
    {
        updateTimer();
    }

    g_cantpSession.rxConsecutiveSeqNum = (g_cantpSession.rxConsecutiveSeqNum + 1) % 16;
}

static void handleFlowControl(const CANTP_FlowControlFrame *frame)
{
    if(g_cantpSession.state != CANTP_SESSION_STATE_SENDING)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    if (frame->flowStatus != CANTP_FLOW_STATUS_CONTINUE)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
        return;
    }

    uint8 framesToSend = frame->blockSize;
    if (framesToSend == 0)
        framesToSend = 0xFF;
    for (uint8 i = 0; i < framesToSend; i++)
    {
        CanMessage canMsg;
        canMsg.id = RESPONSE_CANTP_ID;
        canMsg.dlc = 8;
        canMsg.data[0] = (CANTP_FRAME_TYPE_CONSECUTIVE_FRAME << 4) | (g_cantpSession.txConsecutiveSeqNum & 0x0F);

        uint8 bytesToSend = g_cantpSession.txTotalSize - g_cantpSession.txBufferCounter;
        if (bytesToSend > 7)
            bytesToSend = 7;

        memcpy(canMsg.data + 1, g_cantpSession.txBuffer + g_cantpSession.txBufferCounter, bytesToSend);
        if (bytesToSend < 7)
        {
            memset(canMsg.data + 1 + bytesToSend, CANTP_PADDING_BYTE, 7 - bytesToSend);
            canMsg.dlc = bytesToSend + 1;
        }

        Can_SendMsg(&canMsg);
        g_cantpSession.txBufferCounter += bytesToSend;
        g_cantpSession.txConsecutiveSeqNum = (g_cantpSession.txConsecutiveSeqNum + 1) % 16;
    }
    
    if (g_cantpSession.txBufferCounter >= g_cantpSession.txTotalSize)
    {
        g_cantpSession.state = CANTP_SESSION_STATE_IDLE;
    }
    else
    {
        updateTimer();
    }
}