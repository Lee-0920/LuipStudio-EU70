#ifndef GP80PRINTER_H
#define GP80PRINTER_H

#include "PrinterPlugin/Printer.h"
#include <QObject>
#include <QTcpSocket>
#include <QSerialPort>
#include <memory>

namespace PrinterSpace
{

#define GP80PRINTER_RECV_BUFFER_LEN 1024
#define GP80PRINTER_SEND_BUFFER_LEN 2048

enum class AlignType
{
    Left = 0,
    Center = 1,
    Right = 2,
};

enum class StatusType
{
    PrinterType = 1,        ///<打印机状态
    OfflineType = 2,        ///<脱机状态
    ErrorType = 3,          ///<错误状态
    SensorType = 4,         ///<传感器状态
};

enum class PrinterInfo
{
    Brand = 0x42,       ///<品牌
    Model = 0x43,       ///<型号
};

enum class PrintCharMode
{
    StandardFont = 0x0,     ///<标准字体
    CompressFont = 0x1,     ///<压缩字体
    Bold = 0x8,             ///<加粗
    MutipleHeight = 0x10,   ///<倍高
    MutipleWidth = 0x20,    ///<倍宽
    Underline = 0x80,       ///<下划线
};

enum class ChineseCharMode
{
    ChDefault = 0x1,        ///<默认
    ChMutipleWidth = 0x4,    ///<倍宽
    ChMutipleHeight = 0x8,   ///<倍高
    ChUnderline = 0x80,       ///<下划线
};

enum class PrintFont
{
    StandardFont = 0,       ///<标准字体
    CompressFont = 1,       ///<压缩字体
};

class LUIP_SHARE GP80Printer : public Printer
{
  Q_OBJECT
public:
    explicit GP80Printer(QObject* parent = 0);
    ~GP80Printer();

    bool Connect(QString argument, int connectType);
    bool IsConnected();
    void Close();
    int Error();

    bool Print(QStringList& strList);
    bool PrintImage(QImage& img);
    bool CutPaper();
private:
    bool WriteText(QString str);
    bool WriteBuffer(const char* buffer, int length);

    bool ReadText(QString& str);
    bool ReadBuffer(char* buffer, int length);

private:
    ///***打印命令***///
    bool CmdPrint_NewLine();  ///换行

    ///***位置命令***///
    bool CmdPos_ReLocation();   ///水平定位
    bool CmdPos_SetAlignType(AlignType type);   ///设置对齐方式
    bool CmdPos_SetLeftDistance(int nL, int nH);    ///设置左边距
    bool CmdPos_SetWidth(int nL, int nH);       ///设置打印区域宽度
    bool CmdPos_SetMoveUnit(int x, int y);      ///设置横向/纵向移动距离

    ///***查询与请求命令***///
    char CmdQuery_GetStatus(StatusType type);  ///立即获取当前状态
    char CmdQuery_GetReturnStatus(int type);   ///返回状态
    bool CmdQuery_GetInformation(PrinterInfo info);   ///查询打印机信息

    ///***行间距命令***///
    bool CmdSpace_SetDefaultRowSpace();  ///使用默认行间距 3.75mm
    bool CmdSpace_SetRowSpace(int num);     ///设置行间距

    ///***字符设定命令***///
    bool CmdSetup_InitPrinter();   ///初始化
    bool CmdSetup_SetPrintMode(int mode);   ///选择打印模式
    bool CmdSetup_SetUnderline(bool enable);   ///设置/取消下划线模式
    bool CmdSetup_SetBoldEnable(bool enable);   ///设置/取消加粗模式
    bool CmdSetup_SetDoublePrint(bool enable);  ///设置/取消双重打印模式
    bool CmdSetup_SetFont(PrintFont font);   ///选择字体 标准/压缩
    bool CmdSetup_SetCharSize(int wm, int hm);   ///设置字符大小

    ///***位图命令***///
    bool CmdBmp_SetBmpMode(int m, int nL, int nH, const char* buffer, int len);
    bool CmdBmp_PrintOpticalBmp(int m, int xL, int xH, int yL, int yH, const char* buffer, int len); ///打印光栅位图

    ///***汉字字符控制命令***///
    bool CmdChinese_SetChineseCharMode(int mode);       ///设置汉字字符模式
    bool CmdChinese_EnableChineseMode();        ///选择汉字模式 默认
    bool CmdChinese_SetUnderline(bool enable);  ///设置/取消汉字下划线模式
    bool CmdChinese_DisableChineseMode();       ///取消汉字模式
    bool CmdChinese_SetCharHorizontalSpace(int nleft, int nright);
    bool CmdChinese_SetChineseCharSize(int n);  ///设置汉字字符大小

    ///***切刀命令***///
    bool CmdCut_CutPaper(int n = 0);     ///切纸操作

private:
    QString m_ipStr;

    unsigned int m_serverIP;
    unsigned short m_serverPort;
    ConnectType m_connectType;
    QTcpSocket* m_socket;
    QSerialPort* m_serial;
    bool m_isConnected;

    char sendBuffer[GP80PRINTER_SEND_BUFFER_LEN];
    char recvBuffer[GP80PRINTER_RECV_BUFFER_LEN];
};

}
#endif // GP80PRINTER_H
