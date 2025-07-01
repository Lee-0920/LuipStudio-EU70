#include <map>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QTextCodec>

#include "HJ212Platform.h"
#include "V2017WryProtocol.h"

#define UPLOAD_CMD_LIMIT 10

using namespace HJ212;


V2017WryProtocol::V2017WryProtocol(PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform):
    HJ212Protocol(ProtocolVersion::HJ212_2017, FlagProtocol::HJ2017, info, callback, platform)
{
    SetDefaultPara(/*HJ212_ST_POLLUTIONWATER*/ info->st.toInt(), false);
}

V2017WryProtocol::~V2017WryProtocol()
{

}

/**
 * @brief 远程控制命令分发接口
 */
void V2017WryProtocol::OnExecute(HJ212PackPtr pack)
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
    case (int)WryCNType2017::CmdSetOvetTimeAndReCount:
        exeRtn = CmdSetOvetTimeAndReCount(pack);
        break;
    case (int)WryCNType2017::CmdGetSystemTime:
        exeRtn = CmdGetSystemTime(pack);
        break;
    case (int)WryCNType2017::CmdSetSystemTime:
        exeRtn = CmdSetSystemTime(pack);
        break;
    case (int)WryCNType2017::CmdGetUploadRtdDataInterval:
        exeRtn = CmdGetUploadRtdDataInterval(pack);
        break;
    case (int)WryCNType2017::CmdSetUploadRtdDataInterval:
        exeRtn = CmdSetUploadRtdDataInterval(pack);
        break;
    case (int)WryCNType2017::CmdGetUploadMinuteDataInterval:
        exeRtn = CmdGetUploadMinuteDataInterval(pack);
        break;
    case (int)WryCNType2017::CmdSetUploadMinuteDataInterval:
        exeRtn = CmdSetUploadMinuteDataInterval(pack);
        break;

    case (int)WryCNType2017::CmdGetRealTimeData:
        exeRtn = CmdGetRealTimeData(pack);
        break;
    case (int)WryCNType2017::CmdGetHourHistoricalData:
        exeRtn = CmdGetHourData(pack);
        break;
    case (int)WryCNType2017::CmdGetCheckData:
        exeRtn = CmdGetCheckData(pack);
        break;
    case (int)WryCNType2017::CmdGetAddstandardData:
        exeRtn = CmdGetAddstandardData(pack);
        break;
    case (int)WryCNType2017::CmdGetParallelData:
        exeRtn = CmdGetParallelData(pack);
        break;
    case (int)WryCNType2017::CmdGetZeroCheckData:
        exeRtn = CmdGetZeroCheckData(pack);
        break;
    case (int)WryCNType2017::CmdGetRangeCheckData:
        exeRtn = CmdGetRangeCheckData(pack);
        break;

    case (int)WryCNType2017::CmdStartZeroRangeCheck:
        exeRtn = CmdStartZeroRangeCheck(pack);
        break;
    case (int)WryCNType2017::CmdStartMeasureSample:
        exeRtn = CmdStartMeasureSample(pack);
        break;
    case (int)WryCNType2017::CmdStartClean:
        exeRtn = CmdStartClean(pack);
        break;
    case (int)WryCNType2017::CmdStartComparisonMeasureSample:
        exeRtn = CmdStartComparisonMeasureSample(pack);
        break;

    case (int)WryCNType2017::CmdStartMeasureCheck:
        exeRtn = CmdStartMeasureCheck(pack);
        break;
    case (int)WryCNType2017::CmdStartMeasureAddstandard:
        exeRtn = CmdStartMeasureAddstandard(pack);
        break;
    case (int)WryCNType2017::CmdStartMeasureParallel:
        exeRtn = CmdStartMeasureParallel(pack);
        break;
    case (int)WryCNType2017::CmdStartMeasureZeroCheck:
        exeRtn = CmdStartMeasureZeroCheck(pack);
        break;
    case (int)WryCNType2017::CmdStartMeasureRangeCheck:
        exeRtn = CmdStartMeasureRangeCheck(pack);
        break;
    case (int)WryCNType2017::CmdStartBlankCalibrate:
        exeRtn = CmdStartBlankCalibrate(pack);
        break;
    case (int)WryCNType2017::CmdStartStandardCalibrate:
        exeRtn = CmdStartStandardCalibrate(pack);
        break;

    case (int)WryCNType2017::CmdGetSystemInformation: //3020
        exeRtn = CmdGetEquipmentInfo(pack);
        break;
    case (int)WryCNType2017::CmdSetSystemInformation: //3021
        exeRtn = CmdSetEquipmentInfo(pack);
        break;

    case (int)WryCNType2017::CmdSetOverValue:
        exeRtn = CmdSetOverValue(pack);
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


