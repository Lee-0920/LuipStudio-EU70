#include "RecordDataProxy.h"
#include "LuaEngine/LuaEngine.h"

using namespace OOLUA;
using namespace Lua;

//OOLUA_EXPORT_FUNCTIONS(IntArray, GetInt, SetInt)
OOLUA_EXPORT_FUNCTIONS(IntArray, SetInt)
OOLUA_EXPORT_FUNCTIONS_CONST(IntArray, GetSize, GetLength)

/**
 * @brief 记录文件的数据
 */

OOLUA_EXPORT_FUNCTIONS(RecordData, ResetPos, PushBool, PushInt, PushByte, PushFloat, PushDouble,
                       GetBool, GetInt, GetByte, GetFloat, GetDouble, GetIntArray, PushIntArray,
                       PushString, SetData)
//OOLUA_EXPORT_FUNCTIONS(RecordData, ResetPos, PushBool, PushInt, PushByte, PushFloat, PushDouble,
//                        GetIntArray, PushIntArray)
OOLUA_EXPORT_FUNCTIONS_CONST(RecordData)

namespace ResultData
{
void RecordDataProxy::Proxy(void)
{
    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::register_class<IntArray>(*lua);
    OOLUA::register_class<RecordData>(*lua);
}

}
