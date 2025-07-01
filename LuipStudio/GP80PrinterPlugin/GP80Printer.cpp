#include "GP80Printer.h"
#include <QTextCodec>
#include <QImage>
#include "Log.h"

using PrinterSpace::Printer;

namespace PrinterSpace
{

GP80Printer::GP80Printer(QObject* parent): Printer(parent)
{
    m_socket = new QTcpSocket(this);
    m_serial = new QSerialPort(this);
}

/**
 * @brief 连接
 */
bool GP80Printer::Connect(QString argument, int connectType)
{
    char str[30] = "GP80Printer Connected!";
    m_connectType = (ConnectType)connectType;
    if(m_connectType == ConnectType::Serial)
    {
        if (m_serial == nullptr)
            return false;

        #ifdef _CS_X86_WINDOWS
           m_serial->setPortName("COM30");
        #endif

        #ifdef _CS_ARM_LINUX
            m_serial->setPortName("/dev/ttymxc3");
        #endif

        if (m_serial->open(QIODevice::ReadWrite))
        {
            m_serial->setBaudRate(QSerialPort::Baud115200);
            m_serial->setDataBits(QSerialPort::Data8);
            m_serial->setParity(QSerialPort::NoParity);
            m_serial->setStopBits(QSerialPort::OneStop);
            m_serial->setFlowControl(QSerialPort::NoFlowControl);
//            m_serial->write(str, qstrlen(str));
            return true;
        }
        else
        {
            logger->warn("打印机打开 %s 失败原因: %s", qPrintable(m_serial->portName()), qPrintable(m_serial->errorString()));
            return false;
        }
    }
    else
    {
        m_ipStr = argument;
        m_socket->connectToHost(m_ipStr, 9100, QIODevice::ReadWrite);
        if(!m_socket->waitForConnected(1000))
        {
            logger->warn("Printer connect to %s fail!", m_ipStr.toStdString().c_str());
            m_socket->close();
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 是否连接可用
 */
bool GP80Printer::IsConnected()
{
     if(m_connectType == ConnectType::Serial)
        return m_serial->isOpen();
     else
        return m_socket->isValid();
}

/**
 * @brief 关闭socket
 */
void GP80Printer::Close()
{
    if(m_connectType == ConnectType::Serial)
        m_serial->close();
    else
       m_socket->close();
}

/**
 * @brief 获取socket错误
 */
int GP80Printer::Error()
{
    return (int)m_socket->error();
}

/**
 * @brief 打印字符串链表
 */
bool GP80Printer::Print(QStringList& strList)
{
    if(!this->CmdSetup_InitPrinter())
    {
        return false;
    }

    this->CmdSetup_SetPrintMode((int)PrintCharMode::StandardFont);
    this->CmdChinese_EnableChineseMode();
    this->CmdChinese_SetChineseCharMode((int)ChineseCharMode::ChDefault);

    for(QStringList::Iterator it = strList.begin(); it != strList.end(); it++)
    {
        if(!this->WriteText(*it))
        {
            return false;
        }
    }

    this->CmdPrint_NewLine();

    return true;
}

/**
 * @brief 打印图片
 */
bool GP80Printer::PrintImage(QImage& img)
{
    int width = img.width();
    int height = img.height();
    int w = width / 8;
    int h = ((height + 23) / 24) * 24;

    if(!CmdBmp_PrintOpticalBmp(0, w%256, w/256, h%256, h/256, 0, 0))
    {
        return false;
    }

    for (int hi = 0; hi < h; hi++)
    {
        QByteArray cmd;

        for (int wi = 0; wi < w; wi++)
        {
            char byte = 0x00;

            for (int p = 0; p < 8; ++p)
            {
              int image_x = wi * 8 + p;
              int image_y = hi;
              int v;

              if (image_y >= height)
              {
                v = 0;
              }
              else
              {
                v = qBlue(img.pixel(image_x, image_y)) < 0xc0 ? 1 : 0;
              }
              byte |= (v << (7 - p));
            }
            cmd.append(byte);
        }

        if(!WriteBuffer(cmd.data(), cmd.size()))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief 切纸
 */
bool GP80Printer::CutPaper()
{
    return CmdCut_CutPaper();
}

/**
 * @brief 打印文本
 * @details 标准字体下，默认一行最多48个英文字符，24个中文字符
 */
bool GP80Printer::WriteText(QString str)
{
    bool ret = true;

    QTextCodec *codec = QTextCodec::codecForName("GB2312");
    QByteArray encodedString = codec->fromUnicode(str);

    memset(sendBuffer, 0, sizeof(sendBuffer));
    memcpy(sendBuffer, encodedString.data(), encodedString.length());

    int len = 0;
    if(m_connectType == ConnectType::Serial)
    {
        len = m_serial->write(sendBuffer, qstrlen(sendBuffer));
    }
    else
    {
        len = m_socket->write(sendBuffer, qstrlen(sendBuffer));
    }
    if(len < 0)
    {
        logger->warn("Printer write text fail! error = %d", Error());
        ret = false;
    }

    return ret;
}

/**
 * @brief 输出字符数组
 */
bool GP80Printer::WriteBuffer(const char* buffer, int length)
{
    bool ret = true;

    int len = 0;
    if(m_connectType == ConnectType::Serial)
    {
        len = m_serial->write(buffer, length);
    }
    else
    {
        len = m_socket->write(buffer, length);
    }
    if(len < 0)
    {
        logger->warn("Printer write buffer fail! error = %d", Error());
        ret = false;
    }

    return ret;
}

/**
 * @brief 读取字符串
 */
bool GP80Printer::ReadText(QString& str)
{
    memset(recvBuffer, 0, sizeof(recvBuffer));

    if(ReadBuffer(recvBuffer, GP80PRINTER_RECV_BUFFER_LEN))
    {
        str = QString::fromLatin1(recvBuffer, qstrlen(recvBuffer));

        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief 读取到缓冲区
 */
bool GP80Printer::ReadBuffer(char* buffer, int length)
{
    bool ret = false;

    if(m_socket->waitForReadyRead(1000))
    {
        int len = m_socket->read(buffer, length);
        if(len <= 0)
        {
            logger->warn("Printer read data len = %d. errno = %d", len, (int)Error());
            ret = false;
        }
        else
        {
            ret = true;
        }
    }

    return ret;
}

/**
 * @brief 打印换行
 * @note LF
 * @note 0x0A
 */
bool GP80Printer::CmdPrint_NewLine()
{
    char data[] = {0x0A};
    int len = 1;

    return WriteBuffer(data, len);
}

/**
 * @brief 水平定位
 * @note HT
 * @note 0x09
 */
bool GP80Printer::CmdPos_ReLocation()
{
    char data[] = {0x09};
    int len = 1;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置对齐方式
 * @note ESC  a    n
 * @note 0x1B 0x61 n
 */
bool GP80Printer::CmdPos_SetAlignType(AlignType type)
{
    char data[] = {0x1B, 0x61, (int)type};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置左边距
 * @note GS   L    nL  nH
 * @note 0x1D 0x4C nL  nH
 */
bool GP80Printer::CmdPos_SetLeftDistance(int nL, int nH)
{
    char data[] = {0x1D, 0x4C, nL, nH};
    int len = 4;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置打印区域宽度
 * @note GS   W    nL  nH
 * @note 0x1D 0x57 nL  nH
 */
bool GP80Printer::CmdPos_SetWidth(int nL, int nH)
{
    char data[] = {0x1D, 0x57, nL, nH};
    int len = 4;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置横向/纵向移动距离
 * @note GS   W    x  y
 * @note 0x1D 0x50 x  y
 */
bool GP80Printer::CmdPos_SetMoveUnit(int x, int y)
{
    char data[] = {0x1D, 0x50, x, y};
    int len = 4;

    return WriteBuffer(data, len);
}

/**
 * @brief 读取实时状态
 * @note DLE  EOT  n
 * @note 0x10 0x04 n
 */
char GP80Printer::CmdQuery_GetStatus(StatusType type)
{
    char byte = 0;
    char data[] = {0x10, 0x04, (int)type};
    int len = 3;

    if(WriteBuffer(data, len))
    {
        if(ReadBuffer(&byte, 1))
        {
            return byte;
        }
    }

    return byte;
}

/**
 * @brief 返回状态
 * @note GS   r    n
 * @note 0x1D 0x72 n
 */
char GP80Printer::CmdQuery_GetReturnStatus(int type)
{
    char byte = 0;
    char data[] = {0x1D, 0x72, (int)type};
    int len = 3;

    if(WriteBuffer(data, len))
    {
        if(ReadBuffer(&byte, 1))
        {
            return byte;
        }
    }

    return byte;
}

/**
 * @brief 查询打印机信息
 * @note GS   I    n
 * @note 0x1D 0x49 n
 */
bool GP80Printer::CmdQuery_GetInformation(PrinterInfo info)
{
    char data[] = {0x1D, 0x49, (int)info};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置默认行间距
 * @note ESC  2
 * @note 0x1B 0x32
 */
bool GP80Printer::CmdSpace_SetDefaultRowSpace()
{
    char data[] = {0x1B, 0x32};
    int len = 2;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置行间距
 * @note ESC  3    n
 * @note 0x1B 0x33 n
 */
bool GP80Printer::CmdSpace_SetRowSpace(int num)
{
    char data[] = {0x1B, 0x33, num};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 初始化打印机
 * @note ESC  @
 * @note 0x1B 0x40
 */
bool GP80Printer::CmdSetup_InitPrinter()
{
    char data[] = {0x1B, 0x40};
    int len = 2;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置打印模式
 * @note ESC  !    n
 * @note 0x1B 0x21 n
 * @see PrintCharMode
 */
bool GP80Printer::CmdSetup_SetPrintMode(int mode)
{
    char data[] = {0x1B, 0x21, mode};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置/取消下划线模式
 * @note ESC  -    n
 * @note 0x1B 0x2D n
 */
bool GP80Printer::CmdSetup_SetUnderline(bool enable)
{
    char data[] = {0x1B, 0x2D, (int)enable};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置/取消加粗模式
 * @note ESC  E    n
 * @note 0x1B 0x45 n
 */
bool GP80Printer::CmdSetup_SetBoldEnable(bool enable)
{
    char data[] = {0x1B, 0x45, (int)enable};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置/取消双重打印模式
 * @note ESC  G    n
 * @note 0x1B 0x47 n
 */
bool GP80Printer::CmdSetup_SetDoublePrint(bool enable)
{
    char data[] = {0x1B, 0x47, (int)enable};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 选择字体
 * @note ESC  M    n
 * @note 0x1B 0x4D n
 */
bool GP80Printer::CmdSetup_SetFont(PrintFont font)
{
    char data[] = {0x1B, 0x4D, (int)font};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置字符大小
 * @note GS   !    n
 * @note 0x1B 0x21 n
 */
bool GP80Printer::CmdSetup_SetCharSize(int wm, int hm)
{
    char value = (wm&0x7)<<3 | (hm&0x7);
    char data[] = {0x1D, 0x21, value};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 选择位图模式
 * @note ESC  *    m  nL  nH  d1...dk
 * @note 0x1B 0x2A m  nL  nH  d1...dk
 */
bool GP80Printer::CmdBmp_SetBmpMode(int m, int nL, int nH, const char* buffer, int len)
{
    char data[] = {0x1B, 0x2A, m, nL, nH};

    if(!WriteBuffer(data, 5))
    {
        return false;
    }

    memset(sendBuffer, 0, GP80PRINTER_SEND_BUFFER_LEN);

    int leftLen = len;
    while(leftLen > 0)
    {
        int sendLen = (leftLen > GP80PRINTER_SEND_BUFFER_LEN) ? GP80PRINTER_SEND_BUFFER_LEN : leftLen;

        memcpy(sendBuffer, buffer, sendLen);

        leftLen = leftLen - sendLen;

        if(!WriteBuffer(sendBuffer, sendLen))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief 打印光栅位图
 * @note GS   v    0    m  xL  xH  yL  yH  d1...dk
 * @note 0x1D 0x76 0x30 m  xL  xH  yL  yH  d1...dk
 */
bool GP80Printer::CmdBmp_PrintOpticalBmp(int m, int xL, int xH, int yL, int yH, const char* buffer, int len)
{
    char data[] = {0x1D, 0x76, 0x30, m, xL, xH, yL, yH};

    if(!WriteBuffer(data, 8))
    {
        return false;
    }

    memset(sendBuffer, 0, GP80PRINTER_SEND_BUFFER_LEN);

    int leftLen = len;
    while(leftLen > 0)
    {
        int sendLen = (leftLen > GP80PRINTER_SEND_BUFFER_LEN) ? GP80PRINTER_SEND_BUFFER_LEN : leftLen;

        memcpy(sendBuffer, buffer, sendLen);

        leftLen = leftLen - sendLen;

        if(!WriteBuffer(sendBuffer, sendLen))
        {
            return false;
        }
    }

    return true;
}

/**
 * @brief 设置汉字字符模式
 * @note FS   !    n
 * @note 0x1C 0x21 n
 * @see ChineseCharMode
 */
bool GP80Printer::CmdChinese_SetChineseCharMode(int mode)
{
    char data[] = {0x1C, 0x21, mode};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 选择汉字模式
 * @note FS   &
 * @note 0x1C 0x26
 */
bool GP80Printer::CmdChinese_EnableChineseMode()
{
    char data[] = {0x1C, 0x26};
    int len = 2;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置/取消汉字下划线模式
 * @note FS   -    n
 * @note 0x1C 0x2D n
 */
bool GP80Printer::CmdChinese_SetUnderline(bool enable)
{
    char data[] = {0x1C, 0x2D, (int)enable};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 取消汉字模式
 * @note FS   .
 * @note 0x1C 0x2E
 */
bool GP80Printer::CmdChinese_DisableChineseMode()
{
    char data[] = {0x1C, 0x2E};
    int len = 2;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置汉字字符左右间距
 * @note FS   S    n1 n2
 * @note 0x1C 0x53 n1 n2
 */
bool GP80Printer::CmdChinese_SetCharHorizontalSpace(int nleft, int nright)
{
    char data[] = {0x1C, 0x53, nleft, nright};
    int len = 4;

    return WriteBuffer(data, len);
}

/**
 * @brief 设置/取消汉字倍宽
 * @note FS   W    n
 * @note 0x1C 0x57 n
 */
bool GP80Printer::CmdChinese_SetChineseCharSize(int n)
{
    char data[] = {0x1C, 0x57, n};
    int len = 3;

    return WriteBuffer(data, len);
}

/**
 * @brief 切纸命令
 * @note GS   V    m n
 * @note 0x1D 0x56 m n
 * @note m  0,1,48,49 直接切纸
 * @note m  65,66 进纸n单位并切纸
 */
bool GP80Printer::CmdCut_CutPaper(int n)
{
    int m = 65;
    if(n < 0)
    {
        m = 0;
    }

    char data[] = {0x1D, 0x56, m, n};
    int len = 3;
    if(m == 65 || m == 66)
    {
        len = 4;
    }

    return WriteBuffer(data, len);
}

GP80Printer::~GP80Printer()
{
    if(m_socket != 0)
    {
        m_socket->close();
        delete m_socket;
    }
}

}
