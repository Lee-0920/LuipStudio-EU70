/**
 * @file
 * @brief LUIP 平台控制器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(CONTROLNET_CONTROLNET_H_)
#define CONTROLNET_CONTROLNET_H_

#include "Communication/IEventReceivable.h"
#include "API/DeviceInfoInterface.h"
#include "API/DscpSystemInterface.h"
#include "API/DeviceStatusInterface.h"
#include "API/DeviceUpdateInterface.h"
#include "ISignalNotifiable.h"
#include "LuipShare.h"

using Communication::IEventReceivable;
using Communication::Dscp::DscpAddress;
using Communication::Dscp::DscpEvent;

using namespace Controller::API;

namespace Controller
{

/**
 * @brief LUIP 平台控制器。
 * @details 对下位机实现了DSCP协议的控制板卡进行了基本封装，以便复用。
 */
class LUIP_SHARE Controller : public IEventReceivable
{
public:
    Controller(DscpAddress addr);
    virtual ~Controller();
    virtual bool Init();
    virtual bool Uninit();
    DscpAddress GetAddress();
    void SetAddress(DscpAddress addr);
    bool IsConnected();
    void SetConnectStatus(bool status);
    bool GetConnectStatus();
    DscpSystemInterface* GetIDscp();
    DeviceInfoInterface* GetIDeviceInfo();
    DeviceStatusInterface* GetIDeviceStatus();
    DeviceUpdateInterface* GetIDeviceUpdate();

    // ---------------------------------------
    // IEventReceivable 接口
    virtual void Register(ISignalNotifiable *handle);
    virtual void OnReceive(DscpEvent event);

public:
    // 设备接口集
    DeviceInfoInterface * const IDeviceInfo;
    DscpSystemInterface * const IDscp;
    DeviceStatusInterface * const IDeviceStatus;
    DeviceUpdateInterface * const IDeviceUpdate;

protected:
    DscpAddress m_address;

private:
    bool m_isConnected;
};

}

#endif  //CONTROLNET_CONTROLNET_H_
