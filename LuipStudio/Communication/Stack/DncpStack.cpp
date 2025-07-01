/**
 * @file
 * @brief DNCP 协议栈。
 * @details 为设备构建一个静态全局的协议栈栈体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2015-3-1
 */

#include "Dscp/DscpAddress.h"
#include "Dscp/DscpEvent.h"
#include "Dscp/DscpRespond.h"
#include "Dscp/DscpStatus.h"
#include "DncpMcStack.h"
#include "DncpStack.h"

// 定义底层使用 Lai 实体
#include "eCek/DNCP/Lai/LaiTcpClient.h"
#include "eCek/DNCP/Ll/LinkFrame.h"
#include "eCek/DNCP/Ll/Dslp.h"
#include "eCek/DNCP/Ll/Llci.h"
#include "eCek/DNCP/App/DscpController.h"
#include <QDebug>
#include <QDateTime>

using namespace std;
using namespace Communication::Dscp;

// 协议栈各层实体对象
static LaiTcpClient s_laiControllerDown;
static Dslp s_llControllerDown;

Llci g_llciControllerDown;
extern Llci g_llciMcUp;
static Trp s_netTrp;
static DscpController s_appDscpController;

namespace Communication
{
namespace Stack
{

//static long sendTime;
//static long recvTime;
//static long timeoutCount;


unique_ptr<DncpStack> DncpStack::m_instance(nullptr);

DncpStack::DncpStack()
{
    m_isInitialized = false;
    m_eventHandler = nullptr;
    m_respondHandler = nullptr;
}

DncpStack::~DncpStack()
{
    // 自动结束协议栈，释放资源
    if (m_isInitialized)
    {
        Uninit();
    }
}

/**
 * @brief 获取全局唯一的 DncpStack 对象（单件）。
 */
DncpStack* DncpStack::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new DncpStack);
    }

    return m_instance.get();
}

void DncpStack::SetEventHandler(IDscpReceivable* handler)
{
    m_eventHandler = handler;
}

void DncpStack::SetRespondHandler(IDscpReceivable* handler)
{
    m_respondHandler = handler;
}

/**
 * @brief 初始化协议栈。
 * @param[in] serialPort 协议栈使用的串口，Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
    Linux平台为串口设备文件。
 * @return 是否成功初始化。
 *  只有初始化成功，才能继续使用协议栈收发数据。
 */
bool DncpStack::Init(char* serialPort,
                     bool bridgeMode,
                     const char* bridgeIP,
                     IConnectStatusNotifiable *handle)
{
    Uint8 port = 0;     // 本地端口号，下行接口为0
    Ll* ll;
    Lai* laiToUse;
    Uint8 upperControllerLabel;

    m_bridgeMode = bridgeMode;

    // 各层实体初始化
    if (m_bridgeMode)
    {
        LaiTcpClient_Init(&s_laiControllerDown, bridgeIP, 11000, 1);
        Dslp_Init(&s_llControllerDown, (Lai*) &s_laiControllerDown);
        Lai_Setup((Lai*) &s_laiControllerDown, (LinkAddress) port);
        Lai_Register((Lai*) &s_laiControllerDown, (ILaiHandle*) &s_llControllerDown);
        laiToUse = (Lai*) &s_laiControllerDown;
        ll = (Ll*) &s_llControllerDown;
        upperControllerLabel = 1;
    }
    else
    {
        if (!DncpMcStack_Init(serialPort, handle))
            return false;
        Llci_Init(&g_llciControllerDown, &g_llciMcUp);      // 回环到本地主控
        laiToUse = NULL;
        ll = (Ll*) &g_llciControllerDown;
        upperControllerLabel = 2;
    }

    Trp_Init(&s_netTrp);
    DscpController_Init(&s_appDscpController);

    // 栈结构配置
    Ll_Setup((Ll*) ll, laiToUse, port);
    Ll_Register((Ll*) ll, NET_PROTOCAL_TRP, (ILlHandle*) &s_netTrp);
    // 本层级为第1层
    Trp_Setup(&s_netTrp, 1, 1, UPLINK_ADDR_MAKE(1, upperControllerLabel), DOWNLINK_MASK_MAKE(0, 0xF, 0xFF, 0xFF));
    Trp_Register(&s_netTrp, APP_PROTOCAL_DSCP, (INetHandle*) &s_appDscpController);
    Trp_AddInterface(&s_netTrp, 0,
            DOWNLINK_ADDR_MAKE(0, 0, 0, 0),
            DOWNLINK_MASK_MAKE(0, 0, 0, 0),
            (Ll*) ll);
    DscpController_Setup(&s_appDscpController, (Net*) &s_netTrp);
    DscpController_Register(&s_appDscpController, (IfDscpControllerOnReceived) OnReceive);

    // 开启服务
    if (m_bridgeMode)
    {
        m_isInitialized = LaiTcpClient_Start(&s_laiControllerDown);
    }
    else
    {
        m_isInitialized = true;
    }

    return m_isInitialized;
}

/**
 * @brief 结束协议栈。
 * @note 不使用协议栈时，必须结束它，以释放系统资源。
 */
