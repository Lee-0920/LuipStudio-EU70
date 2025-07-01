#if !defined(UI_UIPROXY_H)
#define UI_UIPROXY_H

#include "oolua.h"

namespace UI
{

class UIProxy
{
public:
    UIProxy() {}
    ~UIProxy() {}

public:
    static void Proxy(OOLUA::Script * lua);

protected:
    static OOLUA::Script* m_lua;
};

}
#endif // UI_UIPROXY_H
