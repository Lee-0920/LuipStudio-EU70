#include "PT63DriveController.h"
#include "PT63DriveControllerPluginProxy.h"
#include "PT63DriveControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.5.0.2");

namespace Controller
{

PT63DriveController * PT63DriveControllerPlugin::m_dc = nullptr;

PT63DriveControllerPlugin::PT63DriveControllerPlugin()
{
}

PT63DriveControllerPlugin::~PT63DriveControllerPlugin()
{

}

bool PT63DriveControllerPlugin::Init(log4cpp::Category *log)
{
    PT63DriveControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *PT63DriveControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new PT63DriveController(addr);
    }
    return m_dc;
}

PT63DriveController *PT63DriveControllerPlugin::GetDriveController()
{
    return m_dc;
}

String PT63DriveControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new PT63DriveControllerPlugin();
}

}
