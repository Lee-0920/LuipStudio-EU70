#include <map>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QTextCodec>

#include "HJ212Platform.h"
#include "V2017DbsProtocol.h"


namespace HJ212
{


V2017DbsProtocol::V2017DbsProtocol(PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform):
    HJ212Protocol(ProtocolVersion::HJ212_2017D, FlagProtocol::HJ2017D, info, callback, platform)
{
    SetDefaultPara(/*HJ212_ST_SURFACEWATER*/ info->st.toInt(), true);
    //2020-11-30 huangyue 客户要求日志用UTF-8格式
    // SetDefaultPara(HJ212_ST_SURFACEWATER, false);
}

V2017DbsProtocol::~V2017DbsProtocol()
{

}


/**
 * @brief 命令响应
 */
void V2017DbsProtocol::OnExecute(HJ212PackPtr pack)
{
    QnRtn qnRtn;
    if(!PacketFormatCheck(pack, qnRtn))
    {
        m_platform->Send(ReplyRequest(pack->GetQN(), (int)qnRtn));
        return;
    }

    int exeRtn = -1;
    std::string qn = pack->GetQN();
    std::string cnStr = pack->GetCN();
    int cn = atoi(cnStr.c_str());

    switch(cn)
    {
    case (int)DbsCNType2017::CmdGetInstrumentTime:  //1011
        exeRtn = CmdGetSystemInfoTime(pack, false);
        break;
    case (int)DbsCNType2017::CmdSetInstrumentTime: //1012
        exeRtn = CmdSetSystemInfoTime(pack, false);
        break;
    case (int)DbsCNType2017::CmdGetSystemTime:  //1014
        exeRtn = CmdGetSystemInfoTime(pack, true);
        break;
    case (int)DbsCNType2017::CmdSetSystemTime: //1015
        exeRtn = CmdSetSystemInfoTime(pack, true);
        break;
    case (int)DbsCNType2017::CmdGetCheckData: //2062:
        exeRtn = CmdGetCheckData(pack);
        break;
    case (int)DbsCNType2017::CmdGetSystemInformation: //3020:
        exeRtn = CmdGetSystemInformation(pack);
        break;
    case (int)DbsCNType2017::CmdSetSystemInformation: //3021:
        exeRtn = CmdSetSystemInformation(pack);
        break;
    case (int)DbsCNType2017::CmdStartMeasureSample: //3044:
        exeRtn = CmdStartMeasure(pack, FlowType::Sample, false);
        break;
    case (int)DbsCNType2017::CmdStartMeasureCheck: //3080:
        exeRtn = CmdStartMeasure(pack, FlowType::StandardCheck);
        break;
    case (int)DbsCNType2017::CmdStartBlankCalibrate: //3085:
        exeRtn = CmdStartMeasure(pack, FlowType::BlankCalibration);
        break;
    case (int)DbsCNType2017::CmdStartStandardCalibrate: //3086:
        exeRtn = CmdStartMeasure(pack, FlowType::StandardCalibration);
        break;
    case (int)DbsCNType2017::CmdStartMeasureRangeCheck: //3084:
        exeRtn = CmdStartMeasure(pack, FlowType::SpanCheck);
        break;

    default:
        m_platform->Send(ReplyRequest(qn, (int)QnRtn::Refuse));
    }

    if (exeRtn >= 0)
        m_platform->Send(ReplyExecuteResult(qn, exeRtn));
}





/**************************************************
 *
 * 结构转字符串
 *
**************************************************/


std::string V2017DbsProtocol::StructToString(TimeSliceData data)
{
    QString dataStr = QString("%1-Avg=%2,%3-Flag=%4;")
                    .arg(data.polId).arg(data.avg,0,'f', GetDecimal(data.polId))
                    .arg(data.polId).arg(data.flag);

    return dataStr.toStdString();
}

std::string V2017DbsProtocol::StructToString(StandardCheckData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-StandardValue=%4,%5-Flag=%6;")
                    .arg(data.polId).arg(data.checkValue,0,'f',decimal)
                    .arg(data.polId).arg(data.standardValue,0,'f',decimal)
                    .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017DbsProtocol::StructToString(EquipmentLog data)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(',')).arg(m_polId);

    dataStr += QString("%1-Info=//%2//").arg(data.infoId).arg(data.content);

    if(dataStr.length() > 13)
    {
        std::string item = dataStr.toStdString();
        if (m_defaultToGB2312)
        {
            QString qStr = QString::fromStdString(item);
            QTextCodec* codec = QTextCodec::codecForName("GB2312");
            QByteArray array = codec->fromUnicode(qStr);
            item = std::string(array.data());
        }
        return item;
    }
    return "";
}


