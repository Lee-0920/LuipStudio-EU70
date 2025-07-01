#include "PrinterPlugin.h"
#include "PrinterPluginProxy.h"

using namespace std;
namespace PrinterSpace
{

std::unique_ptr<PrinterPlugin> PrinterPlugin::m_instance(nullptr);

PrinterPlugin::~PrinterPlugin()
{

}

PrinterPlugin *PrinterPlugin::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new PrinterPlugin);
    }

    return m_instance.get();
}

bool PrinterPlugin::Init(log4cpp::Category* log)
{
    logger = log;
    PrinterPluginProxy::Proxy();
    return true;
}

System::String PrinterPlugin::GetVersion()
{
    return version;
}

}
