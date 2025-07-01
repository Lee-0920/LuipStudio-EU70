#include "Log.h"
#include "MethodDetailWidget.h"
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

#ifdef _CS_X86_WINDOWS
#define TITLE_HIGH  70
#endif

#ifdef _CS_ARM_LINUX
#define TITLE_HIGH  50
#endif
namespace UI
{
MethodDetailWidget::MethodDetailWidget(MethodRecord record, QWidget *parent) : QDialog(parent)
{
#ifdef _CS_ARM_LINUX
    setMinimumSize(800, 600);
    move(0, 0);
#endif
    this->setWindowModality(Qt::WindowModal);

#ifdef _CS_X86_WINDOWS
    this->setFixedSize(820, 620);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif
    m_methodRecord = record;
    this->SpaceInit();
}

void MethodDetailWidget::SpaceInit()
{
    //数据表格
    measureResultTableWidget = new MQtableWidget();

    measureResultTableWidget->resize(TABLE_WIDTH,TABLE_HIGH);
    measureResultTableWidget->setColumnCount(1);//列
    measureResultTableWidget->setRowCount(ROW_LINE);
    measureResultTableWidget->setFixedSize(TABLE_WIDTH,TABLE_HIGH);

    measureResultTableWidget->setColumnWidth(0, 450);

    m_rowName << "方法名称: " << "创建时间: " << "测量类型: "
              << "Tourbo模式: " << "ICR模式: " << "TOC模式: "
              << "电导率(NaCl): " << "自动加试剂: " << "酸剂(uL/分): "
              << "氧化剂(uL/分): " << "冲洗时间(秒): " << "测量次数(离线): "
              << "舍弃次数(离线): ";
    //设置表头
    QFont headFont;
    headFont.setPointSize(16);
    measureResultTableWidget->setRowAndSize(m_rowName,15);
    measureResultTableWidget->setRowFixHigh(m_rowName,37);  //设置行固定高度
    measureResultTableWidget->verticalHeader()->setFont(headFont);
    measureResultTableWidget->verticalHeader()->setFixedWidth(350); // 设置表头的高度
//    measureResultTableWidget->verticalHeader()->setFixedHeight(500); // 设置表头的高度
//    measureResultTableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    measureResultTableWidget->verticalHeader()->setVisible(true);   //显示垂直表头
    measureResultTableWidget->horizontalHeader()->setVisible(false);//隐藏水平表头
    measureResultTableWidget->setShowGrid(true);
    measureResultTableWidget->verticalHeader()->setEditTriggers(QAbstractItemView::NoEditTriggers);
    measureResultTableWidget->verticalHeader()->setTextElideMode(Qt::TextElideMode::ElideMiddle);
    measureResultTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    measureResultTableWidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

//    measureResultTableWidget->setStyleSheet("selection-background-color:rgb(0, 100, 250);"); //设置选中背景色

    QFont dataFont = measureResultTableWidget->font();
    dataFont.setPointSize(16);
    measureResultTableWidget->setFont(dataFont);
    measureResultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
//    measureResultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    measureResultTableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    measureResultTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:40px;}");
    measureResultTableWidget->horizontalScrollBar()->setVisible(false);
    measureResultTableWidget->horizontalScrollBar()->setDisabled(true);
    measureResultTableWidget->verticalScrollBar()->setVisible(false);
    measureResultTableWidget->verticalScrollBar()->setDisabled(false);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addStretch();
    measureResultTableLayout->addWidget(measureResultTableWidget);
    measureResultTableLayout->addStretch();

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

void MethodDetailWidget::SlotQuitButton()
{
    this->close();
}

void MethodDetailWidget::ViewRefresh()
{
    measureResultTableWidget->clear();
    measureResultTableWidget->setRowAndSize(m_rowName,15);
}

void MethodDetailWidget::TableSpaceInit()
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

void MethodDetailWidget::ShowTable()
{
    ViewRefresh();
    TableSpaceInit();
    int row = 0, column = 0;
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.methodName);
    measureResultTableWidget->item(row++, column)->setText(QDateTime::fromTime_t(m_methodRecord.createTime).toString("yyyy-MM-dd HH:mm:ss"));
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.meaType?"离线":"在线");
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.turboMode?"是":"否");
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.ICRMode?"是":"否");
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.TOCMode?"是":"否");
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.ECMode?"是":"否");
    measureResultTableWidget->item(row++, column)->setText(m_methodRecord.autoReagent?"是":"否");
    measureResultTableWidget->item(row++, column)->setText(QString::number(m_methodRecord.reagent1Vol));
    measureResultTableWidget->item(row++, column)->setText(QString::number(m_methodRecord.reagent2Vol));
    measureResultTableWidget->item(row++, column)->setText(QString::number(m_methodRecord.normalRefreshTime));
    measureResultTableWidget->item(row++, column)->setText(QString::number(m_methodRecord.measureTimes));
    measureResultTableWidget->item(row++, column)->setText(QString::number(m_methodRecord.rejectTimes));

}


void MethodDetailWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
    ShowTable();
}

void MethodDetailWidget::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = TITLE_HIGH;
    QPainter painter(this);
    painter.setPen(Qt::white);

//    painter.setColor(QColor(115, 115, 140));
    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
                    height - SHADOW_WIDTH));

//    painter.drawPixmap(
//            QRect(SHADOW_WIDTH, SHADOW_WIDTH, this->width() - 2 * SHADOW_WIDTH,
//                    this->height() - 2 * SHADOW_WIDTH), QPixmap(DEFAULT_SKIN));

    painter.setBrush(Qt::white);

    painter.drawRect(
            QRect(SHADOW_WIDTH, height, this->width() - 2 * SHADOW_WIDTH,
                    TABLE_HIGH - SHADOW_WIDTH));

    painter.setBrush(QBrush(QColor(115, 115, 140)));
    painter.drawRect(
            QRect(SHADOW_WIDTH, height + TABLE_HIGH, this->width() - 2 * SHADOW_WIDTH,
                    this->height() - height - SHADOW_WIDTH));

    QPen pen;
    pen.setColor(QColor(10,105,170));
    pen.setWidth(3);

    painter.setPen(pen);
    painter.drawLine(QPoint(0,0), QPoint(0,this->height()));
    painter.drawLine(QPoint(0,0), QPoint(this->width(),0));
    painter.drawLine(QPoint(0,this->height()-1), QPoint(this->width()-1,this->height()-1));
    painter.drawLine(QPoint(this->width()-1,0), QPoint(this->width()-1,this->height()-1));
}

MethodDetailWidget::~MethodDetailWidget()
{

}

}

