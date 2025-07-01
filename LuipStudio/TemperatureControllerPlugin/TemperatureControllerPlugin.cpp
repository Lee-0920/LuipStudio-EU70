#include "TemperatureController.h"
#include "TemperatureControllerPluginProxy.h"
#include "Temperaturecontrollerplugin.h"

using namespace std;
using namespace System;

const static String version = String("1.5.0.0");

namespace Controller
{

TemperatureController *TemperatureControllerPlugin::m_tc = nullptr;

TemperatureControllerPlugin::TemperatureControllerPlugin()
{
}

TemperatureControllerPlugin::~TemperatureControllerPlugin()
{
}

bool TemperatureControllerPlugin::Init(log4cpp::Category *log)
{
    TemperatureControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *TemperatureControllerPlugin::Create(Communication::Dscp::DscpAddress addr)
{
    if (!m_tc)
    {
        m_tc = new TemperatureController(addr);
    }
    return m_tc;
}

TemperatureController *TemperatureControllerPlugin::GetTemperatureController()
{
    return m_tc;
}

String TemperatureControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new TemperatureControllerPlugin();
}

}
