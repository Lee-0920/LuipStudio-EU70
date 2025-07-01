#ifndef REACTCONTROLLERPLUGIN_H
#define REACTCONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"

using System::String;

class ReactController;

namespace Controller
{

class LUIP_SHARE ReactControllerPlugin : public ControllerPlugin
{

public:
    ReactControllerPlugin();
    virtual ~ReactControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    ReactController * GetReactController();
    String GetVersion();

private:
    static ReactController * m_rc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // REACTCONTROLLERPLUGIN_H

