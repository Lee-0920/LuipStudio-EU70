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
#include "API/OpticalAcquireInterface.h"
#include "LuipShare.h"

using std::list;

namespace Controller
{

/**
 * @brief 光学信号采集器。
 * @details
 */
class LUIP_SHARE OpticalAcquirer : public QObject, public BaseController
{
    Q_OBJECT
public:
    OpticalAcquirer(DscpAddress addr);
    virtual ~OpticalAcquirer();
    bool Init();
    bool Uninit();
    OpticalAcquireInterface* GetIOpticalAcquire();

    // ---------------------------------------
    // IEventReceivable 接口
    void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEventPtr event);

    void StartSignalUpload();
    void StopSignalUpload();
public:
    // 设备接口集
    OpticalAcquireInterface * const IOpticalAcquire;

private:
   list<ISignalNotifiable*> m_notifise;
};

}

#endif  //CONTROLNET_OPTICALACQUIRER_H_
