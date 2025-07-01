/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口Socket-UDP的实现。
 * @details 在Socket上，通过UDP实现一个测试用的Lai层实体，作为上层协议的测试桩。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2012-12-20
 */

#include "LaiUdp.h"

#include <stdio.h>
#include <Windows.h>

//*******************************************************************

// 帧接收状态逻辑

// 帧解析状态定义
#define FPS_FRAME_TRY_SYNC      1   // 帧等待同步中（未同步上）
#define FPS_FRAME_RECEIVING     2   // 帧接收中（已同步上）

// 帧解析过程中的转义状态定义
#define RES_ESC_NORMAL          1   // 正常状态，无转义
#define RES_ESC_ACTION          2   // 转义中

//*******************************************************************
// UDP 

// 线程模拟发送和接收中断
static DWORD LaiUdp_SendingThread(LaiUdp* lai);
static DWORD LaiUdp_ReceivingThread(LaiUdp* lai);



//*******************************************************************
// Lai 实现

static void LaiUdp_Reset(LaiUdp* lai, Uint16 action);
static Bool LaiUdp_IsSendable(LaiUdp* lai);
static void LaiUdp_Send(LaiUdp* lai, Uint8 destAddr, Byte* data, int len);

static const ILaiComm s_kCommInterface =
{
        (IfLaiReset) LaiUdp_Reset,
        (IfLaiIsSendable) LaiUdp_IsSendable,
        (IfLaiSend) LaiUdp_Send
};

/**
 * @brief 复位Lai协议状态逻辑。
 * @details 当上层协议体因为通信异常（如等待超时）需要复位Lai实体时调用。
 *  在Lai协议体通信之前不能也没有必要调用本函数。
 * @param lai 具体的Lai层协议实体。
 * @param action 复位动作，可以是以下一个或多个动作的合并（或）：
 *        - @ref LAI_TX_CLEAR
 *        - @ref LAI_RX_CLEAR 
 *        - @ref LAI_TX_ABORT 
 *        - @ref LAI_RX_ABORT 
 */
void LaiUdp_Reset(LaiUdp* lai, Uint16 action)
{
    if (action & LAI_TX_ABORT)
    {
    }

    if (action & LAI_RX_ABORT)
    {
        lai->recvStatus = FPS_FRAME_TRY_SYNC;
        lai->recvEscStatus = RES_ESC_NORMAL;
    }

    if (action & LAI_TX_CLEAR)
    {
        EnterCriticalSection(&(lai->csSending));
        lai->isSending = FALSE;
        lai->sendBytes = 0;
        LeaveCriticalSection(&(lai->csSending));
    }

    if (action & LAI_RX_CLEAR)
    {
        lai->recvBytes =  0;
    }
}

/**
 * @brief 查询接口是否可发送数据。
 * @details 通常在 Send 之前调用。
 *  <p>可发送表示：本实体可立即发送一帧新数据，此时上层可直接调用 Send()
 *      函数把数据发送出去；
 *  <p>不可发送表示：本实体正在发送数据，或者发送缓冲中无剩余空间可供等待发送。
 *      此时上层实体应该等待 OnSendRequest 的到来进行异步发送。
 *  <p>如果正在发送，意味着发送缓冲存在数据或者硬件正在发送数据，
 *      此时上层可等待异步的发送请求。
 * @param lai 具体的Lai层协议实体，如 LaiRs485 。
 */
Bool LaiUdp_IsSendable(LaiUdp* lai)
{
    Bool status;

    EnterCriticalSection(&(lai->csSending));
    status = lai->isSending;
    LeaveCriticalSection(&(lai->csSending));

    return (status == FALSE);
}

#include "Common/MessageDigest.h"
/**
 * @brief 发送一帧数据。
 * @details 请先调用 IsSendable() 查询发送缓冲是否足够，无剩余空间则应该等待
 *   OnSendRequest 的到来进行异步发送。
 * @param lai 具体的Lai层协议实体，如 LaiRs485 。
 * @param destAddr 本帧要发往的目标地址（链路地址）。
 * @param data 要发送的帧数据（逻辑帧的裸数据）。
 * @param len 要发送的帧长度。
 */
