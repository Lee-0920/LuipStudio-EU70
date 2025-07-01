#include <QThread>

#include "HJ212Platform.h"
#include "API/V2017WryProtocol.h"
#include "API/V2017DbsProtocol.h"

namespace HJ212
{

#define MAX_EXEC_NUM 10                  //等待执行的命令数量
#define SEND_QUEUE_CAPACITY 1000         //发送队列容量
#define SEND_WHILE_SLEEP 1000            //发送循环的休眠时间
#define SLEEP_WHEN_CONNECT_FAILED 60000  //连接失败后，休眠1分钟
#define RECONNECT_WHEN_SEND_FAILED_NUM 5 //发送多次失败后，重连

HJ212Platform::HJ212Platform()
{
    m_stopSendThread = false;
    m_stopReceiveThread = false;
    m_sendAbnormalCount = 0;
    m_waitResponQN = "";
    m_protocol= NULL;
    m_heartPackTimeSamp = 0;
}

HJ212Platform::~HJ212Platform()
{

}

bool HJ212Platform::InitLog(log4cpp::Category* log, log4cpp::Category* hj212Log)
{
    logger = log;
    hj212Logger = hj212Log;
    return true;
}

void HJ212Platform::SetPlatformInfo(PlatformInfo info, HJ212CallBack callBack)
{
    //如果在线程在工程，则需要先关闭线程
    bool isInWorking = m_stopSendThread && m_stopReceiveThread;
    if (isInWorking)
        Stop();

    m_platformInfo = new PlatformInfo(info);
    m_callBack = callBack;

    if (info.connectType == ConnectType::Serial)
        m_connection = HJ212ConnectionPtr(new HJ212SerialConnection(info.serial, OnReceive, (void*)this));
    else
        m_connection = HJ212ConnectionPtr(new HJ212TcpConnection(info.ip.toStdString(), info.port, OnReceive, (void*)this));

    HJ212SendQueuePtr queue(new HJ212SendQueue(SEND_QUEUE_CAPACITY));
    m_sendQueue = queue;

    if (m_protocol != NULL)
        delete m_protocol;

    //协议(没有2015污染源)
    if (info.protocol == ProtocolVersion::HJ212_2017D)
        m_protocol = new V2017DbsProtocol(m_platformInfo, callBack, this);
    else
        m_protocol = new V2017WryProtocol(m_platformInfo, callBack, this);

    //如果前面关闭了线程，则需要重启
    if (isInWorking)
        Start();
}

/**
 * @brief 启动线程
 */
void HJ212Platform::Start()
{
    m_stopSendThread = false;
    m_stopReceiveThread = false;

    std::thread sendThread(SendThreadRun, this);
    std::thread receiveThread(ReceiveThreadRun, this);

    sendThread.detach();
    receiveThread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

/**
 * @brief 停止线程
 */
void HJ212Platform::Stop()
{
    if (!m_stopSendThread && !m_stopReceiveThread)
        return;

    int sleepTime = 10;
    int timeOut = 5000 / sleepTime;

    m_stopSendThread = true;
    m_stopReceiveThread = true;

    while (timeOut--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        if (!m_stopSendThread && !m_stopReceiveThread)
            break;
    }
}


/**
 * @brief 发送线程
 * @note 定时循环,尝试从发送队列中取数据,并发送
 */
void HJ212Platform::SendThreadRun(HJ212Platform* platform)
{
    logger->debug("HJ212Platform send thread start, threadId = %d", QThread::currentThreadId());

    while (!platform->m_stopSendThread)
    {
        //如果网络没有连接(正常只有开机初始化才会发生) or 发送异常的次数超过限制,则重连
        if (platform->m_connection->IsConnected() == false || platform->m_sendAbnormalCount >= RECONNECT_WHEN_SEND_FAILED_NUM)
        {
            logger->debug("HJ212Platform try to connect server...");
            platform->m_connection->CloseConnection();
            if (platform->m_connection->Connect() == false)
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_WHEN_CONNECT_FAILED));
                //防止休眠时，导致关闭线程等太久
                int sleepTime = 2000;
                int cnt = SLEEP_WHEN_CONNECT_FAILED / sleepTime;
                while(cnt--)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                    if (platform->m_stopSendThread)
                    {
                        platform->m_stopSendThread = false;
                        logger->debug("HJ212Platform send thread end, threadId = %d", QThread::currentThreadId());
                        return;
                    }
                }

                continue;
            }
            //重连后立刻发送心跳包
            platform->UploadHeartPack();
            //重置发送异常次数
            platform->m_sendAbnormalCount = 0;
        }


        //发送 - 心跳包
        platform->UploadHeartPack();

        //发送 - 定时上传
        platform->UploadTimingPack();


        //发送 - 服务命令
        std::unique_lock<std::mutex> lock(platform->m_execMutex);
        if (platform->m_execList.empty())
            platform->m_execCondition.wait_for(lock, std::chrono::milliseconds(10));

        if (!platform->m_execList.empty())
        {
            unsigned int size = platform->m_execList.size();
            for (unsigned int i =0; i < size; i++)
            {
                HJ212PackPtr pack = platform->m_execList.front();
                platform->m_protocol->OnExecute(pack);
                platform->m_execList.pop_front();
            }
        }
        lock.unlock();


        //发送 - 主动上传
        HJ212PackPtr pack;
        if (platform->m_sendQueue->Get(pack))
        {
            pack->RefreshQN();
            platform->Send(pack);
        }

        //休眠
        std::this_thread::sleep_for(std::chrono::milliseconds(SEND_WHILE_SLEEP));
    }

    platform->m_stopSendThread = false;
    logger->debug("HJ212Platform send thread end, threadId = %d", QThread::currentThreadId());
}


