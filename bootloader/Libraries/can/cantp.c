/*********************************************************************************************************************/
/*----------------------------------------------------Includes-------------------------------------------------------*/
/*********************************************************************************************************************/
#include "cantp.h"

#include "Ifx_Types.h"
#include "can.h"
#include "uds.h"

/*********************************************************************************************************************/
/*----------------------------------------------Macros & Constants---------------------------------------------------*/
/*********************************************************************************************************************/

#define RESPONSE_CAN_ID 0x7E8
#define FLOW_CONTROL_BLOCK_SIZE 64
#define BUFFER_SIZE 1024


/*********************************************************************************************************************/
/*------------------------------------------------Type Definition----------------------------------------------------*/
/*********************************************************************************************************************/

typedef struct {
    // 수신(RX) 관련 변수
    uint16 rxTotalSize;      // 받아야 할 전체 데이터의 크기
    
    uint8  rxConsecutiveSeqNum;         // 다음에 받아야 할 Consecutive Frame의 순서 번호

    uint8  rxBuffer[BUFFER_SIZE]; // 수신된 데이터를 저장할 버퍼
    uint16 rxBufferCounter;         // 지금까지 받은 데이터의 크기

    uint32 timer;
} CANTP_RxSession;


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

CANTP_RxSession g_cantp_session;

/*********************************************************************************************************************/
/*----------------------------------------------Function Prototypes--------------------------------------------------*/
/*********************************************************************************************************************/

void CANTP_handleCanMessage(const CanMessage* canMsg);
static void handleSingleFrame(const CANTP_SingleFrame* frame);
static void handleFirstFrame(const CANTP_FirstFrame* frame);
static void sendFlowControl(CANTP_FlowStatus flowStatus, uint8 blockSize, uint8 separationTime);
static void handleConsecutiveFrame(const CANTP_ConsecutiveFrame* frame);


/*********************************************************************************************************************/
/*------------------------------------------- Function Implementations-----------------------------------------------*/
/*********************************************************************************************************************/

void CANTP_handleCanMessage(const CanMessage* canMsg) {
    CANTP_FrameType frameType = (CANTP_FrameType)(canMsg->data[0] >> 4);

    switch (frameType) {
        case CANTP_FRAME_TYPE_SINGLE_FRAME:
        {
            CANTP_SingleFrame  singleFrame;
            singleFrame.frameType = frameType;
            singleFrame.payloadLength = canMsg->data[0] & 0x0F;
            memcpy(singleFrame.data, canMsg->data + 1, singleFrame.payloadLength);

            handleSingleFrame(&singleFrame);
            break;
        }

        case CANTP_FRAME_TYPE_FIRST_FRAME:
        {
            CANTP_FirstFrame  firstFrame;
            firstFrame.frameType = frameType;
            firstFrame.payloadLength = (canMsg->data[0] & 0x0F) << 8 | canMsg->data[1];
            memcpy(firstFrame.data, canMsg->data + 2, 6);

            handleFirstFrame(&firstFrame);
            break;
        }
        case CANTP_FRAME_TYPE_CONSECUTIVE_FRAME:
        {
            CANTP_ConsecutiveFrame  consecutiveFrame;
            consecutiveFrame.frameType = frameType;
            consecutiveFrame.sequenceNumber = canMsg->data[0] & 0x0F;
            memcpy(consecutiveFrame.data, canMsg->data + 1, 7);

            handleConsecutiveFrame(&consecutiveFrame);
            break;
        }
        case CANTP_FRAME_TYPE_FLOW_CONTROL:
        {
            CANTP_FlowControlFrame  flowControlFrame;
            flowControlFrame.frameType = frameType;
            flowControlFrame.flowStatus = canMsg->data[0] & 0x0F;
            flowControlFrame.blockSize = canMsg->data[1];
            flowControlFrame.separationTime = canMsg->data[2];

            // TBD
            // handleFlowControl(&flowControlFrame);
            break;
        }   
        default:
            break;
    }
}

static void handleSingleFrame(const CANTP_SingleFrame* frame) {
    UDS_HandleFrame(frame->data, frame->payloadLength);
}

static void handleFirstFrame(const CANTP_FirstFrame* frame) {
    if(frame->payloadLength > BUFFER_SIZE) {
        sendFlowControl(CANTP_FLOW_STATUS_OVERFLOW, 0, 0);
        return;
    }

    g_cantp_session.rxTotalSize = frame->payloadLength;
    g_cantp_session.rxBufferCounter = 6;
    g_cantp_session.rxConsecutiveSeqNum = 1;
    memcpy(g_cantp_session.rxBuffer, frame->data, 6);

    sendFlowControl(CANTP_FLOW_STATUS_CONTINUE, FLOW_CONTROL_BLOCK_SIZE, 0);
}

static void sendFlowControl(CANTP_FlowStatus flowStatus, uint8 blockSize, uint8 separationTime) {
    CANTP_FlowControlFrame fcFrame;
    fcFrame.frameType = CANTP_FRAME_TYPE_FLOW_CONTROL; // FC 프레임 타입
    fcFrame.flowStatus = flowStatus;
    fcFrame.blockSize = blockSize;
    fcFrame.separationTime = separationTime;

    CanMessage canMsg;
    canMsg.id = RESPONSE_CAN_ID;
    canMsg.dlc = 8;
    canMsg.data[0] = (fcFrame.frameType << 4) | (fcFrame.flowStatus & 0x0F);
    canMsg.data[1] = fcFrame.blockSize;
    canMsg.data[2] = fcFrame.separationTime;

    Can_SendMsg(&canMsg);
}

static void handleConsecutiveFrame(const CANTP_ConsecutiveFrame* frame) {
    if(frame->sequenceNumber != g_cantp_session.rxConsecutiveSeqNum) {
        // 시퀀스 번호 불일치 - 오류 처리
        return;
    }

    uint16 bytesToCopy = g_cantp_session.rxTotalSize - g_cantp_session.rxBufferCounter;
    if(bytesToCopy > 7) {
        bytesToCopy = 7;
    }

    memcpy(g_cantp_session.rxBuffer + g_cantp_session.rxBufferCounter, frame->data, bytesToCopy);
    g_cantp_session.rxBufferCounter += bytesToCopy;
    g_cantp_session.rxConsecutiveSeqNum = (g_cantp_session.rxConsecutiveSeqNum + 1) % 16;

    if(g_cantp_session.rxBufferCounter >= g_cantp_session.rxTotalSize) {
        // 모든 데이터 수신 완료
        UDS_HandleFrame(g_cantp_session.rxBuffer, g_cantp_session.rxTotalSize);
    }
}