#ifndef HJ212MANAGER_H
#define HJ212MANAGER_H

#include <QObject>
#include "lua.hpp"
#include "System/Types.h"
#include "HJ212Plugin/HJ212Platform.h"
#include "ResultManager/IResultNotifiable.h"
#include "ResultManager/MeasureRecordFile.h"
#include "StatusManager/IStatusNotifiable.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"

using System::Uint8;
using System::Uint16;
using System::String;
using Result::IResultNotifiable;
using Result::MeasureRecordFile;
using Measure::IStatusNotifiable;
using Measure::RunStatus;
using Measure::RunAction;
using UI::IUpdateWidgetNotifiable;
using namespace Measure;
namespace HJ212
{

enum class HJ212Type
{
    None = 0,
    V2017_Dbs = 1,
    V2017_Wry = 2,
    V2005_Wry = 3,
};

class HJ212Manager: public QObject, public IResultNotifiable, public IUpdateWidgetNotifiable, public IStatusNotifiable
{
    enum UpdateMessageType
    {
        Default,
        UpdateLog,
        UpdateParam,
        UpdateStatus,
        UpdateEquipmentStatus,
        UpdateEquipmentAlarm,
        UpdateMeasureResult,
    };

    struct MeasureResult
    {
        MeasureResult()
        {
            times = 0;
            consistency = 0;
            resultType = 255;
            flag = "N";
        }

        unsigned int times;
        float consistency;
        int resultType;
        int oldTypeNum;
        String flag;
    };

    Q_OBJECT

public:
    static HJ212Manager* Instance();
    HJ212Manager();
    ~HJ212Manager();

    void Init();

    bool IsRespond();
    bool GetMonitoringInfo(int index, MonitoringInfo& info);
    MonitoringSpace::MonitoringFactor GetMonitoringFactor();
    bool StartEquipmentFlow(int flowCode);
    QList<TimeSliceData> GetHourData(QDateTime startTime, QDateTime endTim);
    QList<StandardCheckData> GetStandardCheckData(QDateTime startTime, QDateTime endTime);
    QList<ParallelData> GetParallelData(QDateTime startTime, QDateTime endTime);
    QList<ZeroCheckData> GetZeroCheckData(QDateTime startTime, QDateTime endTime);
    QList<AddStandardData> GetAddStandardData(QDateTime startTime, QDateTime endTime);
    QList<SpanCheckData> GetSpanCheckData(QDateTime startTime, QDateTime endTime);
    RealTimeData GetRealTimeData();
    bool GetLastestRunLog(EquipmentLog& log);
    QStringList GetRunLogList(QDateTime startTime, QDateTime endTime);
    bool GetEquipmentMonitoringInfo(QString infoId, EquipmentParam& params);
    bool GetEquipmentMonitoringInfo(QString infoId, EquipmentStatus& params);
    bool GetEquipmentAllParam(QList<EquipmentParam>& params);
    bool GetEquipmentAllStatus(QList<EquipmentStatus>& status);
    bool SetEquipmentMonitoringInfo(QString infoId, QString value);
    bool ManualUploadMeasureResult(int time,float consistency,int resultType,String flag);

    void RtdToHourDatas(QList<RealTimeData> &rtdList, QList<TimeSliceData> &hourDataList);

public:
    void OnMeasureResultAdded(String name, ResultData::RecordData result);
    void OnCalibrateResultAdded(String name, ResultData::RecordData result);
    void OnUpdateWidget(UI::UpdateEvent event, System::String message);
    void OnStattusChanged(RunStatus status);
    void OnStattusDelay(int time);
    void OnActionChanged(RunAction action);
    void OnActionDelay(int time);    
signals:
    void OnUpdateMessageSignal(int type);
    void OnUpdateLogWithBracketsSignal(bool isBegin);

protected slots:
    void OnUpdateMessageSlot(int type);
/**
 * @brief  上传日志附加尖括号的日志
 * @details  <<附加在日志前，>>附加在日志后
 * @param[in] isBegin true附加<<,false附加>>
 * @return
 */
    void OnUpdateLogWithBrackesSlot(bool isBegin);

private:
    bool InitHj212Script();
    void InitPlatformInfo();
    void InitMonitoringFactors();
    void InitMonitoringInfos();

    SerialPortConfig GetSerialPortConfig();
    PlatformInfo GetPlatformInfo();
    HJ212CallBack GetHJ212CallBack();
    void Test();
    QList<UploadItem> GetUploadItems();
    QList<MonitoringSpace::MonitoringFactor> GetMonitoringFactors();
    QList<MonitoringSpace::MonitoringInfo> GetMonitoringInfos();

    float GetStandardCheckConsistency();
    void GetParallelParam(ParallelData* param);
    void GetAddStandardParam(AddStandardData* param);
    void GetZeroCheckParam(ZeroCheckData* param);
    void GetSpanCheckParam(SpanCheckData* param);

    void UploadAllParam();
    void UploadAllStatus();
    bool UploadMeasureResult();
    void UploadHourData(MeasureResult data);
    void UploadAddStandardData(MeasureResult data);
    void UploadParallelData(MeasureResult data);
    void UploadZeroCheckData(MeasureResult data);
    void UploadSpanCheckData(HJ212Manager::MeasureResult data);
    void UploadStandardCheckData(MeasureResult data);
    void UploadEquipmentInfoStatus(int index);
    void UploadEquipmentLog();

private:
    static std::unique_ptr<HJ212Manager> m_instance;
    HJ212Platform m_platform;
    PlatformInfo m_platformInfo;
    MonitoringFactor m_monitoringFactor;
    MeasureRecordFile* m_resultFiles;
    int m_hj212Type;
    int m_readerIndex;
//    bool m_status;                          //是否启用
    bool m_isRespond;                       //是否等待回应
    QString m_strMeasureRecordName;         //结果文件索引(一般是化学式)

    QMap<QString, MonitoringInfo> m_mapMonitorInfo;

    lua_State * m_mainState;
    lua_State * m_newState;

};

}

#endif // HJ212MANAGER_H
