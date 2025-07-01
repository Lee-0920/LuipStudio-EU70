#include "PT62THgController.h"
#include "PT62THgControllerPluginProxy.h"
#include "PT62THgControllerPlugin.h"

using namespace std;
using namespace System;

const static String version = String("1.1.0.0");

namespace Controller
{

PT62THgController * PT62THgControllerPlugin::m_dc = nullptr;

PT62THgControllerPlugin::PT62THgControllerPlugin()
{
}

PT62THgControllerPlugin::~PT62THgControllerPlugin()
{

}

bool PT62THgControllerPlugin::Init(log4cpp::Category *log)
{
    PT62THgControllerPluginProxy::Proxy();
    logger = log;

    return true;
}

BaseController *PT62THgControllerPlugin::Create(DscpAddress addr)
{
    if (!m_dc)
    {
        m_dc = new PT62THgController(addr);
    }
    return m_dc;
}

PT62THgController *PT62THgControllerPlugin::GetDriveController()
{
    return m_dc;
}

String PT62THgControllerPlugin::GetVersion()
{
    return version;
}

ControllerPlugin *CreatePlugin()
{
    return new PT62THgControllerPlugin();
}

}
