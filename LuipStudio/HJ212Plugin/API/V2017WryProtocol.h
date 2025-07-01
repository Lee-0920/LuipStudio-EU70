#ifndef V2017WRYPROTOCOL_H
#define V2017WRYPROTOCOL_H

#include "HJ212Protocol.h"
#include "HJ212Packet.h"

namespace HJ212
{

class HJ212Platform;

/**
 * @brief 污染源2017协议。
 */
class V2017WryProtocol : public HJ212Protocol
{

public:
    V2017WryProtocol(PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform);
    virtual ~V2017WryProtocol();

    virtual void OnExecute(HJ212PackPtr pack);

public:
    virtual HJ212PackPtr UploadHeartPack(); // 上传心跳包

    virtual QList<HJ212PackPtr> UploadRtdData(QList<RealTimeData> datas);                   // 2011上传实时数据
    virtual QList<HJ212PackPtr> UploadHourData(QList<TimeSliceData> datas);                 // 2061上传小时数据
    virtual QList<HJ212PackPtr> UploadStandardCheckData(QList<StandardCheckData> datas);    // 2062上传核查数据
    virtual QList<HJ212PackPtr> UploadAddStandardData(QList<AddStandardData> datas);        // 2063上传加标回收数据
    virtual QList<HJ212PackPtr> UploadParallelData(QList<ParallelData> datas);              // 2064上传平行样数据
    virtual QList<HJ212PackPtr> UploadZeroCheckData(QList<ZeroCheckData> datas);            // 2065上传零点核查数据
    virtual QList<HJ212PackPtr> UploadSpanCheckData(QList<SpanCheckData> datas);            // 2066上传跨度核查数据

    virtual QList<HJ212PackPtr> UploadSystemInfoLog(QList<EquipmentLog> datas);             // 上传数采仪信息(日志)
    virtual QList<HJ212PackPtr> UploadSystemInfoStatus(QList<EquipmentStatus> datas);       // 上传监测仪信息(状态)
    virtual QList<HJ212PackPtr> UploadSystemInfoParam(QList<EquipmentParam> datas);         // 上传监测仪信息(参数)

private://参数命令

    int CmdSetOvetTimeAndReCount(HJ212PackPtr pack);           ///<1000 设置超时时间及重发次数
    int CmdGetSystemTime(HJ212PackPtr pack);                   ///<1011 提取现场机时间
    int CmdSetSystemTime(HJ212PackPtr pack);                   ///<1012 设置现场机时间
    int CmdGetUploadRtdDataInterval(HJ212PackPtr pack);        ///<1061 提取实时数据间隔
    int CmdSetUploadRtdDataInterval(HJ212PackPtr pack);        ///<1062 设置实时数据间隔
    int CmdGetUploadMinuteDataInterval(HJ212PackPtr pack);     ///<1063 提取分钟数据间隔
    int CmdSetUploadMinuteDataInterval(HJ212PackPtr pack);     ///<1064 设置分钟数据间隔
    int CmdSetOverValue(HJ212PackPtr pack);                    ///<3025 设置超标值


private://数据命令

    template<typename T>
    int CmdSendData(HJ212PackPtr pack, UploadDataType type, QList<T> datas);

    int CmdGetRealTimeData(HJ212PackPtr pack);                 ///<2011 提取监测指标实时数据
    int CmdGetHourData(HJ212PackPtr pack);                     ///<2061 取污染物小时数据
    int CmdGetCheckData(HJ212PackPtr pack);                    ///<2062 取核查数据
    int CmdGetAddstandardData(HJ212PackPtr pack);              ///<2063 取加标回收数据
    int CmdGetParallelData(HJ212PackPtr pack);                 ///<2064 取平行样数据
    int CmdGetZeroCheckData(HJ212PackPtr pack);                ///<2065 取零点核查数据
    int CmdGetRangeCheckData(HJ212PackPtr pack);               ///<2066 取跨度核查数据

private://控制命令

    int CmdStartFlow(HJ212PackPtr pack, FlowType type);
    int CmdStartZeroRangeCheck(HJ212PackPtr pack);                          ///<3011 零点校准量程校准
    int CmdStartMeasureSample(HJ212PackPtr pack);                           ///<3012 即时采样
    int CmdStartClean(HJ212PackPtr pack);                                   ///<3013 启动清洗/反吹
    int CmdStartComparisonMeasureSample(HJ212PackPtr pack);                 ///<3014 比对采样
    int CmdStartMeasureCheck(HJ212PackPtr pack);                            ///<3080 启动单台仪表标液核查
    int CmdStartMeasureAddstandard(HJ212PackPtr pack);                      ///<3081 启动单台仪表加标回收
    int CmdStartMeasureParallel(HJ212PackPtr pack);                         ///<3082 启动单台仪表平行样测试
    int CmdStartMeasureZeroCheck(HJ212PackPtr pack);                        ///<3083 启动单台仪表零点核查
    int CmdStartMeasureRangeCheck(HJ212PackPtr pack);                       ///<3084 启动单台仪表跨度核查
    int CmdStartBlankCalibrate(HJ212PackPtr pack);                          ///<3085 启动空白校准(仪器采用蒸馏水测试结果对仪器进行校准的过程)
    int CmdStartStandardCalibrate(HJ212PackPtr pack);                       ///<3086 启动标样校准(仪器采用标准溶液测试结果对仪器校准系数或工作曲线方程进行校准的过程)

private://状态|参数|日志命令

    int CmdSetEquipmentInfo(HJ212PackPtr pack);    ///<3021 设置参数
    int CmdGetEquipmentInfo(HJ212PackPtr pack);    ///<3020 获取状态和参数

private:

    std::string StructsToString(QList<EquipmentStatus> datas);
    std::string StructsToString(QList<EquipmentParam> datas);
    std::string StructToString(EquipmentLog datas);
    std::string StructToString(RealTimeData datas);
    std::string StructToString(TimeSliceData datas);
    std::string StructToString(StandardCheckData datas);
    std::string StructToString(ZeroCheckData datas);
    std::string StructToString(SpanCheckData datas);
    std::string StructToString(AddStandardData datas);
    std::string StructToString(ParallelData datas);

    template<typename T>
    QMap<std::string, QList<std::string>>ConvertDataMap(QList<T> allDataMap);

    template<typename T>
    QMap<std::string, QList<std::string>>ConvertDataMap(QString dataTime,T allDataMap);
};
}


#endif // V2017WRYPROTOCOL_H
