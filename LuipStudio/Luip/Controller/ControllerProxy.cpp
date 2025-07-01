//#include "System/Types.h"
////#include "Communication/Dscp/DscpAddress.h"
////#include "Communication/IEventReceivable.h"
//#include "ControllerPlugin/API/ManufDate.h"
//#include "ControllerPlugin/API/UpdatableInfo.h"
//#include "ControllerPlugin/API/Version.h"
////#include "ControllerPlugin/API/DeviceInterface.h"
//#include "ControllerPlugin/API/DeviceInfoInterface.h"
//#include "ControllerPlugin/API/DeviceStatusInterface.h"
//#include "ControllerPlugin/API/DeviceUpdateInterface.h"
//#include "ControllerPlugin/API/DscpSystemInterface.h"
//#include "ControllerPlugin/BaseController.h"
#include "ControllerProxy.h"

//using System::Byte;
//using namespace Communication;
//using namespace Communication::Dscp;
//using namespace Controller;
//using namespace Controller::API;


/**
 * @brief 设备网络地址。
 * @details DSCP设备地址，对应网络层地址。
 */
/*
OOLUA_PROXY(DscpAddress)
    OOLUA_CTORS(
        OOLUA_CTOR(Byte, Byte)
        OOLUA_CTOR(Byte, Byte, Byte)
        OOLUA_CTOR(Byte, Byte, Byte, Byte)
    )
    OOLUA_MGET_MSET(a1, GetA1, SetA1)
    OOLUA_MGET_MSET(a2, GetA2, SetA2)
    OOLUA_MGET_MSET(a3, GetA3, SetA3)
    OOLUA_MGET_MSET(a4, GetA4, SetA4)
    OOLUA_MFUNC(Make)
    OOLUA_MFUNC(ToString)

OOLUA_PROXY_END */
OOLUA_EXPORT_FUNCTIONS(DscpAddress, SetA1, SetA2, SetA3, SetA4, Make, ToString)
OOLUA_EXPORT_FUNCTIONS_CONST(DscpAddress, GetA1, GetA2, GetA3, GetA4)

/**
 * @brief 版本号。
 * @details
 */
/*OOLUA_PROXY(Version)
    OOLUA_CTORS(
        OOLUA_CTOR(int, int)
        OOLUA_CTOR(int, int, int)
        OOLUA_CTOR(int, int, int, int)
        OOLUA_CTOR(String)
    )
    OOLUA_MFUNC(GetMajor)
    OOLUA_MFUNC(SetMajor)
    OOLUA_MFUNC(GetMinor)
    OOLUA_MFUNC(SetMinor)
    OOLUA_MFUNC(GetRevision)
    OOLUA_MFUNC(SetRevision)
    OOLUA_MFUNC(GetBuild)
    OOLUA_MFUNC(SetBuild)
    OOLUA_MFUNC(Parse)
    OOLUA_MFUNC(ToString)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(Version, GetMajor, SetMajor, GetMinor,
                       GetMajor, GetRevision, SetRevision , GetBuild,
                       SetBuild, Parse, ToString)
OOLUA_EXPORT_FUNCTIONS_CONST(Version)

/**
 * @brief 设备生产日期。
 * @details
 */
/*OOLUA_PROXY(ManufDate)
    OOLUA_CTORS(
        OOLUA_CTOR(int, int, int)
        OOLUA_CTOR(Byte*)
    )
    OOLUA_MFUNC(SetData)
    OOLUA_MFUNC(GetData)
    OOLUA_MFUNC(SetYear)
    OOLUA_MFUNC(GetYear)
    OOLUA_MFUNC(SetMonth)
    OOLUA_MFUNC(GetMonth)
    OOLUA_MFUNC(SetDay)
    OOLUA_MFUNC(GetDay)
    OOLUA_MFUNC(ToString)
OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(ManufDate, SetData, GetData, SetYear, GetYear,
                       SetMonth, GetMonth, SetDay, GetDay, ToString)
OOLUA_EXPORT_FUNCTIONS_CONST(ManufDate)

/**
 * @brief 可更新的信息。
 * @details 可更新信息保存在控制器对象的内存中，客户代码调用具体接口设置信息时，
 *  如果设置成功，还需要把设置的信息更新到控制器内存中。
 */
/*OOLUA_PROXY(UpdatableInfo)
    OOLUA_MFUNC(Update)
    OOLUA_MFUNC(IsUpdated)
    OOLUA_MFUNC(GetUpdateTime)
OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(UpdatableInfo, Update, IsUpdated, GetUpdateTime)
OOLUA_EXPORT_FUNCTIONS_CONST(UpdatableInfo)

/**
 * @brief 控制器（板卡）信息包装类。
 * @details DeviceInfoInterface 接口的内存映像类。
 */
