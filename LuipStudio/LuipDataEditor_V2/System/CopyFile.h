#ifndef COPYFILE_H
#define COPYFILE_H
#include <QString>
#include <QDir>
#include <QFile>
#include "System/Types.h"

using namespace System;

namespace System
{

class CopyFileAction
{

public:

    bool ExMemoryDetect();
    String GetTargetDir();
    bool TargetDirCheck(QDir dir);

    static bool CopyFileToPath(String sourcePath ,String toPath, bool coverFileIfExist);
    bool CopyDirectoryFiles(const String &fromDir, const String &toDir, bool coverFileIfExist);
};

}
#endif // COPYFILE_H
