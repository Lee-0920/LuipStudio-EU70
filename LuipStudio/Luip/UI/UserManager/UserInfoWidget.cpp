#include "Log.h"
#include "UserInfoWidget.h"
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
#include "UserEditWidget.h"
#include "System/CopyFile.h"
#include "UI/Frame/LoginDialog.h"

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

unique_ptr<UserInfoWidget> UserInfoWidget::m_instance(nullptr);

UserInfoWidget::UserInfoWidget(QWidget *parent) : QDialog(parent)
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

UserInfoWidget* UserInfoWidget::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new UserInfoWidget);
    }

    return m_instance.get();
}


void UserInfoWidget::SpaceInit()
{
    //数据表格
    measureResultTableWidget = new MQtableWidget();

    measureResultTableWidget->resize(800,350);
    measureResultTableWidget->setColumnCount(5);//列
    measureResultTableWidget->setRowCount(ROW_LINE);
    measureResultTableWidget->setFixedSize(800,350);

    measureResultTableWidget->setColumnWidth(0, 130);
    measureResultTableWidget->setColumnWidth(1, 150);
    measureResultTableWidget->setColumnWidth(2, 70);
    measureResultTableWidget->setColumnWidth(3, 215);
    measureResultTableWidget->setColumnWidth(4, 215);

    m_columnName << "用户名称" << "权限等级" << "状态" << "上次登陆日期" << "创建日期" ;
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

    delButton= new QPushButton();
    delButton->setObjectName("brownButton");
    delButton->setText(tr("删除"));
    delButton->setFont(font);
    delButton->setFixedSize(80,40);

    editButton = new QPushButton();
    editButton->setObjectName("brownButton");
    editButton->setText(tr("编辑"));
    editButton->setFont(font);
    editButton->setFixedSize(80,40);

    addButton = new QPushButton();
    addButton->setObjectName("brownButton");
    addButton->setText("添加");
    addButton->setFont(font);
    addButton->setFixedSize(80,40);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
//    bottomLayout->addWidget(toTopButton);
    bottomLayout->addWidget(toBackButton);
    bottomLayout->addWidget(toNextButton);
//    bottomLayout->addWidget(toBottomButton);
    bottomLayout->addWidget(delButton);
    bottomLayout->addWidget(editButton);
    bottomLayout->addWidget(addButton);
    bottomLayout->setSpacing(10);
//    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
//    mainLayout->addSpacing(10);
    mainLayout->addLayout(bottomLayout);

    this->setLayout(mainLayout);

//    connect(toTopButton,SIGNAL(clicked()), this, SLOT(ToTop()));
//    connect(toBottomButton,SIGNAL(clicked()), this, SLOT(ToBottom()));
    connect(toBackButton,SIGNAL(clicked()), this, SLOT(ToBack()));
    connect(toNextButton,SIGNAL(clicked()), this, SLOT(ToNext()));
    connect(delButton,SIGNAL(clicked()), this, SLOT(SlotDelButton()));
    connect(addButton,SIGNAL(clicked()), this, SLOT(SlotAddButton()));
    connect(editButton,SIGNAL(clicked()), this, SLOT(SlotEditButton()));
    connect(this,SIGNAL(MethodSaveForModbusSignal(System::String)), this, SLOT(SlotMethodSaveForModbus(System::String)));
    connect(this,SIGNAL(MethodApplyForModbusSignal(int)), this, SLOT(SlotMethodApplyForModbus(int)));
    connect(this,SIGNAL(MethodDelectForModbusSignal(int)), this, SLOT(SlotMethodDelectForModbus(int)));
}

void UserInfoWidget::SlotQuitButton()
{
    this->close();
}

void UserInfoWidget::ViewRefresh()
{
    measureResultTableWidget->clear();
    measureResultTableWidget->setColumnAndSize(m_columnName,15);
}

void UserInfoWidget::TableSpaceInit()
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

