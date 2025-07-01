#ifndef OutputControllerPlugin_H
#define OutputControllerPlugin_H

#include "lua.hpp"
#include "System/Types.h"
#include "Log.h"
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "ControllerPlugin/ControllerPlugin.h"

using System::String;

class OutputController;

namespace Controller
{

class LUIP_SHARE OutputControllerPlugin : public ControllerPlugin
{

public:
    OutputControllerPlugin();
    virtual ~OutputControllerPlugin();
    bool Init(log4cpp::Category* log);
    BaseController* Create(DscpAddress addr);
    OutputController * GetOutputController();
    String GetVersion();

private:
    static OutputController * m_rc;
};

// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ControllerPlugin *CreatePlugin();
}

}
#endif // OutputControllerPlugin_H

