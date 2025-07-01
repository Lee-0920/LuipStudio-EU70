#ifndef CONTROLLER_PT62THGCONTROLLERPLUGIN_H
#define CONTROLLER_PT62THGCONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class PT62THgController;

namespace Controller
{

class LUIP_SHARE PT62THgControllerPlugin : public ControllerPlugin
{

public:
    PT62THgControllerPlugin();
    virtual ~PT62THgControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    PT62THgController * GetDriveController();
    String GetVersion();

private:
    static PT62THgController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // CONTROLLER_PT62THGCONTROLLERPLUGIN_H