/*OOLUA_PROXY(ControllerInfo, UpdatableInfo)
    OOLUA_MGET_MSET(type, GetType, SetType)
    OOLUA_MGET_MSET(model, GetModel, SetModel)
    OOLUA_MGET_MSET(sn, GetSN, SetSN)
    OOLUA_MGET_MSET(softVersion, GetSoftVersion, SetSoftVersion)
    OOLUA_MGET_MSET(softVersion, GetHardVersion, SetHardVersion)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(ControllerInfo, SetType, SetModel, SetSN, SetSoftVersion, SetHardVersion)
OOLUA_EXPORT_FUNCTIONS_CONST(ControllerInfo, GetType, GetModel, GetSN, GetSoftVersion, GetHardVersion)

/**
 * @brief 设备运行模式。
 * @details
 */
/*OOLUA_PROXY(RunMode)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Application)   // 枚举值
        OOLUA_ENUM(Updater)   // 枚举值
        OOLUA_ENUM(Upgrader)   // 枚举值
    )
OOLUA_PROXY_END*/
OOLUA_EXPORT_NO_FUNCTIONS(RunMode)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 电源供给类型。
 * @details
 */
/*OOLUA_PROXY(PowerSupplyType)
    OOLUA_TAGS(
        Register_class_enums  // 枚举需要特别标注，否则枚举值无效
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Unknown)   // 枚举值
        OOLUA_ENUM(OnGoingAC)   // 枚举值
        OOLUA_ENUM(Bus)   // 枚举值
        OOLUA_ENUM(Box)   // 枚举值
        OOLUA_ENUM(Battery)   // 枚举值
        OOLUA_ENUM(Charger)   // 枚举值

    )
OOLUA_PROXY_END*/
OOLUA_EXPORT_NO_FUNCTIONS(PowerSupplyType)  // 导出函数声明（没有函数也需要声明）

/**
 * @brief 事件接收接口。
 * @details 观察者。
 */
/*
OOLUA_PROXY(IEventReceivable)
    // 特殊标记定义
    OOLUA_TAGS(
        Abstract  // 指定抽象标记，oolua 不会生成 new()，且会忽略 OOLUA_CTORS()
    )
//    OOLUA_MFUNC(OnReceive)

OOLUA_PROXY_END */
OOLUA_EXPORT_FUNCTIONS(IEventReceivable)
OOLUA_EXPORT_FUNCTIONS_CONST(IEventReceivable)

/**
 * @brief AD信号上报接口。
 * @details 观察者。
 */
/*OOLUA_PROXY(ISignalNotifiable)
    // 特殊标记定义
    OOLUA_TAGS(
        Abstract  // 指定抽象标记，oolua 不会生成 new()，且会忽略 OOLUA_CTORS()
    )
    OOLUA_MFUNC(OnSignalChanged)

OOLUA_PROXY_END */
OOLUA_EXPORT_FUNCTIONS(ISignalNotifiable, OnSignalChanged)
OOLUA_EXPORT_FUNCTIONS_CONST(ISignalNotifiable)

/**
 * @brief 设备操作接口。
 * @details 该类作为设备所有编程接口的其类，继承于本类的各具体接口类，
 *  其信息的查询和设置都是通过 DNCP 协议栈直接操作物理设备。
 *  \p 各子类实现时，命令调用（底层）如果超时，需要抛出 TimeoutException 异常。
 */
/*
OOLUA_PROXY(DeviceInterface)
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
        OOLUA_CTOR(DscpAddress, int)
//        OOLUA_CTOR(DscpAddress, UpdatableInfo*)
    )
    OOLUA_MFUNC(GetAddress)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(DeviceInterface, GetAddress)
OOLUA_EXPORT_FUNCTIONS_CONST(DeviceInterface)

/**
 * @brief 设备信息接口。
 * @details 设备信息接口提供了设备基本信息查询的一序列操作，同时提供了生产信息的写入操作。
 */
