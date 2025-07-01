#include "CurveManager.h"
#include <QApplication>
#include "DriveControllerPlugin/CurveData.h"

using namespace std;
using namespace Controller;
using namespace System;
namespace  ResultDetail
{


unique_ptr<CurveManager> CurveManager::m_instance(nullptr);


CurveManager::CurveManager()
{

}

CurveManager::~CurveManager()
{

}

CurveManager* CurveManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CurveManager);
    }

    return m_instance.get();
}


void CurveManager::Init()
{
    m_curveFile = new CurveFile(QApplication::applicationDirPath().toStdString() + "/../LuipData/CurveFile.bin", 5000,
                                Controller::API::Version("1.0.0.0"), "CurveFile", ((60 * 1024 * 1024) / 5000));
    if (!m_curveFile->Load())
    {
        m_curveFile->ClearCurve();
    }
}

CurveFile* CurveManager::GetCurveFile()
{
    return m_curveFile;
}

Uint32 CurveManager::Addcurve(CurveData curve)
{
    return m_curveFile->Addcurve(curve);
}

CurveData CurveManager::Getcurve(Uint32 index)
{
    return m_curveFile->Getcurve(index);
}
}
