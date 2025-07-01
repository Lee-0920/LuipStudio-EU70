#ifndef INTERFACE_WQIMC_INSTRUMENT_AGENT_HANDLE_H_
#define INTERFACE_WQIMC_INSTRUMENT_AGENT_HANDLE_H_

#include <thrift/transport/TBufferTransports.h>
#include "Api/Agent/InstrumentAgent.h"

namespace Interface {
namespace Wqimc {


/**
 * @brief InstrumentAgent 接口实现类。
 */
class InstrumentAgentHandler : public Interface::Wqimc::Api::Agent::InstrumentAgentIf
{
public:
    InstrumentAgentHandler() {}
    virtual ~InstrumentAgentHandler() {}
public:
    virtual void echo(std::string& _return, const std::string& data);
    virtual bool upgrade(const std::string& type, const std::string& newVersion, const std::string& url);
    virtual void getSignals(std::vector<Interface::Wqimc::Api::Agent::Signal> & _return);
    virtual bool setSignalUpdateCycle(const double cycleSec);
    virtual bool changeConfigs(const std::vector<Interface::Wqimc::Api::Agent::Config> & configs);
    virtual bool changeSystemTime(const int64_t time);
    virtual bool restore(const Interface::Wqimc::Api::Agent::RoleType::type privilege, const std::string& profileName);
    virtual bool reboot();
    virtual bool execute(const Interface::Wqimc::Api::Agent::Operation& op);
    virtual bool diagnose(const std::vector<Interface::Wqimc::Api::Agent::Diagnosis> & diags);
    virtual bool stop();
    virtual bool setAuthorization(const Interface::Wqimc::Api::Agent::Authorization& authorization);
    virtual bool calibrationTouchScreen();
    virtual bool reintateFactorypattern();
    virtual bool updataMeasureData(const Interface::Wqimc::Api::Agent::MeasureData& data);
    virtual void getLogFileNames(std::vector<std::string> & _return);
    virtual bool catchLogFile(const std::string& logFileName, const std::string& uploadPath);
    virtual bool cdFoleder(const std::string& filePath);
    virtual void getFileList(std::vector<Interface::Wqimc::Api::Agent::InstrumentFile> & _return, const std::string& filePath, const bool showHiddenFile);
    virtual bool downLoadInstrumentFile(const std::string& filePath, const std::string& uploadPath);
    virtual bool deleteInstrumentFile(const std::string& filePath);
    virtual bool uploadInstrumentFile(const std::string& filePath, const std::string& url);
    virtual bool addFolder(const std::string& filePath);
    void excuteCommand(std::string& _return, const std::string& command);
    virtual bool factorypattern();
    virtual bool faultClearing();

};


}
}

#endif  // INTERFACE_WQIMC_INSTRUMENT_AGENT_HANDLE_H_
