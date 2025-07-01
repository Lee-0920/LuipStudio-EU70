#if !defined(CONTROLLER_API_ELECTRODEACQUIREINTERFACE_H)
#define CONTROLLER_API_ELECTRODEACQUIREINTERFACE_H

#include "../LuipShare.h"
#include "Communication/IEventReceivable.h"
#include "ControllerPlugin/API/DeviceInterface.h"
#include "System/Types.h"

using System::Uint16;
using System::Uint32;
using Communication::IEventReceivable;

namespace Controller
{
namespace API
{

/**
 * @brief 电极扫描结果。
 */
enum ElectrodeScanResult
{
    Finished = 0,       ///< 电极扫描操作正常完成。
    Failed = 1,         ///< 电极扫描操作失败。
};

class LUIP_SHARE ElectrodeAcquireInterface : public DeviceInterface
{
public:
    ElectrodeAcquireInterface(DscpAddress addr);

    // 设置采集电极信号AD上报周期。
    bool SetAcquireElectrodeADNotifyPeriod(float period);

    // 停止电极采集过程。
    bool StopElectrodeADAcquirer();

    // 注册电极信号AD定时上报事件。
    void RegisterAcquireElectrodeADNotice(IEventReceivable *handle);

    // 设置DAC输出电位
    bool SetDACPotential(float potential);

    // 获取DAC输出电位
    float GetDACPotential();

    // 关闭DAC输出
    bool TurnOffDAC();

    // 设置ADC灵敏度
    bool SetADCSensitivity(Uint8 sensitivity);

    // 获取ADC灵敏度
    Uint8 GetADCSensitivity();

    // 获取电极扫描分辨率
    float GetElectrodeScanResolution();

    // 启动电极扫描
    bool StartElectrodeScan(float startPotential, float stopPotential, float scanRate);

    // 查询电极扫描状态
    Uint16 GetScanStatus();

    // 获取电极扫描数据 //不对lua开放
    bool GetElectrodeScanData(float* current, Uint16 pos, Uint16 length);

    // 电极扫描完成事件
    ElectrodeScanResult ExpectElectrodeScan(long timeout);

};

}

}
#endif // CONTROLLER_API_ELECTRODEACQUIREINTERFACE_H
