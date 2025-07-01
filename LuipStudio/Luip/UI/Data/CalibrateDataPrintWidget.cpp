#include "CalibrateDataPrintWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include "PrinterManager/PrinterManager.h"
#include "ResultManager/RecordFields.h"
#include "UI/Frame/Common.h"
#include "LuaEngine/LuaEngine.h"
#include "oolua.h"
#include "Lua/App.h"
#include <QBitmap>
#include <QImage>
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Setting/SettingManager.h"
#include "System/Translate.h"
#include "Setting/Environment.h"

#define SHOW_LINE 10

using namespace std;
using namespace Result;
using namespace PrinterSpace;
using namespace OOLUA;
using namespace Lua;
using namespace System;
using namespace Configuration;

namespace UI
{

CalibrateDataPrintWidget::CalibrateDataPrintWidget(String fileName, QWidget *parent) : QWidget(parent),
    m_fileName(fileName)
{
    this->setFixedSize(540, 420);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    this->setWindowModality(Qt::WindowModal);

    UpdateWidgetManager::Instance()->Register(this);

    QFont font = this->font();
    font.setPointSize(12);
    this->setFont(font);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script * lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    m_ipLabel = new QLabel();
    m_ipLabel->setText(tr("打印机地址"));
    m_ipLabel->setFixedSize(100,40);

    m_ipEdit = new QLineEdit();
    m_ipEdit->setText("");
    m_ipEdit->setFixedSize(160,40);
    m_ipEdit->installEventFilter(CNumberKeyboard::Instance());
    m_ipEdit->setEnabled(false);

    m_connectButton = new QPushButton();
    m_connectButton->setObjectName("brownButton");
    m_connectButton->setText(tr("未连接"));
    m_connectButton->setFixedSize(80,40);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(m_ipLabel);
    topLayout->addWidget(m_ipEdit);
    topLayout->addWidget(m_connectButton);

    m_configGroup = new QGroupBox();
    m_configGroup->setLayout(topLayout);
    m_configGroup->setFixedSize(520,100);
    m_configGroup->setContentsMargins(0,0,0,0);

    m_beginTimeLabel = new QLabel();
    m_beginTimeLabel->setText(tr("起始时间"));
    m_beginTimeLabel->setFixedSize(154, 36);
    m_beginTimeLabel->setFont(font);
    m_beginTimeLabel->setAlignment(Qt::AlignCenter);

    m_beginTimeEdit = new QDateTimeEdit();
    m_beginTimeEdit->setFixedSize(154, 36);
    m_beginTimeEdit->setFont(font);
    m_beginTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
    m_beginTimeEdit->setDateTime(QDateTime(QDate::currentDate().addDays(-14), QTime(0, 0)));

    m_endTimeLabel = new QLabel();
    m_endTimeLabel->setText(tr("结束时间"));
    m_endTimeLabel->setFixedSize(154, 36);
    m_endTimeLabel->setFont(font);
    m_endTimeLabel->setAlignment(Qt::AlignCenter);

    m_limitLabel = new QLabel();
    m_limitLabel->setText(tr("数量限制(条)"));
    m_limitLabel->setFixedSize(120,36);
    m_endTimeLabel->setFont(font);
    m_limitLabel->setAlignment(Qt::AlignCenter);

    m_limitEdit = new QLineEdit();
    m_limitEdit->setText("10");
    m_limitEdit->setFixedSize(80,36);
    m_limitEdit->setAlignment(Qt::AlignCenter);
    m_limitEdit->installEventFilter(CNumberKeyboard::Instance());
//    m_limitEdit->setEnabled(false);

    m_endTimeEdit = new QDateTimeEdit();
    m_endTimeEdit->setFixedSize(154, 36);
    m_endTimeEdit->setFont(font);
    m_endTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm");
//    m_endTimeEdit->setDateTime(QDateTime(QDate::currentDate(), QTime(QTime::currentTime().hour(), 0)));
    m_endTimeEdit->setDateTime(QDateTime::currentDateTime());

    m_txtLog = new TextfileParser(QString(Environment::Instance()->GetAppDataPath().c_str()) + QString("/") + fileName.c_str());

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->addWidget(m_beginTimeLabel);
    labelLayout->addWidget(m_endTimeLabel);
    labelLayout->addWidget(m_limitLabel);
    labelLayout->setSpacing(10);

    QHBoxLayout* editLayout = new QHBoxLayout();
    editLayout->addWidget(m_beginTimeEdit);
    editLayout->addWidget(m_endTimeEdit);
    editLayout->addWidget(m_limitEdit);
    editLayout->setSpacing(10);

    m_headButton = new QPushButton();
    m_headButton->setObjectName("brownButton");
    m_headButton->setText(tr("表头"));
    m_headButton->setFixedSize(80,40);

    m_cutButton = new QPushButton();
    m_cutButton->setObjectName("brownButton");
    m_cutButton->setText(tr("切纸"));
    m_cutButton->setFixedSize(80,40);

    m_printButton = new QPushButton();
    m_printButton->setObjectName("brownButton");
    m_printButton->setText(tr("打印"));
    m_printButton->setFixedSize(80,40);

    m_closeButton = new QPushButton();
    m_closeButton->setObjectName("brownButton");
    m_closeButton->setText(tr("关闭"));
    m_closeButton->setFixedSize(80,40);
    m_closeButton->setFocus();
    m_closeButton->setDefault(true);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_headButton);
    buttonLayout->addWidget(m_cutButton);
    buttonLayout->addWidget(m_printButton);
    buttonLayout->addWidget(m_closeButton);

