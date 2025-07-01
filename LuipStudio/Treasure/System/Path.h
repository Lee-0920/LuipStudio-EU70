#ifndef __TREASURE_SYSTEM_PATH_H__
#define __TREASURE_SYSTEM_PATH_H__

#include <QDir>
#include <QProcess>
#include <QThread>
#include "Logger.h"
#include <dirent.h>

namespace Treasure
{

    class Path
    {
    public:
        static bool Remove(const QString &path)
        {
            if (path.isEmpty())
                return true;

            QDir dir(path);
            if (!dir.exists())
            {
                QFile::remove(path);
                return true;
            }

            return dir.removeRecursively();

//            dir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
//            foreach (QFileInfo fi, dir.entryInfoList())
//            {
//                if (fi.fileName() == "." || fi.fileName() == "..")
//                    continue;

//                if (fi.isFile())
//                    fi.dir().remove(fi.fileName());
//                else
//                    Remove(fi.absoluteFilePath());
//            }

              //会把空的父目录也删除掉
//            return dir.rmpath(dir.absolutePath());
        }

        static bool Mkdirs(const QString& path)
        {
            QString tmpPath = path;
            tmpPath.replace("\\", "/");
            tmpPath.replace("//", "/");

            QDir dir(tmpPath);
            if (dir.exists())
                return true;

            QString parentPath = tmpPath.mid(0, tmpPath.lastIndexOf("/"));
            if (!Mkdirs(parentPath))
                return false;

            if (dir.exists())
                return true;
            return dir.mkdir(tmpPath);
        }

        static QString GetAbsPath(const QString& path)
        {
            QDir nfsDir(path);
            return nfsDir.absolutePath();
        }

        static QList<QFileInfo> GetEntryInfos(const QString& path, QDir::Filters filter = QDir::AllEntries)
        {
            QList<QFileInfo> array;
            QDir dir(path);
            if (!dir.exists())
                return array;

            dir.setFilter(filter);
            foreach (QFileInfo fi, dir.entryInfoList())
            {
                if (fi.fileName() == "." || fi.fileName() == "..")
                    continue;

                array.append(fi);
            }
            return array;
        }

        static bool Exist(QString path)
        {
            QDir dir(path);
            return dir.exists();
        }

        /**
         * @brief 建立文件夹链接
         */
        static bool Link(QString srcPath, QString descPath)
        {
            QProcess::startDetached(QString("ln -s \"%1\" \"%2\"").arg(srcPath).arg(descPath));
            QThread::msleep(50);
            return Exist(descPath);
        }

        static bool Copy(QString srcPath, QString toPath)
        {
            srcPath = srcPath.replace("\\", "/");
            toPath = toPath.replace("\\", "/");

            if (!Mkdirs(toPath))
            {
                trLogger->debug("Treasure::Path::Copy mkdirs path failed: %s",
                                toPath.toStdString().c_str());
                return false;
            }
            QDir dir(srcPath);
            auto array = dir.entryInfoList();
            foreach (auto item, array)
            {
                if (item.fileName() == "." || item.fileName() == "..")
                    continue;

                if (item.isFile())
                {
                    QString toFilePath = toPath + '/' + item.fileName();
                    toFilePath.replace("//", "/");

                    if (QFile::exists(toFilePath))
                        QFile::remove(toFilePath);

                    if (!QFile::copy(item.filePath(), toFilePath))
                    {
                        trLogger->debug("Treasure::Path::Copy file copy failed: %s to %s",
                                        item.filePath().toStdString().c_str(),
                                        toFilePath.toStdString().c_str());
                        return false;
                    }
                }
                else if (item.isDir())
                {
                    QString toSubPath = toPath + '/' + item.fileName();
                    toSubPath.replace("//", "/");
                    if (!Copy(item.filePath(), toSubPath))
                        return false;
                }
            }

            return true;
        }

    };

}

#endif
