#if !defined(RECORDDADAPLUGIN_RECORDDATAPROXY_H)
#define RECORDDADAPLUGIN_RECORDDATAPROXY_H

#include "oolua.h"
#include "RecordData.h"
#include "LuipShare.h"

using ResultData::IntArray;
using ResultData::RecordData;
using System::Uint32;
using System::Byte;

OOLUA_PROXY(LUIP_SHARE,IntArray)

    OOLUA_CTORS(
        OOLUA_CTOR(System::Uint32)
    )
 //   OOLUA_MFUNC(GetData)//此函数无效========================================
    OOLUA_MFUNC_CONST(GetSize)// const 函数版本
    OOLUA_MFUNC_CONST(GetLength)
    OOLUA_MFUNC(SetInt)
//    OOLUA_MEM_FUNC(bool, GetInt, Uint32, out_p<int&>)
OOLUA_PROXY_END

OOLUA_PROXY(LUIP_SHARE,RecordData)
    OOLUA_CTORS(
        OOLUA_CTOR(System::Uint32)
    )
    OOLUA_MFUNC(ResetPos)
    OOLUA_MFUNC(PushBool)
    OOLUA_MFUNC(PushInt)
    OOLUA_MFUNC(PushByte)
    OOLUA_MFUNC(PushFloat)
    OOLUA_MFUNC(PushDouble)
    OOLUA_MFUNC(PushIntArray)
    OOLUA_MFUNC(PushString)
    OOLUA_MFUNC(SetData)
/*
* - out_p<T> ：让 oolua 生成的代理函数在调用被代理函数后，把指定的参数以压栈的形式返回Lua空间，
*   而不是传入参数。可用于返回基本数据类型，也可用于返回C++代理对象，前提是要在被代理函数中创建堆对象，
*   然后由Lua支配它。不能返回的C++中使用的对象，否则会崩溃。
*   Lua 中访问代码: ret, result = readRecordData:GetBool(20)
*   LUA的两个返回值相当于C++的：ret 为GetBool的返回值，类型为bool。result 为第二个实参，类型为bool
*/
    OOLUA_MEM_FUNC(bool, GetBool, Uint32, out_p<bool&>)
    OOLUA_MEM_FUNC(bool, GetInt, Uint32, out_p<int&>)
    OOLUA_MEM_FUNC(bool, GetByte, Uint32, out_p<Byte&>)
    OOLUA_MEM_FUNC(bool, GetFloat, Uint32, out_p<float&>)
    OOLUA_MEM_FUNC(bool, GetDouble, Uint32, out_p<double&>)
/*
* - in_out_p<T> ：让 oolua 生成的代理函数在使用指定的参数调用被代理函数后，
*   同时把指定的参数以压栈的形式返回Lua空间
* Lua 中访问代码:local array = IntArray.new(10)  readRecordData:GetIntArray(20, array)
*/
    OOLUA_MEM_FUNC(bool, GetIntArray, System::Uint32, in_out_p<IntArray&>)
OOLUA_PROXY_END
namespace ResultData
{

class LUIP_SHARE RecordDataProxy
{
public:
    RecordDataProxy() {}
    ~RecordDataProxy() {}

public:
    static void Proxy(void);
};
}
#endif // RECORDDADAPLUGIN_RECORDDATAPROXY_H