std::string V2017WryProtocol::StructsToString(QList<EquipmentStatus> datas)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(';')).arg(m_polId);
    for(QList<EquipmentStatus>::iterator infoIt = datas.begin(); infoIt != datas.end(); infoIt++)
        dataStr += QString("%1-Info=%2;").arg((*infoIt).infoId).arg((*infoIt).valueStr);

    dataStr.replace(dataStr.length()-1, 1, ';'); //末尾字符替换为';'
    if(dataStr.length() > 13)
        return dataStr.toStdString();

    return "";
}

std::string V2017WryProtocol::StructsToString(QList<EquipmentParam> datas)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(';')).arg(m_polId);
    for(QList<EquipmentParam>::iterator infoIt = datas.begin(); infoIt != datas.end(); infoIt++)
        dataStr += QString("%1-Info=%2;").arg((*infoIt).infoId).arg((*infoIt).valueStr);

    dataStr.replace(dataStr.length()-1, 1, ';'); //末尾字符替换为';'
    if(dataStr.length() > 13)
        return dataStr.toStdString();

    return "";
}

std::string V2017WryProtocol::StructToString(EquipmentLog data)
{
    QString dataStr = "";
    if (m_polId != HJ212_NO_POLID)
        dataStr += QString("PolId=%1"+ QString(';')).arg(m_polId);

    dataStr += QString("%1-Info=//%2//").arg(data.infoId).arg(data.content);

    if(dataStr.length() > 13)
        return dataStr.toStdString();
    return "";
}

