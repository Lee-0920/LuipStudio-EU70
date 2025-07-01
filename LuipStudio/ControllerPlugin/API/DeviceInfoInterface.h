/**
 * @file
 * @brief 设备信息接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_DEVICEINFOINTERFACE_H)
#define CONTROLLER_API_DEVICEINFOINTERFACE_H

#include "../LuipShare.h"
#include "System/Types.h"
#include "DeviceInterface.h"
#include "UpdatableInfo.h"
#include "ManufDate.h"
#include "Version.h"

namespace Controller
{
namespace API
{

/**
 * @brief 控制器（板卡）信息包装类。
 * @details DeviceInfoInterface 接口的内存映像类。
 */
class LUIP_SHARE ControllerInfo : public UpdatableInfo
{
public:
    String type;
    String model;
    String sn;
    Version softVersion;
    Version hardVersion;
};

/**
 * @brief 设备信息接口。
 * @details 设备信息接口提供了设备基本信息查询的一序列操作，同时提供了生产信息的写入操作。
 */
class LUIP_SHARE DeviceInfoInterface : public DeviceInterface
{
public:
    DeviceInfoInterface(DscpAddress addr);
    DeviceInfoInterface(DscpAddress addr, ControllerInfo* info);
    // 查询设备的类型名称。
    // 
    // 设备类型是区分不同类别的产品称号，上位机（施工平板、中控）将根据这个字段识别不同的设备。
    String GetType();
    // 设置设备的类型名称。
    // 
    // 通常情况下，本命令不对外公开，上位机（包括生产工具）不能随便更改设备类型。
    bool SetType(String type);
    // 查询设备的型号。
    // 
    // 设备型号包含了设备的类型名称和具体的型别，如某熔配型号为：RSDT8101。
    String GetModel();
    // 设置设备的型号。
    bool SetModel(String model);
    // 查询设备的序列号。
    // 
    // 设备序列号唯一标识了个体设备的整个生命周期，供产品追溯时使用。
    String GetSerialNumber();
    // 设置设备的序列号。
    bool SetSerialNumber(String sn);
    // 查询设备的生产厂商。
    // 
    // 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
    String GetManufacturer();
    // 设置设备的生产厂商。
    bool SetManufacturer(String model);
    // 查询设备的生产日期。
    ManufDate GetManufDate();
    // 设置设备的生产日期。
    bool SetManufDate(ManufDate date);
    // 查询软件版本号。
    // 
    // 软件版本号通常固化在软件代码中，可作后期运维管理用。
    Version GetSoftwareVersion();

    // 查询软件标识。
    //
    // 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
    String GetSoftwareLable();
    // 查询硬件版本号。
    // 
    // 硬件版本号通常分为PCB和BOM两级，如果硬件上不支持则设为0。其余字段为0。
    // 
    // 主版本号为 PCB 版本号，次版本号为 BOM 版本号；
    Version GetHardwareVersion();
};

}
}

#endif  //CONTROLLER_API_DEVICEINFOINTERFACE_H
