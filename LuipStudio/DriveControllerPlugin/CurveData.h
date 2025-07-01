#if !defined(CONTROLLER_CURVEDATA_H_)
#define CONTROLLER_CURVEDATA_H_

#include <vector>
#include <QObject>
#include "LuipShare.h"
#include <memory>

using std::list;

namespace Controller
{

/**
 * @brief 曲线数据。
 * @details
 */
class LUIP_SHARE CurveData
{

public:
    CurveData(float beginVoltage, float endVoltage, int length);
    float* GetWholeData(void);

    bool AddData(float* buffer, int length);
    float GetData(int index)const;
    void SetData(int index, float data);
    int GetLength()const;
    int GetSize()const;
    float GetBegin()const;
    float GetEnd()const;
    float CalculateArea(float beginVoltage, float endVoltage, float windowFrom, float windowTo);
    float CalculatePeak(float beginVoltage, float endVoltage, float windowFrom, float windowTo);
    float GetPeakVolt(float beginVoltage, float endVoltage, float windowFrom, float windowTo);
    float AutoCalculateArea(float beginVoltage, float endVoltage, float peakWidth, float peakLeftLimit, float peakRightLimit, float valleyRightLimit = 0);

private:
    std::shared_ptr<float> m_data;
    int m_index;
    int m_length;
    float m_begin;
    float m_end;
};

}

#endif  //CONTROLLER_CURVEDATA_H_

