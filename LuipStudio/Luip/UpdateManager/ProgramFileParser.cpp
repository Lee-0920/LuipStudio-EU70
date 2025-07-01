
#include <QFile>
#include <QDebug>
#include "Log.h"
#include "System/Hash/md5.h"
#include <QStringList>
#include "ProgramFileParser.h"

namespace Updater
{

//************************************************************************************
// HEX文件格式   :llaaaatt[dd...]cc
// 其中 ： 代表每一行新的数据
// ll 是数据长度域（length），它代表记录当中数据字节（dd...）的数量，2个ASCII字符长度（1个字节的BCD码）.
// aaaa 是地址域（address），它代表记录当中数据的起始地址，4个ASCII字符长度（2个字节的BCD码）.
// tt 是代表HEX记录类型（type）的域，2个ASCII字符长度（1个字节的BCD码）.
// dd 是数据域（data），它代表一个字节的数据.一个记录可以有许多数据字节.记录当中数据字节的数量必须和数据长度域（ll）中指定的数字相符.
// cc 是校验和域(check)，它表示这个记录的校验和，2个ASCII字符长度（1个字节的BCD码）.

//// HEX文件每一行的数据类型判断
#define NORMAL_DATA              0       // 正常的数据类型
#define END_OF_FILE              1       // 文件结束标志
#define ADDRESS_EXTEND           4       // 地址扩展标志

// HEX文件偏移地址
#define FILE_LEN_OFFSET          1       // HEX文件长度偏移，以ASCII字符为单位
#define FILE_ADDRESS_HIGH_OFFSET 3       // HEX文件高地址偏移，以ASCII字符为单位
#define FILE_ADDRESS_LOW_OFFSET  5       // HEX文件低地址偏移，以ASCII字符为单位
#define FILE_TYPE_OFFSET         7       // HEX文件数据类型偏移，以ASCII字符为单位
#define FILE_DATA_OFFSET         9       // HEX文件数据内容偏移，以ASCII字符为单位，第10个字符为数据起始位置，对于数组，需要偏移9个字符

// 升级文件配置项
#define  CONFIG_DELIMITER   '='          // 配置项：分隔符
#define  CONFIG_TYPE        "type"       // 配置项：板卡类型
#define  CONFIG_MODE        "mode"       // 配置项：升级文件模式（application或者updater模式）
#define  CONFIG_VERSION     "version"    // 配置项：版本号
#define  CONFIG_HASH        "hash"       // 配置项：hash校验数值

ProgramFileParser::ProgramFileParser(String fileName)
{
    m_fileDataLen = 0;
    m_version = new String;
    m_mode = new String;
    m_type = new String;
    m_fileName = fileName;
}

ProgramFileParser::~ProgramFileParser()
{
    if (m_version)
        delete m_version;
    if (m_mode)
        delete m_mode;
    if (m_type)
        delete m_type;

    for(vector<ProgramSegment *>::iterator it = m_segments.begin(); it != m_segments.end(); it++)
    {
        if (*it != NULL)
        {
            if (*it)
            delete (*it);
        }
    }
}

/**
 * @brief 2个字符串转换为1个字节的数字。
 * @details 每2个字符可以转换为一个BCD码字节数。
 * @param[out] unsigned char*destData 转换后的结果
 * @param[in]  String buff 需要转换的缓冲字符。缓冲字符可以是多个字符
 * return  ret 转换的结果，转换成返回TRUE
 */
bool ProgramFileParser::TwoStringToAByte(unsigned char* destData, QString buff)
{
    bool ret;

    QString str = buff.mid(0, 2);//获取缓冲字符的前两个字符用于转换
    *destData =  str.toInt(&ret, 16);//把两个字符转换成功对应的16进制数

    return ret;
}

/**
* @brief 字符串转换为BCD码数据。
* @details 将字符串数据转换为BCD吗数据，已转换的数据长度保存在fileDataLen之中。
* @param[out] Uint8* destData 转换之后存储的位置
* @param[in] char* buff 需要转换的ASCII字符。
* @param[in] len 数据长度，以ASCII为单位，如果数据长度为2个字节，则入口参数buf应该为4个ASCII字符。
* @return ret 如果转换成功，则返回为TRUE，如果失败，返回FALSE。
*/
bool ProgramFileParser::StringToData(unsigned char * destData, QString buff, unsigned long  len)
{
    bool ret = TRUE;
    unsigned long  pos = 0;
    while(1)
    {
        if (TRUE == TwoStringToAByte(&destData[m_fileDataLen], buff.mid(pos, 2)))
        {
            pos += 2;
            m_fileDataLen++;
            if (pos == (len - 1))
            {
                break;
            }
            else if (pos >= len)
            {
                ret = FALSE;
                break;
            }
            if ((buff.at(pos) == '\n') || (buff.at(pos) == '\r'))
            {
                break;
            }
        }
        else
        {
            ret = FALSE;
            break;
        }
    }
    return ret;
}

/**
* @brief 解析整个HEX文件。除了文件头
* @details 每一行解析文件，将文件解析之后，保存在segments。如果数据的地址都是连续，则设置为只有一段数据。
* 如果数据地址不连续，则每一份不连续的数据，都记录为一段数据。
* return bool ret 如果文件存在且打开成功，则返回TRUE，否则返回FALSE。或者解析的文件不是完整的HEX文件，也返回FALSE
*/
bool ProgramFileParser::ParseProgram(void)
{
    QString buffer;//一行字符串的缓冲区
    QString::size_type len;//一行字符串的长度
    bool ret = TRUE;
    unsigned char dataSize = 0;//从hex文件解析出一行里包含的数据个数，以字节为单位
    unsigned char dataType = 0;//从hex文件解析出一行字符串的类型
    unsigned char highAddress, lowAddress;//文件行字符串里的高8位地址和低8位地址
    unsigned long  oldAddress = 0;//上一行地址
    unsigned long  newAddress = 0;//当前行地址
    unsigned long  addressOffset = 0;
    unsigned long  nextSegmentAddr = 0;//下一段数据的起始地址
    unsigned long  count = 0;//正常数据行字符串的计数
    long lengthOffset = 0;//保存本行地址与上一行地址的差，用于判断数据是否不连续
    long segmentOffset = 0;
    unsigned char* tempData = new unsigned char[1024 * 1024];//转换的纯数据缓冲区
    QFile file(m_fileName.c_str());

    if (file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            // 读取一行文件
            buffer = in.readLine();

            len = buffer.size();
            if (len > 0)
            {
                if (buffer.at(0) == ':')//识别是否是行开始，可以用于区分文件头和hex原有内容
                {
                    TwoStringToAByte(&dataType, buffer.mid(FILE_TYPE_OFFSET, 2));
//                    qDebug() << "\n dataType" << dataType << endl;
                    switch (dataType)
                    {
                        case NORMAL_DATA:
                            // 正常的数据
                            // 获取当前的地址: 待解析文件的地址 + 地址偏移
                            TwoStringToAByte(&highAddress, buffer.mid(FILE_ADDRESS_HIGH_OFFSET, 2));
                            TwoStringToAByte(&lowAddress, buffer.mid(FILE_ADDRESS_LOW_OFFSET, 2));
                            newAddress = (unsigned long )(highAddress << 8) + lowAddress + addressOffset;
                            lengthOffset = newAddress - oldAddress - dataSize;  // 记录数据是否不连续，偏移了多少字节
                            if (count == 0)
                            {
                                // 记录起始地址
                                nextSegmentAddr = newAddress;
                            }
                            else
                            {
                                if (lengthOffset != 0)
                                {
                                    // 地址不连续，分段记录数据包：地址、偏移位置、长度
                                    m_segments.push_back(new ProgramSegment(tempData+ segmentOffset, m_fileDataLen - segmentOffset, nextSegmentAddr));
                                    segmentOffset = m_fileDataLen;
                                    nextSegmentAddr = newAddress;
                                }
                            }
                            TwoStringToAByte(&dataSize, buffer.mid(FILE_LEN_OFFSET, 2)); // 获取数据长度，长度以字节为单位
                            // 字符串数据转换为数据，保存在tempData,位置由fileDataLen确定
                            StringToData(tempData, buffer.mid(FILE_DATA_OFFSET), dataSize * 2);//这里虽然传入的数据包含有校验和回车换行，但传入的长度是不包含的，所有不会处理
                            oldAddress = newAddress;
                            count++;
                            break;
                        case ADDRESS_EXTEND:
                            // 地址拓展，地址拓展固定为2个字节参数
                            TwoStringToAByte(&highAddress, buffer.mid(FILE_DATA_OFFSET, 2));
                            TwoStringToAByte(&lowAddress, buffer.mid(FILE_DATA_OFFSET + 2, 2));
                            addressOffset = (unsigned long )highAddress << 8 | lowAddress;
                            addressOffset <<= 16;  // 地址拓展固定偏移参数，偏移64K = 2^16，固定左移16位
                            break;
                        case END_OF_FILE:
                            // 文件读取结束
                            m_segments.push_back(new ProgramSegment(tempData + segmentOffset, m_fileDataLen- segmentOffset, nextSegmentAddr));
                            break;
                        default:
//                            qDebug() << "\n Hex file data type error, type =" << dataType << endl;
                            break;
                    }
                }

            }
        }
        file.close();
    }// end of if (file.open(QFile::ReadOnly))
    else
    {
        ret = FALSE;
    }

