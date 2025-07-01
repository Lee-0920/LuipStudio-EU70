#include "Log.h"
#include "ExAutoTempCalibrateDialog.h"
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

namespace UI
{
ExAutoTempCalibrateDialog::ExAutoTempCalibrateDialog(QWidget *parent) : QDialog(parent)
{
    this->setFixedSize(480, 320);
    this->setWindowModality(Qt::WindowModal);

    #ifdef _CS_X86_WINDOWS
        setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
        setAttribute(Qt::WA_TranslucentBackground);
    #endif

    QFont font;
    font.setPointSize(12);

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.profile.hardwareParamIterms.exAutoTempCalibrate", itermsTable);

    int row = 0;
    QSignalMapper *signalMapper = new QSignalMapper(this);

    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);

        ExTempCalibrateItem item;

        String text;
        int index;
        String unit;
        itermTable.at("text", text);
        itermTable.at("index", index);
        itermTable.safe_at("unit", unit);

        item.name = QString::fromStdString(text);
        item.index = index;
        item.unit = QString::fromStdString(unit);

        itermTable.at("checkValue", item.checkValueFunc);
        itermTable.at("calibrateFunc", item.calibrateFunc);

        QPushButton* button = new QPushButton();
        button->setFixedSize(80,45);
        button->setText(tr("校准"));
        button->setObjectName("brownButton");

        item.confirmButton = button;
        connect(button, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(button, row);

        m_itemVec.push_back(item);

        row++;
    }
    oolua_ipairs_end()
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(SlotCalibrateButton(int)));

    // QTableWidget是QT程序中常用的显示数据表格的空间
    m_tempCalibrateTable = new MQtableWidget();

    m_tempCalibrateTable->setColumnCount(5);//设置列数为4

    QFont headerFont;
    headerFont.setPointSize(10);
    headerFont.setBold(true);

    //QTableWidgetItem用来表示表格中的一个单元格，整个表格都需要用逐个单元格构建起来。
    QTableWidgetItem *column0 = new QTableWidgetItem(tr("名称"));
    column0->setFont(headerFont);
    m_tempCalibrateTable->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("索引"));
    column1->setFont(headerFont);
    m_tempCalibrateTable->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("真实值"));
    column2->setFont(headerFont);
    m_tempCalibrateTable->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("单位"));
    column3->setFont(headerFont);
    m_tempCalibrateTable->setHorizontalHeaderItem(3, column3);

    QTableWidgetItem *column4 = new QTableWidgetItem();
    column4->setFont(headerFont);
    m_tempCalibrateTable->setHorizontalHeaderItem(4, column4);

    m_tempCalibrateTable->horizontalHeader()->setFixedHeight(38); // 设置表头的高度

    m_tempCalibrateTable->setColumnWidth(0,120);
    m_tempCalibrateTable->setColumnWidth(1,80);
    m_tempCalibrateTable->setColumnWidth(2,80);
    m_tempCalibrateTable->setColumnWidth(3,78);
    m_tempCalibrateTable->setColumnWidth(4,80);

    m_tempCalibrateTable->setRowCount(m_itemVec.size());
    for(int i = 0;i < m_tempCalibrateTable->rowCount(); i++)//行
    {
        m_tempCalibrateTable->setRowHeight(i,45);
    }

    m_tempCalibrateTable->setFixedSize(440,m_tempCalibrateTable->rowCount()*45+40);


    SpaceInit();

    for(int i = 0; i < m_itemVec.size(); i++)
    {
        m_tempCalibrateTable->item(i, 0)->setText(m_itemVec[i].name);
        m_tempCalibrateTable->item(i, 1)->setText(QString::number(m_itemVec[i].index));
        m_tempCalibrateTable->item(i, 2)->setText("");
        m_tempCalibrateTable->item(i, 3)->setText(m_itemVec[i].unit);
        m_tempCalibrateTable->setCellWidget(i,4,m_itemVec[i].confirmButton);
    }

    QFont dataFont = m_tempCalibrateTable->font();
    dataFont.setPointSize(12);
    m_tempCalibrateTable->setFont(dataFont);

    QHBoxLayout *tableLayout = new QHBoxLayout();
    tableLayout->addWidget(m_tempCalibrateTable);
    tableLayout->setContentsMargins(0, 0, 0, 0);

    m_quitButton = new QPushButton();
    m_quitButton->setObjectName("brownButton");
    m_quitButton->setText(tr("退出"));
    m_quitButton->setFixedSize(80, 45);
    m_quitButton->setFont(font);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_quitButton);
    buttonLayout->addStretch();
    buttonLayout->setSpacing(30);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addStretch();
    dialogLayout->addLayout(tableLayout);
    dialogLayout->addStretch();
    dialogLayout->addLayout(buttonLayout);
    dialogLayout->addStretch();
    dialogLayout->setSpacing(10);
    dialogLayout->setContentsMargins(0, 50, 0, 0);

    this->setLayout(dialogLayout);

    connect(m_quitButton, SIGNAL(clicked()), this, SLOT(SlotQuitButton()));
    connect(m_tempCalibrateTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(SlotDoubleClicked(QTableWidgetItem *)));
    connect(m_tempCalibrateTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(SlotCheckValue(QTableWidgetItem *)));
}

