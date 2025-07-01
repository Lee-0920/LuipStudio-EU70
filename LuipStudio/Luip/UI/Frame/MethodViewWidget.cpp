#include "Log.h"
#include "MethodViewWidget.h"
#include "LuaException.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/SettingManager.h"
#include "UI/Frame/MessageDialog.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QSettings>
#include "Setting/Environment.h"
#include <QSignalMapper>
#include "MethodDetailWidget.h"
#include "System/CopyFile.h"

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace OOLUA;
using namespace std;
using System::CopyFileAction;

#define ROW_LINE 10
#define FileName ("MethodTable.csv")

namespace UI
{

unique_ptr<MethodViewWidget> MethodViewWidget::m_instance(nullptr);

MethodViewWidget::MethodViewWidget(QWidget *parent) : QDialog(parent)
{
    this->setFixedSize(800, 500);
    this->setWindowModality(Qt::WindowModal);

    #ifdef _CS_X86_WINDOWS
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
    #endif
    m_curPage = 0;
    this->SpaceInit();
}

MethodViewWidget* MethodViewWidget::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new MethodViewWidget);
    }

    return m_instance.get();
}


void MethodViewWidget::SpaceInit()
{
    //数据表格
    measureResultTableWidget = new MQtableWidget();

    measureResultTableWidget->resize(700,370);
    measureResultTableWidget->setColumnCount(3);//列
    measureResultTableWidget->setRowCount(ROW_LINE);
    measureResultTableWidget->setFixedSize(700,370);

    measureResultTableWidget->setColumnWidth(0, 400);
    measureResultTableWidget->setColumnWidth(1, 150);
    measureResultTableWidget->setColumnWidth(2, 150);
//    measureResultTableWidget->setColumnWidth(3, 50);


    m_columnName << "日期" << "名称" << "类型";
    //设置表头
    QFont headFont;
    headFont.setPointSize(14);
    measureResultTableWidget->setColumnAndSize(m_columnName,15);
    measureResultTableWidget->horizontalHeader()->setFont(headFont);
    measureResultTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    measureResultTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = measureResultTableWidget->font();
    dataFont.setPointSize(15);
    measureResultTableWidget->setFont(dataFont);
    measureResultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
    measureResultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    measureResultTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:20px;}");
    measureResultTableWidget->horizontalScrollBar()->setVisible(false);
    measureResultTableWidget->horizontalScrollBar()->setDisabled(true);
    measureResultTableWidget->verticalScrollBar()->setVisible(false);
    measureResultTableWidget->verticalScrollBar()->setDisabled(true);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(measureResultTableWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();

    QFont font;                           //字体
    font.setPointSize(14);

//    toTopButton = new QPushButton();
//    toTopButton->setObjectName("brownButton");
//    toTopButton->setText("首页");
//    toTopButton->setFont(font);
//    toTopButton->setFixedSize(80,40);

    toBackButton = new QPushButton();
    toBackButton->setObjectName("brownButton");
    toBackButton->setText("上一页");
    toBackButton->setFont(font);
    toBackButton->setFixedSize(80,40);

    toNextButton = new QPushButton();
    toNextButton->setObjectName("brownButton");
    toNextButton->setText("下一页");
    toNextButton->setFont(font);
    toNextButton->setFixedSize(80,40);

//    toBottomButton = new QPushButton();
//    toBottomButton->setObjectName("brownButton");
//    toBottomButton->setText("尾页");
//    toBottomButton->setFont(font);
//    toBottomButton->setFixedSize(80,40);

    exportButton = new QPushButton();
    exportButton->setObjectName("brownButton");
    exportButton->setText(tr("导出"));
    exportButton->setFont(font);
    exportButton->setFixedSize(80,40);

    delButton= new QPushButton();
    delButton->setObjectName("brownButton");
    delButton->setText(tr("删除"));
    delButton->setFont(font);
    delButton->setFixedSize(80,40);

    detailButton = new QPushButton();
    detailButton->setObjectName("brownButton");
    detailButton->setText(tr("详情"));
    detailButton->setFont(font);
    detailButton->setFixedSize(80,40);
    connect(detailButton,SIGNAL(clicked()), this, SLOT(SlotDetailButton()));

    applyButton = new QPushButton();
    applyButton->setObjectName("brownButton");
    applyButton->setText("应用");
    applyButton->setFont(font);
    applyButton->setFixedSize(80,40);

    QVBoxLayout *rightLayout = new QVBoxLayout();
//    rightLayout->addWidget(toTopButton);
    rightLayout->addWidget(toBackButton);
    rightLayout->addWidget(toNextButton);
//    rightLayout->addWidget(toBottomButton);
    rightLayout->addWidget(exportButton);
    rightLayout->addWidget(delButton);
    rightLayout->addWidget(detailButton);
    rightLayout->addWidget(applyButton);
    rightLayout->addStretch();

    rightLayout->setSpacing(10);
//    rightLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
//    mainLayout->addSpacing(10);
    mainLayout->addLayout(rightLayout);

    this->setLayout(mainLayout);

//    connect(toTopButton,SIGNAL(clicked()), this, SLOT(ToTop()));
//    connect(toBottomButton,SIGNAL(clicked()), this, SLOT(ToBottom()));
    connect(toBackButton,SIGNAL(clicked()), this, SLOT(ToBack()));
    connect(toNextButton,SIGNAL(clicked()), this, SLOT(ToNext()));
    connect(exportButton,SIGNAL(clicked()), this, SLOT(SlotExportButton()));
    connect(delButton,SIGNAL(clicked()), this, SLOT(SlotDelButton()));
    connect(applyButton,SIGNAL(clicked()), this, SLOT(SlotApplyButton()));
    connect(this,SIGNAL(MethodSaveForModbusSignal(System::String)), this, SLOT(SlotMethodSaveForModbus(System::String)));
    connect(this,SIGNAL(MethodApplyForModbusSignal(int)), this, SLOT(SlotMethodApplyForModbus(int)));
    connect(this,SIGNAL(MethodDelectForModbusSignal(int)), this, SLOT(SlotMethodDelectForModbus(int)));
}

