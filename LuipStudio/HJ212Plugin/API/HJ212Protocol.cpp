#include "HJ212Protocol.h"
#include "HJ212Platform.h"

namespace HJ212
{

HJ212Protocol::HJ212Protocol(ProtocolVersion version, FlagProtocol flagProtocol, PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform)
{
    m_protocolVersion = version;
    m_flagProtocol = flagProtocol;
    m_platformInfo = info;
    m_callBack = callback;
    m_platform = platform;
    m_polId = m_callBack.GetPolId();
}

HJ212Protocol::~HJ212Protocol()
{

}

void HJ212Protocol::SetDefaultPara(int st,
                                   bool toGB2312,
                                   bool noFlag,
                                   bool qnInCP,
                                   bool isOldPolId)
{
    m_defaultST = st;
    m_defaultToGB2312 = toGB2312;
    m_defaultNoFlag = noFlag;
    m_defaultQnInCP = qnInCP;
    m_defaultIsOldPolId = isOldPolId;
}

bool HJ212Protocol::IsNeedRespond(UploadDataType type)
{
    if (m_callBack.IsNeedRespond != NULL)
        return m_callBack.IsNeedRespond(type);
    return false;
}

int HJ212Protocol::GetDecimal(QString polId)
{
    MonitoringSpace::MonitoringFactor data;
    if (!m_platform->FindMonitoringFactor(polId, data))
        return 2;
    return data.decimal;
}

bool HJ212Protocol::PacketFormatCheck(HJ212PackPtr pack, QnRtn& error)
{
    error = QnRtn::Accept;

    std::string mn = m_platformInfo->mn.toStdString();
    std::string pw = m_platformInfo->pw.toStdString();
    std::string qn = pack->GetQN();
    qint64 qnt = QString::fromStdString(qn).toLongLong();
    QDateTime qnTime = QDateTime::fromMSecsSinceEpoch(qnt);

    //检查密码
    if(pack->GetPW().compare(pw) != 0)
        error = QnRtn::ErrorPW;
    //检查MN号
    else if(pack->GetMN().compare(mn) != 0)
        error = QnRtn::ErrorMN;
    //检查ST
    else if(pack->GetST() != m_defaultST && pack->GetST() != HJ212_ST_REPLY)
         error = QnRtn::ErrorST;
    //检查QN
    else if(pack->GetQN().length() != HJ212_QN_LENGTH || !qnTime.isValid())
        error = QnRtn::ErrotQN;
    //检查CN的长度
    else if(pack->GetCN().length() != HJ212_CN_LENGTH)
        error = QnRtn::ErrorCN;
    //检查Flag的前5位版本信息
    else if(pack->GetProtocolVersion() != m_protocolVersion)
        error = QnRtn::ErrorFlag;

    if(error == QnRtn::Accept)
        return true;
    return false;
}

/**
 * @brief 请求应答
 * @param qn 通讯包的唯一编号
 * @param code 执行结果
 * @param isRespon 是否需要应答
 * @param isSubpack 是否需要分包(2005有时候需要)
 * @param qnInCP qn号是否放在CP字段中,2005污染源需要放在里面
 */
HJ212PackPtr HJ212Protocol::ReplyRequest(std::string qn, int code, bool isRespon, bool isSubpack)
{
    int flag = (int)FlagRespSubpack::None;
    if (isRespon && isSubpack)
        flag = (int)FlagRespSubpack::RespAndSubpack;
    else if (isRespon)
        flag = (int)FlagRespSubpack::Resp;
    else if (isSubpack)
        flag = (int)FlagRespSubpack::SubPackets;


    HJ212PackPtr pack = CreatNewPack(HJ212_ST_REPLY, qn, HJ212_CN_REPLY_REQUEST, flag, m_defaultQnInCP);
    pack->AddCPParam("QnRtn=" + std::to_string(code));
    return pack;
}

/**
 * @brief 结果应答
 * @param qn 通讯包的唯一编号
 * @param code 执行结果
 * @param noflag 不带flag字段
 * @param qnInCP qn号是否放在CP字段中,2005污染源需要放在里面
 */
HJ212PackPtr HJ212Protocol::ReplyExecuteResult(std::string qn, int code)
{
    int flag = m_defaultNoFlag ? -1 : (int)FlagRespSubpack::None;
    HJ212PackPtr pack = CreatNewPack(HJ212_ST_REPLY, qn, HJ212_CN_REPLY_RESULT, flag, m_defaultQnInCP);
    pack->AddCPParam("ExeRtn=" + std::to_string(code));
    return pack;
}

/**
 * @brief 通知应答
 * @param qn 通讯包的唯一编号
 */
HJ212PackPtr HJ212Protocol::ReplyNotice(std::string qn)
{
    int flag = m_defaultNoFlag ? -1 : (int)FlagRespSubpack::None;
    HJ212PackPtr pack = CreatNewPack(HJ212_ST_REPLY, qn, HJ212_CN_REPLY_NOTICE, flag, m_defaultQnInCP);
    pack->RemoveCPLastChar();
    return pack;
}

HJ212PackPtr HJ212Protocol::ReplySystemBusy(std::string qn)
{
    if (m_protocolVersion == ProtocolVersion::HJ212_2017D)
        return ReplyRequest(qn, (int)QnRtn::DbsErrorBusy, false, false);
    return ReplyRequest(qn, (int)QnRtn::Refuse, false, false);
}

/**
 * @brief 2017编码转2005编码
 */
std::string HJ212Protocol::PolId2017To2005(std::string Id2017)
{
    std::string Id2005 = Id2017;
//    Id2005 = MonitoringSpace::MonitoringFactorManager::Instance()->FindOldPolId(QString::fromStdString(Id2017)).toStdString();
    return Id2005;
}

/**
 * @brief 2005编码转2017编码
 */
std::string HJ212Protocol::PolId2005To2017(std::string Id2005)
{
    std::string Id2017 = Id2005;
//    Id2017 = MonitoringSpace::MonitoringFactorManager::Instance()->FindNewPolId(QString::fromStdString(Id2005)).toStdString();
    return Id2017;
}

/**
 * @brief 获取当前的QN号
 */
std::string HJ212Protocol::GetCurrentQN()
{
    std::string qn = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz").toStdString();
    return qn;
}

/**
 * @brief 解析服务请求的监测因子信息
 */
CmdPackPolIdInfo HJ212Protocol::GetCmdPackPolIdInfo(HJ212PackPtr pack, UploadDataType dataType)
{
    CmdPackPolIdInfo ret;

    std::vector<std::string> array = pack->StringSplit(pack->GetCP(true), {';',','});
    std::string beginTimeStr = "";
    std::string endTimeStr = "";

    //循环获取 PolId=w01018,InfoId=i13004,i13004-Info=168.0  (监测因子 - 信息编码 - 信息相应的值)
    QString currentPolId = HJ212_NO_POLID;
    for (int i = 0; i < (int)array.size(); i++)
    {
        int index;
        std::string item = array[i];

        //开始时间
        if ((index = item.find("BeginTime=")) >= 0)
        {
            beginTimeStr = item.replace(index, std::string("BeginTime=").length(), "");
            ret.beginTime = QDateTime::fromString(QString::fromStdString(beginTimeStr), "yyyyMMddhhmmss");

            //格式化开始时间: 分钟 | 小时 | 日
            if (dataType == UploadDataType::MinuteData)
            {
                int hour = ret.beginTime.time().hour();
                int minute = ret.beginTime.time().minute();
                ret.beginTime = QDateTime(ret.beginTime.date(), QTime(hour,minute,0));
            }
            else if (dataType == UploadDataType::HourData)
            {
                int hour = ret.beginTime.time().hour();
                ret.beginTime = QDateTime(ret.beginTime.date(), QTime(hour,0,0));
            }
            else if (dataType == UploadDataType::DailyData)
            {
                ret.beginTime = QDateTime(ret.beginTime.date(), QTime(0,0,0));
            }
        }
        //结束时间
        else if ((index = item.find("EndTime=")) >= 0)
        {
            endTimeStr = item.replace(index, std::string("EndTime=").length(), "");
            ret.endTime = QDateTime::fromString(QString::fromStdString(endTimeStr), "yyyyMMddhhmmss");
        }
        //监测因子
        else if ((index = item.find("PolId=")) >= 0)
        {
            std::string polID = item.replace(index, std::string("PolId=").length(), "");
            if (polID.empty())
                continue;

            //插入
            if (ret.polIdInfos.find(QString::fromStdString(polID)) == ret.polIdInfos.end())
            {
                ret.polIdInfos.insert(QString::fromStdString(polID), QMap<QString,QString>());
            }
            currentPolId = QString::fromStdString(polID);
        }
        //监测因子的信息
        else if ((index = item.find("InfoId=")) >= 0)
        {
            std::string infoId = item.replace(index, std::string("InfoId=").length(), "");
            if (infoId.empty() || currentPolId.isEmpty())
                continue;

            //信息编码已经存在
            if (ret.polIdInfos[currentPolId].find(QString::fromStdString(infoId)) != ret.polIdInfos[currentPolId].end())
                continue;

            ret.polIdInfos[currentPolId].insert(QString::fromStdString(infoId), QString());
        }
        //监测因子的信息的值
        else if ((index = item.find("-Info=")) >= 0)
        {
            std::string infoId = item.substr(0, index);
            std::string infoValue = item.substr(index + std::string("-Info=").length());
            if (infoId.empty() || currentPolId.isEmpty())
                continue;

            //信息编码已经存在
            if (ret.polIdInfos[currentPolId].find(QString::fromStdString(infoId)) != ret.polIdInfos[currentPolId].end())
                ret.polIdInfos[currentPolId].remove(QString::fromStdString(infoId));

            ret.polIdInfos[currentPolId].insert(QString::fromStdString(infoId), QString::fromStdString(infoValue));
        }
    }

    return ret;
}


/**
 * @brief 创建新的通讯包
 * @param st 系统编码
 * @param qn 通讯包的唯一编号
 * @param cn 命令编码
 * @param flag 标志>=0时有效
 * @param qnInCP qn号是否放在CP字段中,2005污染源需要放在里面
 * @param needRespCN 服务需要应答的CN号
 */
HJ212PackPtr HJ212Protocol::CreatNewPack(int st, std::string qn, std::string cn, int flag, bool qnInCP, std::string needRespCN)
{
    HJ212PackPtr pack = CreatNewPack(st, cn);
    if (flag >= 0)
    {
        pack->AddFlag((int)m_flagProtocol);   //协议(如果有发flag的话,固定要加的)
        pack->AddFlag((int)flag);             //是否回应、是否分包
    }

    if (qnInCP == false)
        pack->SetQN(qn);
    else
        pack->AddCPParam("QN=" + qn + ";");

    pack->SetNeedRespCN(needRespCN);
    return pack;
}

QList<HJ212PackPtr> HJ212Protocol::UploadData(QMap<std::string, QList<std::string>> allDataStrMap, UploadDataType type, std::string cn)
{
    QList<HJ212PackPtr> ret;
    if (allDataStrMap.empty())
        return ret;

    //多时间段数据按多条命令发送
    for(QMap<std::string, QList<std::string>>::Iterator imaps = allDataStrMap.begin(); imaps != allDataStrMap.end(); imaps++)
    {
        //新建包
        int flag = m_defaultNoFlag ? -1 : (int)FlagRespSubpack::None;
        HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, GetCurrentQN(), cn, flag, false);

        //时间
        uploadPack->AddCPParam(imaps.key());

        //默认没有应答,上传数据时再根据配置决定是否有应答
        if(IsNeedRespond(type))
        {
            uploadPack->AddFlag((int)FlagRespSubpack::Resp);
            uploadPack->SetNeedRespCN(HJ212_CN_REPLY_DATA);  //指定数据应答
        }

        QList<std::string> dataMap = imaps.value();
        for(QList<std::string>::iterator its = dataMap.begin(); its != dataMap.end();its++)
        {
            uploadPack->AddCPParam(*its);
        }
        uploadPack->RemoveCPLastChar(); //移除数据段最后的';'
        ret.append(uploadPack);
    }
    return ret;
}

