#ifndef V2017DBSPROTOCOL_H
#define V2017DBSPROTOCOL_H

#include "API/HJ212Protocol.h"
#include "HJ212Packet.h"

namespace HJ212
{

class HJ212Platform;
class V2017DbsProtocol : public HJ212Protocol
{

public:
    V2017DbsProtocol(PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform);
    virtual ~V2017DbsProtocol();

    virtual void OnExecute(HJ212PackPtr pack);

public:
    virtual HJ212PackPtr UploadHeartPack();                                                 // 上传心跳包
    virtual QList<HJ212PackPtr> UploadHourData(QList<TimeSliceData> datas);                 // 上传小时数据
    virtual QList<HJ212PackPtr> UploadStandardCheckData(QList<StandardCheckData> datas);    // 上传核查数据

    virtual QList<HJ212PackPtr> UploadSystemInfoLog(QList<EquipmentLog> datas);             // 上传数采仪信息(日志)
    virtual QList<HJ212PackPtr> UploadSystemInfoStatus(QList<EquipmentStatus> datas);       // 上传监测仪信息(状态)
    virtual QList<HJ212PackPtr> UploadSystemInfoParam(QList<EquipmentParam> datas);         // 上传监测仪信息(参数)

public://未加
    virtual QList<HJ212PackPtr> UploadRtdData(QList<RealTimeData> datas) {Q_UNUSED(datas); return {};}             // 上传实时数据
    virtual QList<HJ212PackPtr> UploadAddStandardData(QList<AddStandardData> datas) {Q_UNUSED(datas); return {};}  // 上传加标回收数据
    virtual QList<HJ212PackPtr> UploadParallelData(QList<ParallelData> datas) {Q_UNUSED(datas); return {};}        // 上传平行样数据
    virtual QList<HJ212PackPtr> UploadZeroCheckData(QList<ZeroCheckData> datas) {Q_UNUSED(datas); return {};}      // 上传零点核查数据
    virtual QList<HJ212PackPtr> UploadSpanCheckData(QList<SpanCheckData> datas) {Q_UNUSED(datas); return {};}      // 上传跨度核查数据

private:
    template<typename T>
    int CmdSendData(HJ212PackPtr pack, UploadDataType type, QList<T> datas);

    int CmdGetCheckData(HJ212PackPtr pack);                //获取标样核查数据
    int CmdGetSystemInformation(HJ212PackPtr pack);        //获取系统\仪表信息
    int CmdSetSystemInformation(HJ212PackPtr pack);        //设置仪表参数
    int CmdStartMeasure(HJ212PackPtr pack, FlowType type, bool needPolId = true); //启动仪表
    int CmdGetSystemInfoTime(HJ212PackPtr pack, bool isSys); //获取系统\仪表时间
    int CmdSetSystemInfoTime(HJ212PackPtr pack, bool isSys); //设置系统\仪表时间

private:
    template<typename T>
    QMap<std::string, QList<std::string>> ConvertDataMap(QList<T> allDataMap);

    template<typename T>
    QMap<std::string, QList<std::string>> ConvertDataMap(QString dataTime, T allDataMap);

    std::string StructToString(TimeSliceData data);
    std::string StructToString(StandardCheckData data);
    std::string StructToString(EquipmentLog data);
    std::string StructsToString(QList<EquipmentStatus> datas);
    std::string StructsToString(QList<EquipmentParam> datas);

};

}

#endif // V2017DBSPROTOCOL_H
