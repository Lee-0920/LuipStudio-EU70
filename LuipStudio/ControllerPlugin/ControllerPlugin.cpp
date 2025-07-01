#include "Log.h"
#include "ControllerPlugin.h"
#include "ControllerPluginProxy.h"
using namespace std;
using namespace System;

const static String version = String("1.0.0.0");

namespace Controller
{

ControllerPlugin::ControllerPlugin()
{

}

ControllerPlugin::~ControllerPlugin()
{

}

BaseController *ControllerPlugin::Create(DscpAddress addr)
{
    (void) addr;
    return nullptr;
}

String ControllerPlugin::GetVersion()
{
    return version;
}

bool ControllerPlugin::Init(log4cpp::Category *log)
{
   ControllerPluginProxy::Proxy();
   logger = log;

   return true;
}

}
