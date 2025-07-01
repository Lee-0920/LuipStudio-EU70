#ifndef NP66DRIVECONTROLLERPLUGIN_H
#define NP66DRIVECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class NP66DriveController;

namespace Controller
{

class LUIP_SHARE NP66DriveControllerPlugin : public ControllerPlugin
{

public:
    NP66DriveControllerPlugin();
    virtual ~NP66DriveControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    NP66DriveController * GetDriveController();
    String GetVersion();

private:
    static NP66DriveController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // NP66DRIVECONTROLLERPLUGIN_H

