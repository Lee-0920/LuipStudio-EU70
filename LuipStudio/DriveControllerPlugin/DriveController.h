/**
 * @file
 * @brief 驱动控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLLER_DRIVECONTROLNET_H_)
#define CONTROLLER_DRIVECONTROLNET_H_

#include <vector>
#include <QObject>
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "API/PeristalticPumpInterface.h"
#include "API/SolenoidValveInterface.h"
#include "API/OpticalMeterInterface.h"
#include "API/TemperatureControlInterface.h"
#include "API/ExtraTemperatureControlInterface.h"
#include "API/ElectrodeAcquireInterface.h"
#include "CurveData.h"

using std::list;

namespace Controller
{

/**
 * @brief 驱动控制器。
 * @details
 */
class LUIP_SHARE DriveController : public QObject, public BaseController
{
    Q_OBJECT

public:
    DriveController(DscpAddress addr);
    virtual ~DriveController();
    bool Init();
    bool Uninit();

    PeristalticPumpInterface* GetIPeristalticPump();
    SolenoidValveInterface* GetISolenoidValve();
    OpticalMeterInterface* GetIOpticalMeter();
    TemperatureControlInterface*  GetITemperatureControl();
    ExtraTemperatureControlInterface*  GetIExtraTemperatureControl();
    ElectrodeAcquireInterface* GetIElectrodeAcquire();

    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    Temperature GetCurrentTemperature();

    float GetMeasureCellTemperature() const;
    Temperature GetCurrentExtraTemperature();

    void MedianFilter(const float *buf, float *fbuf, int length);
    CurveData* GetCurveData(float beginVoltage, float endVoltage, float resolution);

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);
    void StartSignalUpload();
    void StopSignalUpload();

public:
    // 设备接口集
    PeristalticPumpInterface * const IPeristalticPump;
    SolenoidValveInterface * const ISolenoidValve;
    OpticalMeterInterface * const IOpticalMeter;
    TemperatureControlInterface * const ITemperatureControl;
    ExtraTemperatureControlInterface * const IExtraTemperatureControl;
    ElectrodeAcquireInterface * const IElectrodeAcquire;

private:
    list<ISignalNotifiable*> m_notifise;
    Temperature m_temperature;
    Temperature m_extraTemperature;

};

}

#endif  //CONTROLLER_DRIVECONTROLNET_H_