std::string V2017DbsProtocol::StructsToString(QList<EquipmentStatus> datas)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(',')).arg(m_polId);
    for(QList<EquipmentStatus>::iterator infoIt = datas.begin(); infoIt != datas.end(); infoIt++)
        dataStr += QString("%1-Info=%2,").arg((*infoIt).infoId).arg((*infoIt).valueStr);

    dataStr.replace(dataStr.length()-1, 1, ';'); //末尾字符替换为';'
    if(dataStr.length() > 13)
        return dataStr.toStdString();

    return "";
}

std::string V2017DbsProtocol::StructsToString(QList<EquipmentParam> datas)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(',')).arg(m_polId);
    for(QList<EquipmentParam>::iterator infoIt = datas.begin(); infoIt != datas.end(); infoIt++)
        dataStr += QString("%1-Info=%2,").arg((*infoIt).infoId).arg((*infoIt).valueStr);

    dataStr.replace(dataStr.length()-1, 1, ';'); //末尾字符替换为';'
    if(dataStr.length() > 13)
        return dataStr.toStdString();

    return "";
}


/**************************************************
 *
 * 上传数据
 *
**************************************************/

template<typename T>
QMap<std::string, QList<std::string>> V2017DbsProtocol::ConvertDataMap(QList<T> allDataMap)
{
    QMap<std::string, QList<std::string>> allDataStrMap;
    foreach (auto imap, allDataMap)
    {
        std::string timeStr = "DataTime=" + QDateTime::fromString(imap.cDataTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss").toStdString() + ";";
        QList<std::string> dataStrMap;

        std::string dataStr = StructToString(imap);
        if(!dataStr.empty())
        {
            dataStrMap.append(dataStr);
            allDataStrMap.insert(timeStr, dataStrMap);
        }
    }

    return allDataStrMap;
}

template<typename T>
QMap<std::string, QList<std::string>> V2017DbsProtocol::ConvertDataMap(QString dataTime, T allDataMap)
{
    QMap<std::string, QList<std::string>> allDataStrMap;

    std::string timeStr = "DataTime=" + QDateTime::fromString(dataTime, "yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss").toStdString() + ";";
    QList<std::string> dataStrMap;

    std::string dataStr = StructsToString(allDataMap);
    if(!dataStr.empty())
    {
        dataStrMap.append(dataStr);
        allDataStrMap.insert(timeStr, dataStrMap);
    }

    return allDataStrMap;
}


/**
 * @brief 上传心跳
 */
HJ212PackPtr V2017DbsProtocol::UploadHeartPack()
{
    std::string cn = std::to_string((int)DbsCNType2017::UploadHeartPack);
    std::string respCN = "-1";
    FlagRespSubpack flag = FlagRespSubpack::None;
    if(IsNeedRespond(UploadDataType::HeartPack))
    {
        flag = FlagRespSubpack::Resp;
        respCN = HJ212_CN_REPLY_DATA; //等数据应答
    }

    //新建包
    HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, GetCurrentQN(), cn, (int)flag, false, respCN);
    return uploadPack;
}

/**
 * @brief 上传小时数据
 */
QList<HJ212PackPtr> V2017DbsProtocol::UploadHourData(QList<TimeSliceData> allDataMap)
{
    std::string cn = std::to_string((int)DbsCNType2017::UploadHourData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(allDataMap);
    return UploadData(allDataStrMap, UploadDataType::HourData, cn);
}

/**
 * @brief 上传核查数据
 */
QList<HJ212PackPtr> V2017DbsProtocol::UploadStandardCheckData(QList<StandardCheckData> allDataMap)
{
     std::string cn = std::to_string((int)DbsCNType2017::UploadStandardCheckData);
     QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(allDataMap);
     return UploadData(allDataStrMap, UploadDataType::StandardCheckData, cn);
}

/**
 * @brief 上传系统、仪表日志
 */
QList<HJ212PackPtr> V2017DbsProtocol::UploadSystemInfoLog(QList<EquipmentLog> allDataMap)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadInfoLog);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(allDataMap);
    return UploadData(allDataStrMap, UploadDataType::InfoLog, cn);
}

/**
 * @brief 上传系统、仪表状态
 */
