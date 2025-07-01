#ifndef TEMPERATURECONTROLLERPLUGIN_H
#define TEMPERATURECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"

using System::String;

class TemperatureController;

namespace Controller
{

class LUIP_SHARE TemperatureControllerPlugin : public ControllerPlugin
{

public:
    TemperatureControllerPlugin();
    virtual ~TemperatureControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    TemperatureController * GetTemperatureController();
    String GetVersion();

private:
    static TemperatureController * m_tc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // TEMPERATURECONTROLLERPLUGIN_H