std::string V2017WryProtocol::StructToString(RealTimeData data)
{
    int decimal = GetDecimal(data.polId);

    QString dataStr;
    if (!data.mDateTime.isNull())
        dataStr += QString("%1-SampleTime=%2,").arg(data.polId).arg(data.mDateTime);

    dataStr += QString("%1-Rtd=%2,%3-Flag=%4;").arg(data.polId).arg(data.value,0,'f',decimal).arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(TimeSliceData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Cou=%2,%3-Min=%4,%5-Avg=%6,%7-Max=%8,%9-Flag=%10;")
                    .arg(data.polId).arg(data.cou,0,'f',decimal)
                    .arg(data.polId).arg(data.min,0,'f',decimal)
                    .arg(data.polId).arg(data.avg,0,'f',decimal)
                    .arg(data.polId).arg(data.max,0,'f',decimal)
                    .arg(data.polId).arg(data.flag);

    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(StandardCheckData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-StandardValue=%4,%5-Flag=%6;")
                    .arg(data.polId).arg(data.checkValue,0,'f',decimal)
                    .arg(data.polId).arg(data.standardValue,0,'f',decimal)
                    .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(ZeroCheckData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-StandardValue=%4,%5-SpanValue=%6,%7-Flag=%8;")
            .arg(data.polId).arg(data.checkValue,0,'f',decimal)
            .arg(data.polId).arg(data.standardValue,0,'f',decimal)
            .arg(data.polId).arg(data.SpanValue,0,'f',decimal)
            .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(SpanCheckData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-StandardValue=%4,%5-SpanValue=%6,%7-Flag=%8;")
            .arg(data.polId).arg(data.checkValue,0,'f',decimal)
            .arg(data.polId).arg(data.standardValue,0,'f',decimal)
            .arg(data.polId).arg(data.SpanValue,0,'f',decimal)
            .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(AddStandardData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-WaterTime=%4,%5-Water=%6,%7-Chroma=%8,"\
                              "%9-Volume=%10,%11-DVolume=%12,%13-Flag=%14;")
                            .arg(data.polId).arg(data.checkValue,0,'f',decimal)
                            .arg(data.polId).arg(QDateTime::fromString(data.waterTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss"))
                            .arg(data.polId).arg(data.waterValue,0,'f',decimal)
                            .arg(data.polId).arg(data.chroma,0,'f',decimal)
                            .arg(data.polId).arg(data.sVolume,0,'f',decimal)
                            .arg(data.polId).arg(data.dVolume,0,'f',decimal)
                            .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}

std::string V2017WryProtocol::StructToString(ParallelData data)
{
    int decimal = GetDecimal(data.polId);
    QString dataStr = QString("%1-Check=%2,%3-WaterTime=%4,%5-Water=%6,%7-Flag=%8;")
            .arg(data.polId).arg(data.checkValue,0,'f',decimal)
            .arg(data.polId).arg(QDateTime::fromString(data.waterTime,"yyyy-MM-dd hh:mm:ss").toString("yyyyMMddhhmmss"))
            .arg(data.polId).arg(data.waterValue,0,'f',decimal)
            .arg(data.polId).arg(data.flag);
    return dataStr.toStdString();
}


template<typename T>
QMap<std::string, QList<std::string>> V2017WryProtocol::ConvertDataMap(QList<T> allDataMap)
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
QMap<std::string, QList<std::string>> V2017WryProtocol::ConvertDataMap(QString dataTime, T allDataMap)
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



/**************************************************
 *
 * 上传数据
 *
**************************************************/

/**
 * @brief 上传心跳
 */
HJ212PackPtr V2017WryProtocol::UploadHeartPack()
{
    std::string cn = std::to_string((int)WryCNType2017::UploadHeartPack);
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

QList<HJ212PackPtr> V2017WryProtocol::UploadRtdData(QList<RealTimeData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadRtdData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::RtdData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadHourData(QList<TimeSliceData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadHourData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::HourData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadStandardCheckData(QList<StandardCheckData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadStandardCheckData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::StandardCheckData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadAddStandardData(QList<AddStandardData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadAddStandardData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::AddstandardData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadParallelData(QList<ParallelData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadParallelData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::ParallelData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadZeroCheckData(QList<ZeroCheckData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadZeroCheckData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::ZeroCheckData, cn);
}

QList<HJ212PackPtr> V2017WryProtocol::UploadSpanCheckData(QList<SpanCheckData> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadSpanCheckData);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::SpanCheckData, cn);
}


QList<HJ212PackPtr> V2017WryProtocol::UploadSystemInfoLog(QList<EquipmentLog> datas)
{
    std::string cn = std::to_string((int)WryCNType2017::UploadInfoLog);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas);
    return UploadData(allDataStrMap, UploadDataType::InfoLog, cn);
}

/**
 * @brief 上传系统、仪表状态
 */
QList<HJ212PackPtr> V2017WryProtocol::UploadSystemInfoStatus(QList<EquipmentStatus> datas)
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
QList<HJ212PackPtr> V2017WryProtocol::UploadSystemInfoParam(QList<EquipmentParam> datas)
{
    if (datas.size() <= 0)
        return {};
    std::string cn = std::to_string((int)WryCNType2017::UploadInfoParam);
    QMap<std::string, QList<std::string>> allDataStrMap = ConvertDataMap(datas[0].cDataTime, datas);
    return UploadData(allDataStrMap, UploadDataType::InfoParam, cn);
}

/**************************************************
 *
 * 参数命令
 *
**************************************************/

/**
 * @code 1000
 * @brief 命令-设置超时时间及重发次数
 */
int V2017WryProtocol::CmdSetOvetTimeAndReCount(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    int overTime = 0;
    int reCount = 0;
    pack->GetCPParam("OverTime", overTime, ";");
    pack->GetCPParam("ReCount", reCount, "&");

    QString onePattern("^(10|[0-9])");  //匹配0-10
    QRegExp oneRx(onePattern);

    QString threePattern("^[1-9]\\d{0,2}$");  //匹配1-999
    QRegExp threeRx(threePattern);

    if(oneRx.exactMatch(QString::number(reCount)) && threeRx.exactMatch(QString::number(overTime)))
    {
        m_platformInfo->overtime = overTime;
        m_platformInfo->recount = reCount;

        if (m_callBack.SavePlatformInfo(*m_platformInfo))
            return (int)ExeRtn::Success;
    }

    return (int)ExeRtn::Failed;
}


/**
 * @code 1011
 * @brief 命令-提取现场机时间
 */
int V2017WryProtocol::CmdGetSystemTime(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取POLID
    std::string PolId;
    if(!pack->GetCPParam("PolId", PolId, "&"))
        return (int)ExeRtn::WrongCondition;

    if (m_polId.toStdString() != PolId)
        return (int)ExeRtn::WrongCondition;

    //新建包
    HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, pack->GetQN(), pack->GetCN(), (int)FlagRespSubpack::None, m_defaultQnInCP);
    uploadPack->AddCPParam("PolId=" + PolId + ";");

    //获取时间
    std::string systemTime = "SystemTime=" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss").toStdString();
    uploadPack->AddCPParam(systemTime);

    if (!m_platform->Send(uploadPack))
        return (int)ExeRtn::Failed;

    return (int)ExeRtn::Success;
}

/**
 * @code 1012
 * @brief 命令-设置现场机时间
 */
int V2017WryProtocol::CmdSetSystemTime(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取POLID
    std::string PolId;
    if(!pack->GetCPParam("PolId", PolId, ";"))
        return (int)ExeRtn::WrongCondition;

    if (m_polId.toStdString() != PolId)
        return (int)ExeRtn::WrongCondition;

    //获取时间
    std::string SystemTime;
    pack->GetCPParam("SystemTime", SystemTime, "&");
    QDateTime time = QDateTime::fromString(QString::fromStdString(SystemTime), "yyyyMMddhhmmss");
    if (!time.isValid())
        return (int)ExeRtn::WrongCondition;

    if (!m_callBack.SetSystemTime(time))
        return (int)ExeRtn::Failed;
    return (int)ExeRtn::Success;
}


/**
 * @code 1061
 * @brief 命令-提取实时数据间隔
 */
int V2017WryProtocol::CmdGetUploadRtdDataInterval(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //新建包
    HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, pack->GetQN(), pack->GetCN(), (int)FlagRespSubpack::None, m_defaultQnInCP);

    UploadItem item = m_platform->GetUploadItem(UploadDataType::RtdData);
    char str[20] = {0};
    sprintf(str, "RtdInterval=%d", item.interval);
    uploadPack->AddCPParam(str);

    if (!m_platform->Send(uploadPack))
        return (int)ExeRtn::Failed;

    return (int)ExeRtn::Success;
}

/**
 * @code 1062
 * @brief 命令-设置实时数据间隔
 */
int V2017WryProtocol::CmdSetUploadRtdDataInterval(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    int RtdInterval;
    if(!pack->GetCPParam("RtdInterval", RtdInterval, "&"))
        return (int)ExeRtn::WrongCondition;

    QString threePattern("^[1-9]\\d{0,2}$");  //匹配1-999
    QRegExp threeRx(threePattern);

    if(threeRx.exactMatch(QString::number(RtdInterval)))
    {
        UploadItem& item = m_platform->GetUploadItem(UploadDataType::RtdData);
        item.interval = RtdInterval;

        m_callBack.SaveUploadItem(item);
        return (int)ExeRtn::Success;
    }

    return (int)ExeRtn::Failed;
}

/**
 * @code 1063
 * @brief 命令-提取分钟数据间隔
 */
int V2017WryProtocol::CmdGetUploadMinuteDataInterval(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //新建包
    HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, pack->GetQN(), pack->GetCN(), (int)FlagRespSubpack::None, m_defaultQnInCP);

    UploadItem item = m_platform->GetUploadItem(UploadDataType::MinuteData);
    char str[20] = {0};
    sprintf(str, "MinInterval=%d", item.interval);
    uploadPack->AddCPParam(str);

    if (!m_platform->Send(uploadPack))
        return (int)ExeRtn::Failed;

    return (int)ExeRtn::Success;
}

/**
 * @code 1064
 * @brief 命令-设置分钟数据间隔
 */
int V2017WryProtocol::CmdSetUploadMinuteDataInterval(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    int MinInterval;
    if(!pack->GetCPParam("MinInterval", MinInterval, "&"))
        return (int)ExeRtn::WrongCondition;

    QString threePattern("^[1-9]\\d{0,2}$");  //匹配1-999
    QRegExp threeRx(threePattern);

    if(threeRx.exactMatch(QString::number(MinInterval)))
    {
        UploadItem& item = m_platform->GetUploadItem(UploadDataType::MinuteData);
        item.interval = MinInterval;

        m_callBack.SaveUploadItem(item);
        return (int)ExeRtn::Success;
    }

    return (int)ExeRtn::Failed;
}

/**
 * @brief 命令-设置超标值
 */
int V2017WryProtocol::CmdSetOverValue(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //TODO ??
    return (int)ExeRtn::Failed;
}

/**************************************************
 *
 * 数据命令
 *
**************************************************/

template<typename T>
int V2017WryProtocol::CmdSendData(HJ212PackPtr pack, UploadDataType type, QList<T> datas)
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
 * @brief 提取监测指标实时数据
 */
int V2017WryProtocol::CmdGetRealTimeData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::RtdData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto data = m_callBack.GetRealTimeData();
    if (data.polId.isEmpty())
        return (int)ExeRtn::NoData;

    return CmdSendData(pack, UploadDataType::RtdData, QList<RealTimeData>() << data);
}

/**
 * @brief 提取监测指标实时数据
 */
int V2017WryProtocol::CmdGetHourData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::HourData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetHourDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::HourData, datas);
}

/**
 * @brief 获取核查数据
 */
int V2017WryProtocol::CmdGetCheckData(HJ212PackPtr pack)
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
 * @brief 取加标回收数据
 */
int V2017WryProtocol::CmdGetAddstandardData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::AddstandardData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetAddStandardDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::AddstandardData, datas);
}

/**
 * @brief 取平行样数据
 */
int V2017WryProtocol::CmdGetParallelData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::ParallelData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetParallelDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::ParallelData, datas);
}

