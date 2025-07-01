#ifndef CURVEMANAGER_H
#define CURVEMANAGER_H

#include "CurveFile.h"
#include <memory>

namespace  ResultDetail
{

class CurveManager
{
public:
    CurveManager();
    ~CurveManager();
    static CurveManager* Instance();
    void Init();
    System::Uint32 Addcurve(Controller::CurveData curve);
    Controller::CurveData Getcurve(System::Uint32 index);
    CurveFile* GetCurveFile();

private:
    static std::unique_ptr<CurveManager> m_instance;

    CurveFile *m_curveFile;
};

}
#endif // CURVEMANAGER_H
