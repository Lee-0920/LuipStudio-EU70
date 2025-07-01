/**
 * @file
 * @brief 阀映射图。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "NP66ValveMap.h"

namespace Controller
{
namespace API
{

NP66ValveMap::NP66ValveMap()
{

}

NP66ValveMap::NP66ValveMap(Uint32 data)
{
    m_map = data;
}

void NP66ValveMap::SetData(Uint32 data)
{
    m_map = data;
}

Uint32 NP66ValveMap::GetData()
{
    return m_map;
}

void NP66ValveMap::SetOn(int index)
{
    m_map |= 1 << index;
}

void NP66ValveMap::SetOff(int index)
{
    m_map &= ~(1 << index);
}

bool NP66ValveMap::IsOn(int index)
{
    return (m_map & (1 << index));
}

void NP66ValveMap::clear()
{
    m_map = 0;
}

}
}