QList<HJ212PackPtr> V2017DbsProtocol::UploadSystemInfoStatus(QList<EquipmentStatus> datas)
{
    if (datas.size() <= 0)
        return {};
    std::string cn = std::to_string((int)WryCNType2017::UploadInfoStatus);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas[0].cDataTime, datas);
    return UploadData(allDataStrMap, UploadDataType::InfoStatus, cn);
}

/**
 * @brief 上传系统、仪表参数
 */
QList<HJ212PackPtr> V2017DbsProtocol::UploadSystemInfoParam(QList<EquipmentParam> datas)
{
    if (datas.size() <= 0)
        return {};
    std::string cn = std::to_string((int)WryCNType2017::UploadInfoParam);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas[0].cDataTime, datas);
    return UploadData(allDataStrMap, UploadDataType::InfoParam, cn);
}




/**************************************************
 *
 * 提取数据/设置
 *
**************************************************/

template<typename T>
int V2017DbsProtocol::CmdSendData(HJ212PackPtr pack, UploadDataType type, QList<T> datas)
{
    if (datas.size() <= 0)
        return (int)ExeRtn::NoData;

    //转换
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);

    //组装
    QList<HJ212PackPtr> packList = CmdUploadData(allDataStrMap, type, pack->GetCN(), pack->GetQN());

    //发送
    for(auto item = packList.begin(); item != packList.end(); item++)
    {
        if (!m_platform->Send(*item))
            return (int)ExeRtn::Failed;
    }

    return (int)ExeRtn::Success;
}

/**
 * @brief 获取核查数据
 */
int V2017DbsProtocol::CmdGetCheckData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::StandardCheckData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetStandardCheckDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::StandardCheckData, datas);
}

/**
 * @brief 获取系统时间
 */
int V2017DbsProtocol::CmdGetSystemInfoTime(HJ212PackPtr pack, bool isSys)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //新建包
    std::string cn = std::to_string((int)DbsCNType2017::CmdGetSystemTime);
    int flag = m_defaultNoFlag ? -1 : (int)FlagRespSubpack::None;
    HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, pack->GetQN(), cn, flag, m_defaultQnInCP);

    QDateTime time;
    //获取数采仪时间
    if (isSys)
        time = QDateTime::currentDateTime();
    //获取监测仪表时间
    else
    {
        std::string PolId;
        if(!pack->GetCPParam("PolId", PolId, "&"))
            return (int)ExeRtn::WrongCondition;
        if (PolId != m_polId.toStdString())
            return (int)ExeRtn::WrongCondition;

        uploadPack->AddCPParam("PolId=" + PolId + ";");
        time = QDateTime::currentDateTime();
    }

    std::string systemTime = "SystemTime=" + time.toString("yyyyMMddhhmmss").toStdString();
    uploadPack->AddCPParam(systemTime);

    //发送
    ExeRtn result = m_platform->Send(uploadPack) ? ExeRtn::Success : ExeRtn::Failed;
    return (int)result;
}

/**
 * @brief 设置系统时间
 */
int V2017DbsProtocol::CmdSetSystemInfoTime(HJ212PackPtr pack, bool isSys)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    std::string SystemTime = "";
    pack->GetCPParam("SystemTime", SystemTime, "&");
    QDateTime time = QDateTime::fromString(QString::fromStdString(SystemTime), "yyyyMMddhhmmss");
    if (!time.isValid() || m_callBack.SetSystemTime == NULL)
        return (int)ExeRtn::Failed;

    if (isSys)
    {
        if (!m_callBack.SetSystemTime(time))
            return (int)ExeRtn::Failed;
    }
    else
    {
        //获取监测仪ID
        std::string PolId;
        if(!pack->GetCPParam("PolId", PolId, ";"))
            return (int)ExeRtn::WrongCondition;
        if (PolId != m_polId.toStdString())
            return (int)ExeRtn::WrongCondition;

        if (!m_callBack.SetSystemTime(time))
            return (int)ExeRtn::Failed;
    }

    return (int)ExeRtn::Success;
}

/**
 * @brief 启动仪器流程
 */
int V2017DbsProtocol::CmdStartMeasure(HJ212PackPtr pack, FlowType type, bool needPolId)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    if (needPolId)
    {
        //获取POLID
        std::string PolId;
        if(!pack->GetCPParam("PolId", PolId, "&"))
            return (int)ExeRtn::WrongCondition;

        if (m_polId.toStdString() != PolId)
            return (int)ExeRtn::WrongCondition;
    }

    //启动
    bool check = m_callBack.StartEquipmentsFlow(type);
    return check ? (int)ExeRtn::Success:(int)ExeRtn::Failed;
}

/**
 * @brief 获取仪器信息
 */
