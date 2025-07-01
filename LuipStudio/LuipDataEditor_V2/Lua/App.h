#ifndef LUA_APP_H
#define LUA_APP_H

#include "System/Types.h"

namespace Lua
{

class App
{

public:
    App();

    static System::String CreateUUID(void);
};

}


#endif // LUA_APP_H
