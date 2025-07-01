#ifndef CURVEFILE_H
#define CURVEFILE_H

#include "DriveControllerPlugin/CurveData.h"
#include "System/Types.h"
#include <QFile>
#include <QMutex>
#include "ControllerPlugin/API/Version.h"

namespace  ResultDetail
{

struct CurveFileHeader
{
    char curveType[32];  //当前曲线的类型
    System::Byte curveVersion[16];//当前文件的版本号
    char target[24];       //当前仪器的类型
    System::Uint64 index;
    char reserved[176];//保留信息
};

class CurveFile
{
public:
    CurveFile(System::String path, System::Uint32 dataMaxLength, Controller::API::Version ver, System::String curveType, System::Uint32 curveMaxNumber);
    ~CurveFile();

    bool Load();
    bool Save();
    bool ClearCurve(void);

    System::Uint32 Addcurve(Controller::CurveData curve);
    Controller::CurveData Getcurve(System::Uint32 index);

protected:
    virtual bool LoadFrom();
    virtual bool SaveTo();

    String GetCurveType() const;
    bool SetCurveType(String type);
    Controller::API::Version GetCurveVersion() const;
    bool SetCurveVersion(Controller::API::Version ver);
    String GetTarget() const;
    bool SetTarget(String target);
    System::Uint64 GetIndex() const;
    bool SetIndex(System::Uint64 index);

    bool MakeIndex(System::Uint64 index);

private:
    QMutex m_mutex;//写文件的锁
    CurveFileHeader m_FileHeader;
    QFile m_file;
    String m_path;
    System::Uint32 m_fileHeaderSize;
    const System::Uint32 m_dataMaxLength;
    const System::Uint32 m_curveMaxNumber;
    const Controller::API::Version m_setCurveVersion;
    const System::String m_setCurveType;

    System::Int64 GetPos(System::Uint32 index);
};
}
#endif // CURVEFILE_H
