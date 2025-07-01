/**
 * @file
 * @brief 升级文件解析。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/8/2
 */


#ifndef UPDATER_UPDATEFILEPARSER_H
#define UPDATER_UPDATEFILEPARSER_H

#include "System/Types.h"
#include <QTextStream>
#include <QString>
#include "vector"
#include "ProgramSegment.h"

using namespace System;
using std::vector;

namespace Updater
{

class ProgramFileParser
{
public:
    ProgramFileParser(String fileName);
    ~ProgramFileParser();
    bool ParseHeader(void);
    bool ParseProgram(void);
    vector<ProgramSegment*> GetSegmentData(void);
    String GetType(void);
    String GetMode();
    String GetVersion(void);

private:
    String *m_version;
    String *m_mode;
    String *m_type;
    String m_fileName;
    vector<ProgramSegment*> m_segments;
    unsigned long  m_fileDataLen;

    bool TwoStringToAByte(unsigned char* destData, QString buff);
    bool StringToData(unsigned char * destData, QString buff, unsigned long len);
    void Trim(QString *str);

};

}

#endif // UPDATER_UPDATEFILEPARSER_H
