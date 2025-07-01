#include "LiquidController.h"
#include "LiquidControllerPluginProxy.h"
#include "Liquidcontrollerplugin.h"

using namespace std;
using namespace System;

const static String version = String("1.6.0.0");

namespace Controller
{

LiquidController * LiquidControllerPlugin::m_lc = nullptr;

LiquidControllerPlugin::LiquidControllerPlugin()
{
}

LiquidControllerPlugin::~LiquidControllerPlugin()
{

}

bool LiquidControllerPlugin::Init(log4cpp::Category *log)
{
    LiquidControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *LiquidControllerPlugin::Create(DscpAddress addr)
{
    if (!m_lc)
    {
        m_lc = new LiquidController(addr);
    }
    return m_lc;
}

LiquidController *LiquidControllerPlugin::GetLiquidController()
{
    return m_lc;
}

String LiquidControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new LiquidControllerPlugin();
}

}
