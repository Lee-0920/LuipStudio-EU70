/**
 * @file
 * @brief 设备生产日期。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#if !defined(CONTROLLER_API_METERPOINTS_H)
#define CONTROLLER_API_METERPOINTS_H

#include <vector>
#include "../LuipShare.h"

using std::vector;

namespace Controller
{
namespace API
{

/**
 * @brief 光学定量点体积。
 * @details
 */
struct LUIP_SHARE NP66MeterPoint
{
    float setVolume;
    float realVolume;
};

/**
 * @brief 光学定量点。
 * @details 
 */
class LUIP_SHARE NP66MeterPoints
{
public:
    NP66MeterPoints();
    NP66MeterPoints(unsigned int num);
    void SetNum(unsigned int num);
    int GetNum();
    void SetPoint(unsigned int index, NP66MeterPoint point);
    NP66MeterPoint GetPoint(unsigned int index);
    bool operator== ( NP66MeterPoints const& meterPoints) const;
private:
    int m_totalNum;
    vector<NP66MeterPoint> m_points;
};

}
}

#endif  //CONTROLLER_API_METERPOINTS_H