/**
 * @brief 取零点核查数据
 */
int V2017WryProtocol::CmdGetZeroCheckData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::ZeroCheckData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetZeroCheckDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::ZeroCheckData, datas);
}

/**
 * @brief 取跨度核查数据
 */
int V2017WryProtocol::CmdGetRangeCheckData(HJ212PackPtr pack)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取监测因子信息
    CmdPackPolIdInfo polIdInfo = GetCmdPackPolIdInfo(pack, UploadDataType::SpanCheckData);
    QList<QString> polIds = polIdInfo.polIdInfos.keys();
    if (polIds.size() > 0 && polIds.indexOf(m_polId) < 0)
        return (int)ExeRtn::WrongCondition;

    //回调接口获取数据
    auto datas = m_callBack.GetSpanCheckDataList(polIdInfo.beginTime, polIdInfo.endTime);

    return CmdSendData(pack, UploadDataType::SpanCheckData, datas);
}

/**************************************************
 *
 * 控制命令
 *
**************************************************/

int V2017WryProtocol::CmdStartFlow(HJ212PackPtr pack, FlowType type)
{
    m_platform->Send(ReplyRequest(pack->GetQN(), (int)QnRtn::Accept));

    //获取POLID
    std::string PolId;
    if(!pack->GetCPParam("PolId", PolId, "&"))
        return (int)ExeRtn::WrongCondition;

    if (m_polId.toStdString() != PolId)
        return (int)ExeRtn::WrongCondition;

    //启动
    bool check = m_callBack.StartEquipmentsFlow(type);
    return check ? (int)ExeRtn::Success:(int)ExeRtn::Failed;
}


