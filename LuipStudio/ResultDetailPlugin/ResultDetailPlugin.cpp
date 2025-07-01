#include "ResultDetailPlugin.h"
#include "ResultDetailPluginProxy.h"

using namespace std;
namespace ResultDetail
{
unique_ptr<ResultDetailPlugin> ResultDetailPlugin::m_instance(nullptr);

ResultDetailPlugin::~ResultDetailPlugin()
{

}

ResultDetailPlugin *ResultDetailPlugin::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ResultDetailPlugin);
    }

    return m_instance.get();
}

bool ResultDetailPlugin::Init(log4cpp::Category* log)
{
    logger = log;
    ResultDetailPluginProxy::Proxy();
    return true;
}

System::String ResultDetailPlugin::GetVersion()
{
    return version;
}
}