int V2017DbsProtocol::CmdGetSystemInformation(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::InfoParam);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //日志因为有时间戳，所以需要先单独放一份，再整合。
    //状态和参数是获取当前的值，所以只需要一个时间戳，两种放一块
    QList<EquipmentLog> allLogs;
    QList<std::string> infoStrLists;

    //遍历监测因子
    for(auto item = polIdInfo.polIdInfos.begin(); item != polIdInfo.polIdInfos.end(); item++)
    {
        QString polId = item.key();
        QList<QString> infoIdList = item.value().keys();

        QString contentStr = "PolId=" + polId + ",";

        //遍历监测信息
        foreach(QString infoId, infoIdList)
        {
            MonitoringSpace::MonitoringInfo data;
            if (!m_platform->FindMonitoringInfo(infoId, data))
                continue;

            switch(data.infoType)
            {
            case (int)MonitoringSpace::InfoType::Logger:
                {
                    QList<EquipmentLog> logs = m_callBack.GetSysEquipmentLogList(polIdInfo.beginTime, polIdInfo.endTime);
                    allLogs.append(logs);
                }
                break;
             case (int)MonitoringSpace::InfoType::Status:
                {
                    EquipmentStatus status;
                    if (m_callBack.GetSysEquipmentStatus(infoId, status))
                        contentStr += QString("%1-Info=%2,").arg(infoId).arg(status.valueStr);
                }
                break;
            case (int)MonitoringSpace::InfoType::Parameter:
                {
                    EquipmentParam param;
                    if (m_callBack.GetEquipmentParam(infoId, param))
                        contentStr += QString("%1-Info=%2,").arg(infoId).arg(param.valueStr);
                }
                break;
            }
        }

        if(contentStr.size()>13)
        {
            contentStr.replace(contentStr.length()-1, 1, ';');
            infoStrLists.push_back(contentStr.toStdString());
        }
    }


    //提取信息状态/参数
    QMap<std::string, QList<std::string>> infoMap; //{DataTime, list{PolId-Info-Value}};
    QString DataTimeStr = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    if(!infoStrLists.empty())
    {
        std::string timeKey = QString("DataTime=%1;").arg(DataTimeStr).toStdString();
        infoMap.insert(timeKey, infoStrLists);
    }

    //提取日志
    for(QList<EquipmentLog>::iterator it = allLogs.begin(); it != allLogs.end(); it++)
    {
        QString timeStr = QDateTime::fromString((*it).cDataTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss");
        QString logStr = QString("PolId=%1%2%3-Info=//%4//;").arg((*it).polId).arg(",").arg((*it).infoId).arg((*it).content);

        QMap<std::string, QList<std::string>>::iterator its = infoMap.find(timeStr.toStdString());
        if(its != infoMap.end()) //已存在该时间数据
        {
            its.value().push_back(logStr.toStdString());
        }
        else //不存在该时间数据
        {
            QList<std::string> logStrLists;
            logStrLists.push_back(logStr.toStdString());

            std::string timeKey = QString("DataTime=%1;").arg(timeStr).toStdString();
            infoMap.insert(timeKey, logStrLists);
        }
    }

    QList<HJ212PackPtr> packList = CmdUploadData(infoMap, UploadDataType::InfoLog, pack->GetCN(), pack->GetQN());

    //发送
    for(auto item = packList.begin(); item != packList.end(); item++)
    {
        if (!m_platform->Send(*item))
            return (int)ExeRtn::Failed;
    }
    return (int)ExeRtn::Success;
}

/**
 * @brief 设置仪器参数
 */
int V2017DbsProtocol::CmdSetSystemInformation(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::InfoParam);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //遍历监测因子
    ExeRtn ret = ExeRtn::Success;
    for(auto item = polIdInfo.polIdInfos.begin(); item != polIdInfo.polIdInfos.end(); item++)
    {
        QString polId = item.key();

        //遍历监测信息
        for(auto infoItem = item.value().begin(); infoItem != item.value().end(); infoItem++)
        {
            QString infoId = infoItem.key();
            QString value  = infoItem.value();

            MonitoringSpace::MonitoringInfo data;
            if (!m_platform->FindMonitoringInfo(infoId, data))
                continue;

            if (data.infoType != (int)MonitoringSpace::InfoType::Parameter)
                continue;

            if (m_callBack.SetEquipmentParam == NULL || !m_callBack.SetEquipmentParam(infoId, value))
            {
                ret = ExeRtn::Failed;
                break;
            }
        }
    }
    return (int)ret;
}




}
