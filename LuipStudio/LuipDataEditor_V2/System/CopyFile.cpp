#include "Log.h"
#include "CopyFile.h"
#include "System/FileDetecter.h"
#include "LuaEngine/LuaEngine.h"

using namespace Lua;

namespace System
{

bool CopyFileAction::CopyFileToPath(String sourcePath ,String toPath, bool coverFileIfExist)
{
    QString sourceDir(sourcePath.c_str());
    QString toDir(toPath.c_str());
    toDir.replace("\\","/");
    if (sourceDir == toDir)
    {
        return true;
    }
    if (!QFile::exists(sourceDir))
    {
        logger->debug("%s 源文件不存在.", sourceDir.toLatin1().data());
        return false;
    }

    if (QFile::exists(toDir))
    {
        if(coverFileIfExist)
        {
            QFile::remove(toDir);
        }
    }//end if

    if(!QFile::copy(sourceDir, toDir))
    {
        logger->debug("文件复制失败,源路径%s 目标路径 %s", sourceDir.toLatin1().data(), toDir.toLatin1().data());
        return false;
    }
#ifdef    _CS_ARM_LINUX
    system("sync");
#endif
    return true;
}

//拷贝文件夹：
bool CopyFileAction::CopyDirectoryFiles(const String &fromDir, const String &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir.c_str());
    QDir targetDir(toDir.c_str());
    if(!targetDir.exists())
    {    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir())
        {    /**< 当为目录时，递归的进行copy */
            if(!CopyDirectoryFiles(fileInfo.filePath().toStdString(),
                targetDir.filePath(fileInfo.fileName()).toStdString(),
                coverFileIfExist))
                return false;
        }
        else
        {            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName()))
            {
                targetDir.remove(fileInfo.fileName());
            }

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName())))
            {
                    return false;
            }
        }
    }
#ifdef    _CS_ARM_LINUX
    system("sync");
#endif
    return true;
}

bool CopyFileAction::ExMemoryDetect()
{
#ifdef    _CS_ARM_LINUX
    FileDetecter fileDetect;    //U盘检测
    if(!fileDetect.UdiskDetect())
    {
        logger->debug("未检测到U盘,数据导出失败.");
        return false;
    }
#endif
    return true;
}

String CopyFileAction::GetTargetDir()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String sn;
    luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);
    String dataCopyDir;
#ifdef _CS_X86_WINDOWS
    luaEngine->GetLuaValue(state, "setting.common.dataCopyDirWin", dataCopyDir);
#else
    luaEngine->GetLuaValue(state, "setting.common.dataCopyDir", dataCopyDir);
#endif
    String strDir = dataCopyDir + sn;
    return strDir;
}

bool CopyFileAction::TargetDirCheck(QDir dir)
{
    if(!dir.exists())
    {
        if(!dir.mkdir(dir.absolutePath()))
        {
            logger->debug("%s 目录创建失败.", dir.absolutePath().toLatin1().data());
            return false;
        }
    }

    return true;
}



}
