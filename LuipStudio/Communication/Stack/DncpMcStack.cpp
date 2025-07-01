/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief DNCP 主控协议栈。
 * @details 为设备构建一个静态全局的协议栈栈体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2016-6-28
 */

#include <stddef.h>
#include <vector>
#include "DNCP/Lai/LaiTcpServer.h"
#include "DNCP/Ll/LinkFrame.h"
#include "DNCP/Ll/Dslp.h"
#include "DNCP/Ll/Llci.h"
#include "DNCP/Net/Trp.h"
#include "DNCP/App/DscpDevice.h"
#include "DNCP/Port/OS/DscpScheduler.h"
//#include "DevCmdTable.h"
#include "DncpMcStack.h"

// 协议栈各层实体对象
#if defined _CS_USING_LAI_UDP
    #include "DNCP/Lai/LaiUdp.h"
    static LaiUdp s_laiDown;
#elif defined _CS_USING_LAI_RS485
    #include "DNCP/Lai/LaiRS485Master.h"
    static LaiRS485Master  s_laiDown;
#else
    #include "DNCP/Lai/LaiSerial.h"
    static LaiSerial s_laiDown;
#endif

static LaiTcpServer s_laiMcUp;
static Dslp s_llMcUp;
static Dslp s_llMcDown;
static Trp s_netMc;
static DscpDevice s_dscpMc;
Llci g_llciMcUp;    // 与控制器的回环
extern Llci g_llciControllerDown;
std::vector<int> addresses;
static void DncpMcStack_OnLaiTcpClientConnected(Bool isConnected);


/**
 * @brief 初始化协议栈。
 * @param[in] serialPort 协议栈使用的串口，Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
    Linux平台为串口设备文件。
 * @return 是否成功初始化。
 *  只有初始化成功，才能继续使用协议栈收发数据。
 */
