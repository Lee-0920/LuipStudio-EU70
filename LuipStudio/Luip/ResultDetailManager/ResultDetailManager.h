#ifndef RESULTDETAILMANAGER_H
#define RESULTDETAILMANAGER_H

#include "ResultDetailPlugin/ResultDetailWidget.h"
#include <memory>

namespace ResultDetail
{

class ResultDetailManager
{
public:
    ~ResultDetailManager();
    static ResultDetailManager* Instance();
    bool Init();
    void Uninit();
    void LoadPlugin();
    void SetMeasureResultDetailWidget(ResultDetailWidget* widget);
    void SetCalibrateResultDetailWidget(ResultDetailWidget* widget);
    ResultDetailWidget* GetMeasureResultDetailWidget(void);
    ResultDetailWidget* GetCalibrateResultDetailWidget(void);

private:
    static std::unique_ptr<ResultDetailManager> m_instance;
    ResultDetailWidget* m_measureResultDetailWidget;
    ResultDetailWidget* m_calibrateResultDetailWidget;
};

}
#endif // RESULTDETAILMANAGER_H
