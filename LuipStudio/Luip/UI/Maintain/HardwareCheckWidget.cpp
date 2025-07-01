#include "Interface/Wqimc/WqimcManager.h"
#include "UseResourceWidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include <QtAlgorithms>
#include "AlarmManager/AlarmManager.h"
#include "Setting/SettingManager.h"
#include "StatusManager/StatusManager.h"
#include "Log.h"
#include "Common.h"
#include "HardwareCheckWidget.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/ControlConfig.h"

using System::Uint32;
using namespace OOLUA;
using namespace std;
using namespace Configuration;
using namespace Lua;
using namespace Interface::Wqimc::Api::Manager;
using namespace Interface::Wqimc;
using namespace Measure;

namespace UI
{

HardwareCheckWidget::HardwareCheckWidget(QWidget *parent) :
        QWidget(parent)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this); //向窗口更新器注册
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.profile.hardwareParamIterms", itermsTable);

    int useRowCount = 0;
    int administratorRow = 0;
    int superRow = 0;

    itermsTable.at("rowCount", useRowCount);
    itermsTable.at("administratorRow", administratorRow);
    itermsTable.safe_at("superRow", superRow);
    itermsTable.at("synchronize", m_synchronize);
    itermsTable.at("name", m_profileName);
    itermsTable.safe_at("backupParam", m_backupParam);

    QFont font;                           //字体
    font.setPointSize(16);                //字体大小

    this->resize(650,430);//设置表格

    // QTableWidget是QT程序中常用的显示数据表格的空间
    hardwareCheckTableWidget = new MQtableWidget();

    hardwareCheckTableWidget->setColumnCount(5);//设置列数为5

    m_tableInfo.userRow = useRowCount;
    m_tableInfo.hiddenRow = administratorRow + superRow; //管理员和工程师权限需隐藏行数总和
    m_tableInfo.perPageRow = 6;
    m_tableInfo.realRow = 0;
    RowHiddenHandler::Instance()->CreateRow(hardwareCheckTableWidget, m_tableInfo);

    hardwareCheckTableWidget->setFixedSize(586,341);

    QFont headerFont;
    headerFont.setPointSize(15);
    headerFont.setWeight(QFont::DemiBold);

    //QTableWidgetItem用来表示表格中的一个单元格，整个表格都需要用逐个单元格构建起来。
    QTableWidgetItem *column0 = new QTableWidgetItem(tr("类别"));
    column0->setFont(headerFont);
    hardwareCheckTableWidget->setHorizontalHeaderItem(0, column0);

    QTableWidgetItem *column1 = new QTableWidgetItem(tr("标准项"));
    column1->setFont(headerFont);
    hardwareCheckTableWidget->setHorizontalHeaderItem(1, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem(tr("参考值"));
    column2->setFont(headerFont);
    hardwareCheckTableWidget->setHorizontalHeaderItem(2, column2);

    QTableWidgetItem *column3 = new QTableWidgetItem(tr("校准值"));
    column3->setFont(headerFont);
    hardwareCheckTableWidget->setHorizontalHeaderItem(3, column3);

    QTableWidgetItem *column4 = new QTableWidgetItem(tr("单位"));
    column4->setFont(headerFont);
    hardwareCheckTableWidget->setHorizontalHeaderItem(4, column4);

    hardwareCheckTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    hardwareCheckTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        hardwareCheckTableWidget->verticalScrollBar()->setStyleSheet
                ("QScrollBar{background-color:rgb(200,200,200); width: 36;}"
                 "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background-color:rgb(240,240,240)}");
    }

    QFont dataFont = hardwareCheckTableWidget->font();
    dataFont.setPointSize(12);
    hardwareCheckTableWidget->setFont(dataFont);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(hardwareCheckTableWidget);
    measureResultTableLayout->setContentsMargins(0, 0, 0, 0);

    backupButton = new QPushButton();
    backupButton->setObjectName("brownButton");
    backupButton->setText(tr("参数备份"));
    backupButton->setFont(font);
    backupButton->setFixedSize(120,50);

    m_tempCalibrateButton = new QPushButton();
    m_tempCalibrateButton->setObjectName("brownButton");
    m_tempCalibrateButton->setText(tr("热电偶校准"));
    m_tempCalibrateButton->setFont(font);
    m_tempCalibrateButton->setFixedSize(120,50);

    m_exTempCalibrateButton = new QPushButton();
    m_exTempCalibrateButton->setObjectName("brownButton");
    m_exTempCalibrateButton->setText(tr("PT1000校准"));
    m_exTempCalibrateButton->setFont(font);
    m_exTempCalibrateButton->setFixedSize(120,50);

    applicationButton = new QPushButton();
    applicationButton->setObjectName("brownButton");
    applicationButton->setText(tr("更改应用"));
    applicationButton->setFont(font);
    applicationButton->setFixedSize(120,50);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(backupButton);
    bottomLayout->addWidget(m_exTempCalibrateButton);
    bottomLayout->addWidget(m_tempCalibrateButton);
    bottomLayout->addWidget(applicationButton);
    bottomLayout->setContentsMargins(0, 30, 0, 0);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addLayout(bottomLayout);
    leftLayout->addStretch();
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    leftLayout->addStretch();
    this->setLayout(mainLayout);

    hardwareCheckTableWidget->setColumnWidth(0,110);    //设置每行或每列的高度或宽度
    hardwareCheckTableWidget->setColumnWidth(1,124);
    hardwareCheckTableWidget->setColumnWidth(2,117);
    hardwareCheckTableWidget->setColumnWidth(3,107);
    if (m_tableInfo.userRow > m_tableInfo.perPageRow)
    {
        hardwareCheckTableWidget->setColumnWidth(4, 86);
    }
    else
    {
        hardwareCheckTableWidget->setColumnWidth(4, 120);
    }
    for(int i = 0;i < hardwareCheckTableWidget->rowCount();i++)
    {
        hardwareCheckTableWidget->setRowHeight(i,50);
    }
    hardwareCheckTableWidget->setFont(font);

    SpaceInit();

    int type = SettingManager::Instance()->GetParamType();

    Uint8 row = 0;
    oolua_ipairs(itermsTable)
    {
        Table typeTable;
        String typeText;
        HardwareCategory hardwareCategory;
        lua->pull(typeTable);

        typeTable.at("text", typeText);
        QString qTypeTest;
        qTypeTest = QString::fromStdString(typeText);
        QFont typeTestFont = hardwareCheckTableWidget->item(row, 1)->font();
        typeTestFont.setPointSize(GetPointSize(qTypeTest, hardwareCheckTableWidget->columnWidth(0)-10));
        hardwareCheckTableWidget->item(row, 0)->setFont(typeTestFont);
        hardwareCheckTableWidget->item(row, 0)->setText(QObject::tr(typeText.c_str(), "Hardware"));//显示类别

        typeTable.at("get", hardwareCategory.get);
        typeTable.at("set", hardwareCategory.set);

        oolua_ipairs(typeTable)
        {
            Table itermTable;
            lua->pull(itermTable);

            int targetPrivilege = 0;
            bool isTarget = itermTable.safe_at("targetPrivilege", targetPrivilege);

            if (isTarget == false || (type & targetPrivilege))
            {
                HardwareItem hardwareItem;
                String text, unit;


                itermTable.at("refData", hardwareItem.refData);
                itermTable.at("get", hardwareItem.get);
                itermTable.at("checkValue", hardwareItem.checkValueFunc);
                itermTable.at("set", hardwareItem.set);
                itermTable.at("name", hardwareItem.name);

                itermTable.at("writePrivilege", hardwareItem.writePrivilege);
                itermTable.at("readPrivilege", hardwareItem.readPrivilege);

                itermTable.at("text", text);
                QString tmpText;
                tmpText = QString::fromStdString(text);
                QFont tmpFont = hardwareCheckTableWidget->item(row, 1)->font();
                tmpFont.setPointSize(GetPointSize(tmpText, hardwareCheckTableWidget->columnWidth(1)-10));
                hardwareCheckTableWidget->item(row, 1)->setFont(tmpFont);
                hardwareCheckTableWidget->item(row, 1)->setText(QObject::tr(text.c_str(), "Hardware"));//显示名称

                if (itermTable.safe_at("unit", unit))
                {
                    hardwareCheckTableWidget->item(row, 4)->setText(QObject::tr(unit.c_str()));//显示单位
                }
                hardwareCategory.items.insert(make_pair(row, hardwareItem));

                m_singleItems.insert(make_pair(row, hardwareItem));
                row++;
            }
        }
        oolua_ipairs_end()
        m_category.push_back(hardwareCategory);
    }
    oolua_ipairs_end()

    originalData = new float[m_tableInfo.userRow];
    ConfigInit();

    m_tempCalibrateDialog = new AutoTempCalibrateDialog();
    m_tempCalibrateDialog->hide();

    m_exTempCalibrateDialog = new ExAutoTempCalibrateDialog();
    m_exTempCalibrateDialog->hide();

    OnUserChange();

    connect(backupButton, SIGNAL(clicked(bool)), this, SLOT(SlotBackupButton()));
    connect(m_tempCalibrateButton, SIGNAL(clicked(bool)), this, SLOT(SlotTempCalibrateButton()));
    connect(m_exTempCalibrateButton, SIGNAL(clicked(bool)), this, SLOT(SlotExTempCalibrateButton()));
    connect(applicationButton, SIGNAL(clicked(bool)), this, SLOT(ConfigApplication()));
    connect(hardwareCheckTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(SlotDoubleClicked(QTableWidgetItem *)));
    connect(hardwareCheckTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(CheckValue(QTableWidgetItem *)));

    m_isChangeParam = false;
    m_isGetParam = false;
    m_getParamTimer = new QTimer(this);
    connect(m_getParamTimer, SIGNAL(timeout()), this, SLOT(SlotUpdateData()));

    m_setParamTimer = new QTimer(this);
    connect(m_setParamTimer, SIGNAL(timeout()), this, SLOT(SlotChangeParam()));

    m_getParamTimer->start(50);
}

