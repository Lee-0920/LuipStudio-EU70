#ifndef CALIBRATERECORDFILE_H
#define CALIBRATERECORDFILE_H

#include "RecordFile.h"
namespace Result
{
class CalibrateRecordFile:public RecordFile
{
public:
    CalibrateRecordFile(String path, String exportFileName, String formatTableName);
    CalibrateRecordFile& CleanInvalidData(CalibrateRecordFile& originalFile);
private:
    bool LoadFrom();
    bool SaveTo();
    const String m_recordType;
};
}
#endif // CALIBRATERECORDFILE_H
