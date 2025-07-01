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
#include "API/OpticalAcquireInterface.h"

using std::list;

namespace Controller
{

/**
 * @brief 驱动控制器。
 * @details
 */
class LUIP_SHARE TN67DriveController : public QObject, public BaseController
{
    Q_OBJECT

public:
    TN67DriveController(DscpAddress addr);
    virtual ~TN67DriveController();
    bool Init();
    bool Uninit();

    PeristalticPumpInterface* GetIPeristalticPump();
    SolenoidValveInterface* GetISolenoidValve();
    OpticalMeterInterface* GetIOpticalMeter();
    TemperatureControlInterface*  GetITemperatureControl();
    OpticalAcquireInterface* GetIOpticalAcquire();

    float GetReportThermostatTemp(int index) const;
    float GetReportEnvironmentTemp() const;

    void ClearAllRemainEvent();
    void ClearThermostatRemainEvent();
    void ClearPumpRemainEvent();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);
    void StartSignalUpload();
    void StopSignalUpload();
    void SetBoxFanEnable(bool enable);
    bool ReInitInstrument();

public:
    // 设备接口集
    PeristalticPumpInterface * const IPeristalticPump;
    SolenoidValveInterface * const ISolenoidValve;
    OpticalMeterInterface * const IOpticalMeter;
    TemperatureControlInterface * const ITemperatureControl;
    OpticalAcquireInterface * const IOpticalAcquire;

signals:
  void  BoxFanControlSignal(float temp);
public slots:
  void  BoxFanControlSlot(float temp);

private:
    list<ISignalNotifiable*> m_notifise;

    bool m_isEnable;
    bool m_isBoxFanRunning;
    float m_insideEnvironmentTemp;
    bool m_weepingDetectEnable;

    float m_environmentTemp;
    float m_thermostatTempArray[2];
};

}

#endif  //CONTROLLER_DRIVECONTROLNET_H_

