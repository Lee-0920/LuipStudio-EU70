/**
 * @file
 * @brief 阀映射图。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "ValveMap.h"

namespace Controller
{
namespace API
{

RCValveMap::RCValveMap()
{

}

RCValveMap::RCValveMap(Uint32 data)
{
    m_map = data;
}

void RCValveMap::SetData(Uint32 data)
{
    m_map = data;
}

Uint32 RCValveMap::GetData()
{
    return m_map;
}

void RCValveMap::SetOn(int index)
{
    m_map |= 1 << index;
}

void RCValveMap::SetOff(int index)
{
    m_map &= ~(1 << index);
}

bool RCValveMap::IsOn(int index)
{
    return (m_map & (1 << index));
}

void RCValveMap::clear()
{
    m_map = 0;
}

}
}
