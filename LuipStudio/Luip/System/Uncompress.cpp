#include "Uncompress.h"
#include "zlib.h"
#include "zconf.h"
#include "unzip.h"
#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "Log.h"
#include <fcntl.h>
#include <time.h>

#ifdef _CS_X86_WINDOWS
#include "iowin32.h"
#endif


using namespace std;
//压缩包压缩时的文件系统类型
#define FAT_FILE_SYSTEMS 0
#define UNIX_SYSTEMS 3
//Linux下的文件类型
#define FIFO  0x01 /* named pipe (fifo) 命名管道*/
#define FCHR  0x02 /* character special 字符特殊文件 */
#define FDIR  0x04  /* directory 目录文件*/
#define FBLK  0x06  /* block special 块特殊文件*/
#define FREG  0x08  /* regular 普通文件*/
#define FLNK  0x0A  /* symbolic link 符号链接*/
#define FSOCK 0x0C  /* socket 套接字*/
//Linux下的文件的读写权限
#define RUSR 0x01000000 /* R for owner */
#define WUSR 0x00800000 /* W for owner */
#define XUSR 0x00400000 /* X for owner */
#define RGRP 0x00200000 /* R for group */
#define WGRP 0x00100000 /* W for group */
#define XGRP 0x00080000 /* X for group */
#define ROTH 0x00040000 /* R for other */
#define WOTH 0x00020000 /* W for other */
#define XOTH 0x00010000 /* X for other */

