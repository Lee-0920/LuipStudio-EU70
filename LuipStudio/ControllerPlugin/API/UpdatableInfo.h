/**
 * @file
 * @brief 可更新的信息。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_UPDATABLEINFO_H)
#define CONTROLLER_API_UPDATABLEINFO_H

#include "../LuipShare.h"

namespace Controller
{
namespace API
{

/**
 * @brief 可更新的信息。
 * @details 可更新信息保存在控制器对象的内存中，客户代码调用具体接口设置信息时，
 *  如果设置成功，还需要把设置的信息更新到控制器内存中。
 */
class LUIP_SHARE UpdatableInfo
{
public:
    // 更新时间标记。
    void Update();
    // 查询信息是否已更新。
    bool IsUpdated();
    // 获取更新时间。
    long GetUpdateTime();
protected:
    // 信息更新时间。未更新为0，更新之后更新时刻的毫秒数。
    long m_updateTime;
};

}
}

#endif  //CONTROLLER_API_UPDATABLEINFO_H
