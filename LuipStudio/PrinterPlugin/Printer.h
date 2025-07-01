#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include "LuipShare.h"

enum class ConnectType
{
    Serial = 0,       ///<串口连接
    Network = 1,      ///<IP连接
};

namespace PrinterSpace
{

class LUIP_SHARE Printer : public QObject
{
  Q_OBJECT
public:
    explicit Printer(QObject* parent = 0);
    ~Printer();

    virtual bool Connect(QString argument, int connectType);
    virtual bool IsConnected();
    virtual void Close();
    virtual int  Error();

    virtual bool Print(QStringList& strList);
    virtual bool PrintImage(QImage& img);
    virtual bool CutPaper();
};

}
#endif // PRINTER_H