/**
 * @brief 命令-零点校准量程校准
 */
int V2017WryProtocol::CmdStartZeroRangeCheck(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::Calibrate);
}

/**
 * @brief 命令-即时采样
 */
int V2017WryProtocol::CmdStartMeasureSample(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::Sample);
}

/**
 * @brief 命令-启动清洗
 */
int V2017WryProtocol::CmdStartClean(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::clean);
}

/**
 * @brief 命令-比对采样
 */
int V2017WryProtocol::CmdStartComparisonMeasureSample(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::Parallel);
}

/**
 * @brief 命令-启动单台仪表标液核查
 */
int V2017WryProtocol::CmdStartMeasureCheck(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::StandardCheck);
}

/**
 * @brief 命令-启动单台仪表加标回收
 */
int V2017WryProtocol::CmdStartMeasureAddstandard(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::Addstandard);
}

/**
 * @brief 命令-启动单台仪表平行样测试
 */
int V2017WryProtocol::CmdStartMeasureParallel(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::Parallel);
}

/**
 * @brief 命令-启动单台仪表零点核查
 */
int V2017WryProtocol::CmdStartMeasureZeroCheck(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::ZeroCheck);
}

/**
 * @brief 命令-启动单台仪表跨度核查
 */
int V2017WryProtocol::CmdStartMeasureRangeCheck(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::SpanCheck);
}

