#ifndef TN67DRIVECONTROLLERPLUGIN_H
#define TN67DRIVECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class TN67DriveController;

namespace Controller
{

class LUIP_SHARE TN67DriveControllerPlugin : public ControllerPlugin
{

public:
    TN67DriveControllerPlugin();
    virtual ~TN67DriveControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    TN67DriveController * GetDriveController();
    String GetVersion();

private:
    static TN67DriveController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // TN67DRIVECONTROLLERPLUGIN_H

