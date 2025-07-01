#ifndef ERCHASHU_THRIFT_IDISCONNECT_NOTIFIABLE_H_
#define ERCHASHU_THRIFT_IDISCONNECT_NOTIFIABLE_H_

namespace erchashu
{
namespace thrift
{

class IDisconnectNotifiable
{
public:
    IDisconnectNotifiable() {}
    virtual ~IDisconnectNotifiable() {}

    virtual void OnDisconnect() = 0;
};

}
}

#endif  //MODBUS_FUNCODENOTIFIABLE_H
