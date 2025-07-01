/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口 Socket-TCP 的实现。
 * @details 通过 TCP 协议实现一个基于网络的Lai层实体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2012-12-26
 */

#include <stdio.h>
#include <thread>
#include <QThread>
#include <Log.h>
#include "LaiTcpClient.h"

#ifdef _WIN32
    #include <winsock2.h>
    #define CloseSocket(s) closesocket(s)
    #define MSG_NOSIGNAL 0
#else
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CloseSocket(s) close(s)
#endif

//*******************************************************************

// 帧接收状态逻辑

// 帧解析状态定义
#define FPS_FRAME_TRY_SYNC      1   // 帧等待同步中（未同步上）
#define FPS_FRAME_RECEIVING     2   // 帧接收中（已同步上）

// 帧解析过程中的转义状态定义
#define RES_ESC_NORMAL          1   // 正常状态，无转义
#define RES_ESC_ACTION          2   // 转义中

//*******************************************************************
// TCP

// 线程模拟发送和接收中断
static void LaiTcpClient_CommunicatingThread(LaiTcpClient* lai);


//*******************************************************************
// Lai 实现

static void LaiTcpClient_Reset(LaiTcpClient* lai, Uint16 action);
static Bool LaiTcpClient_IsSendable(LaiTcpClient* lai);
static void LaiTcpClient_Send(LaiTcpClient* lai, Uint8 destAddr, Byte* data, int len);

static const ILaiComm s_kCommInterface =
{
        (IfLaiReset) LaiTcpClient_Reset,
        (IfLaiIsSendable) LaiTcpClient_IsSendable,
        (IfLaiSend) LaiTcpClient_Send
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
void LaiTcpClient_Reset(LaiTcpClient* lai, Uint16 action)
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
Bool LaiTcpClient_IsSendable(LaiTcpClient* lai)
{
    return lai->isConnected;
}

#include "Common/MessageDigest.h"
/**
 * @brief 发送一帧数据。如果还没有客户端连接上，调用本函数发送的帧将直接丢弃。
 * @details 请先调用 IsSendable() 查询发送缓冲是否足够，无剩余空间则应该等待
 *   OnSendRequest 的到来进行异步发送。
 * @param lai 具体的Lai层协议实体，如 LaiRs485 。
 * @param destAddr 本帧要发往的目标地址（链路地址）。
 * @param data 要发送的帧数据（逻辑帧的裸数据）。
 * @param len 要发送的帧长度。
 */
void LaiTcpClient_Send(LaiTcpClient* lai, Uint8 destAddr, Byte* buffer, int len)
{
    (void)destAddr;
#define FIFO_SIZE       64
    Byte fifo[FIFO_SIZE];       // 模拟发送FIFO
    Uint16 idxFifo = 0;         // FIFO 写指针（有效数据长度）
#define FIFO_IS_AVAIL()     (idxFifo < FIFO_SIZE)
#define FIFO_PUSH(data)     (fifo[idxFifo++] = data)
#define FIFO_RESET()        (idxFifo = 0)
#define FIFO_LENTGH()       (idxFifo)

    int idxParse;
    Byte data;

    Bool isEscaping;        // 是否待转义
    Byte dataEscaping;      // 转义后的字符

    if (lai->isConnected == TRUE)
    {
        // 转义后，直接阻塞发送
        FIFO_RESET();
        isEscaping = FALSE;

        // 帧起始符
        FIFO_PUSH(FRAME_CHAR_SYNC);

        // 把发送缓冲中的数据搬到发送FIFO中
        for (idxParse = 0; (idxParse < len) || isEscaping;)
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
                    data = buffer[idxParse++];
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
                send(lai->clientSocket, (const char*)fifo, FIFO_LENTGH(), MSG_NOSIGNAL);
                FIFO_RESET();
            }
        }

        // 帧结束符
        if (FIFO_IS_AVAIL())   // 不满
        {
            FIFO_PUSH(FRAME_CHAR_SYNC);
            send(lai->clientSocket, (const char*)fifo, FIFO_LENTGH(), MSG_NOSIGNAL);
        }
        else    // 已满
        {
            send(lai->clientSocket, (const char*)fifo, FIFO_LENTGH(), MSG_NOSIGNAL);
            FIFO_RESET();
            FIFO_PUSH(FRAME_CHAR_SYNC);
            send(lai->clientSocket, (const char*)fifo, FIFO_LENTGH(), MSG_NOSIGNAL);
        }

        // 当前帧已发送完毕，请求发送下一帧
        lai->base.handle->OnSendRequest(lai->base.handle);
    }
}



//*******************************************************************
// TCP 实现相关


/**
 * @brief LaiTcpClient 协议实体初始化，并设置必要的网络连接参数。
 * @param lai 具体的Lai层协议实体。
 * @param serverIP 要连接的 LaiTcpServer 服务器IP。
 * @param serverPort 要连接的 LaiTcpServer 服务端口号。
 * @param serverLinkAddress 对端（服务器）DNCP链路地址，用于标识不同的 LaiTcp 链路。通常设置为1。
 */
