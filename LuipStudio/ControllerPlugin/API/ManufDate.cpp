/**
 * @file
 * @brief 设备生产日期。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "ManufDate.h"

namespace Controller
{
namespace API
{

Byte ManufDate::m_raw[4];

ManufDate::ManufDate()
{
    this->year = 1900;
    this->month = 1;
    this->day = 1;
}

ManufDate::ManufDate(int year, int month, int day)
{
    this->year = year;
    this->month = month;
    this->day = day;
}

ManufDate::ManufDate(Byte* raw)
{
    memcpy(&this->year, raw, 2);
    this->month = raw[2];
    this->day = raw[3];
}

void ManufDate::SetData(Byte* raw)
{
    memcpy(&this->year, raw, 2);
    this->month = raw[2];
    this->day = raw[3];
}

Byte* ManufDate::GetData()
{
    memcpy(m_raw, &this->year, 2);
    m_raw[2] = this->month;
    m_raw[3] = this->day;
    return m_raw;
}

void ManufDate::SetYear(int year)
{
    this->year = year;
}

int ManufDate::GetYear()
{
    return this->year;
}

void ManufDate::SetMonth(int month)
{
    this->month = month;
}

int ManufDate::GetMonth()
{
    return this->month;
}

void ManufDate::SetDay(int day)
{
    this->day = day;
}

int ManufDate::GetDay()
{
    return this->day;
}

String ManufDate::ToString()
{
    return (std::to_string(this->year) + "-" +
            std::to_string(this->month) + "-" +
            std::to_string(this->day));
}

}
}
