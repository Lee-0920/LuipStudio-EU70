#if !defined(CONTROLNET_API_INSTRUMENTINFOINTERFACE_H)
#define CONTROLNET_API_INSTRUMENTINFOINTERFACE_H

#include "../LuipShare.h"
#include "DeviceInterface.h"
#include "Communication/eCek/Common/Types.h"
#include "ManufDate.h"
#include "Version.h"
#include <QUuid>

namespace Controller
{
namespace API
{
/**
 * @brief 仪器信息接口
 * @details 用于查询主控板所在的仪器信息
 */
class LUIP_SHARE InstrumentInfoInterface: public DeviceInterface
{
public:
    InstrumentInfoInterface(DscpAddress addr);
    //查询仪器的类型名称
    String GetType(void);
    //设置仪器的类型名称
    bool SetType(String type);
    //查询仪器的型号
    String GetModel(void);
    //设置仪器的型号
    bool SetModel(String model);
    //查询仪器的序列号
    String GetSn(void);
    //设置仪器的序列号
    bool SetSn(String sn);
    //查询仪器的UUID
    QUuid GetUUID(void);
    //设置仪器的UUID
    bool SetUUID(QUuid id);
    //查询仪器的生产厂商
    String GetManuFacturer(void);
    //设置仪器的生产厂商
    bool SetManuFacturer(String manufacter);
    // 查询设备的生产日期。
    ManufDate GetManufDate(void);
    // 设置设备的生产日期。
    bool SetManufDate(ManufDate date);
    //查询仪器的系统时间
    Uint64 GetSystemTime(void);
    //设置仪器的系统时间
    bool SetSystemTime(Uint64 utcTime);
    // 软件版本号通常固化在软件代码中，可作后期运维管理用。
    Version GetSoftwareVersion(void);
    // 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
    String GetSoftwareLable(void);
    //查询平台软件版本号
    Version GetPlatFormVersion(void);
    //查询平台软件标识
    String GetPlatFormLable(void);
    //查询仪器的名称
    String GetInstrumentName(void);
    //查询仪器的测量参数
    String GetMeasureParameter(void);
    //查询仪器的测量参数编码
    String GetMeasureParameterCode(void);
};

}
}

#endif // CONTROLNET_API_INSTRUMENTINFOINTERFACE_H
