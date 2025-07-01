#if !defined(RESULTMANAGER_MEASURERECORDFILE_H)
#define RESULTMANAGER_MEASURERECORDFILE_H

#include "RecordFile.h"

namespace Result
{
class MeasureRecordFile:public RecordFile
{
public:
    MeasureRecordFile(String path, String exportFileName, String formatTableName);

private:
    bool LoadFrom();
    bool SaveTo();
    const String m_recordType;
};
}
#endif // RESULTMANAGER_MEASURERECORDFILE_H