void DncpStack::Uninit(void)
{
    m_isInitialized = false;

    if (m_bridgeMode)
    {
        LaiTcpClient_Stop(&s_laiControllerDown);
        LaiTcpClient_Uninit(&s_laiControllerDown);
    }
    else
    {
        DncpMcStack_Uninit();
        Llci_Uninit(&g_llciControllerDown);
    }

    // 结束各层服务
    DscpController_Uninit(&s_appDscpController);
    Trp_Uninit(&s_netTrp);
}

/**
 * @brief 查询协议栈是否已经初始化。
 */
bool DncpStack::IsInitialized(void)
{
    return m_isInitialized;
}

/**
 * @brief 重新连接指定端口的串口。
 * @param[in] serialPort 协议栈使用的串口，Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
    Linux平台为串口设备文件。
 * @return 是否成功地重新连接。
 */
bool DncpStack::Reconnect(char* serialPort, IConnectStatusNotifiable *handle)
{
    Bool ret = false;

    m_isInitialized = false;
    if (m_bridgeMode)
    {
        LaiTcpClient_Stop(&s_laiControllerDown);
        m_isInitialized = LaiTcpClient_Start(&s_laiControllerDown);
    }
    else
    {
        ret = DncpMcStack_Reconnect(serialPort, handle);
        m_isInitialized = true;
    }

    return ret;
}

/**
 * @brief 向 DSCP 设备发送命令。
 * @details 阻塞调用，发送成功或失败后才返回。
 * @param[in] addr 目标设备的链路地址。
 * @param[in] cmd 要发送的命令应码。
 * @param[in] data 命令的参数数据。
 * @param[in] len 命令的参数长度。
 * @return 命令是否成功发送。
 *  <p> 该返回值只是指示命令是否成功被目标设备接收，而不指示该命令的执行情况（回应）。
 */
bool DncpStack::Send(DscpAddress addr, int cmd, void* data, int len)
{
    bool ret = false;

    if (m_isInitialized)
    {
//        sendTime =  QDateTime::currentDateTime().toMSecsSinceEpoch();
//        qDebug() << "DncpStack::Send " << cmd << " = "<< sendTime;

        ret = DscpController_SendCmd(&s_appDscpController, NET_ADDRESS_DOWNLINK_MAKE(addr.a1, addr.a2, addr.a3, addr.a4), (CmdCode)cmd, data, (Uint16)len);
    }
    return ret;
}

void DncpStack::OnReceive(Uint32 addr, Uint8 type, Bool isFollowed, Uint16 code, Byte* data, Uint16 len)
{
    (void)isFollowed;
    DncpStack* dncpStack = m_instance.get();
    DscpAddress sourceAddr(NET_ADDRESS_DOWNLINK_GET_D1(addr),
                           NET_ADDRESS_DOWNLINK_GET_D2(addr),
                           NET_ADDRESS_DOWNLINK_GET_D3(addr),
                           NET_ADDRESS_DOWNLINK_GET_D4(addr));

    if (dncpStack->m_isInitialized)
    {
        switch (type)
        {
        case DSCP_PACK_TYPE_RESP_INFO:
            if (dncpStack->m_respondHandler)
            {
                DscpPackPtr pack(new DscpRespond(sourceAddr, code, data, len));
                dncpStack->m_respondHandler->OnReceive(pack);
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< QDateTime::currentDateTime().toMSecsSinceEpoch();
//                recvTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< recvTime - sendTime << " ms";
//                if((recvTime - sendTime) > 50)
//                {
//                    timeoutCount++;
//                }
//                qDebug() << "@@@@@@@@@@@@@@@@@ timeoutCount = "<< timeoutCount;

            }
            break;

        case DSCP_PACK_TYPE_RESP_STATUS:
            if (dncpStack->m_respondHandler)
            {
                DscpPackPtr pack(new DscpRespond(sourceAddr, code, data, len));
                dncpStack->m_respondHandler->OnReceive(pack);
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< QDateTime::currentDateTime().toMSecsSinceEpoch();
//                recvTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< recvTime - sendTime << " ms";
//                if((recvTime - sendTime) > 50)
//                {
//                    timeoutCount++;
//                }
//                qDebug() << "@@@@@@@@@@@@@@@@@ timeoutCount = "<< timeoutCount;
            }
            break;

        case DSCP_PACK_TYPE_EVENT:
            if (dncpStack->m_eventHandler)
            {
                DscpPackPtr pack(new DscpEvent(sourceAddr, code, data, len));
                dncpStack->m_eventHandler->OnReceive(pack);
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< QDateTime::currentDateTime().toMSecsSinceEpoch();
//                recvTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
//                qDebug() << "DncpStack::OnReceive  " << code << " = "<< recvTime - sendTime << " ms";
//                if((recvTime - sendTime) > 50)
//                {
//                    timeoutCount++;
//                }
//                qDebug() << "@@@@@@@@@@@@@@@@@ timeoutCount = "<< timeoutCount;


            }
            break;

        default:
            break;
        }
    }
}

}
}
