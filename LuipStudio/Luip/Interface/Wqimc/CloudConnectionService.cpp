#include <iostream>
#include <boost/make_shared.hpp>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/TProcessor.h>
#include "DuplexThrift/CallFramedTransport.h"
#include "CloudConnectionService.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace Interface::Wqimc::Api::Manager;

namespace Interface {
namespace Wqimc {

unique_ptr<CloudConnectionService> CloudConnectionService::m_instance(nullptr);

CloudConnectionService::CloudConnectionService()
{
    m_serverHost = "localhost";
    m_serverPort = 9090;
    m_isConnected = false;

    m_duplexService = nullptr;
}

CloudConnectionService::~CloudConnectionService()
{
    m_isConnected = false;
}

CloudConnectionService* CloudConnectionService::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CloudConnectionService);
    }

    return m_instance.get();
}

bool CloudConnectionService::Start()
{
    bool ret = false;

    // 客户端调用链
    boost::shared_ptr<CallFramedTransport> transport = boost::make_shared<CallFramedTransport>(20000);
    boost::shared_ptr<TProtocol> protocol = boost::make_shared<TCompactProtocol>(transport);
    m_client = boost::make_shared<InstrumentManagerClient>(protocol);
    m_duplexService = boost::make_shared<DuplexClientService>(transport);
    m_duplexService->setServer(m_serverHost, m_serverPort);

    // 客户端提供的反向 RPC 服务处理链
    boost::shared_ptr<InstrumentAgentHandler> handler = boost::make_shared<InstrumentAgentHandler>();
    m_duplexService->setProcessor(boost::make_shared<Interface::Wqimc::Api::Agent::InstrumentAgentProcessor>(handler));
    m_duplexService->setProtocolFactory(boost::make_shared<TCompactProtocolFactory>());
    m_duplexService->setTransportFactory(boost::make_shared<TFramedTransportFactory>());

    // 启用心跳机制
    m_duplexService->enableHeartbeat(true);
    m_duplexService->setHeartRate(30);

    if (m_connectionNotifier)
    {
        m_duplexService->setConnectionNotifier(m_connectionNotifier);
    }

//    try
//    {
        ret = m_duplexService->connect();

        if (ret)
        {
            m_isConnected = true;
        }
//    }catch (TTransportException ex)
//	{
//        throw ex;
//    }catch(TException ex)
//    {
//        throw ex;
//    }catch(exception ex)
//    {
//        throw ex;
//    }

//    if (ret)
//    {
//        m_duplexService->start();
//    }

    return ret;
}

bool CloudConnectionService::Stop()
{
    unique_lock<mutex> lock(m_callMutex);

    if (m_duplexService != nullptr)
    {
        m_duplexService->stop();
        return true;
    }
    else
    {
        return false;
    }
}

void CloudConnectionService::OnDisconnectStatus()
{
    unique_lock<mutex> lock(m_callMutex);
    m_isConnected = false;
}

bool CloudConnectionService::Reconnect()
{
    unique_lock<mutex> lock(m_callMutex);

    bool ret = false;

//    try
//    {
        ret = m_duplexService->reconnect();

        if (ret)
        {
            m_isConnected = true;
        }
//    }catch (TTransportException ex)
//    {
//        throw ex;
//    }catch(TException ex)
//    {
//        throw ex;
//    }catch(exception ex)
//    {
//        throw ex;
//    }

    return ret;
}

void CloudConnectionService::SetConnectionNotifier(ConnectionNotifier *notifier)
{
    m_connectionNotifier = notifier;
}

void CloudConnectionService::EnableDisconnectNotify(bool enable)
{
    if (m_duplexService)
        m_duplexService->EnableDisconnectNotify(enable);
}

void CloudConnectionService::echo(std::string& _return, const std::string& data)
{
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        m_client->echo(_return, data);
}

bool CloudConnectionService::authenticate(const std::string& code)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->authenticate(code);
    return ret;
}

int64_t CloudConnectionService::getSystemTime()
{
    int64_t ret = 0;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->getSystemTime();
    return ret;
}

bool CloudConnectionService::upload(const Instrument& instrument)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->upload(instrument);
    return ret;
}

bool CloudConnectionService::uploadInstrumentPermissionLevel(const int32_t level)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadInstrumentPermissionLevel(level);
    return ret;
}

bool CloudConnectionService::alarm(const Alarm& alarm)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->alarm(alarm);
    return ret;
}

bool CloudConnectionService::updateStatus(const Status& status)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->updateStatus(status);
    return ret;
}

bool CloudConnectionService::updateAction(const Status& action)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->updateAction(action);
    return ret;
}

bool CloudConnectionService::uploadMeasureData(const MeasureData& data)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadMeasureData(data);
    return ret;
}

bool CloudConnectionService::uploadMeasureCurve(const MeasureCurve& curve)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadMeasureCurve(curve);
    return ret;
}

bool CloudConnectionService::uploadSettingProfiles(const std::vector<Profile> & profilesList)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadSettingProfiles(profilesList);
    return ret;
}

bool CloudConnectionService::updateConfigs(const std::vector<Config> & config)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->updateConfigs(config);
    return ret;
}

bool CloudConnectionService::uploadReagentProfile(const std::vector<ReagentConfigItem> & configs)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadReagentProfile(configs);
    return ret;
}

bool CloudConnectionService::uploadConsumableProfile(const std::vector<ConsumableConfigItem> & configs)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadConsumableProfile(configs);
    return ret;
}

bool CloudConnectionService::updateReagentRemain(const std::vector<ReagentRemain> & remains)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->updateReagentRemain(remains);
    return ret;
}

bool CloudConnectionService::uploadSignalProfile(const Profile& profile)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadSignalProfile(profile);
    return ret;
}

bool CloudConnectionService::updateSignals(const std::vector<Signal> & signalList)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->updateSignals(signalList);
    return ret;
}

bool CloudConnectionService::uploadOperations(const std::vector<OperationSuit> & ops)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadOperations(ops);
    return ret;
}

bool CloudConnectionService::uploadDiagnoses(const std::vector<DiagnosisSuit> & ops)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->uploadDiagnoses(ops);
    return ret;
}

bool CloudConnectionService::notifyDiagnosisResult(const DiagnosisResult& result)
{
    bool ret = false;
    unique_lock<mutex> lock(m_callMutex);
    if (m_isConnected)
        ret = m_client->notifyDiagnosisResult(result);
    return ret;
}



}
}