    QVBoxLayout* printLayout = new QVBoxLayout();
    printLayout->addLayout(labelLayout);
    printLayout->addLayout(editLayout);
    printLayout->addStretch();
    printLayout->addLayout(buttonLayout);

    m_printGroup = new QGroupBox();
    m_printGroup->setLayout(printLayout);
    m_printGroup->setFixedSize(520,200);
    m_printGroup->setContentsMargins(0,0,0,0);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_configGroup, 0, Qt::AlignCenter);
    mainLayout->addWidget(m_printGroup, 0, Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 100, 0, 20);

    this->setLayout(mainLayout);

    m_printer = PrinterManager::Instance()->GetMeasureDataPrinter();

    m_resultFiles = ResultManager::Instance()->GetCalibrateRecordFile(m_fileName);

    luaEngine->GetLuaValue(state, "setting.ui.measureDataPrint.measure.totalWidth", m_totalWidth);

    Table table;
    luaEngine->GetLuaValue(state, "setting.ui.measureDataPrint.calibrate.data", table);

    oolua_ipairs(table)
    {
        Table itemTable;
        lua->pull(itemTable);

        PrintItem item;

        itemTable.at("name", item.name);
        itemTable.at("header", item.header);
        itemTable.safe_at("format", item.format);
        itemTable.safe_at("content", item.content);
        item.isUnitChange = itemTable.safe_at("unitChange", item.unitChangeFunc);
        if (item.isUnitChange)
        {
            itemTable.safe_at("unit", item.unit);
        }
        itemTable.at("width", item.width);

//        qDebug()<<item.name.c_str();

        m_printItems.push_back(item);
    }
    oolua_ipairs_end()

    connect(m_connectButton, SIGNAL(clicked(bool)), this, SLOT(SlotConnectButton()));
    connect(m_printButton, SIGNAL(clicked(bool)), this, SLOT(SlotPrintButton()));
    connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(SlotCloseButton()));
    connect(m_cutButton, SIGNAL(clicked(bool)), this, SLOT(SlotCutButton()));
    connect(m_headButton, SIGNAL(clicked(bool)), this, SLOT(SlotHeadButton()));
    connect(m_beginTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(SlotBeginTimeCheck(QDateTime)));
    connect(m_endTimeEdit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(SlotEndTimeCheck(QDateTime)));
    connect(m_limitEdit, SIGNAL(textChanged(const QString)), this, SLOT(SlotPrintLimitCheck(const QString)));
}

QStringList CalibrateDataPrintWidget::SetDate(QDateTime& theMinDateTime, QDateTime& theMaxDateTime, QStringList& strlist, QString pattern)
{
    QStringList newStrlist;
    for(int i = 0; i < strlist.size() - 1 ; i++)
    {
        QString m = strlist.at(i);
        QRegExp rx(pattern);
        m.indexOf(rx);
        QDateTime dateTime = QDateTime::fromString(rx.cap(1), "yyyy-MM-dd hh:mm:ss");
        if(rx.cap(1).size() > 1 && rx.cap(2).size() > 1 && rx.cap(3).size() > 1)
        {
            if((dateTime > theMinDateTime) && (dateTime < theMaxDateTime))
            {
                newStrlist.append(m);
            }
        }
    }
    return newStrlist;
}

