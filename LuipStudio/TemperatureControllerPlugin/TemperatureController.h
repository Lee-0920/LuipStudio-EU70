/**
 * @file
 * @brief 温度控制器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLLER_TEMPERATURECONTROLNET_H_)
#define CONTROLLER_TEMPERATURECONTROLNET_H_

#include <list>
#include <QObject>
#include "ControllerPlugin/BaseController.h"
#include "API/TemperatureControlInterface.h"
#include "LuipShare.h"

using std::list;

namespace Controller
{

/**
 * @brief 温度控制器。
 * @details
 */
class LUIP_SHARE TemperatureController : public QObject, public BaseController
{
        Q_OBJECT
public:
    TemperatureController(DscpAddress addr);
    virtual ~TemperatureController();
    bool Init();
    bool Uninit();
    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    Temperature GetCurrentTemperature();
    TemperatureControlInterface*  GetITemperatureControl();

     // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);

    void StartSignalUpload();
    void StopSignalUpload();
public:
    // 设备接口集
    TemperatureControlInterface * const ITemperatureControl;

private:
   list<ISignalNotifiable* > m_notifise;
   Temperature m_temperature;
};

}

#endif  //CONTROLNET_TEMPERATURECONTROLNET_H_