void LaiTcpClient_Init(LaiTcpClient* lai, const char* serverIP, Uint16 serverPort, LinkAddress serverLinkAddress)
{
    memset(lai, 0, sizeof(LaiTcpClient));
    lai->base.comm = (ILaiComm*) &s_kCommInterface;
    lai->address = serverLinkAddress;
    lai->isConnected = FALSE;
    lai->clientSocket = 0;
    lai->serverPort = serverPort;
    lai->serverIP = inet_addr(serverIP);
}

/**
 * @brief LaiTcpClient 协议实体结束化，释放相关资源。
 */
void LaiTcpClient_Uninit(LaiTcpClient* lai)
{
    if (lai->clientSocket > 0)
    {
        CloseSocket(lai->clientSocket);
        lai->clientSocket = 0;
    }
}

/**
 * @brief 开启 LaiTcpClient 实体服务。
 * @param lai LaiTcpClient 实体。
 */
Bool LaiTcpClient_Start(LaiTcpClient* lai)
{
    // 状态初始化
    lai->recvStatus = FPS_FRAME_TRY_SYNC;
    lai->recvEscStatus = RES_ESC_NORMAL;
    lai->recvBytes = 0;
    //加载套接字库，创建套接字(WSAStartup()/socket())；
    // 准备 Windows 的 Socket 环境
#ifdef _WIN32
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2,2), &ws))
    {
        logger->warn("TCPModbus WSAStartup() Failt");
        return FALSE;
    }
#endif

    // 连接到服务器
    lai->clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (lai->clientSocket <= 0)
    {
        logger->error("LaiTcpClient socket create failed");
        return FALSE;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = lai->serverIP;
    serverAddress.sin_port = htons(lai->serverPort);
    //向服务器发出连接请求(connect())；
    if (connect(lai->clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)))
    {
        logger->error("LaiTcpClient connect failed");
        CloseSocket(lai->clientSocket);
        lai->clientSocket = 0;
        return FALSE;
    }
    else
    {
        lai->isConnected = TRUE;
        // 创建并启动收发线程
        lai->isCommunicatingThreadRunning = TRUE;
        std::thread threadReceiving(LaiTcpClient_CommunicatingThread, lai);
        threadReceiving.detach();
    }

    return TRUE;
}

/**
 * @brief 停止 LaiTcpClient 实体服务。
 * @param lai LaiTcpClient 实体。
 */
void LaiTcpClient_Stop(LaiTcpClient* lai)
{
    int sleepTime = 10;
    int timeOut = 3000 / sleepTime;

    // 清除 Windows 的 Socket 环境
#ifdef _WIN32
    WSACleanup();
#endif

    // 安全地终止通信线程
    lai->isCommunicatingThreadRunning = FALSE;
    lai->isConnected = FALSE;

    while (timeOut--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // 线程退出后，会设置这个标志位
        if (lai->isCommunicatingThreadRunning)
        {
            lai->isCommunicatingThreadRunning = FALSE;
            break;
        }
    }
}

void LaiTcpClient_CommunicatingThread(LaiTcpClient* lai)
{
    int result = 0;
    int len = 0;
    int i;
    int recvBytes;
    Byte* recvBuffer = lai->recvBuffer;
    Byte data;
    Byte buffer[(FRAME_MAX_SIZE+1)/2];
    Bool isFrameDetected = FALSE;

    struct timeval tm = {1, 0};
    fd_set readSet, tempSet;
    FD_ZERO(&readSet);
    FD_SET(lai->clientSocket, &readSet);
    tm.tv_sec = 1;
    tm.tv_usec = 0;

    logger->debug("LaiTcpClient Thread ID = %d", QThread::currentThreadId());

    while(lai->isCommunicatingThreadRunning)
    {
        FD_ZERO(&tempSet);

        tempSet = readSet;

        tm.tv_sec = 1;
        tm.tv_usec = 0;

        result = select(lai->clientSocket + 1, &tempSet, NULL, NULL, &tm);

        if (result == 0)
        {
            // 等待超时，没有可读写或错误的文件
        }
        else if (result < 0)
        {
            // select 错误
            logger->warn("LaiTcpClient select failed");
            break;
        }
        else
        {
            if (FD_ISSET(lai->clientSocket, &tempSet))  // 客户端发送了数据
            {
                // 读取数据
                len = recv(lai->clientSocket, (char*)buffer, sizeof(buffer), 0);

                if (len <= 0)
                {
                    // 连接被对端断开
                    CloseSocket(lai->clientSocket);
                    FD_CLR(lai->clientSocket, &readSet);
                    lai->isConnected = FALSE;
                    lai->clientSocket = 0;
                    break;
                }
                else
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
                                lai->recvBackup, recvBytes, lai->address);

                            recvBytes = 0;
                            isFrameDetected = FALSE;
                        }
                    }

                    // 更新到对象
                    lai->recvBytes = recvBytes;
                }
            }
            else
            {
            }
        }
    }

    lai->isCommunicatingThreadRunning = TRUE;
    logger->debug("LaiTcpClient Thread Exit");

    if (lai->clientSocket > 0)
    {
        CloseSocket(lai->clientSocket);
        lai->clientSocket = 0;
    }
}

/** @} */