    if (tempData)
    {
        delete[] tempData;
    }

    return ret;
}

vector<ProgramSegment*> ProgramFileParser::GetSegmentData(void)
{
//    qDebug() << "\n total segment = "<< m_segments.size() <<endl;
//    for(vector<ProgramSegment *>::iterator it = m_segments.begin(); it != m_segments.end(); it++)
//    {
//        if (*it != NULL)
//        {
//            qDebug("\n Segment address = 0x%x length = %d", (*it)->GetAddress(), (*it)->GetLength());
//        }
//    }
    return m_segments;
}


void ProgramFileParser::Trim(QString* str)
{
    QString temp = str->trimmed();
    *str = temp;
}

///**
//* @brief 解析文件头
//* @details 1、判断文件是否存在，如果存在，则继续检验；如果不存在或者不能打开，则直接返回。
//* 2、通过hash算法的MD5校验，判断文件是否完整。
//* 3、获取文件的配置信息：板卡类型、板卡型号、软件版本号。
//* @return bool ret 返回解析结果，共有如下几种结果：TRUE:解析成功，FALSE：打开文件不成功,或者校验失败。
//*/
bool ProgramFileParser::ParseHeader(void)
{
    bool ret = TRUE;
    unsigned char hashCheck[16];//通过MD5校验算法计算除了文件头其他内容得到的MD5校验
    unsigned char hash[16];//从文件中读取的MD5校验
    QString buffer;
    bool isDataItemStart = FALSE;// 数据项是否开始，获取到文件的MD5后，文件的后面内容都算数据项
    int i = 0;
    QFile file(m_fileName.c_str());
    QString::size_type len;
    QString result;

    if (file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
         while (!in.atEnd())
        {
            // 读取一行文件
            buffer = in.readLine();
            len = buffer.size();
            if (len > 0)
            {
                result = buffer.section(CONFIG_DELIMITER,0,0);//字符串分割，取第0段
                if (result != NULL)
                {
                    Trim(&result);
                    if (CONFIG_TYPE == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);//字符串分割，取第1段
                        Trim(&result);//删除字符串前后的空白符(空格，回车，TAB等)
                        *m_type = result.toStdString();
                    }
                    else if (CONFIG_MODE == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);
                        Trim(&result);
                        *m_mode = result.toStdString();

                    }
                    else if (CONFIG_VERSION == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);
                        Trim(&result);
                        *m_version = result.toStdString();
                    }
                    else if (CONFIG_HASH == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);//获取文本里的MD5校验
                        Trim(&result);
//                        qDebug() << "\n CONFIG_HASH"<< endl;
                        for (i = 0; i < 16; i++)
                        {
                            TwoStringToAByte(&hash[i], result.mid(i * 2, 2));
//                            qDebug(" 0x%x", hash[i]);
                        }
                        isDataItemStart = TRUE;
                    }
                }
            }
            if (isDataItemStart == TRUE)
            {
                QByteArray str;//文件数据类型转换中间变量
                char* input;//用于MD5计算数据
                MD5_CTX md5c;

                MD5Init(&md5c);

                while (!in.atEnd())
                {
                    buffer = in.read(1024);//紧接着已读取的内容读取剩余的数据项
                    len = buffer.size();
                    //QString转char *
                    str = buffer.toLatin1();
                    input = str.data();

                    MD5Update(&md5c, (unsigned char*)input, len);
                }
                MD5Final(hashCheck, &md5c);

//                qDebug() << "\n\r hashCheck"<< endl;
                for (i = 0; i < 16; i++)//MD5对比
                {
//                     qDebug(" 0x%x", *(hashCheck + i));
                    if (*(hashCheck + i) != hash[i] )
                    {
                        ret = FALSE;
//                        qDebug() << "\n File " << m_fileName << "hash check error" << endl;
                        QString str = "{update} " + QString(m_fileName.c_str()) + " hash check error";
                        logger->warn(str.toStdString());
                    }
                }
                isDataItemStart = FALSE;
                break;
            }
        }
        file.close();
    }
    else
    {
//        qDebug() << "\n Check Can not find or open file " << m_fileName << endl;
        ret = FALSE;
    }

    return ret;
}

/**
* @brief 获取设备板卡类型。
* @details 请在调用ParseHeader()且返回值为OK之后（即成功解析升级文件），才能调用本函数。
* @return char* type 设备型号，String格式。
*/
String ProgramFileParser::GetType(void)
{
//    qDebug() << "\n Type is:" << *m_type << endl;
    return *m_type;
}

/**
* @brief 获取文件运行模式。
* @details 文件运行模式共计两种:application、updater。
* @return char* mode 运行模式，String格式，
*/
String ProgramFileParser::GetMode(void)
{
//    qDebug() << "\n Mode is:" << *m_mode << endl;
    return *m_mode;
}

/**
* @brief 获取软件版本号。
* @details 请在调用UpgradeFile_Parse()且返回值为OK之后（即成功解析升级文件），才能调用本函数。
* @return char* version 软件版本号，String格式
*/
String ProgramFileParser::GetVersion(void)
{
//    qDebug() << "\n Version is:" << *m_version << endl;
    return *m_version;
}

}
