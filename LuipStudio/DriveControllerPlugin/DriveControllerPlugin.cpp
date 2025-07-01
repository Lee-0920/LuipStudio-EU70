#include "DriveController.h"
#include "DriveControllerPluginProxy.h"
#include "DriveControllerplugin.h"

using namespace std;
using namespace System;

const static String version = String("1.0.1.0");

namespace Controller
{

DriveController * DriveControllerPlugin::m_dc = nullptr;

DriveControllerPlugin::DriveControllerPlugin()
{
}

DriveControllerPlugin::~DriveControllerPlugin()
{

}

bool DriveControllerPlugin::Init(log4cpp::Category *log)
{
    DriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *DriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new DriveController(addr);
    }
    return m_dc;
}

DriveController *DriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String DriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new DriveControllerPlugin();
}

}