void LaiUdp_Send(LaiUdp* lai, Uint8 destAddr, Byte* data, int len)
{
    EnterCriticalSection(&(lai->csSending));
    if (lai->isSending == FALSE)
    {
        // 真正发送工作交给发送线程完成，这里只是把该帧数据压到发送帧队列中
        lai->sendTo = destAddr;
        lai->sendBytes = len;
        memcpy(lai->sendBuffer, data, len);
        lai->isSending = TRUE;

        // 触发发送线程
        SetEvent(lai->hSendEvent);
    }
    LeaveCriticalSection(&(lai->csSending));
	
	/*
    // 圆环测试，直接返回Echo包
    unsigned short sum;
    int recvBytes = len;
    memcpy(lai->recvBackup, data, recvBytes);
    lai->recvBackup[2] |= 0x01;		// TRP控制字改为 TRP_ROUTE_DIR_UPLINK
    lai->recvBackup[7] = 0x01;		// DSCP控制字改为 DSCP_TYPE_RESP_INFO
    sum = MessageDigest_Crc16Ccitt(0, lai->recvBackup, len-2);
    memcpy(lai->recvBackup+len-2, &sum, sizeof(sum));
    lai->base.handle->OnReceived(lai->base.handle,
        lai->recvBackup, recvBytes,
        destAddr);
    */
}



//*******************************************************************
// UDP 实现相关


/**
 * @brief LaiUdp 协议实体初始化。
 * @details 转化成UDP端口时，规则为链路标识（ @p linkLabel ） + 链路地址
 * @param lai 具体的Lai层协议实体。
 * @param linkLabel 链路标识。必须是1000的整数倍。
 *  <p> 系统通过链路标识区分不同的虚拟链路。
 *  同一链路上的两个或多个LaiUdp实体，其链路标识应该一致。
 * @param localPort 本地端口号。
 *  <p> 一般来说，下行接口的端口号为0，上行接口的端口号为设备号（1开始）。
 */
void LaiUdp_Init(LaiUdp* lai, Uint16 linkLabel, Uint8 localPort)
{
    unsigned int timeOut;

    memset(lai, 0, sizeof(LaiUdp));
    lai->base.comm = (ILaiComm*) &s_kCommInterface;
    lai->linkLabel = linkLabel;
    lai->localPort = localPort;

    lai->sockaddrLocal.sin_family = AF_INET;
    lai->sockaddrLocal.sin_port = htons(linkLabel + localPort);
    lai->sockaddrLocal.sin_addr.s_addr = INADDR_ANY;       // 监听本机

    lai->socketLocal = socket(AF_INET, SOCK_DGRAM, 0);

    // 配置Socket属性
    // 发送超时：500 ms
    // 接收超时：5 ms
    timeOut = 500;
    setsockopt(lai->socketLocal, SOL_SOCKET, SO_SNDTIMEO, (char*) &timeOut, sizeof(timeOut));
    timeOut = 5;
    setsockopt(lai->socketLocal, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeOut, sizeof(timeOut));

    // 初始化用于保护共享资源的临界区
    InitializeCriticalSection(&(lai->csSending));

    lai->hSendEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

/**
 * @brief LaiUdp 协议实体结束化，释放相关资源。
 */
void LaiUdp_Uninit(LaiUdp* lai)
{
    (void)lai;
}

/**
 * @brief 开启 LaiUdp 实体服务。
 * @param lai LaiUdp 实体。
 */
void LaiUdp_Start(LaiUdp* lai)
{
    // 状态初始化
    lai->recvStatus = FPS_FRAME_TRY_SYNC;
    lai->recvEscStatus = RES_ESC_NORMAL;
    lai->recvBytes = 0;

    lai->isSending = FALSE;
    lai->sendBytes = 0;

    // 创建并启动收发线程
    lai->isThreadTxRunning = TRUE;
    lai->threadSending = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LaiUdp_SendingThread, lai, 0, 0);
    lai->isThreadRxRunning = TRUE;
    lai->threadReceiving = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LaiUdp_ReceivingThread, lai, 0, 0);
    if (lai->threadReceiving == NULL)
    {
        printf("\nError: CreateThread() fault in LaiUdp_Start!");
    }
}

/**
 * @brief 停止 LaiUdp 实体服务。
 * @param lai LaiUdp 实体。
 */
