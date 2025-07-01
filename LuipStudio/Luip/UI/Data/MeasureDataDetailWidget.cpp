#include "Log.h"
#include "MeasureDataDetailWidget.h"
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

using namespace Configuration;
using namespace System;
using namespace Lua;
using namespace OOLUA;

#define ROW_LINE 13
#define TABLE_WIDTH 800
#define TABLE_HIGH  481
#define TITLE_HIGH  70

namespace UI
{
MeasureDataDetailWidget::MeasureDataDetailWidget(MeasureRecord record, QWidget *parent) : QDialog(parent)
{  
    this->setWindowModality(Qt::WindowModal);
    this->setFixedSize(820, 620);
#ifdef _CS_ARM_LINUX
    setMinimumSize(800, 600);
    move(0, 0);
#endif

    #ifdef _CS_X86_WINDOWS
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
        this->setFixedSize(820, 620);
    #endif
    m_measureDataRecord = record;
    this->LoadMeasureFileFormatTable();
    this->SpaceInit();
}

void MeasureDataDetailWidget::LoadMeasureFileFormatTable()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Script *lua = luaEngine->GetEngine();

    Table measureTable;
    luaEngine->GetLuaValue(state, "setting.fileFormat.measure", measureTable);

    int currentId = 0;
    Table formatTable;

    measureTable.at("current", currentId);
    measureTable.at(currentId, formatTable);

    int row = 0;
    //固定开头顺序：名称->类型->时间
//    m_rowName.append(m_measureDataRecord.methodName);
//    m_rowName.append("类型");
//    m_rowName.append("时间");
//    oolua_ipairs(formatTable)
//    {
//        Table itermTable;
//        lua->pull(itermTable);

//        String name;
//        String text;
//        FieldType type;
//        itermTable.at("name", name);
//        itermTable.at("text", text);
//        itermTable.at("type", type);
//        if(name.c_str()!= "dateTime"
//                && name.c_str()!= "meaType")
//        {
//            m_rowName.append(text.c_str());
//        }
//    }
//    oolua_ipairs_end()

    m_rowName << "名称" << "类型" << "测量时间" << "测量时长" << "TOC浓度" << "TC浓度" << "IC浓度"
              << "TC峰值" << "IC峰值" << "初始TC电导池温度" << "初始IC电导池温度" << "反应TC电导池温度"
              << "反应IC电导池温度" << "初始值环境温度" << "反应值环境温度" << "量程" << "Turbo模式" << "ICR模式"
              << "TOC测量" << "电导率测量" << "自动加试剂" << "酸剂流量(uL/分钟)" << "氧化剂流量(uL/分钟)" << "冲洗时间(秒)"
              << "测量次数(离线)" << "舍弃次数(离线)";
}

