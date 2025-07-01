#ifndef CONFIGVECTOR_H
#define CONFIGVECTOR_H

#include <vector>
#include "Interface/Wqimc/Api/Manager/InstrumentManager_types.h"
#include "System/Types.h"

namespace Interface
{
namespace Wqimc
{

class ConfigVector
{
public:
    ConfigVector();
    void Push(Api::Manager::Config config);
    System::Uint32 Size();
    std::vector<Api::Manager::Config> GetConfigList(void);
private:
    std::vector<Api::Manager::Config> configList;
};
}
}
#endif // CONFIGVECTOR_H
