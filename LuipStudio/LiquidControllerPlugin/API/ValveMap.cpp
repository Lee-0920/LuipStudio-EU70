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

LCValveMap::LCValveMap()
{

}

LCValveMap::LCValveMap(Uint32 data)
{
    m_map = data;
}

void LCValveMap::SetData(Uint32 data)
{
    m_map = data;
}

Uint32 LCValveMap::GetData()
{
    return m_map;
}

void LCValveMap::SetOn(int index)
{
    m_map |= 1 << index;
}

void LCValveMap::SetOff(int index)
{
    m_map &= ~(1 << index);
}

bool LCValveMap::IsOn(int index)
{
    return (m_map & (1 << index));
}

void LCValveMap::clear()
{
    m_map = 0;
}

}
}
