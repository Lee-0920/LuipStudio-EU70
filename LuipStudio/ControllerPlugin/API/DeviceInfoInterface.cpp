/**
 * @file
 * @brief 设备信息接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Code/DeviceInfoInterface.h"
#include "ManufDate.h"
#include "Version.h"
#include "DeviceInfoInterface.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 设备信息接口构造。
 * @param[in] addr 设备地址。
 */
DeviceInfoInterface::DeviceInfoInterface(DscpAddress addr /*= 0*/)
    : DeviceInterface(addr)
{

}

/**
 * @brief 设备信息接口构造。
 * @param[in] addr 设备地址。
 * @param[in] info 控制器（板卡）信息。
 */
DeviceInfoInterface::DeviceInfoInterface(DscpAddress addr, ControllerInfo* info)
    : DeviceInterface(addr, info)
{

}

/**
 * @brief 查询设备的类型名称。
 * @details 设备类型是区分不同类别的产品称号，上位机（PC、终端）将根据这个字段
 *  识别不同的设备。各类型的字条串列举如下：
 *  - 主控板：LUMC-S
 *  - 液控板：LULC-S
 *  - 温控板：LUTC-S
 *  - 信号板：LUSC-S
 * @return 设备类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @note 1、各设备在实现该命令时，必须把本字段固化在程序中，即在烧录软件时就设置了正确
 *  的设备类型字串。一般情况下，本字段不允许任何修改，即使生产工装也不会修改。
 * \n 2、具体的设备组件对应的类型名称请参考：《板卡类型规范.pdf》。
 */
String DeviceInfoInterface::GetType()
{
    String type;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_TYPE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        type = (char*)resp->data;
    }

    return type;
}

/**
 * @brief 设置设备的类型名称。
 * @param[in] type 设备类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @return 返回状态，操作是否成功。
 * @note 通常情况下，本命令不对外公开，上位机（包括生产工具）不能随便更改设备类型。
 * @see DSCP_CMD_DII_GET_TYPE
 */
bool DeviceInfoInterface::SetType(String type)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_SET_TYPE,
        type.c_str(), type.length()+1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询设备的型号。
 * @details 设备型号包含了设备的类型名称和具体的型别，如某钥匙型号为：LULC8201。
 * @return 设备型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 */
String DeviceInfoInterface::GetModel()
{
    String model;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_MODEL));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        model = (char*)resp->data;
    }

    return model;
}

/**
 * @brief 设置设备的型号。
 * @details 设备型号包含了设备的名称和具体的型别，如某钥匙型号为：LULC8201。
 * @param[in] model 设备型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_DII_GET_MODEL
 */
bool DeviceInfoInterface::SetModel(String model)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_SET_MODEL,
        model.c_str(), model.length()+1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询设备的序列号。
 * @details 设备序列号唯一标识了个体设备的整个生命周期，供产品追溯时使用。
 * @return 设备序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @note 设备序列号的具体格式请参考公司产品相关文档。
 */
String DeviceInfoInterface::GetSerialNumber()
{
    String sn;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_SN));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        sn = (char*)resp->data;
    }

    return sn;
}

/**
 * @brief 设置设备的序列号。
 * @details 设备序列号唯一标识了个体设备的整个生命周期，供产品追溯时使用。
 * @param[in] sn 设备序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @return 返回状态，操作是否成功。
 * @note 设备序列号的具体格式请参考公司产品相关文档。
 * @see DSCP_CMD_DII_GET_SN
 *
 */
bool DeviceInfoInterface::SetSerialNumber(String sn)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_SET_SN,
        sn.c_str(), sn.length()+1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询设备的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @return 设备的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 */
String DeviceInfoInterface::GetManufacturer()
{
    String manuf;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_MANUFACTURER));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        manuf = (char*)resp->data;
    }

    return manuf;
}

/**
 * @brief 设置设备的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @param[in] manuf 设备的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_DII_GET_MODEL
 */
bool DeviceInfoInterface::SetManufacturer(String manuf)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_SET_MANUFACTURER,
        manuf.c_str(), manuf.length()+1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询设备的生产日期。
 * @return 设备的生产日期，Byte[4]：
 *  - @b year Uint16，年。
 *  - @b month Uint8，月。
 *  - @b day Uint8，日。
 */
ManufDate DeviceInfoInterface::GetManufDate()
{
    ManufDate date;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_MANUF_DATE));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        date = ManufDate(resp->data);
    }
    return date;
}

/**
 * @brief 设置设备的生产日期。
 * @param[in] date 生产日期。
 * - @b year Uint16，年。
 * - @b  month Uint8，月。
 * - @b day Uint8，日。
 * @return 返回状态，操作是否成功。
 * @see DSCP_CMD_DII_GET_MANUF_DATE
 *
 */
bool DeviceInfoInterface::SetManufDate(ManufDate date)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_SET_MANUF_DATE,
        date.GetData(), 4));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询软件版本号。
 * @details 软件版本号通常固化在软件代码中，可作后期运维管理用。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
Version DeviceInfoInterface::GetSoftwareVersion()
{
    Version ver;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_SOFTWARE_VERSION));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        ver.SetData(resp->data, 4);
    }

    return ver;
}

/**
 * @brief 查询软件标识。
 * @details 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
 *  标准版本（非定制）的格式统一为： STD-[SVN Revision]-[YYYY.MM.DD]
 * @return 软件标识，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
String DeviceInfoInterface::GetSoftwareLable()
{
    String lable;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_SOFTWARE_LABEL));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        lable = (char*)resp->data;
    }

    return lable;
}

/**
 * @brief 查询硬件版本号。
 * @details 硬件版本号通常分为PCB和BOM两级，如果硬件上不支持则设为0。其余字段为0。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号，也即是 PCB 版本号；
 *  - @b minor Uint8，次版本号，也即是 BOM 版本号；
 *  - @b revision Uint8，修订版本号，保留；
 *  - @b build Uint8，编译版本号，保留；
 */
Version DeviceInfoInterface::GetHardwareVersion()
{
    Version ver;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DII_GET_HARDWARE_VERSION));
    SyncCaller  syncCaller(m_retries);

    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        ver.SetData(resp->data, 4);
    }
    return ver;
}

}
}