namespace System
{
bool Uncompress::IsDir(String path)
{
    QString dir = path.c_str();
    QStringList stringList = dir.split("/");
    QFileInfo fileInfo(dir);
    bool ret = false;

    if (fileInfo.exists() == false)
    {
        if (stringList.at(stringList.size() - 1) == "" && dir.at(dir.length() - 1) == '/')
        {
            ret = true;
        }
    }
    else
    {
        if (fileInfo.isDir())
        {
            ret  = true;
        }
    }
    return ret;
}

char Uncompress::GetUnZipFileHostSystem(unsigned long version)
{
    return (0xFF & (version >> 8));
}

char Uncompress::GetUnZipFileType(unsigned long  externalAttributes)
{
    return (0xFF & (externalAttributes >> 28));
}

bool Uncompress::SetUnZipFileAuthority(QFile &file, unsigned long  externalAttributes)
{
    QFileDevice::Permissions permissionSpec = 0;
    if (RUSR & externalAttributes)
    {
        permissionSpec |= QFile::ReadOwner;
    }
    if (WUSR & externalAttributes)
    {
        permissionSpec |= QFile::WriteOwner;
    }
    if (XUSR & externalAttributes)
    {
        permissionSpec |= QFile::ExeOwner;
    }
    if (RGRP & externalAttributes)
    {
        permissionSpec |= QFile::ReadGroup;
    }
    if (WGRP & externalAttributes)
    {
        permissionSpec |= QFile::WriteGroup;
    }
    if (XGRP & externalAttributes)
    {
        permissionSpec |= QFile::ExeGroup;
    }
    if (ROTH & externalAttributes)
    {
        permissionSpec |= QFile::ReadOther;
    }
    if (WOTH & externalAttributes)
    {
        permissionSpec |= QFile::WriteOther;
    }
    if (XOTH & externalAttributes)
    {
        permissionSpec |= QFile::ExeOther;
    }
    return file.setPermissions(permissionSpec);
}

bool Uncompress::UnZipToDirectory(String zipPathName, String destDirectory, QStringList &listPic, String password)
{
    bool ret = true;
    if (IsDir(destDirectory))
    {
        QFileInfo fileInfo(destDirectory.c_str());
        if (fileInfo.exists() == true)//如果解压目录存在则删除。
        {
            DeleteDirectory(destDirectory.c_str());
        }
        QDir dir(destDirectory.c_str());
        dir.mkpath(destDirectory.c_str());//创建解压目录
    }
    else
    {
        logger->warn("解压的目标路径 %s 错误", destDirectory.c_str());
        return false;
    }
    if (zipPathName.empty() || destDirectory.empty())
    {
        logger->warn("路径空， 压缩包路径：%s， 解压目录：%s", zipPathName.c_str(), destDirectory.c_str());
        return false;
    }

    unzFile zFile = unzOpen64(zipPathName.c_str());//这个方法可以打开一个压缩文件，返回一个句柄
    if (NULL == zFile)
    {
        logger->warn("打开压缩包 %s 失败", zipPathName.c_str());
        return false;
    }

    unz_global_info64 gi;
    //得到全局的信息。常用的是压缩包里的文件个数
    if (unzGetGlobalInfo64(zFile, &gi) == UNZ_OK)
    {
        for (ZPOS64_T i = 0; i < gi.number_entry; ++i)
        {
            char file[256] = {0};
            char ext[256] = {0};
            char com[1024] = {0};
            unz_file_info64 unzFileInfo;
            //得到相关的文件信息。
            if (unzGetCurrentFileInfo64(zFile, &unzFileInfo, file, sizeof(file), ext, 256, com, 1024) == UNZ_OK)
            {
                string path = destDirectory + file;
                if (UNIX_SYSTEMS == GetUnZipFileHostSystem(unzFileInfo.version))
                {
                    switch(GetUnZipFileType(unzFileInfo.external_fa))
                    {
                        case FDIR : /* directory 目录文件*/
                        {
                            QDir dir(path.c_str());
                            dir.mkpath(path.c_str());
                        }
                        break;
                        case FLNK : /* symbolic link 符号链接*/
                        {
                            int result;
                            if (!password.empty())
                            {
                                result = unzOpenCurrentFilePassword(zFile, password.c_str());
                            }
                            else
                            {
                                result = unzOpenCurrentFile(zFile);
                            }
                            if (result != 0)
                            {
                                logger->warn("解压文件 %s 打开失败。错误代码：%d", path.c_str(), result);
                                ret = false;
                                break;
                            }
                            QString fileName;
                            while(true)//符号文件里保存着指向的文件
                            {
                                char data[1024] = {0};
                                int size= unzReadCurrentFile(zFile, data, sizeof(data));
                                if(size <= 0)
                                {
                                    break;
                                }
                                fileName += data;
                            }
                            //关闭当前文件
                            unzCloseCurrentFile(zFile);
                            QFile::link(fileName, path.c_str());//创建链接文件
                            listPic << path.c_str();
                        }
                        break;
                        default: /* regular 普通文件*/ /* named pipe (fifo) 命名管道*//* character special 字符特殊文件 *//* block special 块特殊文件*//* socket 套接字*/
                        {
                            int result;
                            if (!password.empty())
                            {
                                result = unzOpenCurrentFilePassword(zFile, password.c_str());
                            }
                            else
                            {
                                result = unzOpenCurrentFile(zFile);
                            }
                            if (result != 0)
                            {
                                logger->warn("解压文件 %s 打开失败。错误代码：%d", path.c_str(), result);
                                ret = false;
                                break;
                            }
                            QFile createFile(path.c_str());
                            if (createFile.open(QFile::WriteOnly | QFile::Truncate))
                            {
                                SetUnZipFileAuthority(createFile, unzFileInfo.external_fa);
                                char data[1024] = {0};
                                while(true)
                                {
                                    int size= unzReadCurrentFile(zFile, data, sizeof(data));
                                    if(size <= 0)
                                    {
                                        break;
                                    }
                                    createFile.write(data, size);
                                }
                                createFile.close();
                                listPic << path.c_str();
                            }
                            else
                            {
                                logger->warn("创建解压文件 %s 失败。", path.c_str());
                                ret = false;
                                unzCloseCurrentFile(zFile);
                                break;
                            }
                            //关闭当前文件
                            unzCloseCurrentFile(zFile);
                        }
                        break;
                    }
                }
                else
                {
                    if (IsDir(path))//zlib在压缩过程通过名字后最后一个字符是'/'，来解决文件是文件还是文件夹的问题
                    {
                        QDir dir(path.c_str());
                        dir.mkpath(path.c_str());
                    }
                    else
                    {
                        int result;
                        if (!password.empty())
                        {
                            result = unzOpenCurrentFilePassword(zFile, password.c_str());//打开当前定位的压缩包中的文件
                        }
                        else
                        {
                            result = unzOpenCurrentFile(zFile);/* 无密码 */
                        }
                        if (result != 0)
                        {
                            logger->warn("解压文件 %s 打开失败。错误代码：%d", path.c_str(), result);
                            ret = false;
                            break;
                        }

                        QFile createFile(path.c_str());
                        if (createFile.open(QFile::WriteOnly | QFile::Truncate))
                        {
                            createFile.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther
                                    | QFile::WriteOwner
                                    | QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther);
                            char data[1024] = {0};
                            while(true)
                            {
                                int size= unzReadCurrentFile(zFile, data, sizeof(data));// 从当前压缩包中开始读文件
                                if(size <= 0)
                                {
                                    break;
                                }
                                createFile.write(data, size);
                            }
                            createFile.close();
                            listPic << path.c_str();
                        }
                        else
                        {
                            logger->warn("创建解压文件 %s 失败。", path.c_str());
                            ret = false;
                            unzCloseCurrentFile(zFile);
                            break;
                        }
                        //关闭当前文件
                        unzCloseCurrentFile(zFile);
                    }
                }
            }
            else
            {
                logger->warn("获取解压包 %s 当前文件信息失败。", zipPathName.c_str());
                ret = false;
                break;
            }
            //出错
            if(i < gi.number_entry - 1 && unzGoToNextFile(zFile) != UNZ_OK)//定位到下一个文件
            {
                logger->warn("获取解压包 %s 下一文件失败。", zipPathName.c_str());
                ret = false;
                break;
            }
        }
    }
    else
    {
        logger->warn("获取解压包 %s 文件数量失败。", zipPathName.c_str());
    }
    unzClose(zFile);//关闭zip文件
    return ret;
}

