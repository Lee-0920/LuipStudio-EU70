#if !defined(SETTING_SETTINGMANAGERPROXY_H)
#define SETTING_SETTINGMANAGERPROXY_H

#include "oolua.h"

class SettingManagerProxy
{
public:
    SettingManagerProxy() {}
    ~SettingManagerProxy() {}

public:
    static void Proxy(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};

#endif // SETTING_SETTINGMANAGERPROXY_H
