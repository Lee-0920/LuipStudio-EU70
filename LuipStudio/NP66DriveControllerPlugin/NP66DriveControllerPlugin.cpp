#include "NP66DriveController.h"
#include "NP66DriveControllerPlugin.h"
#include "NP66DriveControllerPluginProxy.h"

using namespace std;
using namespace System;

const static String version = String("1.0.0.0");

namespace Controller
{

NP66DriveController * NP66DriveControllerPlugin::m_dc = nullptr;

NP66DriveControllerPlugin::NP66DriveControllerPlugin()
{
}

NP66DriveControllerPlugin::~NP66DriveControllerPlugin()
{

}

bool NP66DriveControllerPlugin::Init(log4cpp::Category *log)
{
    NP66DriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *NP66DriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new NP66DriveController(addr);
    }
    return m_dc;
}

NP66DriveController *NP66DriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String NP66DriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new NP66DriveControllerPlugin();
}

}
