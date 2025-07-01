#include "TOCDriveController.h"
#include "TOCDriveControllerPluginProxy.h"
#include "TOCDriveControllerplugin.h"

using namespace std;
using namespace System;

const static String version = String("1.5.0.2");

namespace Controller
{

TOCDriveController * TOCDriveControllerPlugin::m_dc = nullptr;

TOCDriveControllerPlugin::TOCDriveControllerPlugin()
{
}

TOCDriveControllerPlugin::~TOCDriveControllerPlugin()
{

}

bool TOCDriveControllerPlugin::Init(log4cpp::Category *log)
{
    TOCDriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *TOCDriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new TOCDriveController(addr);
    }
    return m_dc;
}

TOCDriveController *TOCDriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String TOCDriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new TOCDriveControllerPlugin();
}

}
