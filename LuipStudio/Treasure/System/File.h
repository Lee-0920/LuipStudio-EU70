#ifndef __TREASURE_SYSTEM_FILE_H__
#define __TREASURE_SYSTEM_FILE_H__

#include <memory>
#include <QFile>

namespace Treasure
{

    class File
    {
    public:
        static bool Remove(const QString &path)
        {
            if (path.isEmpty())
                return true;

            if (!QFile::exists(path))
                return true;

            return QFile::remove(path);
        }

        static bool Copy(const QString& path, const QString& toPath)
        {
            if (path.isEmpty() || toPath.isEmpty())
                return false;

            if (!QFile::exists(path))
                return false;

            QFile::remove(toPath);
            QFile fp(path);
            return fp.copy(toPath);
        }

        static QString ReadAll(const QString &path)
        {
            if (path.isEmpty())
                return "";

            if (!QFile::exists(path))
                return "";

            QFile fp(path);
            if (!fp.open(QIODevice::ReadOnly))
                return "";
            QString content = fp.readAll();
            fp.close();
            return content;
        }

        static bool WriteAll(const QString &path, const QString &content)
        {
            if (path.isEmpty())
                return false;

            QFile fp(path);
            if (!fp.open(QIODevice::WriteOnly))
                return false;
            fp.write(content.toStdString().c_str(), content.length());
            fp.close();
            return true;
        }

        static bool Exist(QString path)
        {
            QFile file(path);
            return file.exists();
        }

    public:
        typedef std::shared_ptr<QFile> QFilePtr;

    };

}

#endif
