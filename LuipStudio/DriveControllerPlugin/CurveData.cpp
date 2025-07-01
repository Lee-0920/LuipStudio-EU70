/**
 * @file
 * @brief 曲线数据
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include "CurveData.h"
#include "vector"
#include <QDebug>
#include "Log.h"
using namespace std;

namespace Controller
{

/**
 * @brief 驱动控制器构造。
 */
CurveData::CurveData(float beginVoltage, float endVoltage, int length):
    m_index(0),
    m_length(length),
    m_begin(beginVoltage),
    m_end(endVoltage)
{
    if (m_length > 0)
    {
        m_data.reset(new float[length], std::default_delete<float[]>());
        memset(m_data.get(), 0, sizeof(float)*length);
    }
}

float* CurveData::GetWholeData(void)
{
    return m_data.get();
}


bool CurveData::AddData(float* buffer, int length)
{
    if(m_length - m_index >= length)
    {
        memcpy(&m_data.get()[m_index], buffer, sizeof(float)*length);
        m_index += length;
        return true;
    }
    else
    {
        return false;
    }
}

float CurveData::GetData(int index)const
{
    return m_data.get()[index];
}

void CurveData::SetData(int index, float data)
{
    if(index < 0 || index >= m_length)
    {
        return;
    }
    this->m_data.get()[index] = data;
}

int CurveData::GetLength()const
{
    return this->m_length;
}

int CurveData::GetSize()const
{
    return this->m_length * sizeof(float);
}

float CurveData::GetBegin()const
{

    return this->m_begin;
}

float CurveData::GetEnd()const
{
    return this->m_end;
}

/**
 * @brief 自适应峰面积计算法
 * @return 电流峰面积，double。
 */