void HardwareCheckWidget::showEvent(QShowEvent *event)
{
    CNumberKeyboard *numberKey = new CNumberKeyboard(this);
    if (numberKey)
    {
        delete numberKey;
        numberKey = nullptr;
    }
    (void)event;
    if(false == m_isGetParam)//获取下位机的参数时需要时间的，防止当前没有获取完再一次重新获取
    {
        m_getParamTimer->start(50);//启动获取下位机参数的定时器
    }
}

/**
 * @brief HardwareCheckWidget::SlotUpdateData 获取下位机参数并更新界面
 */
void HardwareCheckWidget::SlotUpdateData()
{
    if(false == m_isChangeParam)//判断是否正在设置下位机参数，如果在设置，等到设置完成后再获取下位机参数
    {
        m_isGetParam = true;
        m_getParamTimer->stop();

        Script *lua = LuaEngine::Instance()->GetEngine();
        lua->call(m_synchronize);//同步数据

        for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
        {
            lua->call(categoryIter->get);
            for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
            {
                Uint8 row = itemsIter->first;
                HardwareItem &item = itemsIter->second;
                float ret;
                lua->call(item.get);
                lua->pull(ret);
                originalData[row] = ret;

                if (0 == ret || 0 == ((int)(ret*1000000)%100))
                {
                    hardwareCheckTableWidget->item(row, 3)->setText(QString::number(ret));
                }
                else
                {
                    hardwareCheckTableWidget->item(row, 3)->setText(QString::number(ret,'f',6));
                }
            }
        }
        m_isGetParam = false;
    }
}

