#ifndef INTERFACE_WQIMC_CLOUD_CONNECTION_SERVICE_H_
#define INTERFACE_WQIMC_CLOUD_CONNECTION_SERVICE_H_

#include <mutex>
#include "DuplexThrift/DuplexClientService.h"
#include "Interface/Wqimc/Api/Manager/InstrumentManager.h"
#include "InstrumentAgentHandler.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace erchashu::thrift;
using namespace Interface::Wqimc::Api::Manager;

namespace Interface {
namespace Wqimc {


/**
 * @brief InstrumentAgent 接口实现类。
 */
class CloudConnectionService : public InstrumentManagerIf
{
public:
    virtual ~CloudConnectionService();
    static CloudConnectionService* Instance();

public:
    /**
     * @brief 设置服务器连接参数。
     * @param host 服务器主机名/IP地址。
     * @param port 服务端口号。
     */
    void SetServer(const std::string& host, int port)
    {
        m_serverHost = host;
        m_serverPort = port;
    }

    bool Start();
    bool Stop();
    void OnDisconnectStatus();
    bool Reconnect();

    void SetConnectionNotifier(ConnectionNotifier *notifier);
    void EnableDisconnectNotify(bool enable);

public:
    // InstrumentManagerIf 接口，多线程同步封装
    virtual void echo(std::string& _return, const std::string& data);
    virtual bool authenticate(const std::string& code);
    virtual int64_t getSystemTime();
    virtual bool upload(const Instrument& instrument);
    virtual bool uploadInstrumentPermissionLevel(const int32_t level);
    virtual bool alarm(const Alarm& alarm);
    virtual bool updateStatus(const Status& status);
    virtual bool updateAction(const Status& action);
    virtual bool uploadMeasureData(const MeasureData& data);
    virtual bool uploadMeasureCurve(const MeasureCurve& curve);
    virtual bool uploadSettingProfiles(const std::vector<Profile> & profilesList);
    virtual bool updateConfigs(const std::vector<Config> & config);
    virtual bool uploadReagentProfile(const std::vector<ReagentConfigItem> & configs);
    virtual bool uploadConsumableProfile(const std::vector<ConsumableConfigItem> & configs);
    virtual bool updateReagentRemain(const std::vector<ReagentRemain> & remains);
    virtual bool uploadSignalProfile(const Profile& profile);
    virtual bool updateSignals(const std::vector<Signal> & signalList);
    virtual bool uploadOperations(const std::vector<OperationSuit> & ops);
    virtual bool uploadDiagnoses(const std::vector<DiagnosisSuit> & ops);
    virtual bool notifyDiagnosisResult(const DiagnosisResult& result);

protected:
    static std::unique_ptr<CloudConnectionService> m_instance;
    CloudConnectionService();

    // 服务响应对象链
    InstrumentAgentHandler m_handler;

    // 客户端调用对象链
    boost::shared_ptr<DuplexClientService> m_duplexService;
    boost::shared_ptr<InstrumentManagerClient> m_client;

    // 服务器连接参数
    std::string m_serverHost;
    int m_serverPort;

    std::mutex m_callMutex;
    bool m_isConnected;
    ConnectionNotifier* m_connectionNotifier;
};


}
}

#endif  // INTERFACE_WQIMC_CLOUD_CONNECTION_SERVICE_H_