float CurveData::AutoCalculateArea(float beginVoltage, float endVoltage, float peakWidth, float peakLeftLimit, float peakRightLimit, float valleyRightLimit)
{
    logger->debug("Auto area method param beginV:%f, endV:%f, peakWidth:%f, peakLimit:[%f,%f] valleyRightLimit:%f", beginVoltage, endVoltage, peakWidth, peakLeftLimit, peakRightLimit, valleyRightLimit);

    float area = 0;
    int diffNum = 3;
    const int length = m_length;
    float step = (endVoltage - beginVoltage) / m_length;
    float windowFrom = peakLeftLimit;
    float windowTo = peakRightLimit;
    QVector<float> xVoltage;
    QVector<float> yCurrent;
    QVector<float> rangeData;
    QVector<float> rangeLeftDiff;
    QVector<float> rangeRightDiff;
    QVector<float> valleyData;
    QVector<float> valleyLeftDiff;
    QVector<float> valleyRightDiff;

    if(length <= 0)
    {
        return area;
    }

    for(int i = 0; i < length; i++)
    {
        xVoltage.append(beginVoltage + i*(endVoltage - beginVoltage)/(length - 1));
        yCurrent.append(m_data.get()[i]);
    }

    // 计算起点值
    int nStartPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= peakLeftLimit)
        {
            nStartPoint = i;
            break;
        }
    }

    // 计算终点值
    int nStopPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= peakRightLimit)
        {
            nStopPoint = i;
            break;
        }
    }

    // 起点和终点间数据点数必须大于等于20个
    if(nStopPoint - nStartPoint + 1 < 20)
    {
        return area;
    }

    for(int i = nStartPoint; i <= nStopPoint; i++)
    {
        rangeData.append(yCurrent[i]);
        rangeLeftDiff.append(yCurrent[i] - yCurrent[i-1]);
        rangeRightDiff.append(yCurrent[i] - yCurrent[i+1]);
    }

    float peak = 0;
    int peakIndex = 0;
    for(int i = diffNum; i < rangeData.length() - diffNum; i++)
    {
        bool findPeak = true;
        for(int j = 0; j < diffNum; j++)
        {
            if( rangeLeftDiff[i-j]<=0||rangeRightDiff[i+j]<0)
            {
                findPeak = false;
                break;
            }
        }
        if(findPeak && rangeData[i] >= peak)
        {
            peak = rangeData[i];
            peakIndex = nStartPoint + i;
        }
    }
    logger->debug("Peak Index:%d, Voltage:%f, Current:%f", peakIndex, xVoltage[peakIndex], yCurrent[peakIndex]);

    //打印峰顶部分数据
    if(peakIndex >= diffNum && peakIndex < length - diffNum)
    {
        QVector<float> peakArray;
        QStringList peakArrayList;
        for(int n = 0; n < diffNum*2+1; n++)
        {
            peakArray.append(yCurrent[peakIndex-diffNum+n]);

            QString pointStr = QString::number(yCurrent[peakIndex-diffNum+n]);
            peakArrayList.append(pointStr);
        }
        QString peakArrayStr = peakArrayList.join(",");
        logger->debug("PeakArrayValue:%s", peakArrayStr.toStdString().c_str());
    }

    //根据峰顶确定的区间
    if(peakIndex != 0)
    {
        windowFrom = xVoltage[peakIndex] - peakWidth;
        windowTo = xVoltage[peakIndex] + peakWidth;
    }

    //寻找峰底
    if(valleyRightLimit > 0)
    {
        // 峰底左侧极限
        int nValleyStartPoint = (nStopPoint + nStartPoint)/2;
        if(peakIndex != 0)
        {
            nValleyStartPoint = peakIndex;
        }

        // 峰底右侧极限
        int nValleyStopPoint = nStopPoint;
        for(int i = 0; i < length; i++)
        {
            if(xVoltage[i] >= peakRightLimit + valleyRightLimit)
            {
                nValleyStopPoint = i;
                break;
            }
        }

        // 峰底搜索数据区
        for(int i = nValleyStartPoint; i <= nValleyStopPoint; i++)
        {
            valleyData.append(yCurrent[i]);
            valleyLeftDiff.append(yCurrent[i] - yCurrent[i-1]);
            valleyRightDiff.append(yCurrent[i] - yCurrent[i+1]);
        }

        float valley = 10000;
        int valleyIndex = nStopPoint;
        for(int i = diffNum; i < valleyData.length() - diffNum; i++)
        {
            bool findValley = true;
            for(int j = 0; j < diffNum; j++)
            {
                if( valleyLeftDiff[i-j]>=0||valleyRightDiff[i+j]>0)
                {
                    findValley = false;
                    break;
                }
            }
            if(findValley && valleyData[i] <= valley)
            {
                valley = valleyData[i];
                valleyIndex = nValleyStartPoint + i;
            }
        }
        logger->debug("Valley Index:%d, Voltage:%f, Current:%f", valleyIndex, xVoltage[valleyIndex], yCurrent[valleyIndex]);

        //打印峰底部分数据
        if(valleyIndex >= diffNum && valleyIndex < length - diffNum)
        {
            QVector<float> valleyArray;
            QStringList valleyArrayList;
            for(int n = 0; n < diffNum*2+1; n++)
            {
                valleyArray.append(yCurrent[valleyIndex-diffNum+n]);

                QString pointStr = QString::number(yCurrent[valleyIndex-diffNum+n]);
                valleyArrayList.append(pointStr);
            }
            QString valleyArrayStr = valleyArrayList.join(",");
            logger->debug("ValleyArrayValue:%s", valleyArrayStr.toStdString().c_str());
        }

        //根据峰顶与峰底确定的区间
        if(peakIndex != 0 && valleyIndex != 0)
        {
            float radius = (valleyIndex - peakIndex)*step;  //峰顶到峰底的距离

            windowFrom = xVoltage[peakIndex] - radius;
            windowTo = xVoltage[peakIndex] + radius;

            logger->debug("Peak to Valley radius = %f", radius);
        }
    }

    if(windowFrom < beginVoltage || windowTo > endVoltage)
    {
        windowFrom = peakLeftLimit;
        windowTo = peakRightLimit;
    }

    area = CalculateArea(beginVoltage, endVoltage, windowFrom, windowTo);

    logger->debug("Auto peak area calculate area = %f, windowFrom = %f, windowTo = %f", area, windowFrom, windowTo);

    return area;
}


/**
 * @brief 曲线拟合法计算电流峰面积
 * @return 电流峰面积，double。
 */
