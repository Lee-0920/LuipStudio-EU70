#include "CM66DriveController.h"
#include "CM66DriveControllerPluginProxy.h"
#include "CM66DriveControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.3.0.3");

namespace Controller
{

CM66DriveController * CM66DriveControllerPlugin::m_dc = nullptr;

CM66DriveControllerPlugin::CM66DriveControllerPlugin()
{
}

CM66DriveControllerPlugin::~CM66DriveControllerPlugin()
{

}

bool CM66DriveControllerPlugin::Init(log4cpp::Category *log)
{
    CM66DriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *CM66DriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new CM66DriveController(addr);
    }
    return m_dc;
}

CM66DriveController *CM66DriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String CM66DriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new CM66DriveControllerPlugin();
}

}