String Uncompress::ParserUnzipVersion(String zipPathName)
{
    String version;
    QFileInfo zipFile(zipPathName.c_str());
    QString zipFileName = zipFile.completeBaseName();

    QStringList zipNameList = zipFileName.split("_");
    if (zipNameList.size() >= 4)
    {
        version = zipNameList.at(3).toStdString();
    }
    return version;
}

bool Uncompress::DeleteDirectory(const QString &path)
{
    if (path.isEmpty())
    {
        logger->debug("删除失败：没有输入删除路径。");
        return false;
    }

    QDir dir(path);
    if(!dir.exists())
    {
        logger->debug("删除失败：路径不存在。");
        return false;
    }

    dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
       if(fi.fileName() == "." || fi.fileName() == "..")
            continue;
       if (fi.isFile())
       {
           fi.dir().remove(fi.fileName());
       }
       else
       {
           DeleteDirectory(fi.absoluteFilePath());
       }
    }
    return dir.rmpath(dir.absolutePath());
}

std::vector<std::string> Uncompress::GetAllFilePath(const string path)
{
    std::vector<std::string> allFileName;
    QDir sourceDir(path.c_str());

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir())
        {
            std::vector<std::string> fileName = GetAllFilePath(fileInfo.filePath().toStdString());
            allFileName.insert(allFileName.end(), fileName.begin(), fileName.end());
        }
        else
        {
            allFileName.push_back(fileInfo.absoluteFilePath().toStdString());
        }
    }
    return allFileName;
}

#ifdef _CS_X86_WINDOWS
bool Uncompress::Filetime(char *f, tm_zip *tmzip, unsigned long *dt)
{
    (void)tmzip;
    bool ret = false;
    {
        FILETIME ftLocal;
        HANDLE hFind;
        WIN32_FIND_DATAA ff32;

        hFind = FindFirstFileA(f,&ff32);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
            FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
            FindClose(hFind);
            ret = true;
        }
    }
    return ret;
}

#else
bool Uncompress::Filetime(char *f, tm_zip *tmzip, unsigned long *dt)
{
    (void)f;
    (void)dt;
    int ret = true;
    struct tm* filedate;
    time_t tm_t=0;

    filedate = localtime(&tm_t);

    tmzip->tm_sec  = filedate->tm_sec;
    tmzip->tm_min  = filedate->tm_min;
    tmzip->tm_hour = filedate->tm_hour;
    tmzip->tm_mday = filedate->tm_mday;
    tmzip->tm_mon  = filedate->tm_mon ;
    tmzip->tm_year = filedate->tm_year;

    return ret;
}
#endif

/**
 * @brief Uncompress::Compress  压缩单个普通文件
 * @param sourcePath
 * @param zipPath
 * @return
 */