void HardwareCheckWidget::SpaceInit()
{
    for(int i = 0;i < hardwareCheckTableWidget->rowCount(); i++)//行
    {
        for(int j = 0;j < hardwareCheckTableWidget->columnCount(); j++)//列
        {           
            hardwareCheckTableWidget->setItem(i, j, new QTableWidgetItem());
            hardwareCheckTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);//设置对齐方式
            if(j!=3 || (i >= m_tableInfo.userRow))//设置不可编辑的单元格
            {
                QTableWidgetItem *item = hardwareCheckTableWidget->item(i, j);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setFlags(Qt::ItemIsEnabled);
            }
        }
    }
}

/**
 * @brief HardwareCheckWidget::ConfigInit 显示各个项目的参考值
 */
void HardwareCheckWidget::ConfigInit()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
    {
        for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
        {
            Uint8 row = itemsIter->first;
            HardwareItem &item = itemsIter->second;
            float ret;

            luaEngine->GetLuaValue(state, "config.hardwareConfig." + item.refData, ret);
            hardwareCheckTableWidget->item(row, 2)->setText(QString::number(ret));
        }
    }
}

void HardwareCheckWidget::SlotDoubleClicked(QTableWidgetItem *item)
{
    if(item->row() < m_singleItems.size())
    {
        if(LoginDialog::userType >= m_singleItems.at(item->row()).writePrivilege)
        {
            if((item->column() == 3 ) &&
                    item->row() < m_tableInfo.userRow)
            {
                int curX = cursor().pos().x();
                int curY = cursor().pos().y();
                int curWidth = hardwareCheckTableWidget->columnWidth(item->column());
                int curHeight = hardwareCheckTableWidget->rowHeight(item->row());

                CNumberKeyboard *numKbd = CNumberKeyboard::Instance();
                if(true == numKbd->isHidden())
                {
                    numKbd->myMove(curX, curY, curWidth, curHeight);
                    numKbd->show();
                }
            }
        }
    }
}