/**
 * @brief 接收线程
 */
void HJ212Platform::ReceiveThreadRun(HJ212Platform *platform)
{
    logger->debug("HJ212Platform receive thread start, threadId = %d", QThread::currentThreadId());

    while(!platform->m_stopReceiveThread)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //如果网络没有连接,则回去休眠
        if (!platform->m_connection->IsConnected())
            continue;

        //正常接收
        platform->m_connection->ReceiveData();
    }

    platform->m_stopReceiveThread = false;
    logger->debug("HJ212Platform receive thread end, threadId = %d", QThread::currentThreadId());
}


/**
 * @brief 检查回应包
 */
bool HJ212Platform::CheckRespPack(HJ212PackPtr pack, HJ212PackPtr resp)
{
    //检查QN 和 CN
    if (resp->GetQN() != pack->GetQN() || resp->GetCN() != pack->GetNeedRespCN())
    {
        logger->debug("HJ212Platform 发送包[QN:%s],回应包[QN:%s],等待[CN:%s],回应[CN:%s]!",
                      pack->GetQN().c_str(), resp->GetQN().c_str(),
                      pack->GetNeedRespCN().c_str(), resp->GetCN().c_str());
        return false;
    }

    //检查PNO 和 PNUM
    std::string respPNO = resp->GetCPParam("PNO");
    std::string respPNUM = resp->GetCPParam("PNUM");

    std::string PNO = pack->GetCPParam("PNO");
    std::string PNUM = pack->GetCPParam("PNUM");

    if (respPNO != PNO || respPNUM != PNUM)
    {
        logger->debug("HJ212Platform 发送包[PNUM:%s],回应包[PNUM:%s],等待[PNO:%s],回应[PNO:%s]!",
                      PNUM.c_str(), respPNUM.c_str(),
                      PNO.c_str(), respPNO.c_str());
        return false;
    }
    return true;
}


/**
 * @brief 发送数据包,并等待回包
 */
bool HJ212Platform::Send(HJ212PackPtr pack)
{
    std::unique_lock<std::mutex> lock(m_sendMutex);

    int error;
    if (m_connection->Send(pack, error) <= 0)
    {
        m_sendAbnormalCount++;
        logger->warn("HJ212Platform send failed[QN:%s CN:%s] error:%d! AbnormalCount:%d,remain:%d will reconnect.",
                     pack->GetQN().c_str(), pack->GetCN().c_str(), error, m_sendAbnormalCount, RECONNECT_WHEN_SEND_FAILED_NUM - m_sendAbnormalCount);
        return false;
    }

    if (pack->IsNeedRespond() == false)
        return true;

    m_waitResponQN = pack->GetQN();

    //等待回包
    int retryNum = m_platformInfo->recount;
    bool result = true;
    for (int i = 0; i <= retryNum; i++)
    {
        std::cv_status check = m_respCondition.wait_for(lock, std::chrono::milliseconds(m_platformInfo->overtime * 1000));

        // 超时重发
        if(check == std::cv_status::timeout)
        {
            logger->warn("HJ212Platform wait resp[QN:%s CN:%s]timeout,remain[N:%d]", pack->GetQN().c_str(), pack->GetCN().c_str(), retryNum - i);
            //重发次数已经没了
            if (retryNum - 1 >= i)
            {
                result = false;
                m_sendAbnormalCount++;
                logger->warn("HJ212Platform wait resp[QN:%s CN:%s]timeout 3per failed!AbnormalCount:%d,remain:%d will reconnect.", pack->GetQN().c_str(), pack->GetCN().c_str(), m_sendAbnormalCount, RECONNECT_WHEN_SEND_FAILED_NUM - m_sendAbnormalCount);
                break;
            }

            m_connection->Send(pack, error);
            continue;
        }

        m_sendAbnormalCount = 0;

        // 校验数据
        std::unique_lock<std::mutex> respLock(m_respMutex);
        HJ212PackPtr resp = m_respPack;
        lock.unlock();

        result = CheckRespPack(pack, resp);
        break;
    }

    return result;
}


