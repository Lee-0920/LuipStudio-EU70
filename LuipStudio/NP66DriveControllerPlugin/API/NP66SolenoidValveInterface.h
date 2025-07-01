/**
 * @file
 * @brief 电磁阀控制接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_SOLENOIDVALVEINTERFACE_H)
#define CONTROLLER_API_SOLENOIDVALVEINTERFACE_H

#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"
#include "NP66ValveMap.h"
#include "../LuipShare.h"

namespace Controller
{
namespace API
{    


/**
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 */
class LUIP_SHARE NP66SolenoidValveInterface : public DeviceInterface
{
public:
    NP66SolenoidValveInterface(DscpAddress addr);
    // 查询系统支持的总电磁阀数目。
    int GetTotalValves();
    // 查询当前开启的阀门映射图。
    NP66ValveMap GetValveMap();
    // 设置要开启的阀门映射图。
    bool SetValveMap(NP66ValveMap map);

};

}
}

#endif  //CONTROLNET_API_SOLENOIDVALVEINTERFACE_H