QList<HJ212PackPtr> HJ212Protocol::CmdUploadData(QMap<std::string, QList<std::string>> allDataStrMap, UploadDataType type, std::string cn, std::string qn)
{
    QList<HJ212PackPtr> ret;
    if (allDataStrMap.empty())
        return ret;

    //多时间段数据按多条命令发送
    int pnum = allDataStrMap.size();
    int pno = 1;
    for(QMap<std::string, QList<std::string>>::Iterator imaps = allDataStrMap.begin(); imaps != allDataStrMap.end(); imaps++)
    {
        //新建包
        int flag = m_defaultNoFlag ? -1 : (int)FlagRespSubpack::None;
        HJ212PackPtr uploadPack = CreatNewPack(m_defaultST, qn, cn, flag, false);

        //时间
        uploadPack->AddCPParam(imaps.key());

        //默认没有应答,上传数据时再根据配置决定是否有应答
        if(m_callBack.IsNeedRespond(type))
        {
            uploadPack->AddFlag((int)FlagRespSubpack::Resp);
            uploadPack->SetNeedRespCN(HJ212_CN_REPLY_DATA);  //指定数据应答
        }

        if (pnum > 1) //需要分包
        {
            uploadPack->AddFlag((int)FlagRespSubpack::SubPackets);
            uploadPack->SetSubPackage(pnum, pno);
            pno++;
        }

        QList<std::string> dataMap = imaps.value();
        for(QList<std::string>::iterator its = dataMap.begin(); its != dataMap.end();its++)
        {
            uploadPack->AddCPParam(*its);
        }
        uploadPack->RemoveCPLastChar(); //移除数据段最后的';'
        ret.append(uploadPack);
    }
    return ret;
}