void HJ212Platform::OnReceive(void* parent, HJ212PackPtr pack)
{
    HJ212Platform* platform = (HJ212Platform*)parent;

    //交互命令
    if (pack->GetCommandType() == CommandType::InteractionCommand)
    {
        std::unique_lock<std::mutex> lock(platform->m_respMutex);
        platform->m_respPack = pack;
        std::string waitQN = platform->m_waitResponQN;
        lock.unlock();

        std::string recvQN = pack->GetQN();
        std::string recvCN = pack->GetCN();

        if (waitQN != "")
        {
            //检查m_waitResponQN是否一致
            if (recvQN == waitQN)
            {
                platform->m_waitResponQN = "";
                platform->m_respCondition.notify_one();
            }
            else
                logger->warn("PHJ212Platform recv resp[QN:%s  CN:%s] is not the wait resp[QN:%s]", recvQN.c_str(), recvCN.c_str(), waitQN.c_str());
        }
        else
        {
            //当前没有需要等待的应答包，服务却回了一个垃圾包
            logger->debug("HJ212Platform recv unexpect-junk-resp[QN:%s  CN:%s]", recvQN.c_str(), recvCN.c_str());
        }
    }
    //控制命令
    else
    {
        if(platform->m_execList.size() > MAX_EXEC_NUM) //超出最大处理数量
        {
            HJ212PackPtr busyPack = platform->m_protocol->ReplySystemBusy(pack->GetQN());
            platform->Send(busyPack);
        }
        else
        {
            std::unique_lock<std::mutex> lock(platform->m_execMutex);
            platform->m_execList.push_back(pack);
            lock.unlock();
            platform->m_execCondition.notify_one();
        }
    }
}



void HJ212Platform::SetMonitoring(MonitoringFactor factor, QList<MonitoringInfo> infos)
{
    m_MonitorFactors = factor;
    foreach(MonitoringInfo item, infos)
    {
        if (m_MonitorInfos.find(item.infoId) == m_MonitorInfos.end())
            m_MonitorInfos.insert(item.infoId, item);
    }
}

bool HJ212Platform::FindMonitoringFactor(QString polId, MonitoringSpace::MonitoringFactor& data)
{
    if (m_MonitorFactors.polId == polId)
    {
        data = m_MonitorFactors;
        return true;
    }
    return false;
}

bool HJ212Platform::FindMonitoringInfo(QString infoId, MonitoringSpace::MonitoringInfo& data)
{
    auto item = m_MonitorInfos.find(infoId);
    if (item == m_MonitorInfos.end())
        return false;
    data = *item;
    return true;
}

void HJ212Platform::SetUploadItems(QList<UploadItem> items)
{
    foreach (UploadItem item, items) {
        m_UploadItems.append(item);
    }
}

bool HJ212Platform::SetUploadItemConfig(UploadDataType type, bool enable, int interval)
{
    for (int i = 0; i < m_UploadItems.size(); i++)
    {
        if (type == m_UploadItems[i].type)
        {
            QMutexLocker lock(&m_uploadItemMutex);

            m_UploadItems[i].enable = enable;
            m_UploadItems[i].interval = interval;
            m_UploadItems[i].lastTimeSamp = QDateTime::currentDateTime().toTime_t();
            return true;
        }
    }
    return false;
}







void HJ212Platform::UploadHeartPack()
{
    if (m_platformInfo->heartPackTime <= 0)
        return;

    time_t currentSecNum = QDateTime::currentDateTime().toTime_t();
    if (currentSecNum >= m_platformInfo->heartPackTime + m_heartPackTimeSamp)
    {
        HJ212PackPtr pack = m_protocol->UploadHeartPack();
        this->Send(pack);

        m_heartPackTimeSamp = currentSecNum;
    }
}