void MethodViewWidget::SlotQuitButton()
{
    this->close();
}

void MethodViewWidget::ViewRefresh()
{
    measureResultTableWidget->clear();
    measureResultTableWidget->setColumnAndSize(m_columnName,15);
}

void MethodViewWidget::TableSpaceInit()
{
    for(int i = 0;i < measureResultTableWidget->rowCount();i++)
    {
        for(int j = 0;j < measureResultTableWidget->columnCount();j++)
        {
            measureResultTableWidget->setItem(i, j, new QTableWidgetItem());
            measureResultTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
            measureResultTableWidget->setRowHeight(i, 30);
        }
    }
}

void MethodViewWidget::ShowTable()
{
    ViewRefresh();
    TableSpaceInit();

    QList<MethodRecord> map;
    DataBaseManager::Instance()->GetMethodTable()->GetData(map);

    m_showFields.clear();
    m_totalMethod = map.count();
//    qDebug("%d", m_totalMethod);
    //使用倒序的方式存入容器中
    reverse(map.begin(), map.end());
    m_showFields = QVector<MethodRecord>::fromList(map);

    //根据当前页码显示方法
    int showIndex = 0, row = 0;
    for (QVector<MethodRecord>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
    {
        if(showIndex++ < m_curPage*ROW_LINE)
        {
            continue;
        }
//        qDebug() << iter->methodName;
        int column = 0;

        measureResultTableWidget->item(row, column++)->setText(QDateTime::fromTime_t(iter->createTime).toString("yyyy-MM-dd HH:mm:ss"));
        measureResultTableWidget->item(row, column++)->setText(iter->methodName);
        if(iter->meaType)
        {
            measureResultTableWidget->item(row++, column++)->setText("离线");
        }
        else
        {
            measureResultTableWidget->item(row++, column++)->setText("在线");
        }
        if(row > measureResultTableWidget->rowCount()-1)
        {
            break;
        }
    }
}


void MethodViewWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
    ShowTable();
}

//void MethodViewWidget::ShowRow(Uint16 row)
//{
//    int column = 0;
//    for (std::vector<MethodRecord>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
//    {
//        measureResultTableWidget->item(row, column++)->setText(QString::number(ret));
//        if(column > 3)
//        {
//            break;
//        }
//    }

//}

void MethodViewWidget::paintEvent(QPaintEvent *event)
{
//    QDialog::paintEvent(event);
//    int height = 50;
//    QPainter painter(this);
//    painter.setPen(Qt::NoPen);

//    painter.drawPixmap(
//            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

//    painter.setBrush(Qt::white);

//    painter.drawRect(
//            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - height - SHADOW_WIDTH));

//    QPen pen;
//    pen.setColor(QColor(10,105,170));
//    pen.setWidth(3);

//    painter.setPen(pen);
//    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
//    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
//    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
//    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));
}

MethodViewWidget::~MethodViewWidget()
{
    if (m_numberKey)
    {
        delete m_numberKey;
        m_numberKey = nullptr;
    }
}


void MethodViewWidget::ToTop()
{
    m_curPage = 0;
    ShowTable();
}

void MethodViewWidget::ToBottom()
{
    m_curPage = m_totalMethod/ROW_LINE;
    ShowTable();
}


void MethodViewWidget::ToBack()
{
    if(m_curPage)
    {
        m_curPage--;
    }
    ShowTable();
}

void MethodViewWidget::ToNext()
{
    if(m_curPage+1 <= (m_totalMethod/ROW_LINE))
    {
        m_curPage++;
    }
    ShowTable();    
}