void UserInfoWidget::ShowTable()
{
    ViewRefresh();
    TableSpaceInit();

    QList<UserRecord> map = DataBaseManager::Instance()->GetUserTable()->SelectData();

    m_showFields.clear();
    m_totalUsers = map.count();
//    qDebug("%d", m_totalUsers);
    //使用倒序的方式存入容器中
    reverse(map.begin(), map.end());
    m_showFields = QVector<UserRecord>::fromList(map);

    //根据当前页码显示方法
    int showIndex = 0, row = 0;
    for (QVector<UserRecord>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
    {
        if(showIndex++ < m_curPage*ROW_LINE)
        {
            continue;
        }
//        qDebug() << iter->methodName;
        int column = 0;
        QString statusStr = DataBaseManager::Instance()->GetUserTable()->GetUserStatus(iter->status);

        measureResultTableWidget->item(row, column++)->setText(iter->userName);
        measureResultTableWidget->item(row, column++)->setText(iter->levelName);
        measureResultTableWidget->item(row, column++)->setText(statusStr);
        measureResultTableWidget->item(row, column++)->setText(QDateTime::fromTime_t(iter->lastLoginTime).toString("yyyy-MM-dd HH:mm:ss"));
        measureResultTableWidget->item(row++, column++)->setText(QDateTime::fromTime_t(iter->dataTime).toString("yyyy-MM-dd HH:mm:ss"));
        if(row > measureResultTableWidget->rowCount()-1)
        {
            break;
        }
    }
}


void UserInfoWidget::showEvent(QShowEvent *event)
{
//    m_quitButton->setFocus();
//    QDialog::showEvent(event);
    ShowTable();
}

//void UserInfoWidget::ShowRow(Uint16 row)
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

void UserInfoWidget::paintEvent(QPaintEvent *event)
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

UserInfoWidget::~UserInfoWidget()
{
    if (m_numberKey)
    {
        delete m_numberKey;
        m_numberKey = nullptr;
    }
}


void UserInfoWidget::ToTop()
{
    m_curPage = 0;
    ShowTable();
}

void UserInfoWidget::ToBottom()
{
    m_curPage = m_totalUsers/ROW_LINE;
    ShowTable();
}


void UserInfoWidget::ToBack()
{
    if(m_curPage)
    {
        m_curPage--;
    }
    ShowTable();
}

void UserInfoWidget::ToNext()
{
    if(m_curPage+1 <= (m_totalUsers/ROW_LINE))
    {
        m_curPage++;
    }
    ShowTable();    
}

void UserInfoWidget::SlotDelButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {        
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString name = measureResultTableWidget->item(row,0)->text();
        QString str = "请确认是否删除[" + name +"]!\n";
        MessageDialog msg(str, this,MsgStyle::YESANDNO);
        if(msg.exec()==QDialog::Rejected)
        {
            return;
        }
        //审计追踪
        DataBaseManager::Instance()->GetAuditTrailTable()->InsertAuditTrail(UI::LoginDialog::userInfo.userName,
                                                                            UI::LoginDialog::userInfo.levelName,
                                                                            "删除用户-" + name,
                                                                            "--",
                                                                            "--",
                                                                            "--");
        DataBaseManager::Instance()->GetUserTable()->DeleteDataFromName(name);
        ShowTable();
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }
}

void UserInfoWidget::SlotEditButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString timeStr = measureResultTableWidget->item(row,0)->text();
        if(!timeStr.isEmpty())
        {
            UserRecord record = m_showFields.at(m_curPage*ROW_LINE + row);
            UserEditWidget *ue = new UserEditWidget(record, this);
            ue->exec();
            ShowTable();
        }
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }

}

void UserInfoWidget::SlotAddButton()
{
    UserRecord record;
    UserEditWidget *ue = new UserEditWidget(record, this);
    ue->exec();
    ShowTable();
}

/*
*[brief]:保存当前方法到数据库
*[param]:String,以#分隔的方法信息字符串
*[note]:[要保证仪器和工作站时间保持一致]
*/
void UserInfoWidget::MethodSaveForModbus(System::String info)
{
    emit MethodSaveForModbusSignal(info);
}

/*
*[brief]:从数据库中查找并应用到当前方法
*[param]:int型时间
*[note]:[要保证仪器和工作站时间保持一致]
*/
void UserInfoWidget::MethodApplyForModbus(int dataTime)
{
    emit MethodApplyForModbusSignal(dataTime);
}

/*
*[brief]:从数据库中查找并删除方法
*[param]:int型时间
*[note]:[要保证仪器和工作站时间保持一致]
*/
void UserInfoWidget::MethodDelectForModbus(int dataTime)
{
     emit MethodDelectForModbusSignal(dataTime);
}

void UserInfoWidget::SlotMethodSaveForModbus(System::String info)
{
//    if(!info.empty())
//    {
//        QString strRd = info.c_str();
//        QStringList list = strRd.split("#");
//        int members = 13;
//        if(list.count() == members)
//        {
//            MethodRecord record;
//            record.methodName = list.at(0);
//            record.createTime = QDateTime::fromString(list.at(1), "yyyy-MM-dd HH:mm:ss").toTime_t();
//            record.meaType = list.at(2).toInt();
//            record.turboMode = list.at(3).toInt();
//            record.ICRMode = list.at(4).toInt();
//            record.TOCMode = list.at(5).toInt();
//            record.ECMode = list.at(6).toInt();
//            record.autoReagent = list.at(7).toInt();
//            record.reagent1Vol = list.at(8).toFloat();
//            record.reagent2Vol = list.at(9).toFloat();
//            record.normalRefreshTime = list.at(10).toInt();
//            record.measureTimes = list.at(11).toInt();
//            record.rejectTimes = list.at(12).toInt();
//            emit MethodUpdateSignal(record, true, true);
//        }
//        else
//        {
//            logger->warn("Modbus方法保存异常，成员存在空值");
//        }
//    }
}

void UserInfoWidget::SlotMethodApplyForModbus(int dataTime)
{
//    QString timeStr = QDateTime::fromTime_t(dataTime).toString("yyyy-MM-dd hh:mm:ss");
//    if(!timeStr.isEmpty())
//    {
//        MethodRecord record;
//        foreach (MethodRecord item, m_showFields)
//        {
//            if(item.createTime == QDateTime::fromString(timeStr, "yyyy-MM-dd HH:mm:ss").toTime_t())
//            {
//                record = item;
//                emit MethodUpdateSignal(record, false, true);
//                break;
//            }
//        }
//    }
}

void UserInfoWidget::SlotMethodDelectForModbus(int dataTime)
{
//    QDateTime time = QDateTime::fromTime_t(dataTime);
//    DataBaseManager::Instance()->GetMethodTable()->DeleteMethod(time);
//    ShowTable();
}

}