void CalibrateDataPrintWidget::SlotPrintButton()
{
    QStringList headList;
    QStringList printList;

    if(!m_printer->IsConnected())
    {
        MessageDialog msg(tr("打印机未连接!\n"), this,MsgStyle::ONLYOK);
        msg.exec();

        return;
    }

//    QString headStr = GetPrintHeadString();
//    headList.push_back(headStr);

//    char lineArray[m_totalWidth+1];
//    memset(lineArray, '-', m_totalWidth);
//    lineArray[m_totalWidth] = '\0';

//    QString lineStr = QString::fromStdString(std::string(lineArray));
//    headList.push_back(lineStr);

    int printerCount = 0;
    int printerLimit =  m_limitEdit->text().toInt();
    QDateTime curTime;

    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    while(printerCount < printerLimit && m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))
    {
        QString printStr;
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        int time;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), "dateTime", time);

        curTime = QDateTime::fromTime_t(time);

        qDebug()<< curTime.toString("yyyy-MM-dd hh:mm:ss");

        if ((curTime >= m_beginTimeEdit->dateTime())
            && (curTime <= m_endTimeEdit->dateTime()))
        {
            printStr = GetPrintCalibrateString();// GetPrintItemString GetPrintCalibrateString
        }
        if(!printStr.isEmpty())
        {
            printList.push_back(printStr);
        }
        qDebug() << printList;
        if(!printList.isEmpty())
        {
            if(!m_printer->Print(printList))
            {
                MessageDialog msg(tr("打印机错误\n")+tr("错误代码:")+QString::number(m_printer->Error()), this, MsgStyle::ONLYOK);
                msg.exec();

                return;
            }
        }
        printList.clear();
        printerCount++;
    }


//    QString pattern("(.*)( .*)( .*)");
//    int row = 0;
//    int currentBlock = 0;

////    QDateTime theMinDateTime  = QDateTime::fromString(minTime->text() + " 00:00:00", "yyyy-MM-dd hh:mm:ss");
////    QDateTime theMaxDateTime  = QDateTime::fromString(maxTime->text() + " 23:59:59", "yyyy-MM-dd hh:mm:ss");

//    QDateTime theMinDateTime = m_beginTimeEdit->dateTime();
//    QDateTime theMaxDateTime = m_endTimeEdit->dateTime();
//    int printerLimit =  m_limitEdit->text().toInt();
//    while(row < printerLimit && currentBlock < m_txtLog->TotalBlock())
//    {
//        char *s = m_txtLog->SeekBlock(currentBlock);
//        QString str(s);
//        QStringList strlist;
//        QStringList newStrlist;
//        strlist = str.split("\r\n");

//        newStrlist = SetDate(theMinDateTime, theMaxDateTime, strlist, pattern);

//        currentBlock = newStrlist.size();

//        while(currentBlock > 0 && row < printerLimit)
//        {
//            currentBlock--;
//            QString m = newStrlist.at(currentBlock);
//            if(m.size() > 0)
//            {
//                QRegExp rx(pattern);
//                m.indexOf(rx);
//                if(rx.cap(1).size() > 1 && rx.cap(2).size() > 1 && rx.cap(3).size() > 1)
//                {
//                    QFont font;
//                    font.setPointSize(12);
//                    printList.append(rx.cap(1).trimmed());
//                    printList.append(rx.cap(2).trimmed());
//                    printList.append(rx.cap(3).trimmed());
//                    row++;
//                }
//            }

//            if(!printList.isEmpty())
//            {
//                printList.push_back(QString("\n"));
//            }

//        //    QBitmap map(":/img/img/logo_576_120.bmp");
//        //    QImage img = map.toImage();

//            if(!printList.isEmpty())
//            {
//                if(!m_printer->Print(printList))
//                {
//                    MessageDialog msg(tr("打印机错误\n")+tr("错误代码:")+QString::number(m_printer->Error()), this, MsgStyle::ONLYOK);
//                    msg.exec();

