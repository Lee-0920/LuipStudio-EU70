/*
 * MasterUpdate.h
 *
 *  Created on: 2016年9月13日
 *      Author: liangliwen
 */


#ifndef UPDATER_MASTERUPDATE_H
#define UPDATER_MASTERUPDATE_H


#include "System/Types.h"
#include <QTextStream>
#include <QString>

using namespace System;

namespace Updater
{
class MasterUpdate
{
public:
    MasterUpdate(String fileName, bool isDeleteZip);
    MasterUpdate(String toolFileName);
    ~MasterUpdate();
    bool Update(void);
    bool ParseHeader(String fileName);
    String GetType(void);
    String GetVersion(void);
    String GetUnzipDestDirectory(void);
private:
    void Trim(QString *str);
    String m_fileName;
    String *m_version;
    String *m_type;
    bool m_isDeleteZip;
    String m_destDirectory;
    bool m_isRunTool;
};
}

#endif // UPDATER_MASTERUPDATE_H
