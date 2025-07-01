#if !defined(RESULTMANAGER_IRESULTNOTIFIABLE_H_)
#define RESULTMANAGER_IRESULTNOTIFIABLE_H_

#include "RecordDataPlugin/RecordData.h"

using System::String;

namespace Result
{

/**
 * @brief 结果上报接口。
 * @details 观察者。
 */
class IResultNotifiable
{
public:
    IResultNotifiable() {}
    virtual ~IResultNotifiable() {}

public:
    virtual void OnMeasureResultAdded(String name, ResultData::RecordData result) = 0;
    virtual void OnCalibrateResultAdded(String name, ResultData::RecordData result) = 0;
};

}

#endif  //RESULTMANAGER_IRESULTNOTIFIABLE_H_
