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
#include "API/TitrationOperateInterface.h"
//#include "API/OpticalAcquireInterface.h"

using std::list;

namespace Controller
{

/**
 * @brief 驱动控制器。
 * @details
 */
class LUIP_SHARE CM66DriveController : public QObject, public BaseController
{
    Q_OBJECT

public:
    CM66DriveController(DscpAddress addr);
    virtual ~CM66DriveController();
    bool Init();
    bool Uninit();

    PeristalticPumpInterface* GetIPeristalticPump();
    SolenoidValveInterface* GetISolenoidValve();
    OpticalMeterInterface* GetIOpticalMeter();
    TemperatureControlInterface*  GetITemperatureControl();
    TitrationOperateInterface* GetITitrationOperate();

    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    Temperature GetCurrentTemperature();

    void ClearAllRemainEvent();
    void ClearThermostatRemainEvent();
    TitrationData GetPacketTitrationData(long timeout);
    //TitrationResult StartTitration(TitrationParameter param);
    //void StopTitrationOperate();
    bool ReInitInstrument();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);
    void StartSignalUpload();
    void StopSignalUpload();

signals:
  void  TempToLuaSignal(float tTemp, float eTemp);

public slots:
  void  TempToLuaSlot(float tTemp, float eTemp);

public:
    // 设备接口集
    PeristalticPumpInterface * const IPeristalticPump;
    SolenoidValveInterface * const ISolenoidValve;
    OpticalMeterInterface * const IOpticalMeter;
    TemperatureControlInterface * const ITemperatureControl;
    TitrationOperateInterface * const ITitrationoperate;
    //OpticalAcquireInterface * const IOpticalAcquire;

private:
    list<ISignalNotifiable*> m_notifise;
    Temperature m_temperature;
    Temperature m_extraTemperature;
    float m_environmentTemp;
    float m_thermostatTemp;
    int m_sendTempCount;
};

}

#endif  //CONTROLLER_DRIVECONTROLNET_H_

