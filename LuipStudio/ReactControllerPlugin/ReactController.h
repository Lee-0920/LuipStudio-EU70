/**
 * @file
 * @brief 光学信号采集器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLNET_OPTICALACQUIRER_H_)
#define CONTROLNET_OPTICALACQUIRER_H_

#include <QObject>
#include "ControllerPlugin/BaseController.h"
#include "API/TemperatureControlInterface.h"
#include "API/OpticalAcquireInterface.h"
#include "API/RCSolenoidValveInterface.h"
#include "LuipShare.h"


#define DATA_MAX_LENGTH 4095

using std::list;

namespace Controller
{

/**
 * @brief 反应堆控制器
 * @details
 */
class LUIP_SHARE ReactController : public QObject, public BaseController
{
    Q_OBJECT
public:
    ReactController(DscpAddress addr);
    virtual ~ReactController();
    bool Init();
    bool Uninit();

    float GetDigestTemperature() const;
    float GetEnvironmentTemperature() const;
    RCTemperature GetCurrentTemperature();
    float GetPeakTemperature(int startIndex, int endIndex, bool isExtra);

    double GetScanData(int index) const;
    double GetScanDataRef(int index) const;
    int GetScanLen(void) const;
    double GetData(void);
    double GetDataRef(void);
    double NDIRResultHandle(int startIndex, int endIndex, int validCnt, int step, int increment,int filterStep, int throwNum, bool isExtra) const;
    void Filter(const double *buf, double *fbuf, int length, int filterStep, int throwNum) const;
    double GetRefTemp(int index) const;
    double GetMeaTemp(int index) const;
    void ClearBuf();
    RCTemperatureControlInterface*  GetITemperatureControl();
    OpticalAcquireInterface* GetIOpticalAcquire();
    RCSolenoidValveInterface* GetISolenoidValve();
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
    OpticalAcquireInterface * const IOpticalAcquire;
    RCTemperatureControlInterface * const ITemperatureControl;
    RCSolenoidValveInterface * const ISolenoidValve;
private:
   list<ISignalNotifiable*> m_notifise;
   RCTemperature m_temperature;   
   float m_environmentTemp;
   int m_sendEnvTempCount;
   double m_scanData[DATA_MAX_LENGTH];
   int m_scanLen = -1;
   double m_scanDataRef[DATA_MAX_LENGTH];
   float m_MeaTemp[DATA_MAX_LENGTH];
   float m_RefTemp[DATA_MAX_LENGTH];
};

}

#endif  //CONTROLNET_OPTICALACQUIRER_H_

