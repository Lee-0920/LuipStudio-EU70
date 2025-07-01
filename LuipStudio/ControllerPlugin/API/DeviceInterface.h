/**
 * @file
 * @brief 设备操作接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_DEVICEINTERFACE_H)
#define CONTROLLER_API_DEVICEINTERFACE_H

#include "../LuipShare.h"
#include "Communication/Dscp/DscpPacket.h"
#include "Communication/EventHandler.h"
#include "System/Types.h"

using System::String;
using Communication::Dscp::DscpAddress;
using Communication::Dscp::DscpEventPtr;

namespace Controller
{
namespace API
{

class UpdatableInfo;

/**
 * @brief 设备操作接口。
 * @details 该类作为设备所有编程接口的其类，继承于本类的各具体接口类，
 *  其信息的查询和设置都是通过 DNCP 协议栈直接操作物理设备。
 *  \p 各子类实现时，命令调用（底层）如果超时，需要抛出 TimeoutException 异常。
 */
class LUIP_SHARE DeviceInterface
{
public:
    DeviceInterface();
    DeviceInterface(DscpAddress addr);
    DeviceInterface(DscpAddress addr,int retries);
    DeviceInterface(DscpAddress addr, UpdatableInfo* info);
    ~DeviceInterface();

    DscpAddress GetAddress();
    void SetAddress(DscpAddress addr);
    int GetTimeout();
    void SetTimeout(int ms);
    int GetRetries();
    void SetRetries(int retries);
    void SetUpdatableInfo(UpdatableInfo* info);
    DscpEventPtr Expect(int code, int timeout);

protected:
    // 接口命令发往的设备地址。
    DscpAddress m_addr;
    // 同步调用的超时时间。
    int m_timeout;
    // 超时/错误重试次数。
    int m_retries;
    UpdatableInfo* m_info;

};

}
}

#endif  //CONTROLLER_API_DEVICEINTERFACE_H