void MethodViewWidget::SlotExportButton()
{
    QString result = "";
    CopyFileAction copyFileAction;
    QString strDir = copyFileAction.GetTargetDir().c_str();
    QDir dir(strDir);

    QString sPath = Environment::Instance()->GetAppDataPath().c_str() + QString("/") + QString(FileName);
    QString dPath = dir.filePath(FileName);

    qDebug() << dPath;

    bool isFail = false;
    QString errmsg;
    if (!copyFileAction.ExMemoryDetect(errmsg)) //U盘检测
    {
        MessageDialog msg(errmsg, this);
        msg.exec();
        return;
    }

    if (!copyFileAction.TargetDirCheck(dir)) //拷贝目录检测
    {
        MessageDialog msg(tr("创建目录，方法数据导出失败"), this);
        msg.exec();
        return;
    }

    if  (DataBaseManager::Instance()->GetMethodTable()->ExportTableToCsv(dPath))
    {
        result = tr("方法数据导出成功");
        logger->info("方法数据导出成功");
    }
    else
    {
        result = tr("方法数据导出失败");
        logger->info("方法数据导出失败");
    }
#ifdef    _CS_ARM_LINUX
    system("sync");
#endif
    MessageDialog msg(result, this);
    msg.exec();
}

void MethodViewWidget::SlotDelButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {        
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString timeStr = measureResultTableWidget->item(row,0)->text();
        QString nameStr = measureResultTableWidget->item(row,1)->text();
        QString str = "请确认是否删除方法[" + nameStr + "]?" ;
        MessageDialog msg(str, this,MsgStyle::YESANDNO);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
        QDateTime time = QDateTime::fromString(timeStr,"yyyy-MM-dd hh:mm:ss");
        DataBaseManager::Instance()->GetMethodTable()->DeleteMethod(time);
        ShowTable();
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }
}

void MethodViewWidget::SlotDetailButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString timeStr = measureResultTableWidget->item(row,0)->text();
        if(!timeStr.isEmpty())
        {
            MethodRecord record = m_showFields.at(m_curPage*ROW_LINE + row);
            MethodDetailWidget *md = new MethodDetailWidget(record, this);
            md->exec();
        }
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }

}

void MethodViewWidget::SlotApplyButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString timeStr = measureResultTableWidget->item(row,0)->text();
        if(!timeStr.isEmpty())
        {
            MethodRecord record = m_showFields.at(m_curPage*ROW_LINE + row);
            emit MethodUpdateSignal(record, false, false);
        }
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }
}

/*
*[brief]:保存当前方法到数据库
*[param]:String,以#分隔的方法信息字符串
*[note]:[要保证仪器和工作站时间保持一致]
*/
void MethodViewWidget::MethodSaveForModbus(System::String info)
{
    emit MethodSaveForModbusSignal(info);
}

/*
*[brief]:从数据库中查找并应用到当前方法
*[param]:int型时间
*[note]:[要保证仪器和工作站时间保持一致]
*/
void MethodViewWidget::MethodApplyForModbus(int dataTime)
{
    emit MethodApplyForModbusSignal(dataTime);
}

/*
*[brief]:从数据库中查找并删除方法
*[param]:int型时间
*[note]:[要保证仪器和工作站时间保持一致]
*/
void MethodViewWidget::MethodDelectForModbus(int dataTime)
{
     emit MethodDelectForModbusSignal(dataTime);
}

void MethodViewWidget::SlotMethodSaveForModbus(System::String info)
{
    if(!info.empty())
    {
        QString strRd = info.c_str();
        QStringList list = strRd.split("#");
        int members = 13;
        if(list.count() == members)
        {
            MethodRecord record;
            record.methodName = list.at(0);
            record.createTime = QDateTime::fromString(list.at(1), "yyyy-MM-dd HH:mm:ss").toTime_t();
            record.meaType = list.at(2).toInt();
            record.turboMode = list.at(3).toInt();
            record.ICRMode = list.at(4).toInt();
            record.TOCMode = list.at(5).toInt();
            record.ECMode = list.at(6).toInt();
            record.autoReagent = list.at(7).toInt();
            record.reagent1Vol = list.at(8).toFloat();
            record.reagent2Vol = list.at(9).toFloat();
            record.normalRefreshTime = list.at(10).toInt();
            record.measureTimes = list.at(11).toInt();
            record.rejectTimes = list.at(12).toInt();
            emit MethodUpdateSignal(record, true, true);
        }
        else
        {
            logger->warn("Modbus方法保存异常，成员存在空值");
        }
    }
}

void MethodViewWidget::SlotMethodApplyForModbus(int dataTime)
{
    QString timeStr = QDateTime::fromTime_t(dataTime).toString("yyyy-MM-dd hh:mm:ss");
    if(!timeStr.isEmpty())
    {
        MethodRecord record;
        foreach (MethodRecord item, m_showFields)
        {
            if(item.createTime == QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss").toTime_t())
            {
                record = item;
                emit MethodUpdateSignal(record, false, true);
                break;
            }
        }
    }
}

void MethodViewWidget::SlotMethodDelectForModbus(int dataTime)
{
    QDateTime time = QDateTime::fromTime_t(dataTime);
    DataBaseManager::Instance()->GetMethodTable()->DeleteMethod(time);
    ShowTable();
}

}

