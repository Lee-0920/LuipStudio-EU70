/**
 * @file
 * @brief 光学信号采集器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLNET_OUTPUTCONTROLLER_H_)
#define CONTROLNET_OUTPUTCONTROLLER_H_

#include <QObject>
#include "ControllerPlugin/BaseController.h"
#include "API/TemperatureControlInterface.h"
#include "API/IOControlInterface.h"
#include "LuipShare.h"


using std::list;

namespace Controller
{

/**
 * @brief 反应堆控制器
 * @details
 */
class LUIP_SHARE OutputController : public QObject, public BaseController
{
    Q_OBJECT
public:
    OutputController(DscpAddress addr);
    virtual ~OutputController();
    bool Init();
    bool Uninit();

    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    OCTemperature GetCurrentTemperature();

    OCTemperatureControlInterface*  GetITemperatureControl();   
    IOControlInterface*  GetIOutputControl();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);

    void StartSignalUpload();
    void StopSignalUpload();

signals:
  void  EnvTempToLuaSignal(float temp);

public slots:
  void  EnvTempToLuaSlot(float temp);

public:
    // 设备接口集   
    OCTemperatureControlInterface * const ITemperatureControl;
    IOControlInterface * const IOutputControl;
private:
   list<ISignalNotifiable*> m_notifise;
   OCTemperature m_temperature;
   float m_environmentTemp;
   int m_sendEnvTempCount;
};

}

#endif  //CONTROLNET_OPTICALACQUIRER_H_