void LaiUdp_Stop(LaiUdp* lai)
{
    int timeOut = 800;

    // 安全地终止收发线程
    lai->isThreadTxRunning = FALSE;
    lai->isThreadRxRunning = FALSE;

    SetEvent(lai->hSendEvent);  // 唤醒发送线程

    while (timeOut--)
    {
        Sleep(1);

        // 线程退出后，会设置这个标志位
        if (lai->isThreadTxRunning && lai->isThreadRxRunning)
        {
            break;
        }
    }

    // 清除 SOCKET 资源
    closesocket(lai->socketLocal);
}

/**
 * @brief UDP 发送线程。
 * @details 发送线程相当于嵌入式裸系统中的数据发送中断。
 *  - 通过控制发送的速率，来模拟线路的通信带宽；
 *  - 通过引入数据混淆，来模拟线路的通信质量；
 */
DWORD LaiUdp_SendingThread(LaiUdp* lai)
{
#define FIFO_SIZE       16
    Byte fifo[FIFO_SIZE];       // 模拟发送FIFO
    Uint16 idxFifo = 0;         // FIFO 写指针（有效数据长度）
#define FIFO_IS_AVAIL()     (idxFifo < FIFO_SIZE)
#define FIFO_PUSH(data)     (fifo[idxFifo++] = data)
#define FIFO_RESET()        (idxFifo = 0)
#define FIFO_LENTGH()       (idxFifo)

    Bool isSending;
    int idxParse;
    Byte data;

    Bool isEscaping;        // 是否待转义
    Byte dataEscaping;      // 转义后的字符

    Byte* sendBuffer;
    int sendBytes;

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");       // 发往回环接口

    while(lai->isThreadTxRunning)
    {
        WaitForSingleObject(lai->hSendEvent, INFINITE);

        EnterCriticalSection(&(lai->csSending));
        isSending = lai->isSending;
        LeaveCriticalSection(&(lai->csSending));

        if (isSending)     // 有数据要发送
        {
            sendBuffer = lai->sendBuffer;
            sendBytes = lai->sendBytes;
            dest.sin_port = htons(lai->linkLabel + lai->sendTo);

            FIFO_RESET();
            isEscaping = FALSE;

            // 帧起始符
            FIFO_PUSH(FRAME_CHAR_SYNC);

            // 把发送缓冲中的数据搬到发送FIFO中
            for (idxParse = 0; (idxParse < sendBytes) || isEscaping;)
            {
                // FIFO 还有空间则继续解析，无空间则把FIFO发送出去
                if (FIFO_IS_AVAIL())
                {
                    // 处理上次的转义字符
                    if (isEscaping)
                    {
                        FIFO_PUSH(dataEscaping);
                        isEscaping = FALSE;
                    }
                    else
                    {
                        data = sendBuffer[idxParse++];
                        if (data == FRAME_CHAR_SYNC || data == FRAME_CHAR_ESC)
                        {
                            FIFO_PUSH(FRAME_CHAR_ESC);
                            isEscaping = TRUE;
                            dataEscaping = (data == FRAME_CHAR_SYNC) ? FRAME_CHAR_RAW_SYNC : FRAME_CHAR_RAW_ESC;
                        }
                        else
                        {
                            FIFO_PUSH(data);
                        }
                    }
                }
                else    // FIFO 已无空间
                {
                    // 把FIFO中的数据发送出去
                    sendto(lai->socketLocal, fifo, FIFO_LENTGH(), 0, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));
                    FIFO_RESET();
                }
            }

            // 帧结束符
            if (FIFO_IS_AVAIL())   // 不满
            {
                FIFO_PUSH(FRAME_CHAR_SYNC);
                sendto(lai->socketLocal, fifo, FIFO_LENTGH(), 0, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));
            }
            else    // 已满
            {
                sendto(lai->socketLocal, fifo, FIFO_LENTGH(), 0, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));
                FIFO_RESET();
                FIFO_PUSH(FRAME_CHAR_SYNC);
                sendto(lai->socketLocal, fifo, FIFO_LENTGH(), 0, (struct sockaddr*)&dest, sizeof(struct sockaddr_in));
            }

            EnterCriticalSection(&(lai->csSending));
            lai->isSending = FALSE;
            LeaveCriticalSection(&(lai->csSending));

            // 当前帧已发送完毕，请求发送下一帧
            //Sleep(1);
            lai->base.handle->OnSendRequest(lai->base.handle);
        }
    }

    lai->isThreadTxRunning = TRUE;

    return 0;
}