void MeasureDataDetailWidget::SpaceInit()
{
    //数据表格
    measureResultTableWidget = new MQtableWidget();

    measureResultTableWidget->resize(TABLE_WIDTH,TABLE_HIGH);
    measureResultTableWidget->setColumnCount(1);//列
    measureResultTableWidget->setRowCount(m_rowName.count());
    measureResultTableWidget->setFixedSize(TABLE_WIDTH,TABLE_HIGH);

    measureResultTableWidget->setColumnWidth(0, 450);

    //设置表头
    QFont headFont;
    headFont.setPointSize(16);
    measureResultTableWidget->setRowAndSize(m_rowName,15);
    measureResultTableWidget->setRowFixHigh(m_rowName,37);  //设置行固定高度
    measureResultTableWidget->verticalHeader()->setFont(headFont);
    measureResultTableWidget->verticalHeader()->setFixedWidth(350); // 设置表头的高度
    measureResultTableWidget->verticalHeader()->setVisible(true);   //显示垂直表头
    measureResultTableWidget->horizontalHeader()->setVisible(false);//隐藏水平表头
    measureResultTableWidget->setShowGrid(true);
    measureResultTableWidget->verticalHeader()->setEditTriggers(QAbstractItemView::NoEditTriggers);
//    measureResultTableWidget->verticalHeader()->setTextElideMode(Qt::TextElideMode::ElideMiddle);
//    measureResultTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    measureResultTableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

    QFont dataFont = measureResultTableWidget->font();
    dataFont.setPointSize(16);
    measureResultTableWidget->setFont(dataFont);
    measureResultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
//    measureResultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    measureResultTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    measureResultTableWidget->horizontalScrollBar()->setVisible(false);
    measureResultTableWidget->horizontalScrollBar()->setDisabled(true);
    measureResultTableWidget->verticalScrollBar()->setStyleSheet
            ("QScrollBar{background-color:rgb(200,200,200); width: 40;}"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(measureResultTableWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addStretch();
    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();

    QFont font;                           //字体
    font.setPointSize(14);

    quitButton = new QPushButton();
    quitButton->setObjectName("brownButton");
    quitButton->setText(tr("确定"));
    quitButton->setFont(font);
    quitButton->setFixedSize(80,40);
    connect(quitButton,SIGNAL(clicked()), this, SLOT(SlotQuitButton()));
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(quitButton);
    bottomLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);
    mainLayout->setContentsMargins(0, TITLE_HIGH, 0, 15);

    this->setLayout(mainLayout);
}

void MeasureDataDetailWidget::SlotQuitButton()
{
    this->close();
}

void MeasureDataDetailWidget::ViewRefresh()
{
    measureResultTableWidget->clear();
    measureResultTableWidget->setRowAndSize(m_rowName,15);
}

void MeasureDataDetailWidget::TableSpaceInit()
{
    for(int i = 0;i < measureResultTableWidget->rowCount();i++)
    {
        for(int j = 0;j < measureResultTableWidget->columnCount();j++)
        {
            measureResultTableWidget->setItem(i, j, new QTableWidgetItem());
            measureResultTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

/*
1-名称 2-类型 3-测量时间 4-测量时长 5-TOC浓度 6-TC浓度
7-IC浓度 8-TC峰值 9-IC峰值 10-初始TC电导池温度 11-初始IC电导池温度 12-反应TC电导池温度
13-反应IC电导池温度 14-初始值环境温度 15-反应值环境温度 16-量程 17-Turbo模式 18-ICR模式
19-TOC测量 20-电导率测量 21-自动加试剂 22-酸剂流量 23-氧化剂流量 24-冲洗时间
25-测量次数 26-舍弃次数
*/
void MeasureDataDetailWidget::ShowTable()
{
    ViewRefresh();
    TableSpaceInit();
    int column = 0;
    measureResultTableWidget->item(0, column)->setText(m_measureDataRecord.methodName);
    measureResultTableWidget->item(1, column)->setText(m_measureDataRecord.meaType?"离线":"在线");
    measureResultTableWidget->item(2, column)->setText(QDateTime::fromTime_t(m_measureDataRecord.measureDateTime).toString("yyyy-MM-dd hh:mm:ss"));
    measureResultTableWidget->item(3, column)->setText(QString::number(m_measureDataRecord.measureconsumeDateTime));
    measureResultTableWidget->item(4, column)->setText(QString::number(m_measureDataRecord.consistency));
    measureResultTableWidget->item(5, column)->setText(QString::number(m_measureDataRecord.consistencyTC));
    measureResultTableWidget->item(6, column)->setText(QString::number(m_measureDataRecord.consistencyIC));
    measureResultTableWidget->item(7, column)->setText(QString::number(m_measureDataRecord.peakTC));
    measureResultTableWidget->item(8, column)->setText(QString::number(m_measureDataRecord.peakIC));
    measureResultTableWidget->item(9, column)->setText(QString::number(m_measureDataRecord.initCellTempTC));
    measureResultTableWidget->item(10, column)->setText(QString::number(m_measureDataRecord.initCellTempIC));
    measureResultTableWidget->item(11, column)->setText(QString::number(m_measureDataRecord.finalCellTempTC));
    measureResultTableWidget->item(12, column)->setText(QString::number(m_measureDataRecord.finalCellTempIC));
    measureResultTableWidget->item(13, column)->setText(QString::number(m_measureDataRecord.initEnvironmentTemp));
    measureResultTableWidget->item(14, column)->setText(QString::number(m_measureDataRecord.finalEnvironmentTemp));
    measureResultTableWidget->item(15, column)->setText(QString::number(m_measureDataRecord.currentRange));
    measureResultTableWidget->item(16, column)->setText(m_measureDataRecord.turboMode?"是":"否");
    measureResultTableWidget->item(17, column)->setText(m_measureDataRecord.ICRMode?"是":"否");
    measureResultTableWidget->item(18, column)->setText(m_measureDataRecord.TOCMode?"是":"否");
    measureResultTableWidget->item(19, column)->setText(m_measureDataRecord.ECMode?"是":"否");
    measureResultTableWidget->item(20, column)->setText(m_measureDataRecord.autoReagent?"是":"否");
    measureResultTableWidget->item(21, column)->setText(QString::number(m_measureDataRecord.reagent1Vol));
    measureResultTableWidget->item(22, column)->setText(QString::number(m_measureDataRecord.reagent2Vol));
    measureResultTableWidget->item(23, column)->setText(QString::number(m_measureDataRecord.normalRefreshTime));
    measureResultTableWidget->item(24, column)->setText(QString::number(m_measureDataRecord.measureTimes));
    measureResultTableWidget->item(25, column)->setText(QString::number(m_measureDataRecord.rejectTimes));
}


void MeasureDataDetailWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
    ShowTable();
}

void MeasureDataDetailWidget::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = TITLE_HIGH;
    QPainter painter(this);
    painter.setPen(Qt::white);

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    height - SHADOW_WIDTH));

    painter.setBrush(Qt::white);

    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                    TABLE_HIGH - SHADOW_WIDTH));

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, height + TABLE_HIGH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - TABLE_HIGH - SHADOW_WIDTH));

    QPen pen;
    pen.setColor(QColor(10,105,170));
    pen.setWidth(3);

    painter.setPen(pen);
    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));
}

MeasureDataDetailWidget::~MeasureDataDetailWidget()
{

}

}