//                    return;
//                }
//            }
//            printList.clear();
//        }

//        if(row < printerLimit)
//        {
//            if(currentBlock + 1 < m_txtLog->TotalBlock())
//            {
//                currentBlock++;
//            }
//            else
//            {
//                break;
//            }
//        }
//    }
}

QString CalibrateDataPrintWidget::GetPrinterAddress()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    String address;
    luaEngine->GetLuaValue(state, "config.system.printer.address", address);

    return QString::fromStdString(address);
}

int CalibrateDataPrintWidget::GetPrinterConnectType()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    int connectType;
    luaEngine->GetLuaValue(state, "config.system.printer.connectType", connectType);

    return connectType;
}

QString CalibrateDataPrintWidget::GetPrintHeadString()
{
    QString headStr;
    OOLUA::Script* lua = LuaEngine::Instance()->GetEngine();
    for(QVector<PrintItem>::Iterator iter = m_printItems.begin(); iter != m_printItems.end(); iter++)
    {
        if (iter->isUnitChange)
        {
            String unit;
            lua->call(iter->unitChangeFunc, iter->unit, UnitChange::Read);
            lua->pull(unit);

            QString str = QString::fromStdString(iter->header).remove(QChar(' '));
            str += QString::fromStdString(unit);
            str.insert(0, QChar(' '));
            str.append(QChar(' '));
            headStr += str;
        }
        else
        {
            headStr += QString::fromStdString(iter->header);
        }
    }
    headStr += "\0";

    return headStr;
}

QString CalibrateDataPrintWidget::GetPrintCalibrateString()
{
    wchar_t strBuff[m_totalWidth+1];
    wmemset(strBuff, ' ', m_totalWidth);
    strBuff[m_totalWidth] = '\0';
    QString printStr;

    int offset = 0;
    Script * lua = LuaEngine::Instance()->GetEngine();
    for(QVector<PrintItem>::Iterator iter = m_printItems.begin(); iter != m_printItems.end(); iter++)
    {
        int chRevise = 0;
        QString readStr;

        if(!iter->content.empty())
        {
            readStr = QObject::tr(iter->content.c_str());

            if(readStr.toStdString().size() > readStr.size())  //中文字符
            {
                readStr = readStr.left((iter->width-1)/2);
                chRevise = readStr.size()*2 <= iter->width?-readStr.size():-iter->width/2;
            }
            else
            {
                readStr = readStr.left(iter->width-1);
            }
        }
        else
        {
            FieldType type;
            if (!m_resultFiles->GetRecordFields()->GetFieldType(iter->name, type))
            {
                offset += iter->width;
                continue;
            }
            switch(type)
            {
                case FieldType::Bool:
                    {
                        bool ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        readStr = QString::number(ret);
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Byte:
                    {
                        Byte ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Int:
                    {
                        int ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }

                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Float:
                    {
                        float ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }
                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }

                        readStr = readStr.left(iter->width - 1);

                        int space = (iter->width - readStr.size())/2;
                        if(space > 0)
                        {
                            for(int i = 0; i < space; i++)
                            {
                                readStr.prepend(' ');
                                readStr.append(' ');
                            }
                        }
                    }
                    break;
                case FieldType::Double:
                    {
                        double ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }

                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret,'f',9);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }

                        if(iter->name == "curveK")
                        {
                            readStr.prepend(" A = ");
                            readStr.append("*C");
                        }
                        else if(iter->name == "curveB")
                        {
                            if(ret < 0)
                            {
                                readStr.remove('-');
                                readStr.prepend("-");
                            }
                            else
                            {
                                readStr.prepend("+");
                            }
                        }

                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::IntArray:
                    break;
                case FieldType::Time:
                    {
                        int ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        readStr = QDateTime::fromTime_t(ret).toString(iter->format.c_str());
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Enum:
                    {
                        String str;
                        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), iter->name, str);

                        readStr = QObject::tr(str.c_str());

                        if(readStr.toStdString().size() > readStr.size())  //中文字符
                        {
                            readStr = readStr.left((iter->width-1)/2);
                            chRevise = readStr.size()*2 <= iter->width?-readStr.size():-iter->width/2;
                        }
                        else
                        {
                            readStr = readStr.left(iter->width-1);
                        }
                    }
                    break;
            }
        }

        wmemcpy(strBuff+offset, readStr.toStdWString().c_str(), readStr.toStdWString().size()< iter->width?readStr.toStdWString().size():iter->width);
