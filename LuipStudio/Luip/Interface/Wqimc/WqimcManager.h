
#if !defined(WQIMC_MANAGER_H)
#define WQIMC_MANAGER_H

#include <memory>
#include "Interface/Wqimc/Api/Manager/InstrumentManager_types.h"
#include "ResultManager/ResultManager.h"
#include <QThread>
#include <QObject>
#include "ConfigVector.h"
#include <QMetaType>
#include "ReagentRemainVector.h"
#include <QTimer>
#include "HttpClient.h"
#include "IWqimcStatusNotifiable.h"
#include "StatusManager/RunStatus.h"
#include "StatusManager/RunAction.h"
#include "StatusManager/IStatusNotifiable.h"
#include "AlarmManager/AlarmManager.h"
#include "DuplexThrift/DuplexClientService.h"
#include "InstrumentAgentHandler.h"

Q_DECLARE_METATYPE(Interface::Wqimc::Api::Manager::MeasureData)
Q_DECLARE_METATYPE(Interface::Wqimc::Api::Manager::MeasureCurve)
Q_DECLARE_METATYPE(std::vector<Interface::Wqimc::Api::Manager::Config>)
Q_DECLARE_METATYPE(std::vector<Interface::Wqimc::Api::Manager::ReagentRemain>)
Q_DECLARE_METATYPE(Interface::Wqimc::Api::Manager::Alarm)
Q_DECLARE_METATYPE(Interface::Wqimc::Api::Manager::Status)
Q_DECLARE_METATYPE(Interface::Wqimc::Api::Manager::DiagnosisResult)
Q_DECLARE_METATYPE(Interface::Wqimc::Api::Agent::Config)

using namespace Measure;

