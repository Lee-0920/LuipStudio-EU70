#ifndef TOCDRIVECONTROLLERPLUGIN_H
#define TOCDRIVECONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "oolua.h"

using System::String;

class TOCDriveController;

namespace Controller
{

class LUIP_SHARE TOCDriveControllerPlugin : public ControllerPlugin
{

public:
    TOCDriveControllerPlugin();
    virtual ~TOCDriveControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    TOCDriveController * GetDriveController();
    String GetVersion();

private:
    static TOCDriveController * m_dc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // TOCDRIVECONTROLLERPLUGIN_H

