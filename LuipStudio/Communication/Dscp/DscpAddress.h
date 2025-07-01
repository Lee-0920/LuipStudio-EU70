/**
 * @file
 * @brief 设备网络地址。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(COMMUNICATION_DSCP_DSCPADDRESS_H)
#define COMMUNICATION_DSCP_DSCPADDRESS_H

#include "../LuipShare.h"
#include "System/Types.h"

using System::Byte;
using System::Uint16;
using System::String;

namespace Communication
{
namespace Dscp
{

/**
 * @brief 设备网络地址。
 * @details DSCP设备地址，对应网络层地址。
 */
class LUIP_SHARE DscpAddress
{
public:
    Byte a1;        ///< 第1层地址
    Byte a2;        ///< 第2层地址
    Byte a3;        ///< 第3层地址
    Byte a4;        ///< 第4层地址

    DscpAddress()
    {
        this->a1 = 0;
        this->a2 = 0;
        this->a3 = 0;
        this->a4 = 0;
    }

    DscpAddress(Byte a1, Byte a2)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->a3 = 0;
        this->a4 = 0;
    }

    DscpAddress(Byte a1, Byte a2, Byte a3)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->a3 = a3;
        this->a4 = 0;
    }

    DscpAddress(Byte a1, Byte a2, Byte a3, Byte a4)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->a3 = a3;
        this->a4 = a4;
    }

    void Make(Byte a1, Byte a2, Byte a3, Byte a4)
    {
        this->a1 = a1;
        this->a2 = a2;
        this->a3 = a3;
        this->a4 = a4;
    }

    friend bool operator==(const DscpAddress& addr1, const DscpAddress& addr2)
    {
        return ((addr1.a1 == addr2.a1) &&
                (addr1.a2 == addr2.a2) &&
                (addr1.a3 == addr2.a3) &&
                (addr1.a4 == addr2.a4));
    }

    String ToString()
    {
        String str;

        if (a4 != 0)
            str = std::to_string(a1) + "." +
                  std::to_string(a2) + "." +
                  std::to_string(a3) + "." +
                  std::to_string(a4);
        else if (a3 != 0)
            str = std::to_string(a1) + "." +
                  std::to_string(a2) + "." +
                  std::to_string(a3);
        else if (a2 != 0)
            str = std::to_string(a1) + "." +
                  std::to_string(a2);
        else
            str = std::to_string(a1);

        return str;
    }
};

}
}

#endif  //CONTROLNET_COMMUNICATION_DSCP_DSCPADDRESS_H