//        qDebug("size:%d ,offset %d", readStr.toStdWString().size(), offset);
//        qDebug()<<QString::fromStdWString(std::wstring(strBuff));
        offset += iter->width + chRevise;
    }

    if(offset < m_totalWidth)
    {
        strBuff[offset] = '\0';
    }

    printStr = QString::fromStdWString(std::wstring(strBuff));

    return printStr;
}

QString CalibrateDataPrintWidget::GetPrintItemString()
{
    wchar_t strBuff[m_totalWidth+1];
    wmemset(strBuff, ' ', m_totalWidth);
    strBuff[m_totalWidth] = '\0';
    QString printStr;

    int offset = 0;
    Script * lua = LuaEngine::Instance()->GetEngine();
    for(QVector<PrintItem>::Iterator iter = m_printItems.begin(); iter != m_printItems.end(); iter++)
    {
        int chRevise = 0;
        QString readStr;

        if(!iter->content.empty())
        {
            readStr = QObject::tr(iter->content.c_str());

            if(readStr.toStdString().size() > readStr.size())  //中文字符
            {
                readStr = readStr.left((iter->width-1)/2);
                chRevise = readStr.size()*2 <= iter->width?-readStr.size():-iter->width/2;
            }
            else
            {
                readStr = readStr.left(iter->width-1);
            }
        }
        else
        {
            FieldType type;
            if (!m_resultFiles->GetRecordFields()->GetFieldType(iter->name, type))
            {
                offset += iter->width;
                continue;
            }
            switch(type)
            {
                case FieldType::Bool:
                    {
                        bool ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        readStr = QString::number(ret);
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Byte:
                    {
                        Byte ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Int:
                    {
                        int ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }

                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Float:
                    {
                        float ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }
                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }

                        readStr = readStr.left(iter->width - 1);

                        int space = (iter->width - readStr.size())/2;
                        if(space > 0)
                        {
                            for(int i = 0; i < space; i++)
                            {
                                readStr.prepend(' ');
                                readStr.append(' ');
                            }
                        }
                    }
                    break;
                case FieldType::Double:
                    {
                        double ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        if (iter->isUnitChange)
                        {
                            lua->call(iter->unitChangeFunc, ret, UnitChange::Read);
                            lua->pull(ret);
                        }

                        if (iter->format.empty())
                        {
                            readStr = QString::number(ret,'f',9);
                        }
                        else
                        {
                            readStr = QString::asprintf(iter->format.c_str(), ret);
                        }
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::IntArray:
                    break;
                case FieldType::Time:
                    {
                        int ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);

                        readStr = QDateTime::fromTime_t(ret).toString(iter->format.c_str());
                        readStr = readStr.left(iter->width-1);
                    }
                    break;
                case FieldType::Enum:
                    {
                        String str;
                        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), iter->name, str);

                        readStr = QObject::tr(str.c_str());

                        if(readStr.toStdString().size() > readStr.size())  //中文字符
                        {
                            readStr = readStr.left((iter->width-1)/2);
                            chRevise = readStr.size()*2 <= iter->width?-readStr.size():-iter->width/2;
                        }
                        else
                        {
                            readStr = readStr.left(iter->width-1);
                        }
                    }
                    break;
            }
        }

        wmemcpy(strBuff+offset, readStr.toStdWString().c_str(), readStr.toStdWString().size()< iter->width?readStr.toStdWString().size():iter->width);

        offset += iter->width + chRevise;
    }

    if(offset < m_totalWidth)
    {
        strBuff[offset] = '\0';
    }

    printStr = QString::fromStdWString(std::wstring(strBuff));

    return printStr;
}

void CalibrateDataPrintWidget::SlotConnectButton()
{
    if(m_printer->IsConnected())
    {
        m_printer->Close();
        m_connectButton->setText(tr("未连接"));
    }
    else
    {
        if(m_printer->Connect(GetPrinterAddress(), GetPrinterConnectType()))
        {
            m_connectButton->setText(tr("已连接"));
        }
        else
        {
            m_connectButton->setText(tr("未连接"));
        }
    }
}

