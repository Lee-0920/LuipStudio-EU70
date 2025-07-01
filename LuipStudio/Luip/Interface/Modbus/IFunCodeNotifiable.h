#if !defined(MODBUS_FUNCODENOTIFIABLE_H)
#define MODBUS_FUNCODENOTIFIABLE_H

namespace Interface
{
namespace Modbus
{

class IFunCodeNotifiable
{
public:
    IFunCodeNotifiable() {}
    virtual ~IFunCodeNotifiable() {}

    virtual void OnRegisterRead(int funCode, int regAddr, int regNum) = 0;
    virtual void OnRegisterWrited(int funCode, int regAddr, int regNum) = 0;
};

}
}

#endif  //MODBUS_FUNCODENOTIFIABLE_H
