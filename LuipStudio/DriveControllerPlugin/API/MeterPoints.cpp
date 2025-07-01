/**
 * @file
 * @brief 设备生产日期。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "MeterPoints.h"
#include <QDebug>

#define PRECISE 0.000001

namespace Controller
{
namespace API
{

MeterPoints::MeterPoints()
{
    m_totalNum = 0;
}

MeterPoints::MeterPoints(unsigned int num)
{
    m_totalNum = num;

    for (int i =0; i < m_totalNum; i++)
    {
        MeterPoint point={0,0};
        m_points.push_back(point);
    }
}

void MeterPoints::SetNum(unsigned int num)
{
    m_totalNum = num;
}

int MeterPoints::GetNum()
{
    return m_totalNum;
}

void MeterPoints::SetPoint(unsigned int index, MeterPoint point)
{
    if (m_points.empty() || index > m_points.size())
    {
        m_points.push_back(point);
    }
    else
    {
        m_points[index-1].setVolume = point.setVolume;
        m_points[index-1].realVolume = point.realVolume;
    }
}

MeterPoint MeterPoints::GetPoint(unsigned int index)
{
    return  m_points.at(index-1);
}

bool MeterPoints::operator== (const MeterPoints &meterPoints) const
{
    bool ret = true;
    if(this->m_totalNum == meterPoints.m_totalNum)
    {
        for(int i = 0; i < this->m_totalNum; i++)
        {
            if (qAbs(this->m_points[i].setVolume - meterPoints.m_points[i].setVolume) > PRECISE)
            {
                ret = false;
                break;
            }
            if (qAbs(this->m_points[i].realVolume - meterPoints.m_points[i].realVolume) > PRECISE)
            {
                ret = false;
                break;
            }
        }
    }
    else
    {
        ret = false;
    }
    return ret;
}

}
}
