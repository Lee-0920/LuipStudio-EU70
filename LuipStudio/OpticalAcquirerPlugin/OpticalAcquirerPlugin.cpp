#include "OpticalAcquirer.h"
#include "OpticalAcquirerPluginProxy.h"
#include "OpticalAcquirerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.0.1.0");

namespace Controller
{

OpticalAcquirer * OpticalAcquirerPlugin::m_oa= nullptr;

OpticalAcquirerPlugin::OpticalAcquirerPlugin()
{
}

OpticalAcquirerPlugin::~OpticalAcquirerPlugin()
{
}

bool OpticalAcquirerPlugin::Init(log4cpp::Category *log)
{
    OpticalAcquirerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *OpticalAcquirerPlugin::Create(DscpAddress addr)
{
    if (!m_oa)
    {
        m_oa = new OpticalAcquirer(addr);
    }
    return m_oa;
}

OpticalAcquirer *OpticalAcquirerPlugin::GetOpticalAcquirer()
{
    return m_oa;
}

String OpticalAcquirerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new OpticalAcquirerPlugin();
}

}
