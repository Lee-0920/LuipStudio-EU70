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
 * @date 2012-12-25
 */

#include <stdio.h>
#include <thread>
#include <QThread>
#include <Log.h>
#include "LaiTcpServer.h"

#ifdef _WIN32
    #include <winsock2.h>
    #define CloseSocket(s) closesocket(s)
    #define MSG_NOSIGNAL 0
    typedef int socklen_t;
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
static void LaiTcpServer_CommunicatingThread(LaiTcpServer* lai);


//*******************************************************************
// Lai 实现

static void LaiTcpServer_Reset(LaiTcpServer* lai, Uint16 action);
static Bool LaiTcpServer_IsSendable(LaiTcpServer* lai);
static void LaiTcpServer_Send(LaiTcpServer* lai, Uint8 destAddr, Byte* data, int len);

static const ILaiComm s_kCommInterface =
{
        (IfLaiReset) LaiTcpServer_Reset,
        (IfLaiIsSendable) LaiTcpServer_IsSendable,
        (IfLaiSend) LaiTcpServer_Send
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
void LaiTcpServer_Reset(LaiTcpServer* lai, Uint16 action)
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
Bool LaiTcpServer_IsSendable(LaiTcpServer* lai)
{
    (void)lai;
    return TRUE;
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
void LaiTcpServer_Send(LaiTcpServer* lai, Uint8 destAddr, Byte* buffer, int len)
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

    if (lai->isClientConnected == TRUE)
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
                //int send(SOCKET sock, const char *buf, int len, int flags);
                //sock 为要发送数据的套接字，buf 为要发送的数据的缓冲区地址，len 为要发送的数据的字节数，
                //flags 为发送数据时的选项。
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
 * @brief LaiTcpServer 协议实体初始化。
 * @param lai 具体的Lai层协议实体。
 * @param listenPort 本协议实体作为TCP服务器需要监听的端口号。
 */
void LaiTcpServer_Init(LaiTcpServer* lai, Uint16 listenPort)
{
    memset(lai, 0, sizeof(LaiTcpServer));
    lai->base.comm = (ILaiComm*) &s_kCommInterface;
    lai->isClientConnected = FALSE;
    lai->listenPort = listenPort;
    lai->onConnected = NULL;

}

/**
 * @brief 设置网络连接事件处理器。
 * @param lai 具体的Lai层协议实体。
 * @param handle 当客户端连接状态改变时要回调的函数，详见 @ref IfLaiTcpOnClientConnected 。
 */
void LaiTcpServer_SetConnectionHandle(LaiTcpServer* lai, IfLaiTcpOnClientConnected handle)
{
    lai->onConnected = handle;
}


/**
 * @brief LaiTcpServer 协议实体结束化，释放相关资源。
 */
void LaiTcpServer_Uninit(LaiTcpServer* lai)
{
    (void)lai;
}

/**
 * @brief 开启 LaiTcpServer 实体服务。
 * @param lai LaiTcpServer 实体。
 */
void LaiTcpServer_Start(LaiTcpServer* lai)
{
    // 状态初始化
    lai->recvStatus = FPS_FRAME_TRY_SYNC;
    lai->recvEscStatus = RES_ESC_NORMAL;
    lai->recvBytes = 0;

    // 准备 Windows 的 Socket 环境
#ifdef _WIN32
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2,2), &ws))
    {
        logger->warn("TCPModbus WSAStartup() Failt");
        return;
    }
#endif

    // 创建并启动收发线程
    lai->isCommunicatingThreadRunning = TRUE;
    std::thread threadReceiving(LaiTcpServer_CommunicatingThread, lai);
    threadReceiving.detach();
}

/**
 * @brief 停止 LaiTcpServer 实体服务。
 * @param lai LaiTcpServer 实体。
 */
