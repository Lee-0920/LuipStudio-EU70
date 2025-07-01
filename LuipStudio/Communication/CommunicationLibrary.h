#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <memory>
#include "Log.h"
#include "System/Types.h"
#include "Stack/DncpMcStack.h"
#include "LuipShare.h"

using System::String;
class IConnectStatusNotifiable;

namespace Communication
{
class LUIP_SHARE CommunicationLibrary
{

public:
    ~CommunicationLibrary();
    static CommunicationLibrary* Instance();
    bool Init(log4cpp::Category* log);
    void Uninit();
    void Proxy();
    void AddSlaveNode(int addr);
    void SetDncpMcStackCmdTable(DscpCmdTable cmdTable, DscpVersion version);
    bool InitDncpStack(char* serialPort, bool bridgeMode,
              const char* bridgeIP, IConnectStatusNotifiable *handle);
    String GetVersion();

private:
    static std::unique_ptr<CommunicationLibrary> m_instance;
};
}
#endif // COMMUNICATION_H
