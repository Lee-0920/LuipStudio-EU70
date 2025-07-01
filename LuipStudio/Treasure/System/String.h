#ifndef __TREASURE_SYSTEM_STRING__
#define __TREASURE_SYSTEM_STRING__

#include <QMap>
#include <QVector>
#include <QString>

#define trString Treasure::String

namespace Treasure
{

typedef QMap<QString,QString> StringMap;

class String
{
public:
    /**
     * @brief 查找子串位置
     * @param str 字符串
     * @param substr 子串
     * @param number 第几个子串
     * @return < 0表示没有
     */
    static int Find(const QString& str, const QString& substr, int number = 0)
    {
        if (number < 0 || str.isEmpty() || substr.isEmpty())
            return -1;

        int index = 0;
        int tmpIndex = 0;
        for (int i = 0; i <= number; i++)
        {
            index = str.indexOf(substr, tmpIndex);
            if (index < 0)
                return -1;

            tmpIndex = index + substr.length();
        }
        return index;
    }

    /**
     * @brief 获取子串
     * @param str 字符串
     * @param begstr 开始标志
     * @param endstr 结束标志
     * @param endstrs 结束标志数组（从第0个开始尝试匹配）
     * @param begNumber 第几个开始标志
     */
    static QString SubString(const QString& str, const QString& begstr, const QString& endstr, int begNumber = 0)
    {
        QVector<QString> endstrs = {endstr};
        return SubString(str, begstr, endstrs, begNumber);
    }

    static QString SubString(const QString& str, const QString& begstr, const QVector<QString>& endstrs, int begNumber = 0)
    {
        int begIndex = Find(str, begstr, begNumber);
        if (begIndex < 0)
            return "";
        begIndex += begstr.length();

        int endIndex = -1;
        foreach(QString item, endstrs)
        {
            endIndex = str.indexOf(item, begIndex);
            if (endIndex >= 0)
                break;
        }
        if (endIndex < 0)
            return "";

        return str.mid(begIndex, endIndex - begIndex);
    }

    /**
     * @brief 分割字符串
     * @param str 字符串
     * @param seps 多个分隔符
     */
    static QStringList Split(const QString& str, const QVector<QString>& seps)
    {
        QStringList array = {str};
        foreach (QString item, seps)
        {
            if (item.isEmpty())
                continue;

            QStringList subarray;
            foreach(QString sub, array)
            {
                subarray.append(sub.split(item));
            }

            array.append(subarray);
        }
        return array;
    }

    /**
     * @brief 合并
     */
    static QString Join(std::vector<std::string> array, QString sep)
    {
        QString ret = "";
        foreach(auto item, array)
        {
           ret.append(item.c_str());
           if (sep.length() > 0)
                ret += sep;
        }

        if (sep.length() > 0 && ret.endsWith(sep))
        {
            ret = ret.left(ret.length() - sep.length());
        }
        return ret;

    }


public:
    static bool ConvertToNumber(const QString& str, QString& value)
    {
        value = str;
        return true;
    }

    static bool ConvertToNumber(const QString& str, int& value)
    {
        bool success;
        value = str.toInt(&success);
        return success;
    }

    static bool ConvertToNumber(const QString& str, uint& value)
    {
        bool success;
        value = str.toUInt(&success);
        return success;
    }

    static bool ConvertToNumber(const QString& str, double& value)
    {
        bool success;
        value = str.toDouble(&success);
        return success;
    }
};
}

#endif