float CurveData::CalculateArea(float beginVoltage, float endVoltage, float windowFrom, float windowTo)
{
    float peakVoltage = 0;
    float peakCurrent = -10000;
    float baseCurrent = 0;
    float area = 0;
    const int length = m_length;
    float xVoltage[length];
    float yCurrent[length];
    memcpy(&yCurrent[0], &m_data.get()[0], sizeof(float)*length);

    if(length <= 0)
    {
        return area;
    }

    for(int i = 0; i < length; i++)
    {
        xVoltage[i] = beginVoltage + i*(endVoltage - beginVoltage)/(length - 1);
    }

    // 计算起点值
    int nStartPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowFrom)
        {
            nStartPoint = i;
            break;
        }
    }

    // 计算终点值
    int nStopPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowTo)
        {
            nStopPoint = i;
            break;
        }
    }

    // 起点和终点间数据点数必须大于等于10个
    if(nStopPoint - nStartPoint + 1 < 10)
    {
        return area;
    }

    // 获取线性拟合的数据
    float fxVoltage[] =
    {
        xVoltage[nStartPoint],
        xVoltage[nStartPoint+1],
        xVoltage[nStartPoint+2],
        xVoltage[nStartPoint+3],
        xVoltage[nStartPoint+4],
        xVoltage[nStopPoint-4],
        xVoltage[nStopPoint-3],
        xVoltage[nStopPoint-2],
        xVoltage[nStopPoint-1],
        xVoltage[nStopPoint]
    };

    float fyCurrent[] =
    {
        yCurrent[nStartPoint],
        yCurrent[nStartPoint+1],
        yCurrent[nStartPoint+2],
        yCurrent[nStartPoint+3],
        yCurrent[nStartPoint+4],
        yCurrent[nStopPoint-4],
        yCurrent[nStopPoint-3],
        yCurrent[nStopPoint-2],
        yCurrent[nStopPoint-1],
        yCurrent[nStopPoint]
    };

    // 求平均值
    float fMeanX = 0;
    float fMeanY = 0;
    float fMeanX2 = 0;
    float fMeanXY = 0;
    for(int i = 0; i < 10; i++)
    {
        fMeanX += fxVoltage[i];
        fMeanY += fyCurrent[i];
        fMeanX2 += fxVoltage[i]*fxVoltage[i];
        fMeanXY += fxVoltage[i]*fyCurrent[i];
    }
    fMeanX = fMeanX/10;
    fMeanY = fMeanY/10;
    fMeanX2 = fMeanX2/10;
    fMeanXY = fMeanXY/10;

    //求斜率
    if(fabs(fMeanX2-fMeanX*fMeanX) < 1.0e-7)
    {
        return false;
    }

    float k = (fMeanXY-fMeanX*fMeanY)/(fMeanX2-fMeanX*fMeanX);

    //求截距
    float b = fMeanY - k*fMeanX;

    // 计算峰值和面积
    for(int i = nStartPoint; i <= nStopPoint; i++)
    {
        baseCurrent = k*xVoltage[i] + b;
        if((yCurrent[i] - baseCurrent) > peakCurrent)
        {
            peakCurrent = yCurrent[i] - baseCurrent;
            peakVoltage = xVoltage[i];
        }

        if(yCurrent[i] - baseCurrent > 0 && i < nStopPoint)
        {
            area += (yCurrent[i] - baseCurrent)*(xVoltage[i+1] - xVoltage[i]);
        }
    }

    return area;
}

/**
 * @brief 曲线拟合法计算电流峰值
 * @return 峰值电流，double。
 */
float CurveData::CalculatePeak(float beginVoltage, float endVoltage, float windowFrom, float windowTo)
{
    float peakVoltage = 0;
    float peakCurrent = -10000;
    float baseCurrent = 0;
    const int length = m_length;
    float xVoltage[length];
    float yCurrent[length];
    memcpy(&yCurrent[0], &m_data.get()[0], sizeof(float)*length);

    if(length <= 0)
    {
        return peakCurrent;
    }

    for(int i = 0; i < length; i++)
    {
        xVoltage[i] = beginVoltage + i*(endVoltage - beginVoltage)/(length - 1);
    }

    // 计算起点值
    int nStartPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowFrom)
        {
            nStartPoint = i;
            break;
        }
    }

    // 计算终点值
    int nStopPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowTo)
        {
            nStopPoint = i;
            break;
        }
    }

    // 起点和终点间数据点数必须大于等于10个
    if(nStopPoint - nStartPoint + 1 < 10)
    {
        return peakCurrent;
    }

    // 获取线性拟合的数据
    float fxVoltage[] =
    {
        xVoltage[nStartPoint],
        xVoltage[nStartPoint+1],
        xVoltage[nStartPoint+2],
        xVoltage[nStartPoint+3],
        xVoltage[nStartPoint+4],
        xVoltage[nStopPoint-4],
        xVoltage[nStopPoint-3],
        xVoltage[nStopPoint-2],
        xVoltage[nStopPoint-1],
        xVoltage[nStopPoint]
    };

    float fyCurrent[] =
    {
        yCurrent[nStartPoint],
        yCurrent[nStartPoint+1],
        yCurrent[nStartPoint+2],
        yCurrent[nStartPoint+3],
        yCurrent[nStartPoint+4],
        yCurrent[nStopPoint-4],
        yCurrent[nStopPoint-3],
        yCurrent[nStopPoint-2],
        yCurrent[nStopPoint-1],
        yCurrent[nStopPoint]
    };

    // 求平均值
    float fMeanX = 0;
    float fMeanY = 0;
    float fMeanX2 = 0;
    float fMeanXY = 0;
    for(int i = 0; i < 10; i++)
    {
        fMeanX += fxVoltage[i];
        fMeanY += fyCurrent[i];
        fMeanX2 += fxVoltage[i]*fxVoltage[i];
        fMeanXY += fxVoltage[i]*fyCurrent[i];
    }
    fMeanX = fMeanX/10;
    fMeanY = fMeanY/10;
    fMeanX2 = fMeanX2/10;
    fMeanXY = fMeanXY/10;

    //求斜率
    if(fabs(fMeanX2-fMeanX*fMeanX) < 1.0e-7)
    {
        return false;
    }

    float k = (fMeanXY-fMeanX*fMeanY)/(fMeanX2-fMeanX*fMeanX);

    //求截距
    float b = fMeanY - k*fMeanX;

    // 计算峰值
    for(int i = nStartPoint; i <= nStopPoint; i++)
    {
        baseCurrent = k*xVoltage[i] + b;
        if((yCurrent[i] - baseCurrent) > peakCurrent)
        {
            peakCurrent = yCurrent[i] - baseCurrent;
            peakVoltage = xVoltage[i];
        }

        if(peakCurrent < 0)
        {
            peakCurrent = 0;
        }
    }

    return peakCurrent;
}