Bool DncpMcStack_Init(char* serialPort, IConnectStatusNotifiable *handle)
{
    Bool isDownlinkInterfaceInitialized = FALSE;
    Uint8 portMcUp = 1;
    Uint8 port = 0;
    if (nullptr == serialPort)
    {
        serialPort = (char*)_CS_SERIAL_PORT;
    }

    // 各层实体初始化
#if defined _CS_USING_LAI_UDP
    Uint8 portMcDown = 0;
    LaiUdp_Init(&s_laiDown, 12000, portMcDown);     // 对端的链路标识应该一致
#elif defined _CS_USING_LAI_RS485
    isDownlinkInterfaceInitialized = LaiRS485Master_Init(&s_laiDown, serialPort);
    LaiRS485Master_SetConnectionHandle(&s_laiDown, handle);
//    LaiRS485Master_AddSlaveNode(&s_laiDown, LAIRS485MASTER_FirstLevel, 1);
//    LaiRS485Master_AddSlaveNode(&s_laiDown, LAIRS485MASTER_FirstLevel, 16);
//    LaiRS485Master_AddSlaveNode(&s_laiDown, LAIRS485MASTER_FirstLevel, 32);
    for (std::vector<int>::iterator it = addresses.begin();
         it != addresses.end(); it++)
    {
        LaiRS485Master_AddSlaveNode(&s_laiDown, LAIRS485MASTER_FirstLevel, (*it));
    }
    //设置拉取超时时间
    LaiRS485Master_SetPullMaxTimeOut(&s_laiDown, 10);//时间必须大，不然数据回来了，当成不回来，会造成总线异常
#else
    isDownlinkInterfaceInitialized == LaiSerial_Init(&s_laiDown, serialPort);
#endif
    LaiTcpServer_Init(&s_laiMcUp, 11000);           // 对端的链路标识应该一致
    LaiTcpServer_SetConnectionHandle(&s_laiMcUp, DncpMcStack_OnLaiTcpClientConnected);
    Dslp_Init(&s_llMcUp, (Lai*) &s_laiMcUp);
    Dslp_Init(&s_llMcDown, (Lai*) &s_laiDown);
    Llci_Init(&g_llciMcUp, &g_llciControllerDown);
    Trp_Init(&s_netMc);
    DscpDevice_Init(&s_dscpMc);

    // 栈结构配置
    // 上行接口
    Lai_Setup((Lai*) &s_laiMcUp, (LinkAddress) portMcUp);
    Lai_Register((Lai*) &s_laiMcUp, (ILaiHandle*) &s_llMcUp);
    Lai_SetMaxTransNum((Lai*) &s_laiMcUp, 4);
    Ll_Setup((Ll*) &s_llMcUp, (Lai*) &s_laiMcUp, portMcUp);
    Ll_Register((Ll*) &s_llMcUp, NET_PROTOCAL_TRP, (ILlHandle*) &s_netMc);
    // 本地回环接口（连接到控制器）
    Ll_Setup((Ll*) &g_llciMcUp, NULL, 1);
    Ll_Register((Ll*) &g_llciMcUp, NET_PROTOCAL_TRP, (ILlHandle*) &s_netMc);
    // 下行接口
    Lai_Setup((Lai*) &s_laiDown, (LinkAddress) port);
    Lai_Register((Lai*) &s_laiDown, (ILaiHandle*) &s_llMcDown);
    Lai_SetMaxTransNum((Lai*) &s_laiDown, 2);
    Ll_Setup((Ll*) &s_llMcDown, (Lai*) &s_laiDown, port);
    Ll_Register((Ll*) &s_llMcDown, NET_PROTOCAL_TRP, (ILlHandle*) &s_netMc);
    // 网络层：本层级为第2层
    Trp_SetRouteMode(&s_netMc, TRP_ROUTE_MODE_MULTICAST);  // 打开多播路由，以便事件能广播到多个上行接口
    Trp_Setup(&s_netMc, 3, 1, UPLINK_ADDR_MAKE(2, 0), DOWNLINK_MASK_MAKE(0, 0, 0xFF, 0xFF));
    Trp_Register(&s_netMc, APP_PROTOCAL_DSCP, (INetHandle*) &s_dscpMc);
    Trp_AddInterface(&s_netMc, 0,       // 上行到本地控制器，回环接口，一直有效
        UPLINK_ADDR_MAKE(1, 2),
        UPLINK_MASK_MAKE(0xF, 2),
        (Ll*) &g_llciMcUp);
    Trp_AddInterface(&s_netMc, 1,       // 上行到顶层控制器，仅有客户端连接时有效，未连接时不让其匹配
        UPLINK_ADDR_MAKE(1, 1),
        UPLINK_MASK_MAKE(0, 0),
        NULL);
    if (isDownlinkInterfaceInitialized)
    {
        Trp_AddInterface(&s_netMc, 2,       // 其它的下行包转发到这个接口
            DOWNLINK_ADDR_MAKE(0, 0, 0x00, 0),
            DOWNLINK_MASK_MAKE(0, 0, 0x00, 0),
            (Ll*) &s_llMcDown);
    }
    else
    {
        Trp_AddInterface(&s_netMc, 2,       // 添加未正常初始化的接口，永远不让其匹配
            DOWNLINK_ADDR_MAKE(0xF, 0xF, 0xFF, 0xFF),
            DOWNLINK_MASK_MAKE(0, 0, 0x00, 0),
            (Ll*) NULL);
    }
    // 应用层： Mc 设备
//    DscpDevice_Setup(&s_dscpMc, (Net*) &s_netMc, DevCmdTable_GetTable(), DevCmdTable_GetVersion());
    DscpScheduler_Init(&s_dscpMc);

    // 开启服务
    if (isDownlinkInterfaceInitialized)
    {
#if defined _CS_USING_LAI_UDP
        LaiUdp_Start(&s_laiDown);
#elif defined _CS_USING_LAI_RS485
        LaiRS485Master_Start(&s_laiDown);
#else
        LaiSerial_Start(&s_laiDown);
#endif
    }

    LaiTcpServer_Start(&s_laiMcUp);

    return isDownlinkInterfaceInitialized;
}

