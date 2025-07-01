#include "ReactController.h"
#include "ReactControllerPluginProxy.h"
#include "ReactControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.4.0.0");

namespace Controller
{

ReactController * ReactControllerPlugin::m_rc= nullptr;

ReactControllerPlugin::ReactControllerPlugin()
{
}

ReactControllerPlugin::~ReactControllerPlugin()
{
}

bool ReactControllerPlugin::Init(log4cpp::Category *log)
{
    ReactControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *ReactControllerPlugin::Create(DscpAddress addr)
{
    if (!m_rc)
    {
        m_rc = new ReactController(addr);
    }
    return m_rc;
}

ReactController *ReactControllerPlugin::GetReactController()
{
    return m_rc;
}

String ReactControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new ReactControllerPlugin();
}

}