/**
 * @brief 曲线拟合法计算电流峰值对应电压
 * @return 电压，double。
 */
float CurveData::GetPeakVolt(float beginVoltage, float endVoltage, float windowFrom, float windowTo)
{
    float peakVoltage = 0;
    float peakCurrent = -10000;
    float baseCurrent = 0;
    const int length = m_length;
    float xVoltage[length];
    float yCurrent[length];
    memcpy(&yCurrent[0], &m_data.get()[0], sizeof(float)*length);

    if(length <= 0)
    {
        return peakVoltage;
    }

    for(int i = 0; i < length; i++)
    {
        xVoltage[i] = beginVoltage + i*(endVoltage - beginVoltage)/(length - 1);
    }

    // 计算起点值
    int nStartPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowFrom)
        {
            nStartPoint = i;
            break;
        }
    }

    // 计算终点值
    int nStopPoint = 0;
    for(int i = 0; i < length; i++)
    {
        if(xVoltage[i] >= windowTo)
        {
            nStopPoint = i;
            break;
        }
    }

    // 起点和终点间数据点数必须大于等于10个
    if(nStopPoint - nStartPoint + 1 < 10)
    {
        return peakVoltage;
    }

    // 获取线性拟合的数据
    float fxVoltage[] =
    {
        xVoltage[nStartPoint],
        xVoltage[nStartPoint+1],
        xVoltage[nStartPoint+2],
        xVoltage[nStartPoint+3],
        xVoltage[nStartPoint+4],
        xVoltage[nStopPoint-4],
        xVoltage[nStopPoint-3],
        xVoltage[nStopPoint-2],
        xVoltage[nStopPoint-1],
        xVoltage[nStopPoint]
    };

    float fyCurrent[] =
    {
        yCurrent[nStartPoint],
        yCurrent[nStartPoint+1],
        yCurrent[nStartPoint+2],
        yCurrent[nStartPoint+3],
        yCurrent[nStartPoint+4],
        yCurrent[nStopPoint-4],
        yCurrent[nStopPoint-3],
        yCurrent[nStopPoint-2],
        yCurrent[nStopPoint-1],
        yCurrent[nStopPoint]
    };

    // 求平均值
    float fMeanX = 0;
    float fMeanY = 0;
    float fMeanX2 = 0;
    float fMeanXY = 0;
    for(int i = 0; i < 10; i++)
    {
        fMeanX += fxVoltage[i];
        fMeanY += fyCurrent[i];
        fMeanX2 += fxVoltage[i]*fxVoltage[i];
        fMeanXY += fxVoltage[i]*fyCurrent[i];
    }
    fMeanX = fMeanX/10;
    fMeanY = fMeanY/10;
    fMeanX2 = fMeanX2/10;
    fMeanXY = fMeanXY/10;

    //求斜率
    if(fabs(fMeanX2-fMeanX*fMeanX) < 1.0e-7)
    {
        return false;
    }

    float k = (fMeanXY-fMeanX*fMeanY)/(fMeanX2-fMeanX*fMeanX);

    //求截距
    float b = fMeanY - k*fMeanX;

    // 计算峰值
    for(int i = nStartPoint; i <= nStopPoint; i++)
    {
        baseCurrent = k*xVoltage[i] + b;
        if((yCurrent[i] - baseCurrent) > peakCurrent)
        {
            peakCurrent = yCurrent[i] - baseCurrent;
            peakVoltage = xVoltage[i];
        }

        if(peakCurrent < 0)
        {
            peakCurrent = 0;
        }
    }

    return peakVoltage;
}


}
