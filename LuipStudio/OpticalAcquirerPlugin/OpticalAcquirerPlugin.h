#ifndef OPTICALACQUIRERPLUGIN_H
#define OPTICALACQUIRERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"

using System::String;

class OpticalAcquirer;

namespace Controller
{

class LUIP_SHARE OpticalAcquirerPlugin : public ControllerPlugin
{

public:
    OpticalAcquirerPlugin();
    virtual ~OpticalAcquirerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    OpticalAcquirer * GetOpticalAcquirer();
    String GetVersion();

private:
    static OpticalAcquirer * m_oa;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}

#endif // OPTICALACQUIRERPLUGIN_H
