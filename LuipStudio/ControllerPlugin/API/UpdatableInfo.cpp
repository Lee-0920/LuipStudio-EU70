/**
 * @file
 * @brief 可更新的信息。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "UpdatableInfo.h"

namespace Controller
{
namespace API
{

void UpdatableInfo::Update()
{

}

bool UpdatableInfo::IsUpdated()
{
    return true;
}

long UpdatableInfo::GetUpdateTime()
{
    return m_updateTime;
}

}
}