/**
 * @brief HardwareCheckWidget::SlotChangeParam 点击应用后设置下位机参数
 */
void HardwareCheckWidget::SlotChangeParam()
{
    m_setParamTimer->stop();
    m_isChangeParam = true;
    bool isUpdater = false;
    QString message;

    Script *lua = LuaEngine::Instance()->GetEngine();

    for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
    {
        bool isChange = false;
        //查找某一类别里的所有行是否有改动
        for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
        {
            Uint8 row = itemsIter->first;
            if(qAbs((originalData[row] - hardwareCheckTableWidget->item(row, 3)->text().toFloat())) > PRECISE)
            {
                isChange = true;
                isUpdater = true;
                break;
            }
        }
        if (isChange)
        {
            //某一类别的所有行，获取界面上的值，并调用设置函数，把值保存在缓存中。
            ConfigVector configVector;
            for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
            {
                Uint8 row = itemsIter->first;
                HardwareItem &item = itemsIter->second;
                float ret =  hardwareCheckTableWidget->item(row, 3)->text().toFloat();
                lua->call(item.set, ret);

                Config config;
                config.__set_profile(m_profileName);
                config.__set_config(item.name);
                config.__set_value(hardwareCheckTableWidget->item(row, 3)->text().toStdString());
                configVector.Push(config);
            }
            bool ret = false;
            String str;
            lua->call(categoryIter->set);//调用类别的设置函数，把缓存中的值设置到下位机
            lua->pull(str);//取出执行set后的错误信息
            lua->pull(ret);//取出执行set后的结果
            if (ret == false)//执行失败
            {
                message += QObject::tr(str.c_str());
            }
            else
            {
                QThread::sleep(1);
                bool isEquation = true;
                lua->call(categoryIter->get);//获取下位机值
                for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
                {
                    Uint8 row = itemsIter->first;
                    HardwareItem &item = itemsIter->second;
                    float ret;
                    lua->call(item.get);
                    lua->pull(ret);
                    if (qAbs(ret - hardwareCheckTableWidget->item(row, 3)->text().toFloat())>PRECISE)//对比下位机的值与界面上设置值是否一样
                    {
                        QString err = hardwareCheckTableWidget->item(row, 1)->text() + "无法设置到下位机，设置值 "
                                + hardwareCheckTableWidget->item(row, 3)->text() + ",当前值 " + QString::number(ret);
                        logger->notice(err.toStdString());
                        isEquation = false;
                        break;
                    }
                }
                Uint8 row = categoryIter->items.begin()->first;
                QString typeText = hardwareCheckTableWidget->item(row, 0)->text();//取出类别名
                if (isEquation)
                {
                    typeText += tr("设置成功\n");
                    if (configVector.Size() > 0)
                    {
                        WqimcManager::Instance()->updateConfigs(configVector);
                    }
                }
                else
                {
                    typeText += tr("设置失败\n");
                }
                logger->notice(typeText.toStdString());
                message += typeText;
            }
        }
    }
    m_isChangeParam = false;
    if(true == isUpdater)
    {
        logger->info("修改硬件校准参数");
    }
    else
    {

        message = tr("无任何参数更改\n");
    }

    MessageDialog msg(message, this);
    msg.exec();

    if(false == m_isGetParam)//设置完后重新读取下位机参数
    {
        m_getParamTimer->start(50);
    }
}

void HardwareCheckWidget::ConfigApplication()
{
//    if (!StatusManager::Instance()->IsIdleStatus())  // 空闲状态
//    {
//        MessageDialog msg("请确认仪器状态是否为空闲！\n", this,MsgStyle::ONLYOK);
//        if(msg.exec()==QDialog::Accepted)
//        {
//            return;
//        }
//    }

    if(false == m_isChangeParam)//避免重复点击应用按钮
    {
        m_setParamTimer->start(50);
    }
}

void HardwareCheckWidget::SlotTempCalibrateButton()
{
    if(m_tempCalibrateDialog)
    {
        m_tempCalibrateDialog->exec();
    }
}

void HardwareCheckWidget::SlotBackupButton()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.ui.profile.hardwareParamIterms", itermsTable);
    bool isBackup = itermsTable.safe_at("backupParam", m_backupParam);
    QString infoText = "参数备份失败，SP版本不支持";
    bool ret = false;
    if (isBackup == false)
    {
        logger->info(infoText.toStdString().c_str());
    }
    else
    {
       lua->call(m_backupParam);//同步数据
       lua->pull(ret);
       if(ret)
       {
           infoText = "参数备份成功!";
       }
       else
       {
           infoText = "通信异常，参数备份失败!";
       }
    }

    MessageDialog msg(infoText, this, MsgStyle::ONLYOK);
    if(msg.exec()== QDialog::Accepted)
    {
        return;
    }
}

