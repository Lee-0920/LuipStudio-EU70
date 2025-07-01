/**
 * @file
 * @brief 设备生产日期。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(CONTROLLER_API_MANUFDATE_H)
#define CONTROLLER_API_MANUFDATE_H

#include "../LuipShare.h"
#include "System/Types.h"

using System::String;
using System::Byte;

namespace Controller
{
namespace API
{

/**
 * @brief 设备生产日期。
 * @details 
 */
class LUIP_SHARE ManufDate
{
public:
    ManufDate();
    ManufDate(int year, int month, int day);
    ManufDate(Byte* raw);
    void SetData(Byte* raw);
    Byte* GetData();
    void SetYear(int year);
    int GetYear();
    void SetMonth(int month);
    int GetMonth();
    void SetDay(int day);
    int GetDay();
    // 返回格式为：YYYY-MM-DD 的字符串。
    String ToString();
private:
    short year;
    Byte month;
    Byte day;
    static Byte m_raw[4];
};

}
}

#endif  //CONTROLLER_API_MANUFDATE_H
