/**
 * @file
 * @brief 液路控制器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLLER_LIQUIDCONTROLNET_H_)
#define CONTROLLER_LIQUIDCONTROLNET_H_

#include <vector>
#include <QObject>
#include "LuipShare.h"
#include "ControllerPlugin/BaseController.h"
#include "API/PeristalticPumpInterface.h"
#include "API/SolenoidValveInterface.h"
#include "API/OpticalMeterInterface.h"
#include "API/LCSolenoidValveInterface.h"
#include "API/LCPeristalticPumpInterface.h"
#include "API/LCTemperatureControlInterface.h"
using std::list;

namespace Controller
{

/**
 * @brief 液路控制器。
 * @details
 */
class LUIP_SHARE LiquidController : public QObject, public BaseController
{
    Q_OBJECT

public:
    LiquidController(DscpAddress addr);
    virtual ~LiquidController();
    bool Init();
    bool Uninit();

    LCPeristalticPumpInterface* GetIPeristalticPump();
    OpticalMeterInterface* GetIOpticalMeter();
    LCSolenoidValveInterface* GetISolenoidValve();
    LCTemperatureControlInterface* GetITemperatureControl();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);

    void StartSignalUpload();
    void StopSignalUpload();
public:
    // 设备接口集
    LCPeristalticPumpInterface * const IPeristalticPump;
    LCSolenoidValveInterface * const ISolenoidValve;
    OpticalMeterInterface * const IOpticalMeter;
    LCTemperatureControlInterface * const ITemperatureControl;
private:
    list<ISignalNotifiable*> m_notifise;

};

}

#endif  //CONTROLLER_LIQUIDCONTROLNET_H_