namespace Interface
{
namespace Wqimc
{

struct UpLoadFile
{
    bool isZip;
    System::String filePath;
    System::String url;
};

struct DownloadFile
{
    System::String url;
    System::String saveFilePath;
};

enum class ConnectStep
{
    InitConnect = 0,    //初始连接
    ReConnect,          //掉线重连
    CheckConnectStatus, //检查连接
    Nomal,              // 正常
};

class WqimcManager:
    public QObject,
    public Measure::IStatusNotifiable,
     public erchashu::thrift::ConnectionNotifier
{
    Q_OBJECT
public:
    static WqimcManager *Instance();
    bool Init();

    void SetCableStatus(bool status);
    void ChangeSN();

    bool alarm(const Measure::Alarm& alarm);
    bool updateConfigs(ConfigVector configVector);
    bool uploadReagentProfile();
    bool uploadConsumableProfile();
    bool updateReagentRemain(ReagentRemainVector reagentRemainVector);
    bool setSignalUpdateCycle(float cycleSec);
    void reboot();
    bool upgrade(const System::String& type, const System::String& newVersion, const System::String& url);
    bool notifyDiagnosisResult(const Interface::Wqimc::Api::Manager::DiagnosisResult& result);
    bool calibrationTouchScreen();
    bool catchLogFile(const std::string& urlSpec, const std::string& logFileName, const bool isZip);
    void uploadMeasureData(String name, ResultData::RecordData recordData);
    void uploadMeasureCurve(String name, ResultData::RecordData recordData);
    bool Download(const System::String& url, const System::String& saveFilePath);

    void OnStattusChanged(Measure::RunStatus status);
    void OnStattusDelay(int time);
    void OnActionChanged(Measure::RunAction action);
    void OnActionDelay(int time);

    void Register(IWqimcStatusNotifiable *handle);
    void ChangeWqimcStatus(WqimcStatus wqimcStatus);

    std::string GetSignalFormat(std::string name);

    void ChangeHardwareParam(const Interface::Wqimc::Api:: Agent::Config config);

protected:
    void onDisconnected();

private:
    WqimcManager();
    bool InitConnectService();
    bool ReconnectService();
    void EnableDisconnectNotify(bool enable);

    void CheckEnableConnet();
    void CheckSNValid();
    void CheckCableStatus();

    void SetConnectedFlag(bool flag);
    bool GetConnectedFlag();

    bool UploadConnectInfo();

    bool upload();
    bool echo();
    bool UpdateAction(const Interface::Wqimc::Api::Manager::Status &action);

    bool uploadInstrumentPermissionLevel(const int32_t level);
    int64_t getSystemTime();
    bool uploadSettingProfiles();
    bool uploadSignalProfile();
    bool uploadOperations();
    bool uploadDiagnoses();
    Api::Manager::MeasureItem MakeMeasureItem(Result::OperateRecordData *operateRecordData, ResultData::RecordData recordData, System::String fiedName);
    QFile *OpenFileForWrite(const QString &fileName);

    void ExceptionLogOutput(String path, std::exception& e);
    void ClearExceptioFlag();

    static std::unique_ptr<WqimcManager> m_instance;
    bool m_isConnectedFlag;

    QThread *workThread;
    QTimer *m_timer;
    lua_State * m_newState;
    double m_UpdateCycle;

    HttpClient *httpClient;

    System::String m_upgradeType;
    bool m_isUpgrading;
    QFile *m_upgradeFile;

    std::list<UpLoadFile> m_upLoadFileQueue;
    std::mutex m_lockUpLoadFileQueue;
    bool m_isUploading;

    std::list<DownloadFile> m_downloadFileQueue;
    std::mutex m_lockDownloadFileQueue;
    bool m_isDownloading;
    QFile *m_downloadFile;

    std::list<IWqimcStatusNotifiable*> m_notifise;

    std::string measureRusultFormat;
    std::string curveFormat;
    std::map<std::string, std::string> m_signalFormat;

    bool m_isCalibration;
    std::mutex m_connectLock;
    int m_UploadExceptionCnt;

    QTimer *m_reconnectTimer;
    std::mutex m_disconnectLock;
    bool m_isEnableConnet;
    bool m_isValidSN;
    bool m_cableConnected;
    ConnectStep m_connectStep;

signals:
    void InitSignal();
    void UpdateConfigsSignals(const std::vector<Interface::Wqimc::Api::Manager::Config> & config);
    void UploadMeasureDataSignals(const Interface::Wqimc::Api::Manager::MeasureData& data);
    void UploadMeasureCurveSignals(const Interface::Wqimc::Api::Manager::MeasureCurve& curve);
    void UploadReagentProfileSignals();
    void UpdateReagentRemainSignals(const std::vector<Interface::Wqimc::Api::Manager::ReagentRemain> &reagentRemainList);
    void UploadConsumableProfileSignals();
    void SetSignalUpdateCycleSignals(const double cycleSec);
    void RebootSignals();
    void UpgradeSignals(const System::String& url);
    void AlarmSignals(const Interface::Wqimc::Api::Manager::Alarm& alarm);
    void UpdateStatusSignals(const Interface::Wqimc::Api::Manager::Status& status);
    void UpdateActionSignals(const Interface::Wqimc::Api::Manager::Status& action);
    void NotifyDiagnosisResultSignals(const Interface::Wqimc::Api::Manager::DiagnosisResult& result);
    void CalibrationTouchScreenSignals();
    void UpLoadFileSignal();
    void DownloadFileSignal();
    void ChangeHardwareParamSignals(const Interface::Wqimc::Api::Agent::Config config);
    void DisconnectSignal();

private slots:
    void InitSlot();
    void UpdateConfigsSlots(const std::vector<Interface::Wqimc::Api::Manager::Config>& config);
    bool UploadMeasureDataSlots(const Interface::Wqimc::Api::Manager::MeasureData& data);
    bool UploadMeasureCurveSlots(const Interface::Wqimc::Api::Manager::MeasureCurve& curve);
    bool UploadReagentProfileSlots();
    void UpdateReagentRemainSlots(const std::vector<Interface::Wqimc::Api::Manager::ReagentRemain> &reagentRemainList);
    void UploadConsumableProfileSlots();
    void UpdaterSignalTimerSlots();
    void SetSignalUpdateCycleSlots(const double cycleSec);
    void RebootSlots();
    void UpgradeSlots(const System::String& url);
    void AlarmSlots(const Interface::Wqimc::Api::Manager::Alarm& alarm);
    void UpdateStatusSlots(const Interface::Wqimc::Api::Manager::Status& status);
    void UpdateActionSlots(const Interface::Wqimc::Api::Manager::Status& action);
    void NotifyDiagnosisResultSlots(const Interface::Wqimc::Api::Manager::DiagnosisResult& result);
    void UpgradeFinishedSlot();
    void UpgradeErrorSlot(const QString message);
    void UpgradeReadyReadSlot(const QByteArray &data);
    void CalibrationTouchScreenSlot();
    void UpLoadFileSlot();
    void UpLoadFinishedSlot();
    void UpLoadErrorSlot(const QString message);
    void DownloadFileSlot();
    void DownloadFinishedSlot();
    void DownloadErrorSlot(const QString message);
    void DownloadReadyReadSlot(const QByteArray &data);
    void ChangeHardwareParamSlot(const Interface::Wqimc::Api::Agent::Config config);
    void ConnectTimerSlot();
    void DisconnectSlot();
};

}
}

#endif  //WQIMC_MANAGER_H