void HJ212Platform::UploadRtdData(QList<RealTimeData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadRtdData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadHourData(QList<TimeSliceData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadHourData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadStandardCheckData(QList<StandardCheckData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadStandardCheckData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadAddStandardData(QList<AddStandardData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadAddStandardData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadParallelData(QList<ParallelData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadParallelData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadZeroCheckData(QList<ZeroCheckData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadZeroCheckData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadSpanCheckData(QList<SpanCheckData> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadSpanCheckData(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}







void HJ212Platform::UploadSystemInfoLog(QList<EquipmentLog> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadSystemInfoLog(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadSystemInfoStatus(QList<EquipmentStatus> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadSystemInfoStatus(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}

void HJ212Platform::UploadSystemInfoParam(QList<EquipmentParam> datas)
{
    if (!m_connection->IsConnected())
        return;

    QList<HJ212PackPtr> items = m_protocol->UploadSystemInfoParam(datas);
    for(auto item = items.begin(); item != items.end(); item++)
    {
        m_sendQueue->Add(*item);
    }
}




/*****************************************************
 *
 * 定时上传数据包
 *
 ****************************************************/

void HJ212Platform::UploadTimingPack()
{
    time_t currentSecNum = QDateTime::currentDateTime().toTime_t();
    for(int i = 0; i < m_UploadItems.size(); i++)
    {
        {
            QMutexLocker lock(&m_uploadItemMutex);
            if (!m_UploadItems[i].enable)
                continue;
            if (m_UploadItems[i].interval <= 0)
                continue;
            if (currentSecNum < m_UploadItems[i].interval + m_UploadItems[i].lastTimeSamp)
                continue;

            //更新时间戳
            m_UploadItems[i].lastTimeSamp = currentSecNum;
        }

        switch(m_UploadItems[i].type)
        {
        case UploadDataType::InfoStatus://仪器信息状态
            UploadTimingInfoStatusPack();
            break;
        case UploadDataType::InfoParam://仪器信息参数
            UploadTimingInfoParamsPack();
            break;
        case UploadDataType::RtdData://实时数据
            UploadTimingRtdDataPack();
            break;
        case UploadDataType::HourData://小时数据
            UploadTimingHourDataPack();
            break;
        default:
            break;
        }

    }
}

void HJ212Platform::UploadTimingRtdDataPack()
{
    auto data = m_callBack.GetRealTimeData();
    //发送
    UploadRtdData({data});
}

void HJ212Platform::UploadTimingHourDataPack()
{
    auto data = m_callBack.GetNewestHourDataList();
    if (data.size() <= 0)
        return;
    //发送
    UploadHourData({data});
}

void HJ212Platform::UploadTimingInfoStatusPack()
{
    //筛选监测信息 (仪表状态)
    QList<QString> infoIds;
    for(auto item = m_MonitorInfos.begin(); item != m_MonitorInfos.end(); item++)
    {
        if ((*item).infoType != (int)MonitoringSpace::InfoType::Status)
            continue;
        infoIds.append((*item).infoId);
    }
    if (infoIds.size() <= 0)
        return;

    //遍历获取监测因子仪表的状态信息
    QList<EquipmentStatus> allPolIdsStatuses;
    for (int j = 0; j < infoIds.size(); j++)
    {
        EquipmentStatus status;
        if (!m_callBack.GetSysEquipmentStatus(infoIds[j], status))
            continue;
        allPolIdsStatuses.append(status);
    }
    if (allPolIdsStatuses.size() <= 0)
        return;

    //发送
    UploadSystemInfoStatus({allPolIdsStatuses});
}

void HJ212Platform::UploadTimingInfoParamsPack()
{
    //筛选监测信息
    QList<QString> infoIds;
    for(auto item = m_MonitorInfos.begin(); item != m_MonitorInfos.end(); item++)
    {
        if ((*item).infoType != (int)MonitoringSpace::InfoType::Parameter)
            continue;
        infoIds.append((*item).infoId);
    }
    if (infoIds.size() <= 0)
        return;

    //遍历获取监测因子仪表信息
    QList<EquipmentParam> allPolIdsStatuses;
    for (int j = 0; j < infoIds.size(); j++)
    {
        EquipmentParam status;
        if (!m_callBack.GetEquipmentParam(infoIds[j], status))
            continue;
        allPolIdsStatuses.append(status);
    }
    if (allPolIdsStatuses.size() <= 0)
        return;

    //发送
    UploadSystemInfoParam({allPolIdsStatuses});
}

UploadItem& HJ212Platform::GetUploadItem(UploadDataType type)
{
    for (int i = 0; i < m_UploadItems.size(); i++)
    {
        if (m_UploadItems[i].type == type)
            return m_UploadItems[i];
    }

    UploadItem item;
    item.type = type;
    item.enable = false;
    item.interval = 0;
    m_UploadItems.append(item);
    return m_UploadItems[m_UploadItems.size() - 1];
}

}

