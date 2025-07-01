#ifndef HJ212PLATFORM_H
#define HJ212PLATFORM_H

#include <map>
#include <atomic>
#include <condition_variable>

#include "Log.h"
#include "LuipShare.h"

#include "HJ212TcpConnection.h"
#include "HJ212SerialConnection.h"
#include "HJ212SendQueue.h"
#include "API/HJ212Def.h"
#include "API/HJ212Protocol.h"

namespace HJ212
{

using namespace MonitoringSpace;

class HJ212Platform
{
public:
    HJ212Platform();
    ~HJ212Platform();

    bool InitLog(log4cpp::Category* log, log4cpp::Category* hj212Log = NULL);      // 设置日志句柄
    void SetPlatformInfo(PlatformInfo info, HJ212CallBack callBack);               // 设置平台信息和回调接口
    void SetMonitoring(MonitoringFactor factors, QList<MonitoringInfo> infos);     //设置监测因子和监测信息表格
    void SetUploadItems(QList<UploadItem> items);                                  // 设置定时上传的配置
    void Start();                                                                  // 平台启动
    void Stop();                                                                   // 平台停止

    bool SetUploadItemConfig(UploadDataType type, bool enable, int interval = 0);  // 动态修改定时上传配置

public:
    void UploadRtdData(QList<RealTimeData> datas);                   // 上传小时数据
    void UploadHourData(QList<TimeSliceData> datas);                 // 上传小时数据
    void UploadStandardCheckData(QList<StandardCheckData> datas);    // 上传核查数据
    void UploadAddStandardData(QList<AddStandardData> datas);        // 上传加标回收数据
    void UploadParallelData(QList<ParallelData> datas);              // 上传平行样数据
    void UploadZeroCheckData(QList<ZeroCheckData> datas);            // 上传零点核查数据
    void UploadSpanCheckData(QList<SpanCheckData> datas);            // 上传跨度核查数据

    void UploadSystemInfoLog(QList<EquipmentLog> datas);             // 上传数采仪信息(日志)
    void UploadSystemInfoStatus(QList<EquipmentStatus> datas);       // 上传监测仪信息(状态)
    void UploadSystemInfoParam(QList<EquipmentParam> datas);         // 上传监测仪信息(参数)

public:
    bool FindMonitoringFactor(QString polId, MonitoringSpace::MonitoringFactor& data);  //查找监测因子
    bool FindMonitoringInfo(QString infoId, MonitoringSpace::MonitoringInfo& data);     //查找监测信息



private:
    static void SendThreadRun(HJ212Platform* platform);     //发送线程
    static void ReceiveThreadRun(HJ212Platform *platform);  //接收线程
    static void OnReceive(void* parent, HJ212PackPtr pack); //接受回调

    bool CheckRespPack(HJ212PackPtr pack, HJ212PackPtr resp);//检查应答包
    void UploadHeartPack();                                  //上传心跳包

    void UploadTimingPack();                                 //上传定时包
    void UploadTimingRtdDataPack();                          //上传实时数据定时包
    void UploadTimingHourDataPack();                         //上传小时数据定时包
    void UploadTimingInfoStatusPack();                       //上传仪表状态定时包
    void UploadTimingInfoParamsPack();                       //上传仪表参数定时包

public:
    bool Send(HJ212PackPtr pack);                            //发送数据包
    UploadItem& GetUploadItem(UploadDataType type);           //获取上传项


private:
    HJ212CallBack m_callBack;               //接口回调

    PlatformInfo* m_platformInfo;           //平台信息
    HJ212Protocol* m_protocol;              //HJ212协议接口
    HJ212ConnectionPtr m_connection;        //网络连接
    HJ212SendQueuePtr m_sendQueue;          //发送队列

    std::atomic_bool m_stopSendThread;      //停止发送线程
    std::atomic_bool m_stopReceiveThread;   //停止接收线程

    int m_sendAbnormalCount;                //发送异常计数
    int m_heartPackTimeSamp;                //心跳时间戳

    std::mutex m_sendMutex;                 //发送互斥量

    std::string m_waitResponQN;             //等待回应的QN号
    HJ212PackPtr m_respPack;                //回应包
    std::mutex m_respMutex;                 //回应互斥量
    std::condition_variable m_respCondition;//回应的条件变量

    std::mutex m_execMutex;                 //命令包互斥量
    std::condition_variable m_execCondition;//命令包的条件变量
    std::list<HJ212PackPtr> m_execList;     //服务发送的提取、设置命令包

    MonitoringFactor              m_MonitorFactors;  //监测因子
    QMap<QString, MonitoringInfo> m_MonitorInfos;      //监测信息

    QList<UploadItem> m_UploadItems;                   //定时上传的配置列表
    QMutex m_uploadItemMutex;
};

}

#endif // HJ212PLATFORM_H
