#ifndef DRIVECONTROLLERPLUGIN_H
#define DRIVECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class DriveController;

namespace Controller
{

class LUIP_SHARE DriveControllerPlugin : public ControllerPlugin
{

public:
    DriveControllerPlugin();
    virtual ~DriveControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    DriveController * GetDriveController();
    String GetVersion();

private:
    static DriveController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // DRIVECONTROLLERPLUGIN_H

