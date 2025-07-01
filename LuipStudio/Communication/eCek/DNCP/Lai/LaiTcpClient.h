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

#ifndef DNCP_LAI_TCP_CLIENT_H_
#define DNCP_LAI_TCP_CLIENT_H_

#include <mutex>
#include <condition_variable>
#include "Lai.h"

#ifdef __cplusplus
extern "C" {
#endif

//*******************************************************************
// LaiTcpClient 定义

/**
 * @brief 基于TCP的Lai层实体对象（客户端）。
 */
typedef struct LaiTcpClientStruct
{
    Lai base;                           ///< 实现Lai实体的基本数据结构

    // LaiTcpClient实现相关
    LinkAddress address;                ///< 对端链路地址
    Bool isConnected;                   ///< 是否连接上服务器
    Uint16 serverPort;                  ///< 服务器的监听端口
    unsigned long serverIP;             ///< 服务器IP地址
    int clientSocket;                   ///< 客户端 Socket

    // 线程状态控制
    Bool isCommunicatingThreadRunning;

    // 接收逻辑
    Uint8 recvStatus;                   ///< 接收状态（帧解析状态机）
    Uint8 recvEscStatus;                ///< 接收转义状态
    int recvBytes;                      ///< 接收到的字节长度
    Byte recvBuffer[FRAME_MAX_SIZE];    ///< 接收缓冲
    Byte recvBackup[FRAME_MAX_SIZE];    ///< 接收备用缓冲
}
LaiTcpClient;


//*******************************************************************
// TCP 实现相关

void LaiTcpClient_Init(LaiTcpClient* lai, const char* serverIP, Uint16 serverPort, LinkAddress serverLinkAddress);

Bool LaiTcpClient_Start(LaiTcpClient* lai);
void LaiTcpClient_Stop(LaiTcpClient* lai);

void LaiTcpClient_Uninit(LaiTcpClient* lai);


#ifdef __cplusplus
}
#endif

#endif  // DNCP_LAI_TCP_CLIENT_H_

/** @} */