bool Uncompress::Compress(const std::string& sourcePath, const std::string& zipPath)
{
    int err = 0;
    QFileInfo fi(sourcePath.c_str());
    std::string filenameinzip = fi.fileName().toStdString();

    char* sourceFileName = const_cast<char *>(sourcePath.c_str());

    if (QFile::exists(zipPath.c_str()))
    {
        QFile::remove(zipPath.c_str());
    }

    zipFile zf = NULL;
    zf = zipOpen64(zipPath.c_str(), 0);

    if (zf == NULL)
    {
        logger->warn("创建 %s 压缩包失败", zipPath.c_str());
        return false;
    }

    zip_fileinfo zi;

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
    zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    Filetime(sourceFileName, &zi.tmz_date, &zi.dosDate);

    err = zipOpenNewFileInZip(zf, filenameinzip.c_str(), &zi,
                     NULL, 0, NULL, 0, NULL,
                     Z_DEFLATED,
                     8);

    if (err != ZIP_OK)
    {
        logger->warn("在压缩包里创建 %s 文件失败", sourceFileName);
        zipClose(zf,0);
        QFile::remove(zipPath.c_str());
        return false;
    }

    Int64 pos = 0;
    QFile compressFile(sourceFileName);
    if (compressFile.open(QFile::ReadOnly))
    {
        while(true)
        {
            char data[1024] = {0};

            compressFile.seek(pos);
            int len = compressFile.read(data, 1024);
            pos += len;
            if (len > 0)
            {
                err = zipWriteInFileInZip(zf, data, len);
                if (err < 0)
                {
                    logger->warn(" %s 文件写入压缩包失败。", sourceFileName);
                    zipCloseFileInZip(zf);
                    compressFile.close();
                    zipClose(zf,0);
                    QFile::remove(zipPath.c_str());
                    return false;
                }
            }

            if (len <= 0 || pos >= compressFile.size())
            {
                break;
            }
        }
        compressFile.close();
    }
    else
    {
        logger->warn("打开 %s 文件失败。", sourceFileName);
        zipCloseFileInZip(zf);
        zipClose(zf,0);
        QFile::remove(zipPath.c_str());
        return false;
    }
    zipCloseFileInZip(zf);
    zipClose(zf,0);
    return true;
}


/**
 * @brief Uncompress::CompressFileList  压缩多个普通文件
 * @param sourcePathList
 * @param zipPath
 * @return
 */
bool Uncompress::CompressFileList(const std::list<std::string> &sourcePathList, const std::string &zipPath, const char* password)
{
    if (QFile::exists(zipPath.c_str()))
    {
        QFile::remove(zipPath.c_str());
    }

    zipFile zf = NULL;
    zf = zipOpen64(zipPath.c_str(), 0);

    if (zf == NULL)
    {
        logger->warn("创建 %s 压缩包失败", zipPath.c_str());
        return false;
    }

    int err = 0;
    std::list<std::string>::const_iterator it;
    for(it = sourcePathList.begin(); it != sourcePathList.end(); it++)
    {
        std::string sourcePath = (*it);

        QFileInfo fi(sourcePath.c_str());
        std::string filenameinzip = fi.fileName().toStdString();

        char* sourceFileName = const_cast<char *>(sourcePath.c_str());

        zip_fileinfo zi;

        zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
        Filetime(sourceFileName, &zi.tmz_date, &zi.dosDate);

        uLong crc = crc32(0L, Z_NULL, 0);

        Int64 filepos = 0;
        QFile file(sourceFileName);
        if (file.open(QFile::ReadOnly))
        {
            while(true)
            {
                char buffer[10240] = {0};

                file.seek(filepos);
                int len = file.read(buffer, 10240);
                filepos += len;
                if (len > 0)
                {
                    crc = crc32(crc, (const Bytef*)buffer, len);
                }

                if (len <= 0 || filepos >= file.size())
                {
                    break;
                }
            }
            file.close();
        }
        else
        {
            logger->warn("打开 %s 文件失败。", sourceFileName);
            QFile::remove(zipPath.c_str());
            return false;
        }

        err = zipOpenNewFileInZip3(zf, filenameinzip.c_str(), &zi,
                        NULL, 0, NULL, 0, NULL, Z_DEFLATED, 8, 0,
                        -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, crc);

        if (err != ZIP_OK)
        {
            logger->warn("在压缩包里创建 %s 文件失败 错误码 = %d", sourceFileName, err);
            zipClose(zf,0);
            QFile::remove(zipPath.c_str());
            return false;
        }

        Int64 pos = 0;
        QFile compressFile(sourceFileName);
        if (compressFile.open(QFile::ReadOnly))
        {
            while(true)
            {
                char data[1024] = {0};

                compressFile.seek(pos);
                int len = compressFile.read(data, 1024);
                pos += len;
                if (len > 0)
                {
                    err = zipWriteInFileInZip(zf, data, len);
                    if (err < 0)
                    {
                        logger->warn(" %s 文件写入压缩包失败。", sourceFileName);
                        zipCloseFileInZip(zf);
                        compressFile.close();
                        zipClose(zf,0);
                        QFile::remove(zipPath.c_str());
                        return false;
                    }
                }

                if (len <= 0 || pos >= compressFile.size())
                {
                    break;
                }
            }
            compressFile.close();
        }
        else
        {
            logger->warn("打开 %s 文件失败。", sourceFileName);
            zipCloseFileInZip(zf);
            zipClose(zf,0);
            QFile::remove(zipPath.c_str());
            return false;
        }
    }
    zipCloseFileInZip(zf);
    zipClose(zf,0);
    return true;
}

}
