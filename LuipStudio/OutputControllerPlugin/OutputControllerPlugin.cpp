#include "OutputController.h"
#include "OutputControllerPluginProxy.h"
#include "OutputControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.4.0.0");

namespace Controller
{

OutputController * OutputControllerPlugin::m_rc= nullptr;

OutputControllerPlugin::OutputControllerPlugin()
{
}

OutputControllerPlugin::~OutputControllerPlugin()
{
}

bool OutputControllerPlugin::Init(log4cpp::Category *log)
{
    OutputControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *OutputControllerPlugin::Create(DscpAddress addr)
{
    if (!m_rc)
    {
        m_rc = new OutputController(addr);
    }
    return m_rc;
}

OutputController *OutputControllerPlugin::GetOutputController()
{
    return m_rc;
}

String OutputControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new OutputControllerPlugin();
}

}