void ExAutoTempCalibrateDialog::SpaceInit()
{
    for(int i = 0;i < m_tempCalibrateTable->rowCount(); i++)//行
    {
        for(int j = 0;j < m_tempCalibrateTable->columnCount(); j++)//列
        {
            m_tempCalibrateTable->setItem(i, j, new QTableWidgetItem());
            m_tempCalibrateTable->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            if(j!=2)//设置不可编辑的单元格
            {
                QTableWidgetItem *item = m_tempCalibrateTable->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

void ExAutoTempCalibrateDialog::SlotCalibrateButton(int row)
{
    if(m_tempCalibrateTable->item(row,2)->text().isEmpty())
    {
        MessageDialog msg(tr("真实值不能为空\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    Script *lua = LuaEngine::Instance()->GetEngine();
    ExTempCalibrateItem* calibrateItem = &m_itemVec[row];

    float value = m_tempCalibrateTable->item(row,2)->text().toFloat();
    bool status;
    lua->call(calibrateItem->calibrateFunc, calibrateItem->index, value);
    lua->pull(status);
    if(status)
    {
        MessageDialog msg(tr("温度校准成功\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }
    else
    {
        MessageDialog msg(tr("温度校准失败\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }
}

void ExAutoTempCalibrateDialog::SlotCheckValue(QTableWidgetItem *item)
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    ExTempCalibrateItem* calibrateItem = &m_itemVec[item->row()];

    String ret =  item->text().toStdString();
    lua->call(calibrateItem->checkValueFunc, item->text().toStdString());
    lua->pull(ret);
    item->setText(ret.c_str());
}

void ExAutoTempCalibrateDialog::SlotDoubleClicked(QTableWidgetItem *item)
{
    if(item->row() < m_itemVec.size())
    {
        if((item->column() == 2 ))
        {
            int curX = cursor().pos().x();
            int curY = cursor().pos().y();
            int curWidth = m_tempCalibrateTable->columnWidth(item->column());
            int curHeight = m_tempCalibrateTable->rowHeight(item->row());

            CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
            if(true == numKbd->isHidden())
            {
                numKbd->myMove(curX, curY, curWidth, curHeight);
                numKbd->show();
            }
        }
    }
}

void ExAutoTempCalibrateDialog::SlotQuitButton()
{
    this->close();
}

void ExAutoTempCalibrateDialog::showEvent(QShowEvent *event)
{
    m_quitButton->setFocus();
    QDialog::showEvent(event);
}

void ExAutoTempCalibrateDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);
    int height = 50;
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
}

ExAutoTempCalibrateDialog::~ExAutoTempCalibrateDialog()
{
    if (m_numberKey)
    {
        delete m_numberKey;
        m_numberKey = nullptr;
    }
}

}

