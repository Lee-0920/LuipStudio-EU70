#include "NT66CurvePlugin.h"
#include "NT66CurvePluginProxy.h"
#include "CurveManager.h"

using namespace std;

namespace ResultDetail
{

NT66CurvePlugin::~NT66CurvePlugin()
{

}

bool NT66CurvePlugin::Init(log4cpp::Category* log)
{
    logger = log;
    NT66CurvePluginProxy::Proxy();
    CurveManager::Instance()->Init();

    return true;
}

System::String NT66CurvePlugin::GetVersion()
{
    return version;
}
}

ResultDetail::ResultDetailPlugin *CreatePlugin()
{
    return new ResultDetail::NT66CurvePlugin();
}

