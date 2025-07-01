/**
 * @file
 * @brief LUIP 平台控制器。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "System/Exception.h"
#include "BaseController.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

BaseController::BaseController(DscpAddress addr)
    :IDeviceInfo(new DeviceInfoInterface(addr)),
    IDscp(new DscpSystemInterface(addr)),
    IDeviceStatus(new DeviceStatusInterface(addr)),
    IDeviceUpdate(new DeviceUpdateInterface(addr)),
    m_isConnected(false)
{
    m_address = addr;
}

BaseController::~BaseController()
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
bool BaseController::Init()
{
    return true;
}

/**
 * @brief 结束化 Controller 环境。
 */
bool BaseController::Uninit()
{
    return true;
}

DscpAddress BaseController::GetAddress()
{
    return m_address;
}

void BaseController::SetAddress(DscpAddress addr)
{
    m_address = addr;
    IDscp->SetAddress(addr);
    IDeviceInfo->SetAddress(addr);
}

bool BaseController::IsConnected()
{
    return IDscp->Echo();
}

void BaseController::SetConnectStatus(bool status)
{
    m_isConnected = status;
}

bool BaseController::GetConnectStatus()
{
    return m_isConnected;
}

void BaseController::Register(ISignalNotifiable *handle)
{
    (void)handle;
}

void BaseController::OnReceive(DscpEventPtr event)
{
    (void)event;
}

void BaseController::StopSignalUpload()
{

}

void BaseController::StartSignalUpload()
{

}

DscpSystemInterface *BaseController::GetIDscp()
{
    return IDscp;
}

DeviceInfoInterface *BaseController::GetIDeviceInfo()
{
    return IDeviceInfo;
}

DeviceStatusInterface *BaseController::GetIDeviceStatus()
{
    return IDeviceStatus;
}

DeviceUpdateInterface *BaseController::GetIDeviceUpdate()
{
    return IDeviceUpdate;
}
    
}
