#ifndef CONTROLLERPLUGIN_H
#define CONTROLLERPLUGIN_H

#include "lua.hpp"
#include "System/Types.h"
#include "LuipShare.h"
#include "BaseController.h"
#include "oolua.h"

using System::String;

namespace Controller
{

class LUIP_SHARE  ControllerPlugin
{
public:
    ControllerPlugin();
    virtual ~ControllerPlugin();
    virtual bool Init(log4cpp::Category* log);
    virtual BaseController* Create(DscpAddress addr);
    virtual String GetVersion();  
};

}
#endif // CONTROLLERPLUGIN_H
