#include "Printer.h"

namespace PrinterSpace
{

Printer::Printer(QObject* parent): QObject(parent)
{

}

bool Printer::Connect(QString argument, int connectType)
{
    return false;
}

bool Printer::IsConnected()
{
    return false;
}

void Printer::Close()
{

}

int  Printer::Error()
{
    return 0;
}

bool Printer::Print(QStringList& strList)
{
    return false;
}

bool Printer::PrintImage(QImage& img)
{
    return false;
}

bool Printer::CutPaper()
{
    return false;
}

Printer::~Printer()
{

}

}
