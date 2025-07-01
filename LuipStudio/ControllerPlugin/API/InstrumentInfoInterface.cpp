#include "InstrumentInfoInterface.h"
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Code/InstrumentInfoInterface.h"

using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{
namespace API
{

InstrumentInfoInterface::InstrumentInfoInterface(DscpAddress addr) :
        DeviceInterface(addr)
{

}

/**
 * @brief 查询仪器的类型名称。
 * @details 仪器类型是区分不同类别的产品称号，上位机（云平台）将根据这个字段
 *  识别不同的仪器。各类型的字条串列举如下：
 *  - PT62比色法产品：PT62S
 *  - 伏安溶出法产品：
 *  - 实验室产品：
 *  - 电极法产品：
 * @return 仪器类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @note 各仪器在实现该命令时，必须把本字段固化在程序中，即在烧录软件时就设置了正确
 *  的仪器类型字串。一般情况下，本字段不允许任何修改，即使生产工装也不会修改。
 */
String InstrumentInfoInterface::GetType(void)
{
    String type;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_TYPE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        type = (char*) resp->data;
    }

    return type;
}

/**
 * @brief 设置仪器的类型名称。
 * @param name 仪器类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 通常情况下，本命令不对外公开，上位机（包括生产工具）不能随便更改仪器类型。
 * @see DSCP_CMD_III_GET_TYPE
 */
bool InstrumentInfoInterface::SetType(String type)
{
    Uint16 status = DscpStatus::Error;
    if ((type.length() + 1) <= 16)
    {
        DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_TYPE,
            type.c_str(), type.length() + 1));
        SyncCaller syncCaller(m_retries);
        status = syncCaller.SendWithStatus(cmd);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的型号。
 * @details 仪器型号包含了仪器的类型名称和具体的型别，如 PhotoTek 6000 COD。
 * @return 仪器型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 */
String InstrumentInfoInterface::GetModel(void)
{
    String model;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_MODEL));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        model = (char*) resp->data;
    }

    return model;
}

/**
 * @brief 设置仪器的型号。
 * @details 仪器型号包含了仪器的名称和具体的型别，如PT62单通道型号为：PT62S01。
 * @param model 仪器型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MODEL
 */
bool InstrumentInfoInterface::SetModel(String model)
{
    Uint16 status = DscpStatus::Error;
    if ((model.length() + 1) <= 24)
    {
        DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_MODEL,
            model.c_str(), model.length() + 1));
        SyncCaller syncCaller(m_retries);
        status = syncCaller.SendWithStatus(cmd);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的序列号。
 * @details 仪器序列号唯一标识了个体仪器的整个生命周期，供产品追溯时使用。
 * @return 仪器序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @note 仪器序列号的具体格式请参考公司产品相关文档。
 */
String InstrumentInfoInterface::GetSn(void)
{
    String sn;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_SN));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        sn = (char*) resp->data;
    }

    return sn;
}

/**
 * @brief 设置仪器的序列号。
 * @details 仪器序列号唯一标识了个体仪器的整个生命周期，供产品追溯时使用。
 * @param sn 仪器序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 仪器序列号的具体格式请参考公司产品相关文档。
 * @see DSCP_CMD_III_GET_SN
 */
bool InstrumentInfoInterface::SetSn(String sn)
{
    Uint16 status = DscpStatus::Error;
    if ((sn.length() + 1) <= 32)
    {
        DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_SN,
            sn.c_str(), sn.length() + 1));
        SyncCaller syncCaller(m_retries);
        status = syncCaller.SendWithStatus(cmd);
    }
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的UUID。
 * @details 仪器UUID唯一标识了个体仪器，生产时写入。
 * @return 仪器的UUID，Byte[16]，格式见UUID相关规范。
 */
QUuid InstrumentInfoInterface::GetUUID(void)
{
    QUuid id;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_UUID));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        QByteArray array((const char *) resp->data, 16);
        id = QUuid::fromRfc4122(array);
    }

    return id;
}

/**
 * @brief 设置仪器的UUID。
 * @details 仪器UUID唯一标识了个体仪器，生产时写入。
 * @param uuid 要写入的仪器UUID，Byte[16]，格式见UUID相关规范。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_UUID
 */
bool InstrumentInfoInterface::SetUUID(QUuid id)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_UUID,
        id.toRfc4122().data(), id.toRfc4122().size()));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);

    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @return 仪器的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 */
String InstrumentInfoInterface::GetManuFacturer(void)
{
    String manuf;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_MANUFACTURER));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        manuf = (char*) resp->data;
    }

    return manuf;
}