void DncpMcStack_OnLaiTcpClientConnected(Bool isConnected)
{
    // 根据连接状态，更改网络层的路由策略
    // 仅在客户端连接上时才把数据包转发到该接口，否则丢弃（不让其匹配）
    if (isConnected)
    {
        Trp_AddInterface(&s_netMc, 1,       // 上行到顶层控制器，
            UPLINK_ADDR_MAKE(1, 1),
            UPLINK_MASK_MAKE(0xF, 1),
            (Ll*) &s_llMcUp);
    }
    else
    {
        Trp_AddInterface(&s_netMc, 1,
            UPLINK_ADDR_MAKE(1, 1),
            UPLINK_MASK_MAKE(0, 0),
            NULL);
    }

}

/**
 * @brief 结束协议栈。
 * @note 不使用协议栈时，必须结束它，以释放系统资源。
 */
void DncpMcStack_Uninit(void)
{
    // 结束各层服务
#if defined _CS_USING_LAI_UDP
    LaiUdp_Stop(&s_laiDown);
#elif defined _CS_USING_LAI_RS485
    LaiRS485Master_Stop(&s_laiDown);
#else
    LaiSerial_Stop(&s_laiDown);
#endif
    LaiTcpServer_Stop(&s_laiMcUp);

    DscpScheduler_Uninit();
    DscpDevice_Uninit(&s_dscpMc);
    Trp_Uninit(&s_netMc);
    Dslp_Uninit(&s_llMcUp);
    Dslp_Uninit(&s_llMcDown);
    Llci_Uninit(&g_llciMcUp);

#if defined _CS_USING_LAI_UDP
    LaiUdp_Uninit(&s_laiDown);
#elif defined _CS_USING_LAI_RS485
    LaiRS485Master_Uninit(&s_laiDown);
#else
    LaiSerial_Uninit(&s_laiDown);
#endif
    LaiTcpServer_Uninit(&s_laiMcUp);
}


/**
 * @brief 重新连接指定端口的串口。
 * @param[in] serialPort 协议栈使用的串口，Windows 的名称形如："COM1"；Linux 的名称形如："ttyS0"。
    Linux平台为串口设备文件。
 * @return 是否成功地重新连接。
 */
Bool DncpMcStack_Reconnect(char* serialPort, IConnectStatusNotifiable *handle)
{
    Bool ret = FALSE;
    //Uint8 port = 0;

#if defined _CS_USING_LAI_UDP
    LaiUdp_Stop(&s_laiDown);
    LaiUdp_Uninit(&s_laiDown);
    LaiUdp_Init(&s_laiDown, 10000, 0);
    LaiUdp_Start(&s_laiDown);
    ret = TRUE;
#elif defined _CS_USING_LAI_RS485
    LaiRS485Master_Stop(&s_laiDown);
    LaiRS485Master_Uninit(&s_laiDown);
    if (LaiRS485Master_Init(&s_laiDown, serialPort))
    {
        LaiRS485Master_SetConnectionHandle(&s_laiDown, handle);
        ret = TRUE;
    }
    LaiRS485Master_Start(&s_laiDown);
#else
    LaiSerial_Stop(&s_laiDown);
    LaiSerial_Uninit(&s_laiDown);
    if (LaiSerial_Init(&s_laiDown, serialPort))
    {
        ret = TRUE;
    }

    LaiSerial_Start(&s_laiDown);
#endif

    LaiTcpServer_Stop(&s_laiMcUp);
    LaiTcpServer_Start(&s_laiMcUp);

    return ret;
}

/**
 * @brief DSCP 命令队列处理函数。
 * @note 需要在后台中定时调用。
 */
void DncpMcStack_HandleDscpCmd(void)
{
    DscpDevice_Handle(&s_dscpMc);
}


/** @} */

void DncpMcStack_SetDscpCmdTable(DscpCmdTable cmdTable, DscpVersion version)
{
    DscpDevice_Setup(&s_dscpMc, (Net*) &s_netMc, cmdTable, version);
}

void DncpMcStack_AddSlaveNode(int addr)
{
  addresses.push_back(addr);
}
