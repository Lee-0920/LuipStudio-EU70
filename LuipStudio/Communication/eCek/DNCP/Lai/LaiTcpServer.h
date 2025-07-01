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

#ifndef DNCP_LAI_TCP_SERVER_H_
#define DNCP_LAI_TCP_SERVER_H_

#include <mutex>
#include <condition_variable>
#include "Lai.h"

#ifdef __cplusplus
extern "C" {
#endif

//*******************************************************************
// LaiTcpServer 定义

/**
 * @brief LaiTcpServer 接口函数：网络连接事件处理器。
 * @details 当 Lai 层检测到有 LaiTcp 客户端连接上时，通知上层实体及时处理。
 * @param[in] isConnected 连接状态，TRUE 为客户端连接，FALSE为客户端断开连接。
 */
typedef void (* IfLaiTcpOnClientConnected)(Bool isConnected);


/**
 * @brief 基于TCP的Lai层实体对象（服务端）。
 * @details LaiTcpServer 仅支持一个客户端的连接，第二个连接将冲掉第一个连接。
 */
typedef struct LaiTcpServerStruct
{
    Lai base;                           ///< 实现Lai实体的基本数据结构

    // LaiTcpServer实现相关
    Bool isClientConnected;             ///< 是否有客户端连接上
    Uint16 listenPort;                  ///< 监听端口
    int clientSocket;                   ///< 客户端 Socket
    IfLaiTcpOnClientConnected onConnected;    ///< 连接事件回调

    // 线程状态控制
    Bool isCommunicatingThreadRunning;

    // 接收逻辑
    Uint8 recvStatus;                   ///< 接收状态（帧解析状态机）
    Uint8 recvEscStatus;                ///< 接收转义状态
    int recvBytes;                      ///< 接收到的字节长度
    Byte recvBuffer[FRAME_MAX_SIZE];    ///< 接收缓冲
    Byte recvBackup[FRAME_MAX_SIZE];    ///< 接收备用缓冲
}
LaiTcpServer;


//*******************************************************************
// TCP 实现相关

void LaiTcpServer_Init(LaiTcpServer* lai, Uint16 listenPort);
void LaiTcpServer_SetConnectionHandle(LaiTcpServer* lai, IfLaiTcpOnClientConnected handle);

void LaiTcpServer_Start(LaiTcpServer* lai);
void LaiTcpServer_Stop(LaiTcpServer* lai);

void LaiTcpServer_Uninit(LaiTcpServer* lai);


#ifdef __cplusplus
}
#endif

#endif  // DNCP_LAI_TCP_SERVER_H_

/** @} */
