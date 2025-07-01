#ifndef LUA_APP_H
#define LUA_APP_H

#include "System/Types.h"

namespace Lua
{

/**
 * @brief  单位转换类型。
 * @details
 */
enum class UnitChange
{
  Read = 0,
  Write = 1,
};

/**
 * @brief  标线XY转换类型。
 * @details
 */
enum class CurveXYChange
{
  Read = 0,
  Write = 1,
};

class App
{

public:
    App();

    static void Sleep(int ms);
    static System::String CreateUUID(void);
    static void SetScreenTime(float darkTime, float closeTime, float userExitTime);
    static void SetSceenEnable(bool isEnable);
    static int GetCurrentRole();
    static System::String GetCurrentRoleStr();
    static long GetOSRunTime();
    static System::String GetCurrentUserName();
    static System::String GetCurrentLevelName();
};

}


#endif // LUA_APP_H
