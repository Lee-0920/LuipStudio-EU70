/******************** (C) COPYRIGHT 2011 LABSUN ********************************
* File Name          : numberKeyboard.cpp
* Author             : WZ
* Version            : V1.0.1
* Date               : 2015/06/24
* Description        : This file provides all the numberKeyboard function.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, LABSUN SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "NumberKeyboard.h"
#include "Log.h"
#include <QDebug>
#include <sys/time.h>


/*******************************************************************************
* Function Name  : CNumberKeyboard
* Description    : 构造函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
CNumberKeyboard::CNumberKeyboard(QWidget * parent)
    :QWidget(parent)
    ,keyButtonWidth(60)
    ,keyButtonHeight(60)
    ,rowSpace(9)
    ,columnSpace(9)
    ,keyButtonVector()
    ,keyCodeVector()
{
    //resize(310, 75);
    resize(300, 300);

    this->setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Window, QColor(215, 220, 228));
    this->setPalette(p);

    // 初始化键码表
    keyCodeVector.append("0");        // KEY_0
    keyCodeVector.append("1");        // KEY_1
    keyCodeVector.append("2");        // KEY_2
    keyCodeVector.append("3");        // KEY_3
    keyCodeVector.append("4");        // KEY_4
    keyCodeVector.append("5");        // KEY_5
    keyCodeVector.append("6");        // KEY_6
    keyCodeVector.append("7");        // KEY_7
    keyCodeVector.append("8");        // KEY_8
    keyCodeVector.append("9");        // KEY_9
    keyCodeVector.append("+");        // KEY_POSITVE
    keyCodeVector.append("-");        // KEY_NEGATIVE
    keyCodeVector.append(".");        // KEY_DOT
    keyCodeVector.append(":");        // KEY_COLON
    keyCodeVector.append("\b");       // KEY_BASPACE
    keyCodeVector.append("q");        // KEY_QUIT
 //   keyCodeVector.append(":");

    // 容错检查
    if (keyCodeVector.size() != MAX_KEY)
    {
//        qDebug() << "keyCodeVector number error";
        logger->error("keyCodeVector number error");
        return;
    }

    // 初始化数据键盘上的控件
    QPushButton * keyPushButton = NULL;

    for(int i = 0; i < MAX_KEY; i++)
    {
        keyPushButton = new QPushButton(this);
        keyButtonVector.append(keyPushButton);
        keyPushButton->setFlat(true);
        keyPushButton->setFocusPolicy(Qt::NoFocus);
        keyPushButton->setFixedSize(keyButtonWidth, keyButtonHeight);
        keyPushButton->setText(QString(keyCodeVector.at(i)));
        keyPushButton->setStyleSheet("QPushButton:pressed { \
                                           border-radius:10px; \
                                           border:1px; \
                                           border-color:gray;\
                                           border-style: inset;\
                                           background-color: rgb(70, 110, 235);\
                                           color:white;}"
                                      "QPushButton{font-size:25px; \
                                           border-radius:10px; \
                                           border:1px; \
                                           border-color:gray;\
                                           border-style: outset;\
                                           background-color: white}");

        if (KEY_BACKSPAC == i)
        {
            keyPushButton->setText("DEL");
            keyPushButton->setStyleSheet("QPushButton:pressed { \
                                              border-radius:10px; \
                                              border:1px; \
                                              border-color:gray; \
                                              border-style: inset; \
                                              background-color: rgb(70, 110, 235); \
                                              color:white;}"
                                         "QPushButton{font-size:20px; \
                                              border-radius:10px; \
                                              border:1px; \
                                              border-color:gray; \
                                              border-style: outset; \
                                              background-color: rgb(190, 190, 190);}");
        }
        if (KEY_QUIT == i)
        {
            keyPushButton->setText(tr("退出"));
            keyPushButton->setStyleSheet("QPushButton:pressed { \
                                              border-radius:10px; \
                                              border:1px; \
                                              border-color:gray; \
                                              border-style: inset; \
                                              background-color: rgb(70, 110, 235); \
                                              color:white;}"
                                         "QPushButton{font-size:20px; \
                                              border-radius:10px; \
                                              border:1px; \
                                              border-color:gray; \
                                              border-style: outset; \
                                              background-color: rgb(190, 190, 190);}");
        }

        // 建立信号和槽的连接
        connect(keyPushButton, SIGNAL(released()), this, SLOT(slotKeyButtonReleased()));
    }

    // 第一行
    QHBoxLayout * firstLineLayout = new QHBoxLayout;
    firstLineLayout->setSpacing(0);
    firstLineLayout->addStretch();
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_7));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_8));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_9));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_BACKSPAC));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addStretch();
    firstLineLayout->setMargin(0);

    // 第二行
    QHBoxLayout * secondLineLayout = new QHBoxLayout;
    secondLineLayout->setSpacing(0);
    secondLineLayout->addStretch();
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_4));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_5));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_6));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_POSITIVE));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addStretch();
    secondLineLayout->setMargin(0);

    // 第三行
    QHBoxLayout * thirdLineLayout = new QHBoxLayout;
    thirdLineLayout->setSpacing(0);
    thirdLineLayout->addStretch();
    thirdLineLayout->addSpacing(columnSpace);
    thirdLineLayout->addWidget(keyButtonVector.at(KEY_1));
    thirdLineLayout->addSpacing(columnSpace);
    thirdLineLayout->addWidget(keyButtonVector.at(KEY_2));
    thirdLineLayout->addSpacing(columnSpace);
    thirdLineLayout->addWidget(keyButtonVector.at(KEY_3));
    thirdLineLayout->addSpacing(columnSpace);
    thirdLineLayout->addWidget(keyButtonVector.at(KEY_NEGATIVE));
    thirdLineLayout->addSpacing(columnSpace);
    thirdLineLayout->addStretch();
    thirdLineLayout->setMargin(0);

    // 第四行
    QHBoxLayout * fourthLineLayout = new QHBoxLayout;
    fourthLineLayout->setSpacing(0);
    fourthLineLayout->addStretch();
    fourthLineLayout->addSpacing(columnSpace);
    fourthLineLayout->addWidget(keyButtonVector.at(KEY_COLON));
    fourthLineLayout->addSpacing(columnSpace);
    fourthLineLayout->addWidget(keyButtonVector.at(KEY_0));
    fourthLineLayout->addSpacing(columnSpace);
    fourthLineLayout->addWidget(keyButtonVector.at(KEY_DOT));
    fourthLineLayout->addSpacing(columnSpace);
    fourthLineLayout->addWidget(keyButtonVector.at(KEY_QUIT));
    fourthLineLayout->addSpacing(columnSpace);
    fourthLineLayout->addStretch();
    fourthLineLayout->setMargin(0);


   /* QHBoxLayout * firstLineLayout = new QHBoxLayout;
    firstLineLayout->setSpacing(0);
    firstLineLayout->addStretch();
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_1));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_3));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_5));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_7));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_9));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_DOT));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addWidget(keyButtonVector.at(KEY_BACKSPAC));
    firstLineLayout->addSpacing(columnSpace);
    firstLineLayout->addStretch();
    firstLineLayout->setMargin(0);

    // 第二行
    QHBoxLayout * secondLineLayout = new QHBoxLayout;
    secondLineLayout->setSpacing(0);
    secondLineLayout->addStretch();
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_2));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_4));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_6));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_8));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_0));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_NEGATIVE));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addWidget(keyButtonVector.at(KEY_QUIT));
    secondLineLayout->addSpacing(columnSpace);
    secondLineLayout->addStretch();
    secondLineLayout->setMargin(0);*/

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->addStretch();
    mainLayout->addSpacing(rowSpace);
    mainLayout->addLayout(firstLineLayout);
    mainLayout->addSpacing(rowSpace);
    mainLayout->addLayout(secondLineLayout);
    mainLayout->addSpacing(rowSpace);
    mainLayout->addLayout(thirdLineLayout);
    mainLayout->addSpacing(rowSpace);
    mainLayout->addLayout(fourthLineLayout);
    mainLayout->addSpacing(rowSpace);
    mainLayout->addStretch();
    mainLayout->setMargin(5);

    this->setLayout(mainLayout);
    setFocusPolicy(Qt::NoFocus);

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
}

