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
#include "API/NP66PeristalticPumpInterface.h"
#include "API/NP66SolenoidValveInterface.h"
#include "API/NP66OpticalMeterInterface.h"
#include "API/NP66TemperatureControlInterface.h"
#include "API/NP66OpticalAcquireInterface.h"

using std::list;

namespace Controller
{

/**
 * @brief 驱动控制器。
 * @details
 */
class LUIP_SHARE NP66DriveController : public QObject, public BaseController
{
    Q_OBJECT

public:
    NP66DriveController(DscpAddress addr);
    virtual ~NP66DriveController();
    bool Init();
    bool Uninit();

    NP66PeristalticPumpInterface* GetIPeristalticPump();
    NP66SolenoidValveInterface* GetISolenoidValve();
    NP66OpticalMeterInterface* GetIOpticalMeter();
    NP66TemperatureControlInterface*  GetITemperatureControl();
    NP66OpticalAcquireInterface* GetIOpticalAcquire();

    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    NP66Temperature GetCurrentTemperature();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);
    void StartSignalUpload();
    void StopSignalUpload();
    void SetBoxFanEnable(bool enable);

public:
    // 设备接口集
    NP66PeristalticPumpInterface * const IPeristalticPump;
    NP66SolenoidValveInterface * const ISolenoidValve;
    NP66OpticalMeterInterface * const IOpticalMeter;
    NP66TemperatureControlInterface * const ITemperatureControl;
    NP66OpticalAcquireInterface * const IOpticalAcquire;

signals:
  void  BoxFanControlSignal(float temp);
public slots:
  void  BoxFanControlSlot(float temp);

private:
    list<ISignalNotifiable*> m_notifise;
    NP66Temperature m_temperature;
    NP66Temperature m_extraTemperature;

    bool m_isEnable;
    bool m_isBoxFanRunning;
    float m_insideEnvironmentTemp;

};

}

#endif  //CONTROLLER_DRIVECONTROLNET_H_

