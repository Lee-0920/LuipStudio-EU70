#ifndef LIQUIDCONTROLLERPLUGIN_H
#define LIQUIDCONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class LiquidController;

namespace Controller
{

class LUIP_SHARE LiquidControllerPlugin : public ControllerPlugin
{

public:
    LiquidControllerPlugin();
    virtual ~LiquidControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    LiquidController * GetLiquidController();
    String GetVersion();

private:
    static LiquidController * m_lc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // LIQUIDCONTROLLERPLUGIN_H