/*******************************************************************************
* Function Name  : ~CNumberKeyboard
* Description    : 析构函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
CNumberKeyboard::~CNumberKeyboard(void)
{

}

CNumberKeyboard *CNumberKeyboard::Instance()
{
    static CNumberKeyboard instance;
    if(false == instance.isHidden())
        instance.hide();
    return &instance;
}

bool CNumberKeyboard::eventFilter(QObject *target, QEvent *event)
{
    if(event->type()== QEvent::FocusIn)
    {
        QWidget * focusWidget = (QWidget *)target;
        QPoint g_point = focusWidget->mapToGlobal(QPoint(0, 0));
        myMove(g_point.x(), g_point.y(), focusWidget->width(), focusWidget->height());
        this->show();
    }
    else if(event->type()== QEvent::FocusOut)
    {
        this->hide();
    }
    return QObject::eventFilter(target, event);
}

void CNumberKeyboard::myMove(int iFocusWidgetX, int iFocusWidgetY, int iFocusWidgetWidth, int iFocusWidgetHeight)
{
    int iMovX,iMovY,iWidth,iHeight,iDesktopWidth,iDesktopHeight;

    iWidth = this->width();
    iHeight = this->height();
    iDesktopWidth = QApplication::desktop()->width();
    iDesktopHeight = QApplication::desktop()->height();

    iMovY = iFocusWidgetY + iFocusWidgetHeight;
    if((iMovY + iHeight) > iDesktopHeight)
        iMovY = iFocusWidgetY - iFocusWidgetHeight - iHeight;
    iMovX = iFocusWidgetX - (iWidth - iFocusWidgetWidth) / 2;
    if(iMovX < 0)
        iMovX = 0;
    else if((iMovX + iWidth) > iDesktopWidth)
        iMovX = iDesktopWidth - iWidth;
    if (iMovX < 0)
    {
        iMovX = 10;
    }
    if (iMovY < 0)
    {
        iMovY = 10;
    }
    if (iMovX > 500)
    {
        iMovX = 500;
    }
    if (iMovY > 300)
    {
        iMovY = 300;
    }
    move(iMovX, iMovY);
}
/*******************************************************************************
* Function Name  : slotKeyButtonReleased
* Description    : 按键释放事件处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CNumberKeyboard::slotKeyButtonReleased(void)
{
    // 获取应用程序焦点窗体
    QWidget *focusWidget = QApplication::focusWidget();

    if (NULL == focusWidget)
        return;

    // 获取发送释放按键的按钮句柄
    QObject *keyButtonReleased = sender();

    for (int i = 0; i < MAX_KEY; i++)
    {
        if (keyButtonVector.at(i) != keyButtonReleased)
            continue;
        int key = 0;
        QKeyEvent keyEvent = QKeyEvent(QEvent::KeyPress, Qt::Key_No, Qt::NoModifier);

        switch(i)
        {
        case KEY_0:
        case KEY_1:
        case KEY_2:
        case KEY_3:
        case KEY_4:
        case KEY_5:
        case KEY_6:
        case KEY_7:
        case KEY_8:
        case KEY_9:
            key = keyCodeVector.at(i).toInt();
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 正号
        case KEY_POSITIVE:
            key = Qt::Key_Plus;
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 负号
        case KEY_NEGATIVE:
            key = Qt::Key_Minus;
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 小数点
        case KEY_DOT:
            key = Qt::Key_Period;
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 冒号
        case KEY_COLON:
            key = Qt::Key_Colon;
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 删除按钮
        case KEY_BACKSPAC:
            key = Qt::Key_Backspace;
            keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);

            keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
            QCoreApplication::sendEvent(focusWidget, &keyEvent);
            break;

        // 退出按钮
        case KEY_QUIT:
            QApplication::focusWidget()->clearFocus();
            this->hide();
            break;

        default:break;
        }

    }
}

/*******************************************************************************
* Function Name  : sizeHint
* Description    : 窗体理想尺寸大小
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
QSize CNumberKeyboard::sizeHint(void) const
{
    return QSize(300, 300);
}
