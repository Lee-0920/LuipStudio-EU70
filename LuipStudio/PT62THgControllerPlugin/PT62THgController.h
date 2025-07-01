/**
 * @file
 * @brief 驱动控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLLER_PT62THGCONTROLNET_H_)
#define CONTROLLER_PT62THGCONTROLNET_H_

#include <vector>
#include <map>
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
 * @brief 算法。
 * @details
 */
enum class Algorithms
{
  Peak = 0,
  Average = 1,
  HalfArea = 2,
  AllArea = 3,
};

/**
 * @brief 结果类型。
 * @details
 */
enum class OpticalResultType
{
  Finished = 0,
  Failed = 1,
  Stopped = 2,
};

/**
 * @brief 光学信号采集处理结果。
 * @details
 */
struct OpticalResult
{
    int type;                        // 采集数据的结果类型
    Uint32 reference;                ///参考AD值处理的结果
    Uint32 measure;                  ///测量AD值的处理结果
};

/**
 * @brief 驱动控制器。
 * @details
 */
class LUIP_SHARE PT62THgController : public QObject, public BaseController
{
    Q_OBJECT

public:
    PT62THgController(DscpAddress addr);
    virtual ~PT62THgController();
    bool Init();
    bool Uninit();

    PeristalticPumpInterface* GetIPeristalticPump();
    SolenoidValveInterface* GetISolenoidValve();
    OpticalMeterInterface* GetIOpticalMeter();
    TemperatureControlInterface*  GetITemperatureControl();
    OpticalAcquireInterface* GetIOpticalAcquire();

    float GetEnvironmentSignalTemp() const;
    float GetThermostatSignalTemp(Uint8 index) const;

    float GetCurrEnvironmentTemp();
    float GetCurrThermostatTemp(Uint8 index);

    void ClearThermostatRemainEvent();

    OpticalResult StartAcquireAD(float samplePeriod, int acqTimes, Algorithms algorithms = Algorithms::Peak);
    void StopAcquireAD();
    void SetInitAD(Uint32 refAD, Uint32 meaAD);

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
    OpticalAcquireInterface * const IOpticalAcquire;
    TemperatureControlInterface * const ITemperatureControl;

signals:
  void  EnvTempToLuaSignal(float temp);
public slots:
  void  EnvTempToLuaSlot(float temp);

private:
    list<ISignalNotifiable*> m_notifise;
    vector<float> m_ThermostatTemps;
    float m_environmentTemp;
    int m_sendEnvTempCount;

    bool ReInitInstrument();
    void SlideFilter(std::vector<float> &ads);
    float m_intAbs;
    bool m_isStopAcquireAD;
    bool m_isOnesAcquireFinish;

};

}

#endif  //CONTROLLER_PT62THGCONTROLNET_H_