/**
 * @brief 设置仪器的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @param manufacturer 仪器的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MODEL
 */
bool InstrumentInfoInterface::SetManuFacturer(String manufacter)
{
    Uint16 status = DscpStatus::Error;
    qDebug("SetManuFacturer: %s", manufacter.c_str());
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_MANUFACTURER,
        (void *)manufacter.c_str(), manufacter.length() + 1));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的生产日期。
 * @return 仪器的生产日期，Byte[4]：
 *  - @b year Uint16，年。
 *  - @b month Uint8，月。
 *  - @b day Uint8，日。
 */
ManufDate InstrumentInfoInterface::GetManufDate(void)
{
    ManufDate date;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_MANUF_DATE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        date = ManufDate(resp->data);
    }
    return date;
}

/**
 * @brief 设置仪器的生产日期。
 * @param year Uint16，年。
 * @param month Uint8，月。
 * @param day Uint8，日。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MANUF_DATE
 */
bool InstrumentInfoInterface::SetManufDate(ManufDate date)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_MANUF_DATE,
        date.GetData(), 4));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询仪器的系统时间。
 * @return 仪器的系统时间， Uint64 ， 从 1970-01-01T00:00:00 (UTC) 开始计时的毫秒数。
 * @note 实现指示：参考：  QDateTime::currentMSecsSinceEpoch()
 */
Uint64 InstrumentInfoInterface::GetSystemTime(void)
{
    Uint64 utcTime;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_SYSTEM_TIME));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        utcTime = *(Uint64 *) resp->data;
    }
    return utcTime;
}

/**
 * @brief 设置仪器的系统时间。
 * @param utcTime Uint64 UTC时间，从 1970-01-01T00:00:00 (UTC) 开始计时的毫秒数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_SYSTEM_TIME
 */
bool InstrumentInfoInterface::SetSystemTime(Uint64 utcTime)
{
    Uint16 status = DscpStatus::Error;
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_SET_SYSTEM_TIME,
        &utcTime, sizeof(Uint64)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 查询应用软件版本号。
 * @details 软件版本号通常固化在软件代码中，可作后期运维管理用。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
Version InstrumentInfoInterface::GetSoftwareVersion(void)
{
    Version ver;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_SOFTWARE_VERSION));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        ver.SetData(resp->data, 4);
    }

    return ver;
}

/**
 * @brief 查询应用软件标识。
 * @details 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
 *  标准版本（非定制）的格式统一为： STD-[SVN Revision]-[YYYY.MM.DD]
 * @return 软件标识，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
String InstrumentInfoInterface::GetSoftwareLable(void)
{
    String label;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_SOFTWARE_LABEL));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        label = (char*) resp->data;
    }

    return label;
}

/**
 * @brief 查询平台软件版本号。
 * @details 软件版本号通常固化在软件代码中，可作后期运维管理用。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
Version InstrumentInfoInterface::GetPlatFormVersion(void)
{
    Version ver;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_PLATFORM_VERSION));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        ver.SetData(resp->data, 4);
    }

    return ver;
}

/**
 * @brief 查询平台软件标识。
 * @details 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
 *  标准版本（非定制）的格式统一为： STD-[SVN Revision]-[YYYY.MM.DD]
 * @return 软件标识，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
String InstrumentInfoInterface::GetPlatFormLable(void)
{
    String label;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_PLATFORM_LABEL));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        label = (char*) resp->data;
    }

    return label;
}

/**
 * @brief 查询仪器的名称。
 * @details 仪器名称表征了仪器的用途，如： COD Online Analyzer。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
String InstrumentInfoInterface::GetInstrumentName(void)
{
    String name;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_INSTRUMENT_NAME));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        name = (char*) resp->data;
    }

    return name;
}

/**
 * @brief 查询仪器的测量参数。
 * @details 测量参数指仪器能测量的水体中的某一化学元素，如 COD。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
String InstrumentInfoInterface::GetMeasureParameter(void)
{
    String Parameter;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_MEASURE_PARAMETER));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        Parameter = (char*) resp->data;
    }

    return Parameter;
}

/**
 * @brief 查询仪器的测量参数编码。
 * @details 测量参数编码指行业统一定义的水体元素代码，如 COD的编码为011。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 */
String InstrumentInfoInterface::GetMeasureParameterCode(void)
{
    String Code;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_III_GET_MEASURE_PARAMETER_CODE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        Code = (char*) resp->data;
    }

    return Code;
}

}
}
