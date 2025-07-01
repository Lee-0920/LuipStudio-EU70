/**
 * @file
 * @brief 阀映射图。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(LC_CONTROLLER_API_VALVEMAP_H)
#define LC_CONTROLLER_API_VALVEMAP_H

#include "System/Types.h"
#include "../LuipShare.h"

using System::Uint32;

namespace Controller
{
namespace API
{

/**
 * @brief 阀映射图。
 * @details 
 */
class LUIP_SHARE LCValveMap
{
public:
    LCValveMap();
    LCValveMap(Uint32 data);
    void SetData(Uint32 data);
    Uint32 GetData();
    void SetOn(int index);
    void SetOff(int index);
    bool IsOn(int index);
    void clear();

private:
    Uint32 m_map;

};

}
}

#endif  //CONTROLNET_API_VALVEMAP_H