void HardwareCheckWidget::SlotExTempCalibrateButton()
{
    if(m_exTempCalibrateDialog)
    {
        m_exTempCalibrateDialog->exec();
    }
}

void HardwareCheckWidget::OnUserChange()
{
    if(LoginDialog::userType == Super)
    {
        applicationButton->show();
        hardwareCheckTableWidget->setColumnWidth(4, 90);
        for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
        {
            for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
            {
                Uint8 row = itemsIter->first;

                hardwareCheckTableWidget->setRowHidden(row, false);

                QTableWidgetItem *item = hardwareCheckTableWidget->item(row, 3);
                item->setFlags((Qt::ItemFlags)63);
            }
        }
    }
    else if(LoginDialog::userType == Administrator)
    {
        applicationButton->show();
        hardwareCheckTableWidget->setColumnWidth(4, 125);
        for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
        {
            for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
            {
                Uint8 row = itemsIter->first;
                HardwareItem &hardwareItem = itemsIter->second;
                if (Administrator >= hardwareItem.readPrivilege)
                {
                    hardwareCheckTableWidget->setRowHidden(row, false);
                }
                else
                {
                    hardwareCheckTableWidget->setRowHidden(row, true);
                }

                QTableWidgetItem *item = hardwareCheckTableWidget->item(row, 3);
                if (Administrator >= hardwareItem.writePrivilege)
                {
                    item->setFlags((Qt::ItemFlags)63);
                }
                else
                {
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    item->setFlags(Qt::ItemIsEnabled);
                }
            }
        }
    }
    else if(LoginDialog::userType == Maintain)
    {
        applicationButton->hide();
        for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
        {
            for (map<Uint8, HardwareItem>::iterator itemsIter = categoryIter->items.begin();itemsIter != categoryIter->items.end(); ++itemsIter)
            {
                Uint8 row = itemsIter->first;
                HardwareItem &hardwareItem = itemsIter->second;
                if (Maintain >= hardwareItem.readPrivilege)
                {
                    hardwareCheckTableWidget->setRowHidden(row, false);
                }
                else
                {
                    hardwareCheckTableWidget->setRowHidden(row, true);
                }

                QTableWidgetItem *item = hardwareCheckTableWidget->item(row, 3);
                if (Maintain >= hardwareItem.writePrivilege)
                {
                    item->setFlags((Qt::ItemFlags)63);
                }
                else
                {
                    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                    item->setFlags(Qt::ItemIsEnabled);
                }
            }
        }
    }

    RowHiddenHandler::Instance()->AdjustRowHidden(hardwareCheckTableWidget, m_tableInfo);
}

/**
 * @brief HardwareCheckWidget::CheckValue 检查输入的数值是否符合规则
 */
void HardwareCheckWidget::CheckValue(QTableWidgetItem *item)
{
    for (std::vector<HardwareCategory>::iterator categoryIter = m_category.begin();categoryIter != m_category.end(); ++categoryIter)
    {
        if (categoryIter->items.count(item->row()))
        {
            Script *lua = LuaEngine::Instance()->GetEngine();
            HardwareItem hardwareItem = categoryIter->items[item->row()];

            String ret =  item->text().toStdString();
            lua->call(hardwareItem.checkValueFunc, ret);
            lua->pull(ret);
            item->setText(ret.c_str());
        }
    }
}

int HardwareCheckWidget::GetPointSize(QString& text, int limitWidth)
{
    QFont font = hardwareCheckTableWidget->font();
    int size = font.pointSize();

    QFontMetrics fontWidth(font);
    int textWidth = fontWidth.width(text);

    while(size > 8 && textWidth > limitWidth)
    {
        size = size - 1;
        font.setPointSize(size);

        QFontMetrics fontWidth(font);
        textWidth = fontWidth.width(text);
    }

    if (size >FONT_SIZE_SETTINGS(12))
    {
        size = FONT_SIZE_SETTINGS(12);
    }
    return size;
}

HardwareCheckWidget::~HardwareCheckWidget()
{

}

void HardwareCheckWidget::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)message;
    if (event == UpdateEvent::WqimcChangeParam || event == UpdateEvent::AutoPumpCheck)
    {
        if(false == m_isGetParam)
        {
            m_getParamTimer->start(50);
        }
    }
}

}
