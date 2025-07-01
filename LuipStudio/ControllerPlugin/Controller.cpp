/**
 * @file
 * @brief LUIP 平台控制器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "System/Exception.h"
#include "Controller.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

Controller::Controller(DscpAddress addr)
    :IDeviceInfo(new DeviceInfoInterface(addr)),
    IDscp(new DscpSystemInterface(addr)),
    IDeviceStatus(new DeviceStatusInterface(addr)),
    IDeviceUpdate(new DeviceUpdateInterface(addr)),
    m_isConnected(false)
{
    m_address = addr;
}

Controller::~Controller()
{
    if (IDscp)
        delete IDscp;
    if (IDeviceInfo)
        delete IDeviceInfo;
    if (IDeviceStatus)
        delete IDeviceStatus;
    if (IDeviceUpdate)
        delete IDeviceUpdate;
}


/**
 * @brief 初始化 Controller 环境。
 */
bool Controller::Init()
{
    return true;
}

/**
 * @brief 结束化 Controller 环境。
 */
bool Controller::Uninit()
{
    return true;
}

DscpAddress Controller::GetAddress()
{
    return m_address;
}

void Controller::SetAddress(DscpAddress addr)
{
    m_address = addr;
    IDscp->SetAddress(addr);
    IDeviceInfo->SetAddress(addr);
}

bool Controller::IsConnected()
{
    return IDscp->Echo();
}

void Controller::SetConnectStatus(bool status)
{
    m_isConnected = status;
}

bool Controller::GetConnectStatus()
{
    return m_isConnected;
}

void Controller::Register(ISignalNotifiable *handle)
{
    (void)handle;
}

void Controller::OnReceive(DscpEvent event)
{
    (void)event;
}

DscpSystemInterface *Controller::GetIDscp()
{
    return IDscp;
}

DeviceInfoInterface *Controller::GetIDeviceInfo()
{
    return IDeviceInfo;
}

DeviceStatusInterface *Controller::GetIDeviceStatus()
{
    return IDeviceStatus;
}

DeviceUpdateInterface *Controller::GetIDeviceUpdate()
{
    return IDeviceUpdate;
}
    
}
