#include "GP80PrinterPlugin.h"
#include "GP80PrinterPluginProxy.h"

using namespace std;
namespace PrinterSpace
{

std::unique_ptr<GP80PrinterPlugin> GP80PrinterPlugin::m_instance(nullptr);

GP80PrinterPlugin::~GP80PrinterPlugin()
{

}

GP80PrinterPlugin *GP80PrinterPlugin::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new GP80PrinterPlugin);
    }

    return m_instance.get();
}

bool GP80PrinterPlugin::Init(log4cpp::Category* log)
{
    logger = log;
    GP80PrinterPluginProxy::Proxy();
    return true;
}

System::String GP80PrinterPlugin::GetVersion()
{
    return version;
}

}

PrinterSpace::GP80PrinterPlugin *CreatePlugin()
{
    return new PrinterSpace::GP80PrinterPlugin();
}

