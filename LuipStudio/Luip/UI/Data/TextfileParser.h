#ifndef UI_DATA_TEXTFILEPARSER_H
#define UI_DATA_TEXTFILEPARSER_H

#include <memory>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include "System/Types.h"

using System::Uint8;

using System::Uint32;
using System::Uint64;

using System::Uint32;
using System::Int64;

namespace UI
{


class TextfileParser
{
public:
    TextfileParser(QString fileName);
  //  bool AddRecord(MeasureResult result);
    char* GetCurrentRecord();
    bool HasNext();
    bool HasPrevious();
    bool MoveNext();
    bool MovePrevious();
    bool MoveToLast();
    void MoveToFirst();
    Uint32 GetRemainNum();
    Uint32 GetHeadNum();
    long long GetPosition();
    int  TotalBlock();
    char *SeekBlock(int currentBlock);
    long long  TotalSize();
    QString GetFileName(){return m_fileName;}
    bool ClearRecord();

private:
  // MeasureResult m_currentResult;
   Int64 m_pos;
   QMutex m_mutex;
   char m_buffer[21480];
   QString m_fileName;
};

}
#endif // UI_DATA_TEXTFILEPARSER_H