void LaiTcpServer_Stop(LaiTcpServer* lai)
{
    int sleepTime = 10;
    int timeOut = 3000 / sleepTime;

    // 清除 Windows 的 Socket 环境
#ifdef _WIN32
    WSACleanup();
#endif

    // 安全地终止通信线程
    lai->isCommunicatingThreadRunning = FALSE;
    lai->isClientConnected = FALSE;

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

void LaiTcpServer_CommunicatingThread(LaiTcpServer* lai)
{
    int maxSocket = -1;
    int listenSocket = -1;
    int clientSocket = -1;
    struct timeval tm = {1, 0};

    logger->debug("LaiTcpServer Thread ID = %d", QThread::currentThreadId());
    //int socket(int af, int type, int protocol);
    //af 为地址族（Address Family），也就是 IP 地址类型，常用的有 AF_INET 和 AF_INET6。
    //AF 是“Address Family”的简写，INET是“Inetnet”的简写。AF_INET 表示 IPv4 地址，例如 127.0.0.1；
    //AF_INET6 表示 IPv6 地址，1030::C9B4:FF12:48AA:1A2B。
    // type 为数据传输方式，常用的有 SOCK_STREAM 和 SOCK_DGRAM
    // SOCK_STREAM 表示面向连接的数据传输方式。数据可以准确无误地到达另一台计算机，如果损坏或丢失，可以重新发送，但效率相对较慢。
    //SOCK_DGRAM 表示无连接的数据传输方式。计算机只管传输数据，不作数据校验，如果数据在传输中损坏，或者没有到达另一台计算机，是没有办法补救的。也就是说，数据错了就错了，无法重传。
    //protocol 表示传输协议，常用的有 IPPROTO_TCP 和 IPPTOTO_UDP，分别表示 TCP 传输协议和 UDP 传输协议。
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket <= 0)
    {
        logger->error("LaiTcpServer socket create failed");
        return;
    }
    //绑定套接字到一个IP地址和一个端口上(bind())；
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;//使用IPv4地址  sin_family 和 socket() 的第一个参数的含义相同，取值也要保持一致。
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);// htonl 将long类型数据从主机字节序转换为网络字节序。
    //sin_prot 为端口号。uint16_t 的长度为两个字节，理论上端口号的取值范围为 0~65536，
    //但 0~1023 的端口一般由系统分配给特定的服务程序，
    //例如 Web 服务的端口号为 80，FTP 服务的端口号为 21，所以我们的程序要尽量在 1024~65536 之间分配端口号。
    serverAddress.sin_port = htons(lai->listenPort);//htons 将short类型数据从主机字节序转换为网络字节序。
    //可以认为，sockaddr 是一种通用的结构体，可以用来保存多种类型的IP地址和端口号，
    //而 sockaddr_in 是专门用来保存 IPv4 地址的结构体
    if (bind(listenSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)))
    {
        logger->error("LaiTcpServer socket bind failed");
        return;
    }
    //将套接字设置为监听模式等待连接请求(listen())；
    //sock 为需要进入监听状态的套接字，backlog 为请求队列的最大长度。
    //所谓被动监听，是指当没有客户端请求时，套接字处于“睡眠”状态，只有当接收到客户端请求时，套接字才会被“唤醒”来响应请求。
    if (listen(listenSocket, 10))//10为等待连接数目
    {
        logger->error("LaiTcpServer socket listen failed");
        return;
    }

    int result = 0;
    int len = 0;
    int i;
    int recvBytes;
    Byte* recvBuffer = lai->recvBuffer;
    Byte data;
    Byte buffer[(FRAME_MAX_SIZE+1)/2];
    Bool isFrameDetected = FALSE;

    struct sockaddr_in clientAddress;
    int addressLen = sizeof(clientAddress);
    fd_set readSet, tempSet;
    FD_ZERO(&readSet);//清除一个文件描述符集合
    FD_SET(listenSocket, &readSet);//添加fd到集合
    maxSocket = listenSocket;
    tm.tv_sec = 1;// 设置为要等待的秒数
    tm.tv_usec = 0;//设置为要等待的微秒数

    while(lai->isCommunicatingThreadRunning)
    {
        FD_ZERO(&tempSet);
        tempSet = readSet;

        tm.tv_sec = 1;
        tm.tv_usec = 0;
        //int select(int nfds, fd_set *readset, fd_set *writeset,fd_set* exceptset, struct tim *timeout);
        //需要检查的文件描述字个数（即检查到fd_set的第几位），数值应该比三组fd_set中所含的最大fd值更大，
        //一般设为三组fd_set中所含的最大fd值加1（如在readset,
        //writeset,exceptset中所含最大的fd为5，则nfds=6，因为fd是从0开始的）。
        //设这个值是为提高效率，使函数不必检查fd_set的所有1024位。
        //readset  用来检查可读性的一组文件描述字。
        //writeset 用来检查可写性的一组文件描述字。
        //exceptset 用来检查是否有异常条件出现的文件描述字。(注：错误不包括在异常条件之内)
        //timeout=NULL（阻塞：直到有一个fd位被置为1函数才返回）
        //timeout所指向的结构设为非零时间（等待固定时间：有一个fd位被置为1或者时间耗尽，函数均返回）
        //timeout所指向的结构，时间设为0（非阻塞：函数检查完每个fd后立即返回）
        result = select(maxSocket+1, &tempSet, NULL, NULL, &tm);

        if (result == 0)
        {
            // 等待超时，没有可读写或错误的文件
        }
        else if (result < 0)
        {
            // select 错误
            logger->warn("LaiTcpServer select failed");
            break;
        }
        else
        {
            if (FD_ISSET(listenSocket, &tempSet))   // 新客户端接入
            {
                // 关闭老连接
                if (clientSocket > 0)
                {
                    CloseSocket(clientSocket);
                    FD_CLR(clientSocket, &readSet);//从集合中移去fd
                }
                //请求到来后，接受连接请求，返回一个新的对应于此次连接的套接字(accept())
                //会阻塞进程，直到有客户端连接上来为止
                // 使用新的连接取代老的连接，断开老的连接
                clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, (socklen_t*)&addressLen);

                if (clientSocket > 0)
                {
                    FD_SET(clientSocket, &readSet);//添加fd到集合
                    if (maxSocket < clientSocket)
                        maxSocket = clientSocket;
                    lai->isClientConnected = TRUE;
                    lai->clientSocket = clientSocket;
                    if (lai->onConnected)
                        lai->onConnected(TRUE);

                    logger->debug("LaiTcpServer accept new client: %d.%d.%d.%d:%d",
                                  ((clientAddress.sin_addr.s_addr))&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>8)&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>16)&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>24)&0xFF,
                                  ntohs(clientAddress.sin_port));//ntohs将short类型数据从网络字节序转换为主机字节序
                }
            }
            else if (FD_ISSET(clientSocket, &tempSet)) //测试fd是否在集合中  // 客户端发送了数据
            {
                // 读取数据
                len = recv(clientSocket, (char*)buffer, sizeof(buffer), 0);
                //recv() 返回 0 的唯一时机就是收到FIN包时。
                //FIN 包表示数据传输完毕，计算机收到 FIN 包后就知道对方不会再向自己传输数据，
                //当调用 read()/recv() 函数时，如果缓冲区中没有数据，就会返回 0，表示读到了”socket文件的末尾“。
                if (len <= 0)
                {
                    // 连接被对端断开
                    CloseSocket(clientSocket);
                    FD_CLR(clientSocket, &readSet);
                    clientSocket = 0;
                    lai->isClientConnected = FALSE;
                    lai->clientSocket = clientSocket;
                    if (lai->onConnected)
                        lai->onConnected(FALSE);

                    logger->debug("LaiTcpServer lost client: %d.%d.%d.%d:%d",
                                  ((clientAddress.sin_addr.s_addr))&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>8)&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>16)&0xFF,
                                  ((clientAddress.sin_addr.s_addr)>>24)&0xFF,
                                  ntohs(clientAddress.sin_port));
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
                                lai->recvBackup, recvBytes, 0);

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
    logger->debug("LaiTcpServer Thread Exit");
    //关闭套接字，关闭加载的套接字库(closesocket()/WSACleanup())。
    if (clientSocket > 0)
    {
        CloseSocket(clientSocket);
    }
    if (listenSocket > 0)
    {
        CloseSocket(listenSocket);
    }
}

/** @} */
