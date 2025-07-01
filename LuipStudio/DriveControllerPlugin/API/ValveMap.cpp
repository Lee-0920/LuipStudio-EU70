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

ValveMap::ValveMap()
{

}

ValveMap::ValveMap(Uint32 data)
{
    m_map = data;
}

void ValveMap::SetData(Uint32 data)
{
    m_map = data;
}

Uint32 ValveMap::GetData()
{
    return m_map;
}

void ValveMap::SetOn(int index)
{
    m_map |= 1 << index;
}

void ValveMap::SetOff(int index)
{
    m_map &= ~(1 << index);
}

bool ValveMap::IsOn(int index)
{
    return (m_map & (1 << index));
}

void ValveMap::clear()
{
    m_map = 0;
}

}
}