/**
 * @brief 创建新的通讯包
 * @param st 系统编码
 * @param cn 命令编码
 */
HJ212PackPtr HJ212Protocol::CreatNewPack(int st, std::string cn)
{
    HJ212PackPtr pack(new HJ212Packet(m_protocolVersion));
    pack->SetST(st);
    pack->SetCN(cn);
    pack->SetPW(m_platformInfo->pw.toStdString());
    pack->SetMN(m_platformInfo->mn.toStdString());
    return pack;
}

/**
 * @brief 把无序的仪器监测日志链表按PolId分类
 */
QMap<QString, QList<EquipmentLog>> HJ212Protocol::ClassifyEquipmentInfo(const QList<EquipmentLog>& list)
{
    QMap<QString, QList<EquipmentLog>> map;
    if(list.empty())
        return map;

    for(QList<EquipmentLog>::const_iterator it = list.begin(); it != list.end(); it++)
    {
        QString polId = (*it).polId;
        EquipmentLog data = (*it);

        QMap<QString, QList<EquipmentLog>>::Iterator its = map.find(polId);
        if(its != map.end())
            map[polId].push_back(data);
        else
        {
            QList<EquipmentLog> tempList;
            tempList.push_back(data);
            map.insert(polId, tempList);
        }
    }
    return map;
}


