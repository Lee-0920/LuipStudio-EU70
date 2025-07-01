/**
 * @file
 * @brief 版本号。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(CONTROLLER_API_VERSION_H)
#define CONTROLLER_API_VERSION_H

#include "../LuipShare.h"
#include "System/Types.h"

using System::String;
using System::Byte;

namespace Controller
{
namespace API
{

/**
 * @brief 版本号。
 * @details 
 */
class LUIP_SHARE Version
{
public:
    Version();
    Version(int major, int minor);
    Version(int major, int minor, int revision);
    Version(int major, int minor, int revision, int build);
    Version(String ver);
    Version(const void* raw, int len);
    void SetData(void* raw, int len);
    // 获取版本号对象的裸数据。
    void GetData(void* raw, int len);
    int GetMajor();
    void SetMajor(int major);
    int GetMinor();
    void SetMinor(int minor);
    int GetRevision();
    void SetRevision(int revision);
    int GetBuild();
    void SetBuild(int build);
    // 从字符串中解析版本号。
    bool Parse(String ver);

    // 转化为字符串。
    // 
    // 如果后两位 revision 和 build 为0，则不以显示，如：
    // 1.0 （实为 1.0.0.0）
    // 1.3 （实为 1.3.0.0）
    // 1.3.5 （实为 1.3.5.0）
    // 1.3.5.8 （实为 1.3.5.8）
    String ToString() const;
    String ToCompleteVersionString() const;

    bool operator ==(const Version &other);
    bool operator !=(const Version &other);

private:
    // 主版本号
    int m_major;
    // 次版本号
    int m_minor;
    // 修订版本号
    int m_revision;
    // 编译版本号
    int m_build;
};

}
}

#endif  //CONTROLLER_API_VERSION_H