/**
 * @brief 命令-启动空白校准
 */
int V2017WryProtocol::CmdStartBlankCalibrate(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::BlankCalibration);
}

/**
 * @brief 命令-启动标样校准
 */
int V2017WryProtocol::CmdStartStandardCalibrate(HJ212PackPtr pack)
{
    return CmdStartFlow(pack, FlowType::StandardCalibration);
}



/**************************************************
 *
 * 状态|参数|日志命令
 *
**************************************************/

/**
 * @brief 获取参数和状态
 */
int V2017WryProtocol::CmdGetEquipmentInfo(HJ212PackPtr pack)
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
        if (polId != m_polId)
            continue;

        QList<QString> infoIdList = item.value().keys();
        QString contentStr = "PolId=" + polId + ";";

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
                        contentStr += QString("%1-Info=%2;").arg(infoId).arg(status.valueStr);
                }
                break;
            case (int)MonitoringSpace::InfoType::Parameter:
                {
                    EquipmentParam param;
                    if (m_callBack.GetEquipmentParam(infoId, param))
                        contentStr += QString("%1-Info=%2;").arg(infoId).arg(param.valueStr);
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
        QString logStr = QString("PolId=%1%2%3-Info=//%4//;").arg((*it).polId).arg(";").arg((*it).infoId).arg((*it).content);

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
 * @brief 设置采样器参数
 */
int V2017WryProtocol::CmdSetEquipmentInfo(HJ212PackPtr pack)
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
        if (polId != m_polId)
            continue;

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
