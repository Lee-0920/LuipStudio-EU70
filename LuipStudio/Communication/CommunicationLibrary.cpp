#include "Stack/DncpStack.h"
#include "Log.h"
#include "AsyncScheduler.h"
#include "EventHandler.h"
#include "CommunicationLibrary.h"
#include "CommunicationException.h"
#include "CommunicationProxy.h"

using namespace std;
using namespace Communication::Stack;
using namespace Communication;
using namespace System;

const static String version = String("1.0.0.0");

namespace Communication
{
unique_ptr<CommunicationLibrary> CommunicationLibrary::m_instance(nullptr);

CommunicationLibrary::~CommunicationLibrary()
{

}

CommunicationLibrary *CommunicationLibrary::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CommunicationLibrary);
    }

    return m_instance.get();
}

bool CommunicationLibrary::Init(log4cpp::Category* log)
{
    logger = log;

    AsyncScheduler::Instance()->Start();
    EventHandler::Instance()->Init();

    return true;
}

void CommunicationLibrary::Uninit()
{
    DncpStack::Instance()->Uninit();
}

void CommunicationLibrary::Proxy()
{
    CommunicationProxy::Proxy();
    CommunicationExceptionProxy::Init();
}

void CommunicationLibrary::AddSlaveNode(int addr)
{
    DncpMcStack_AddSlaveNode(addr);
}

void CommunicationLibrary::SetDncpMcStackCmdTable(DscpCmdTable cmdTable, DscpVersion version)
{
    DncpMcStack_SetDscpCmdTable(cmdTable, version);
}

bool CommunicationLibrary::InitDncpStack(char *serialPort, bool bridgeMode, const char *bridgeIP, IConnectStatusNotifiable *handle)
{
    return DncpStack::Instance()->Init(serialPort, bridgeMode, bridgeIP, handle);
}

String CommunicationLibrary::GetVersion()
{
    return version;
}

}
