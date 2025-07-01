#include "TN67DriveController.h"
#include "TN67DriveControllerPluginProxy.h"
#include "TN67DriveControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.1.0.6");

namespace Controller
{

TN67DriveController * TN67DriveControllerPlugin::m_dc = nullptr;

TN67DriveControllerPlugin::TN67DriveControllerPlugin()
{
}

TN67DriveControllerPlugin::~TN67DriveControllerPlugin()
{

}

bool TN67DriveControllerPlugin::Init(log4cpp::Category *log)
{
    TN67DriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *TN67DriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new TN67DriveController(addr);
    }
    return m_dc;
}

TN67DriveController *TN67DriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String TN67DriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new TN67DriveControllerPlugin();
}

}
