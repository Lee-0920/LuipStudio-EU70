#if !defined(LUA_LOAD_SCRIPT_H)
#define LUA_LOAD_SCRIPT_H

#include <memory>
#include "oolua.h"

namespace Lua
{

class LoadScript
{
public:
    ~LoadScript();
    static LoadScript* Instance();

public:
    bool LoadInitScript();
    bool LoadPluginScript();
    bool LoadInstrumentScript();
    bool InitWidgetUserConfig();
    bool InitInstrument();

private:
    LoadScript();
    static std::unique_ptr<LoadScript> m_instance;

};

}
#endif  //LUA_ENGINE_H
