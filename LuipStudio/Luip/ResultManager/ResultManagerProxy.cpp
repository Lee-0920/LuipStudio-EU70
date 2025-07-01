#include "ResultManagerProxy.h"
#include "ResultManager.h"
#include "RecordDataPlugin/RecordDataProxy.h"

using namespace Result;
using namespace OOLUA;
using namespace std;

Script* ResultManagerProxy::m_lua;

/**
 * enum class 枚举包装。
 * 独立为全局枚举，在 Lua 中的用法：
 * 1. 可跟类一样使用，创建的对象为 userdata 类型。
 * 2. 只用作值时： FieldType.Bool 为一个 number
 */
/**
 * @brief 字段的数据类型
 */
OOLUA_PROXY(,FieldType)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Bool)   // 枚举值
        OOLUA_ENUM(Byte)   // 枚举值
        OOLUA_ENUM(Int)   // 枚举值
        OOLUA_ENUM(Float)   // 枚举值
        OOLUA_ENUM(Double)   // 枚举值
        OOLUA_ENUM(IntArray)   // 枚举值
        OOLUA_ENUM(Time)   // 枚举值
        OOLUA_ENUM(Enum)   // 枚举值
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(FieldType)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 测量类型。
 * @details
 */
OOLUA_PROXY(,MeasureType)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Blank)
        OOLUA_ENUM(Sample)
        OOLUA_ENUM(Standard)
        OOLUA_ENUM(Check)
        OOLUA_ENUM(Addstandard)
        OOLUA_ENUM(Parallel)
        OOLUA_ENUM(ZeroCheck)
        OOLUA_ENUM(RangeCheck)
        OOLUA_ENUM(ExtAddstandard)
        OOLUA_ENUM(ExtParallel)
        OOLUA_ENUM(QualityHandle)
        OOLUA_ENUM(None)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(MeasureType)

/**
 * @brief 记录文件字段的信息
 */
OOLUA_PROXY(,RecordField)
    // 公共成员变量，oolua 通过自动产生的 getter/setter 函数实现，默认函数名为get/set加下划线加变量名
    OOLUA_MGET_MSET(size, GetSize, SetSize)// 改名，不使用默认的get_/set_前缀，自己定义lua中的新函数名为 RecordField:GetSize()
    OOLUA_MGET_MSET(start, GetStart, SetStart)
    OOLUA_MGET_MSET(name, GetName, SetName)
    OOLUA_MGET_MSET(type, GetType, SetType)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RecordField, SetType, SetName, SetSize, SetStart)
OOLUA_EXPORT_FUNCTIONS_CONST(RecordField, GetType, GetName, GetSize, GetStart)

/**
 * @brief 记录文件数据模板
 */
OOLUA_PROXY(,RecordFields)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(std::string)
    )
    //Lua 中访问代码： _, field1 = recordFields:GetField("mode")
   OOLUA_MEM_FUNC(bool, GetField, std::string, out_p<RecordField&>)
   OOLUA_MFUNC_CONST(GetFieldsSize)
OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(RecordFields, GetField)
OOLUA_EXPORT_FUNCTIONS_CONST(RecordFields, GetFieldsSize)

/**
 * @brief 结果管理器
 */
OOLUA_PROXY(,ResultManager)
    OOLUA_TAGS(
        No_public_constructors  // 单例，不能在 lua 中创建对象
    )
    OOLUA_SFUNC(Instance)   //Lua 中访问代码：ResultManager.Instance()
    OOLUA_MFUNC(AddMeasureRecord)
    OOLUA_MFUNC(AddCalibrateRecord)
    OOLUA_MFUNC(AddCalibrateRecordSlots)
    OOLUA_MFUNC(GetMeasureRecordDataSize)
    OOLUA_MFUNC(GetCalibrateRecordDataSize)
    OOLUA_MFUNC(AddAuditTrailRecord)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(ResultManager, AddMeasureRecord, AddCalibrateRecordSlots,AddCalibrateRecord, GetMeasureRecordDataSize, GetCalibrateRecordDataSize,
                       AddAuditTrailRecord)
OOLUA_EXPORT_FUNCTIONS_CONST(ResultManager)

namespace Result
{
void ResultManagerProxy::ProxyClass(Script * lua)
{
    m_lua = lua;

    m_lua->register_class<RecordField>();
    m_lua->register_class<RecordFields>();
    m_lua->register_class<ResultManager>();
    m_lua->register_class_static<ResultManager>("Instance",
             &OOLUA::Proxy_class<ResultManager>::Instance); // 静态函数要单独注册


}

void ResultManagerProxy::ProxyEnum(Script * lua)
{
    m_lua = lua;

    m_lua->register_class<FieldType>();
    m_lua->register_class<MeasureType>();
}
}