/**
 * @brief 把无序的仪器监测状态链表按PolId分类
 */
QMap<QString, QList<EquipmentStatus>> HJ212Protocol::ClassifyEquipmentInfo(const QList<EquipmentStatus>& list)
{
    QMap<QString, QList<EquipmentStatus>> map;
    if(!list.empty())
    {
        for(QList<EquipmentStatus>::const_iterator it = list.begin(); it != list.end(); it++)
        {
            QString polId = (*it).polId;
            EquipmentStatus data = (*it);

            QMap<QString, QList<EquipmentStatus>>::Iterator its = map.find(polId);
            if(its != map.end())
            {
                map[polId].push_back(data);
            }
            else
            {
                QList<EquipmentStatus> tempList;
                tempList.push_back(data);

                map.insert(polId, tempList);
            }
        }
    }

    return map;
}

/**
 * @brief 把无序的仪器监测参数链表按PolId分类
 */
QMap<QString, QList<EquipmentParam>> HJ212Protocol::ClassifyEquipmentInfo(const QList<EquipmentParam>& list)
{
    QMap<QString, QList<EquipmentParam>> map;
    if(!list.empty())
    {
        for(QList<EquipmentParam>::const_iterator it = list.begin(); it != list.end(); it++)
        {
            QString polId = (*it).polId;
            EquipmentParam data = (*it);

            QMap<QString, QList<EquipmentParam>>::Iterator its = map.find(polId);
            if(its != map.end())
            {
                map[polId].push_back(data);
            }
            else
            {
                QList<EquipmentParam> tempList;
                tempList.push_back(data);

                map.insert(polId, tempList);
            }
        }
    }

    return map;
}



}
