#ifndef CM66DRIVECONTROLLERPLUGIN_H
#define CM66DRIVECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class CM66DriveController;

namespace Controller
{

class LUIP_SHARE CM66DriveControllerPlugin : public ControllerPlugin
{

public:
    CM66DriveControllerPlugin();
    virtual ~CM66DriveControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    CM66DriveController * GetDriveController();
    String GetVersion();

private:
    static CM66DriveController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // CM66DRIVECONTROLLERPLUGIN_H