/**
 * @brief UDP 接收线程。
 * @details 接收线程相当于嵌入式裸系统中的数据接收中断。
 */
DWORD LaiUdp_ReceivingThread(LaiUdp* lai)
{
    Byte buffer[FRAME_MAX_SIZE/2];
    struct sockaddr_in sockaddrFrom;
    int sockaddrFromLength = sizeof sockaddrFrom;
    int len;
    int i;
    Byte data;
    UINT16 recvBytes;
    Bool isFrameDetected = FALSE;

    Byte* recvBuffer = lai->recvBuffer;

    // 监听UDP
    bind(lai->socketLocal, (struct sockaddr*)&(lai->sockaddrLocal), sizeof(lai->sockaddrLocal));

    while(lai->isThreadRxRunning)
    {
        len = recvfrom(lai->socketLocal, buffer, sizeof buffer, 0,
            (struct sockaddr*)&sockaddrFrom, &sockaddrFromLength);

        if (len != SOCKET_ERROR)
        {
            // 接到一堆数据，解析中。。。

            // 使用局部变量，以提高效率，减少代码
            recvBytes = lai->recvBytes;

            for (i = 0; i < len; i++)
            {
                data = buffer[i];


                // 帧同步状态机
                if (lai->recvStatus == FPS_FRAME_TRY_SYNC)
                {
                    if (data == FRAME_CHAR_SYNC)
                    {
                        lai->recvStatus = FPS_FRAME_RECEIVING;
                    }
                    else
                    {
                        // 帧外数据统计
                        (lai->base.commStat.outOfFrameBytes)++;
                    }
                }
                else
                {
                    // FPS_FRAME_RECEIVING  帧接收中（已同步上）

                    // 转义状态机
                    if (lai->recvEscStatus == RES_ESC_ACTION)
                    {
                        if (data == FRAME_CHAR_RAW_SYNC)      // 原始字符为同步符
                        {
                            data = FRAME_CHAR_SYNC;
                        }
                        else if (data == FRAME_CHAR_RAW_ESC)  // 原始字符为转义符
                        {
                            data = FRAME_CHAR_ESC;
                        }
                        else
                        {
                            // 未定义的转义状态，通信可能出错，终结该帧
                            data = FRAME_CHAR_SYNC;
                            lai->recvStatus = FPS_FRAME_TRY_SYNC;

                            // 认为解析到一帧
                            isFrameDetected = TRUE;
                        }

                        recvBuffer[recvBytes++] = data;
                        lai->recvEscStatus = RES_ESC_NORMAL;
                    }
                    else
                    {
                        // 未转义

                        if (data == FRAME_CHAR_ESC)
                        {
                            lai->recvEscStatus = RES_ESC_ACTION;
                        }
                        else if (data == FRAME_CHAR_SYNC)
                        {
                            // 忽略空帧，防止帧同步符异常造成之后同步不上的问题
                            if (recvBytes > 0)
                            {
                                // 解析到一帧
                                isFrameDetected = TRUE;
                                lai->recvStatus = FPS_FRAME_TRY_SYNC;
                            }
                        }
                        else
                        {
                            // 正常字符，提交
                            recvBuffer[recvBytes++] = data;
                        }
                    }
                }

                // 检测到新的数据帧
                if (isFrameDetected)
                {
                    ++(lai->base.commStat.recvFrames);

                    // 拷贝到备用缓冲，防止数据被冲掉
                    memcpy(lai->recvBackup, recvBuffer, recvBytes);
                    lai->base.handle->OnReceived(lai->base.handle,
                        lai->recvBackup, recvBytes,
                        ntohs(sockaddrFrom.sin_port) - lai->linkLabel);

                    recvBytes = 0;
                    isFrameDetected = FALSE;
                }
            }

            // 更新到对象
            lai->recvBytes = recvBytes;

            //printf("\n[%d]Received data: %s", ntohs(sockaddrFrom.sin_port) - lai->linkLabel, buffer);
        }
        //else
        //{
        //    len = WSAGetLastError();
        //    printf("\nSOCKET_ERROR while recvfrom: %d", len);
        //}
    }

    lai->isThreadRxRunning = TRUE;

    return 0;
}

/** @} */