void CalibrateDataPrintWidget::SlotCloseButton()
{
    m_printer->Close();
    m_connectButton->setText(tr("未连接"));
    this->hide();
}

void CalibrateDataPrintWidget::SlotHeadButton()
{
    if(!m_printer->IsConnected())
    {
        MessageDialog msg(tr("打印机未连接!\n"), this,MsgStyle::ONLYOK);
        msg.exec();

        return;
    }

    QStringList headList;

    QString headStr = GetPrintHeadString();
    headList.push_back(headStr);

    char lineArray[m_totalWidth+1];
    memset(lineArray, '-', m_totalWidth);
    lineArray[m_totalWidth] = '\0';

    QString lineStr = QString::fromStdString(std::string(lineArray));
    headList.push_back(lineStr);

    if(SettingManager::Instance()->GetManufactureType() == ManufactureType::LS)
    {
        QBitmap map(":/img/img/logo_576_120.bmp");
        QImage img = map.toImage();

        if(!m_printer->PrintImage(img))
        {
            MessageDialog msg(tr("打印机错误\n")+tr("错误代码:")+QString::number(m_printer->Error()), this, MsgStyle::ONLYOK);
            msg.exec();

            return;
        }
    }

    if(!m_printer->Print(headList))
    {
        MessageDialog msg(tr("打印机错误\n")+tr("错误代码:")+QString::number(m_printer->Error()), this, MsgStyle::ONLYOK);
        msg.exec();

        return;
    }
}

void CalibrateDataPrintWidget::SlotCutButton()
{
    if(!m_printer->IsConnected())
    {
        MessageDialog msg(tr("打印机未连接!\n"), this,MsgStyle::ONLYOK);
        msg.exec();

        return;
    }

    if(!m_printer->CutPaper())
    {
        MessageDialog msg(tr("打印机错误\n")+tr("错误代码:")+QString::number(m_printer->Error()), this, MsgStyle::ONLYOK);
        msg.exec();

        return;
    }
}

void CalibrateDataPrintWidget::SlotBeginTimeCheck(QDateTime time)
{
    if(time > m_endTimeEdit->dateTime())
    {
        MessageDialog msg(tr("起始时间不能晚于结束时间"), this,MsgStyle::ONLYOK);
        msg.exec();
        m_beginTimeEdit->setDateTime(m_endTimeEdit->dateTime());
    }
}

void CalibrateDataPrintWidget::SlotEndTimeCheck(QDateTime time)
{
    if(time < m_beginTimeEdit->dateTime())
    {
        MessageDialog msg(tr("结束时间不能早于起始时间"), this,MsgStyle::ONLYOK);
        msg.exec();
        m_endTimeEdit->setDateTime(m_beginTimeEdit->dateTime());
    }
}

void CalibrateDataPrintWidget::SlotPrintLimitCheck(QString str)
{
    if(str.toInt() > 1000)
    {
        MessageDialog msg(tr("超过打印数量上限"), this,MsgStyle::ONLYOK);
        msg.exec();
        m_limitEdit->setText("1000");
    }
}


void CalibrateDataPrintWidget::Show(QWidget *parent, int roleType)
{
    (void)roleType;
    this->setParent(parent, Qt::FramelessWindowHint | Qt::Dialog);
    this->show();
}

void CalibrateDataPrintWidget::showEvent(QShowEvent *event)
{
    if(!m_printer->IsConnected())
    {
        this->SlotConnectButton();
    }
    if((ConnectType)GetPrinterConnectType() == ConnectType::Serial)
    {
        m_ipLabel->setText(tr("打印机串口"));
        m_ipEdit->setText("COM30");
    }
    else
    {
        m_ipEdit->setText(GetPrinterAddress());
    }

    m_closeButton->setFocus();
    QWidget::showEvent(event);
}

void CalibrateDataPrintWidget::paintEvent(QPaintEvent *event)
{
    int height = 100;
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    painter.drawPixmap(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

    painter.setBrush(Qt::white);

    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - SHADOW_WIDTH));

    QPen pen;
    pen.setColor(QColor(10,105,170));
    pen.setWidth(3);

    painter.setPen(pen);
    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));

    QWidget::paintEvent(event);
}

void CalibrateDataPrintWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)message;    
}


}
