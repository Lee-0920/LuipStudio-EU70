#ifndef __TREASURE_SYSTEM_MATH__
#define __TREASURE_SYSTEM_MATH__

#include <math.h>
#include <ctime>
#include <QMap>
#include <QVector>
#include <QString>

#define trMath Treasure::Math

namespace Treasure
{

class Math
{

private:
    /**
     * @brief 设置种子
     */
    static void InitRandom()
    {
        static bool isInit = false;
        if (!isInit)
        {
            srand((unsigned int)time(0));
            isInit = true;
        }
    }

public:
    enum class Unit
    {
        Bit = 0,
        Byte,
        KB,
        MB,
        GB
    };

    static float UnitConver(float num, Unit src, Unit desc)
    {
        float ret = num;
        int isrc = (int)src;
        int idesc = (int)desc;
        for (; isrc > idesc; isrc--)
        {
            ret *= 1024;
        }
        for (; isrc < idesc; isrc++)
        {
            ret /= 1024;
        }
        return ret;
    }

public:
    /**
     * @brief 获取随机数
     */
    static int GetRandomInt(int min, int max)
    {
        InitRandom();
        if (min >= max)
            return min;
        int ret = rand() % (max - min) + min;
        return ret;
    }

    /**
     * @brief 获取随机数
     */
    static float GetRandomFloat(int min, int max)
    {
        int num = GetRandomInt(min, max);
        float val = (float)(rand()) / RAND_MAX;
        float retval = num + val;
        return retval;
    }



public:
    static bool IsIn(int value, int min, int max)
    {
        if (value >= min && value <= max)
            return true;
        return false;
    }

    static bool IsIn(QString svalue, int min, int max)
    {
        bool ok = false;
        int value = svalue.toInt(&ok);
        if (!ok)
            return false;

        if (value >= min && value <= max)
            return true;
        return false;
    }

    static int Max(int item1, int item2)
    {
        return item1 > item2 ? item1 : item2;
    }

    static int Min(int item1, int item2)
    {
        return item1 > item2 ? item2 : item1;
    }

    /**
     * @brief 输出十六进制字符串
     */
    static QString ToHexString(unsigned char* buffer, int len, QString sep = "")
    {
        QStringList array;
        for (int i = 0; i < len; i++)
        {
            array.append(ToHexString(buffer[i]));
        }
        return array.join(sep);
    }

    /**
     * @brief 十进制转十六进制字符串
     */
    static QString ToHexString(unsigned char value)
    {
        return QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
    }

    /**
     * @brief 十进制转十六进制字符串
     */
    static QString ToHexString(unsigned short value)
    {
        return QString("%1").arg(value, 4, 16, QLatin1Char('0')).toUpper();
    }

    /**
     * @brief 十进制转十六进制字符串
     */
    static QString ToHexString(unsigned int value)
    {
        return QString("%1").arg(value, 8, 16, QLatin1Char('0')).toUpper();
    }

    /**
     * @brief HEX转十六进制字符串
     */
    static QString ToHexString(QByteArray value)
    {
        return QString::fromStdString(value.toHex().toStdString()).toUpper();
    }

    /**
     * @brief 数字字符串转HEX
     */
    static QByteArray ToHexByteArray(QString value)
    {
        return QByteArray::fromHex(value.toStdString().c_str());
    }

    static QByteArray ToHexByteArray(QList<uchar> values)
    {
        QByteArray data;
        foreach(auto item, values)
        {
            data.append(item);
        }
        return data;
    }

    static QByteArray ToHexByteArray(ushort value)
    {
        QByteArray data;
        data.append((value & 0xFF00) >> 8);
        data.append(value & 0x00FF);
        return data;
    }

    static QByteArray ToHexByteArray(uint value)
    {
        QByteArray data;
        data.append((value & 0xFF000000) >> 24);
        data.append((value & 0xFF0000) >> 16);
        data.append((value & 0xFF00) >> 8);
        data.append(value & 0xFF);
        return data;
    }

    static QByteArray ToHexByteArray(uint64_t value)
    {
        QByteArray data;
        data.append((value & 0xFF00000000000000) >> 56);
        data.append((value & 0xFF000000000000) >> 48);
        data.append((value & 0xFF0000000000) >> 40);
        data.append((value & 0xFF00000000) >> 32);
        data.append((value & 0xFF000000) >> 24);
        data.append((value & 0xFF0000) >> 16);
        data.append((value & 0xFF00) >> 8);
        data.append((value & 0xFF));
        return data;
    }

    /**
     * @brief 十六进制字符串转十进制
     */
    static unsigned int ToUInt(QString hexString)
    {
        unsigned int value = 0;
        sscanf(hexString.toStdString().c_str(), "%x", &value);
        return value;
    }

    /**
     * @brief short转十六进制
     */
    static void ToHexCharArray(unsigned short value, unsigned char* buffer, int len)
    {
        if (len < 2)
            return;

        buffer[0] = (value & 0xFF00) >> 8;
        buffer[1] = (value & 0x00FF);
    }

    /**
     * @brief int转十六进制
     */
    static void ToHexCharArray(unsigned int value, unsigned char* buffer, int len)
    {
        if (len < 4)
            return;

        buffer[0] = (value & 0xFF000000) >> 24;
        buffer[1] = (value & 0x00FF0000) >> 16;
        buffer[2] = (value & 0x0000FF00) >> 8;
        buffer[3] = (value & 0x000000FF);
    }

    static void ToHexCharArray(QString hexString, unsigned char* buffer, int len)
    {
        if (len < 0)
            return;

        auto data = QByteArray::fromHex(hexString.toStdString().c_str());
        int minLen = Min(data.size(), len);
        memcpy(buffer, data.data(), minLen);
    }

    /**
     * @brief 16进制小数转10进制
     */
    static double Decimal16To10(uint value)
    {
        double lowV = 0;
        for (int i = 0; i < 8; i++)
        {
            double tmp = (value >> ((7 - i) * 4)) & 0xf;
            lowV += tmp / pow(16, i + 1);
        }

        return lowV;
    }

};
}

#endif
