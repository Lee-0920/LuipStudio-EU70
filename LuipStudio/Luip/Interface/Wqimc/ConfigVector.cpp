#include "ConfigVector.h"
namespace Interface
{
namespace Wqimc
{
ConfigVector::ConfigVector()
{

}

void ConfigVector::Push(Api::Manager::Config config)
{
    configList.push_back(config);
}

System::Uint32 ConfigVector::Size()
{
    return configList.size();
}

std::vector<Api::Manager::Config> ConfigVector::GetConfigList(void)
{
    return configList;
}

}
}