/*OOLUA_PROXY(DeviceInfoInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
        OOLUA_CTOR(DscpAddress, ControllerInfo*)
    )
    OOLUA_MFUNC(GetType)
    OOLUA_MFUNC(SetType)
    OOLUA_MFUNC(GetModel)
    OOLUA_MFUNC(SetModel)
    OOLUA_MFUNC(GetSerialNumber)
    OOLUA_MFUNC(SetSerialNumber)
    OOLUA_MFUNC(GetManufacturer)
    OOLUA_MFUNC(SetManufacturer)
    OOLUA_MFUNC(GetManufDate)
    OOLUA_MFUNC(SetManufDate)
    OOLUA_MFUNC(GetSoftwareVersion)
    OOLUA_MFUNC(GetSoftwareLable)
    OOLUA_MFUNC(GetHardwareVersion)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(DeviceInfoInterface, GetType, SetType, GetModel, SetModel,
                       GetSerialNumber, SetSerialNumber, GetManufacturer, SetManufacturer,
                       GetManufDate, SetManufDate, GetSoftwareVersion, GetSoftwareLable, GetHardwareVersion)
OOLUA_EXPORT_FUNCTIONS_CONST(DeviceInfoInterface)

/**
 * @brief 设备状态接口。
 * @details 设备状态接口提供了设备实时状态的查询操作。
 */
/*OOLUA_PROXY(DeviceStatusInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(GetRunMode)
    OOLUA_MFUNC(Blink)
    OOLUA_MFUNC(GetPowerSupplyType)
    OOLUA_MFUNC(Initialize)
    OOLUA_MFUNC(FactoryDefault)
    OOLUA_MFUNC(Reset)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(DeviceStatusInterface, GetRunMode, Blink, GetPowerSupplyType,
                       Initialize, FactoryDefault, Reset, RegisterResetNotice)
OOLUA_EXPORT_FUNCTIONS_CONST(DeviceStatusInterface)

/**
 * @brief 设备升级接口。
 * @details 提供了设备程序更新的一序列操作，通过该接口，能为设备在线地更新程序。
 */
/*OOLUA_PROXY(DeviceUpdateInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(DeviceUpdateInterface)
OOLUA_EXPORT_FUNCTIONS_CONST(DeviceUpdateInterface)

/**
 * @brief Dscp 系统接口。
 * @details 提供 DSCP 系统命令的调用操作。
 */
/*OOLUA_PROXY(DscpSystemInterface, DeviceInterface)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(Echo)
    OOLUA_MFUNC(GetVersion)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(DscpSystemInterface, Echo, GetVersion)
OOLUA_EXPORT_FUNCTIONS_CONST(DscpSystemInterface)

/**
 * @brief LUIP 平台控制器。
 * @details 对下位机实现了DSCP协议的控制板卡进行了基本封装，以便复用。
 */
/*OOLUA_PROXY(BaseController, IEventReceivable)
    OOLUA_TAGS(
        No_default_constructor //没有默认构造函数时可以使用此标签
    )
    OOLUA_CTORS(
        OOLUA_CTOR(DscpAddress)
    )
    OOLUA_MFUNC(GetAddress)
    OOLUA_MFUNC(SetAddress)
    OOLUA_MFUNC(IsConnected)
    OOLUA_MFUNC(SetConnectStatus)
    OOLUA_MFUNC(GetConnectStatus)
    OOLUA_MEM_FUNC(maybe_null<lua_return<DscpSystemInterface*> >, GetIDscp)
    OOLUA_MEM_FUNC(maybe_null<lua_return<DeviceInfoInterface*> >, GetIDeviceInfo)
    OOLUA_MEM_FUNC(maybe_null<lua_return<DeviceStatusInterface*> >, GetIDeviceStatus)
    OOLUA_MEM_FUNC(maybe_null<lua_return<DeviceUpdateInterface*> >, GetIDeviceUpdate)

OOLUA_PROXY_END*/
OOLUA_EXPORT_FUNCTIONS(BaseController, GetAddress, GetAddress, SetAddress, IsConnected,
                       SetConnectStatus, GetConnectStatus, GetIDscp, GetIDeviceInfo,
                       GetIDeviceStatus, GetIDeviceUpdate)
OOLUA_EXPORT_FUNCTIONS_CONST(BaseController)

void ControllerProxy(Script *lua)
{
    lua->register_class<DscpAddress>();
    lua->register_class<Version>();
    lua->register_class<ManufDate>();
    lua->register_class<RunMode>();
    lua->register_class<PowerSupplyType>();
    lua->register_class<UpdatableInfo>();
    lua->register_class<ControllerInfo>();
    lua->register_class<DeviceInterface>();
    lua->register_class<DeviceInfoInterface>();
    lua->register_class<DeviceStatusInterface>();
    lua->register_class<DeviceUpdateInterface>();
    lua->register_class<DscpSystemInterface>();
    lua->register_class<IEventReceivable>();
    lua->register_class<BaseController>();
}
