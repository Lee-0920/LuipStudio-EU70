#include "InputKeyBoard.h"
///#include "def.h"

#include <QMessageBox>
#include <QDebug>
#include <QByteArray>

#include <time.h>
#include <stdio.h>
#include<sys/time.h>

///InputKeyBoard *InputKeyBoard::instance = NULL;

std::unique_ptr<InputKeyBoard> InputKeyBoard::m_instance(nullptr);

//小写
QString lowerLetter[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", ":", "+", "-", "*", "/", "DEL", "空", ".", "?!@", "返回", "回车",  \
                         "a", "b", "c", "d", "e","f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", \
                         "w", "x", "y", "z", "en", "Enter", "123", "Shift", "Space", ",", ".", "", "", "?!@", "DEL", "", "", "", "", "", "",\
                         "~", "`", "!", "@", "#", "$", "%", "^", "&&", "*", "(", ")", "_", "-", "+", "=", "{", "}", "[", "]",\
                         "|", "\\", ":", ";", "\"", "'", "<", ">", ",", "DEL", ".", "。", "?", "/", "《", "》", "", "", "", "返回", "退出"};//
//大写
QString upperLetter[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "@", ":", "+", "-", "*", "/", "DEL", "空", ".", "?!@", "返回", "回车",  \
                         "A", "B", "C", "D", "E","F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", \
                         "W", "X", "Y", "Z", "EN", "Enter", "123", "Shift", "Space", ",", ".", "", "", "?!@", "DEL", "", "", "", "", "", "",\
                         "~", "`", "!", "@", "#", "$", "%", "^", "&&", "*", "(", ")", "_", "-", "+", "=", "{", "}", "[", "]",\
                         "|", "\\", ":", ";", "\"", "'", "<", ">", ",", "DEL", ".", "。", "?", "/", "《", "》", "", "", "", "返回", "退出"};//

/*******************************************************************************
* Function Name  : InputKeyBoard
* Description    : 构造函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
InputKeyBoard::InputKeyBoard(QWidget * parent)
    : QDialog(parent)
    , isLower(true)
    , isChinese(false)
    , isNumber(false)
    , isSymbol(false)
    , keyButtonWidth(48)
    , keyButtonHeight(48)
    , rowSpace(6)
    , columnSpace(6)
    , keyButtonVector()
    , keyCodeVector()
    , pageCount(0)
    , topRowHeight(75)
{

    this->setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Window, QColor(215, 220, 228));
    this->setPalette(p);

    numberWidget = new QWidget(this);
    numberWidget->move(0, 0);
    numberWidget->setFixedSize(280, 230);
    numberWidget->hide();
    numberWidget->setFocusPolicy(Qt::NoFocus);

    symbolWidget = new QWidget(this);
    symbolWidget->move(0, 0);
    symbolWidget->setFixedSize(530, 230);
    symbolWidget->hide();
    symbolWidget->setFocusPolicy(Qt::NoFocus);

    topRowWidget = new QWidget();
    topRowWidget->setFixedSize(530, topRowHeight);
    topRowWidget->move(0, 0);
    topRowWidget->hide();

    inputLineEdit = new QLineEdit();
    inputLineEdit->setFixedSize(508, 30);

    charWidget = new QWidget(this);
    charWidget->move(0, 0);
    charWidget->setFixedSize(530, 310 - topRowHeight);
    charWidget->show();
    charWidget->setFocusPolicy(Qt::NoFocus);

    initDialog();
    setInputMethod();
    setFocusPolicy(Qt::NoFocus);
    this->setStyleSheet("QDialog{border:none;}");

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
}

//InputKeyBoard *InputKeyBoard::getInstance(QWidget * parent)
//{
//    qDebug("getInstance");
//    if(instance == NULL)
//    {
//        qDebug("new cinputmethod");
//        instance = new InputKeyBoard(parent);
//    }

//    return instance;
//}

//InputKeyBoard *InputKeyBoard::Instance()
//{
//    static InputKeyBoard instance;
//    if(false == instance.isHidden())
//        instance.hide();
//    return &instance;
//}

InputKeyBoard *InputKeyBoard::Instance()
{
    static InputKeyBoard instance;
    if(false == instance.isHidden())
        instance.hide();
    return &instance;
}

/**
 * @brief 获取全局唯一的 FlowManager 对象（单件）。
 */
//InputKeyBoard* InputKeyBoard::Instance()
//{
//    if (!m_instance)
//    {
//        m_instance.reset(new InputKeyBoard);
//    }

//    return m_instance.get();
//}


/*******************************************************************************
* Function Name  : ~InputKeyBoard
* Description    : 析构函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
InputKeyBoard::~InputKeyBoard()
{

}

/*******************************************************************************
* Function Name  : setInputMethod
* Description    : 设置输入法函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::setInputMethod()
{
    if(isNumber && !isSymbol)//数字输入法
    {
        resize(numberWidget->width(), numberWidget->height());
        numberWidget->show();
        charWidget->hide();
        symbolWidget->hide();
    }
    else if(!isNumber && !isSymbol)//字符输入
    {
        resize(charWidget->width(), charWidget->height());
        numberWidget->hide();
        charWidget->show();
        symbolWidget->hide();
    }
    else if(isSymbol)//标点符号输入
    {
        resize(symbolWidget->width(), symbolWidget->height());
        charWidget->hide();
        numberWidget->hide();
        symbolWidget->show();
    }

    //中英文切换状态设置
    keyButtonVector.at(KEY_en)->setText(isChinese ? QString("中") : QString("EN"));

    //清键值
    keyCodeVector.clear();

    //大小写切换
    for(int i=0; i<MAX_KEY; i++)
    {
        if(isLower)
            keyCodeVector.append(lowerLetter[i]);
        else
            keyCodeVector.append(upperLetter[i]);
    }

    for(int i=0; i<MAX_KEY; i++)
    {
        keyButtonVector.at(i)->setText(QString(keyCodeVector.at(i)));
    }

}

/*******************************************************************************
* Function Name  : initDialog
* Description    : 初始化界面函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::initDialog()
{
    // 初始化键码表
    for(int i=0; i<MAX_KEY; i++)
        keyCodeVector.append(lowerLetter[i]);

    // 初始化键盘上的控件
    QPushButton * keyPushButton = NULL;
    for(int i = 0; i < MAX_KEY; i++)
    {
        keyPushButton = new QPushButton;
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

        if (KEY_BackSpace == i || KEY_numberBackSpace == i || KEY_SymbolBackSpace == i)
        {
            keyPushButton->setText("DEL");
            if(KEY_BackSpace == i)
            {
                keyPushButton->setFixedSize(keyButtonWidth * 1.5, keyButtonHeight);
            }

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
        else if(KEY_Space == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 3.1, keyButtonHeight);
        }
        else if(KEY_Enter == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 2, keyButtonHeight);
        }
        else if(KEY_QUIT == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 1.5, keyButtonHeight);
        }
        else if(KEY_number == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 1.5, keyButtonHeight);
        }
        else if(KEY_CapsLock == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 1.5, keyButtonHeight);
        }
        else if(KEY_Hanzi1 == i || KEY_Hanzi2 == i || KEY_Hanzi3 == i || KEY_Hanzi4 == i || KEY_Hanzi5 == i || KEY_Hanzi6 == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth * 8/6.0, keyButtonHeight/1.2);
        }
        else if(KEY_nextPage == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth, keyButtonHeight/1.2);

            QPixmap pixmap(":/img/right");
            keyPushButton->setIcon(pixmap);
            keyPushButton->setIconSize(pixmap.size());
        }
        else if(KEY_prePage == i)
        {
            keyPushButton->setFixedSize(keyButtonWidth, keyButtonHeight/1.2);

            QPixmap pixmap(":/img/left");
            keyPushButton->setIcon(pixmap);
            keyPushButton->setIconSize(pixmap.size());
        }

        // 建立信号和槽的连接
        connect(keyPushButton, SIGNAL(released()), this, SLOT(slotKeyButtonReleased()));
    }


    //汉字输入时汉字选择界面
    QHBoxLayout *topRowLayout = new QHBoxLayout();
    topRowLayout->addWidget(keyButtonVector.at(KEY_prePage));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi1));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi2));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi3));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi4));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi5));
    topRowLayout->addWidget(keyButtonVector.at(KEY_Hanzi6));
    topRowLayout->addWidget(keyButtonVector.at(KEY_nextPage));
    topRowLayout->setMargin(0);
    topRowLayout->setSpacing(4);
    topRowLayout->setAlignment(Qt::AlignLeft);

    QVBoxLayout *topWidgetLayout = new QVBoxLayout(topRowWidget);
    topWidgetLayout->addWidget(inputLineEdit);
    topWidgetLayout->addLayout(topRowLayout);
    topWidgetLayout->setMargin(0);
    topWidgetLayout->setSpacing(4);
    topWidgetLayout->setAlignment(Qt::AlignLeft);

    //字符输入第一行
    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    firstRowLayout->setSpacing(2);
    firstRowLayout->addStretch();
    firstRowLayout->addWidget(keyButtonVector.at(KEY_q));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_w));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_e));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_r));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_t));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_y));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_u));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_i));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_o));
    firstRowLayout->addWidget(keyButtonVector.at(KEY_p));
    firstRowLayout->addStretch();

    //字符输入第二行
    QHBoxLayout *secondRowLayout = new QHBoxLayout();
    secondRowLayout->setSpacing(2);
    secondRowLayout->addStretch();
    secondRowLayout->addSpacing(columnSpace);
    secondRowLayout->addWidget(keyButtonVector.at(KEY_a));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_s));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_d));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_f));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_g));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_h));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_j));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_k));
    secondRowLayout->addWidget(keyButtonVector.at(KEY_l));
    secondRowLayout->addStretch();

    //字符输入第三行
    QHBoxLayout *thirdRowLayout = new QHBoxLayout();
    thirdRowLayout->setSpacing(2);
    thirdRowLayout->addStretch();
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_CapsLock));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_z));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_x));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_c));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_v));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_b));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_n));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_m));
    thirdRowLayout->addWidget(keyButtonVector.at(KEY_BackSpace));
    thirdRowLayout->addStretch();

    //字符输入第四行
    QHBoxLayout *forthRowLayout = new QHBoxLayout();
    forthRowLayout->setSpacing(2);
    forthRowLayout->addStretch();
    forthRowLayout->addWidget(keyButtonVector.at(KEY_number));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_en));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_Comma));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_Space));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_Period));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_symbol));
    //forthRowLayout->addWidget(keyButtonVector.at(KEY_Enter));
    forthRowLayout->addWidget(keyButtonVector.at(KEY_QUIT));
    forthRowLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(charWidget);
    mainLayout->addWidget(topRowWidget);
    mainLayout->addLayout(firstRowLayout);
    mainLayout->addLayout(secondRowLayout);
    mainLayout->addLayout(thirdRowLayout);
    mainLayout->addLayout(forthRowLayout);
    mainLayout->setSpacing(2);

    //数字输入第一行
    QHBoxLayout *firstRowNumberLayout = new QHBoxLayout();
    firstRowNumberLayout->setSpacing(2);
    firstRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberPlus));
    firstRowNumberLayout->addWidget(keyButtonVector.at(KEY_1));
    firstRowNumberLayout->addWidget(keyButtonVector.at(KEY_2));
    firstRowNumberLayout->addWidget(keyButtonVector.at(KEY_3));
    firstRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberBackSpace));

    //数字输入第二行
    QHBoxLayout *secondRowNumberLayout = new QHBoxLayout();
    secondRowNumberLayout->setSpacing(2);
    secondRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberMinus));
    secondRowNumberLayout->addWidget(keyButtonVector.at(KEY_4));
    secondRowNumberLayout->addWidget(keyButtonVector.at(KEY_5));
    secondRowNumberLayout->addWidget(keyButtonVector.at(KEY_6));
    secondRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberSpace));

    //数字输入第三行
    QHBoxLayout *thirdRowNumberLayout = new QHBoxLayout();
    thirdRowNumberLayout->setSpacing(2);
    thirdRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberMultiply));
    thirdRowNumberLayout->addWidget(keyButtonVector.at(KEY_7));
    thirdRowNumberLayout->addWidget(keyButtonVector.at(KEY_8));
    thirdRowNumberLayout->addWidget(keyButtonVector.at(KEY_9));
    thirdRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberSymbol));

    //数字输入第四行
    QHBoxLayout *forthRowNumberLayout = new QHBoxLayout();
    forthRowNumberLayout->setSpacing(2);
    forthRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberDivide));
    forthRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberColon));
    forthRowNumberLayout->addWidget(keyButtonVector.at(KEY_0));
    forthRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberPeriod));
    forthRowNumberLayout->addWidget(keyButtonVector.at(KEY_numberReturn));

    QVBoxLayout *numberMainLayout = new QVBoxLayout(numberWidget);
    numberMainLayout->addLayout(firstRowNumberLayout);
    numberMainLayout->addLayout(secondRowNumberLayout);
    numberMainLayout->addLayout(thirdRowNumberLayout);
    numberMainLayout->addLayout(forthRowNumberLayout);

    //标点符号输入第一行
    QHBoxLayout *firstRowSymbolLayout = new QHBoxLayout();
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolAsciiTilde));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolQuoteLeft));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolExclam));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolAt));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolNumberSign));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolDollar));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolPercent));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolAsciiCircum));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolAmpersand));
    firstRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolAsterisk));
    firstRowSymbolLayout->setSpacing(2);
    firstRowSymbolLayout->setMargin(5);
    firstRowSymbolLayout->setAlignment(Qt::AlignLeft);

    //标点符号输入第二行
    QHBoxLayout *secondRowSymbolLayout = new QHBoxLayout();
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolParentLeft));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolParentRight));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolUnderscore));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolMinus));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolPlus));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolEqual));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBraceLeft));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBraceRight));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBracketLeft));
    secondRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBracketRight));
    secondRowSymbolLayout->setSpacing(2);
    secondRowSymbolLayout->setMargin(5);
    secondRowSymbolLayout->setAlignment(Qt::AlignLeft);

    //标点符号输入第三行
    QHBoxLayout *thirdRowSymbolLayout = new QHBoxLayout();
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBar));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBackslash));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolColon));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolSeicolon));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolQuoteDbl));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolApostrophe));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolLess));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolGreater));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolComma));
    thirdRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolBackSpace));
    thirdRowSymbolLayout->setSpacing(2);
    thirdRowSymbolLayout->setMargin(5);
    thirdRowSymbolLayout->setAlignment(Qt::AlignLeft);

    //标点符号输入第四行
    QHBoxLayout *forthRowSymbolLayout = new QHBoxLayout();
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolPeriod));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolPeriodZh));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolQuestion));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolSlash));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolLessZh));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolGreaterZh));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_Symbolx1));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_Symbolx2));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_Symbolx3));
    forthRowSymbolLayout->addWidget(keyButtonVector.at(KEY_SymbolReturn));
    forthRowSymbolLayout->setSpacing(2);
    forthRowSymbolLayout->setMargin(5);
    forthRowSymbolLayout->setAlignment(Qt::AlignLeft);

    QVBoxLayout *mainSymbolLayout = new QVBoxLayout(symbolWidget);
    mainSymbolLayout->addLayout(firstRowSymbolLayout);
    mainSymbolLayout->addLayout(secondRowSymbolLayout);
    mainSymbolLayout->addLayout(thirdRowSymbolLayout);
    mainSymbolLayout->addLayout(forthRowSymbolLayout);
    mainSymbolLayout->setSpacing(2);

}

/*******************************************************************************
* Function Name  : slotKeyButtonReleased
* Description    : 按键释放事件处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::slotKeyButtonReleased(void)
{
    // 获取应用程序焦点窗体
    QWidget * focusWidget = QApplication::focusWidget();

    if (NULL == focusWidget)
        return;

    // 获取发送释放按键的按钮句柄
    QObject * keyButtonReleased = sender();

    for (int i = 0; i < MAX_KEY; i++)
    {
        if (keyButtonVector.at(i) == keyButtonReleased)
        {
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

            case KEY_a:
            case KEY_b:
            case KEY_c:
            case KEY_d:
            case KEY_e:
            case KEY_f:
            case KEY_g:
            case KEY_h:
            case KEY_i:
            case KEY_j:
            case KEY_k:
            case KEY_l:
            case KEY_m:
            case KEY_n:
            case KEY_o:
            case KEY_p:
            case KEY_q:
            case KEY_r:
            case KEY_s:
            case KEY_t:
            case KEY_u:
            case KEY_v:
            case KEY_w:
            case KEY_x:
            case KEY_y:
            case KEY_z:
            if(!(isChinese && isLower))
            {
                key = keyCodeVector.at(i).toInt();
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;
            }
            else
            {
                QString text = inputLineEdit->text();
                text.append(keyCodeVector.at(i));
                inputLineEdit->setText(text);
                matching(inputLineEdit->text());
                break;
            }

            case KEY_Hanzi1:
            case KEY_Hanzi2:
            case KEY_Hanzi3:
            case KEY_Hanzi4:
            case KEY_Hanzi5:
            case KEY_Hanzi6:
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyButtonVector.at(i)->text());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 小数点
            case KEY_Period:
            case KEY_numberPeriod:
            case KEY_SymbolPeriod:
                key = Qt::Key_Period;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ,
            case KEY_Comma:
            case KEY_SymbolComma:
                key = Qt::Key_Comma;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // @
            case KEY_numberAt:
            case KEY_SymbolAt:
                key = Qt::Key_At;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // :
            case KEY_numberColon:
            case KEY_SymbolColon:
                key = Qt::Key_Colon;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 负号
            case KEY_numberMinus:
            case KEY_SymbolMinus:
                key = Qt::Key_Minus;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 加号
            case KEY_numberPlus:
            case KEY_SymbolPlus:
                key = Qt::Key_Plus;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ~
            case KEY_SymbolAsciiTilde:
                key = Qt::Key_AsciiTilde;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // `
            case KEY_SymbolQuoteLeft:
                key = Qt::Key_QuoteLeft;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // !
            case KEY_SymbolExclam:
                key = Qt::Key_Exclam;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // #
            case KEY_SymbolNumberSign:
                key = Qt::Key_NumberSign;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // $
            case KEY_SymbolDollar:
                key = Qt::Key_Dollar;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // %
            case KEY_SymbolPercent:
                key = Qt::Key_Percent;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ^
            case KEY_SymbolAsciiCircum:
                key = Qt::Key_AsciiCircum;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // "&"
            case KEY_SymbolAmpersand:
                key = Qt::Key_Ampersand;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, "&");
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // *
            case KEY_numberMultiply:
            case KEY_SymbolAsterisk:
                key = Qt::Key_Asterisk;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // (
            case KEY_SymbolParentLeft:
                key = Qt::Key_ParenLeft;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // )
            case KEY_SymbolParentRight:
                key = Qt::Key_ParenRight;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // _
            case KEY_SymbolUnderscore:
                key = Qt::Key_Underscore;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // =
            case KEY_SymbolEqual:
                key = Qt::Key_Equal;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // {
            case KEY_SymbolBraceLeft:
                key = Qt::Key_BraceLeft;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // }
            case KEY_SymbolBraceRight:
                key = Qt::Key_BraceRight;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // [
            case KEY_SymbolBracketLeft:
                key = Qt::Key_BracketLeft;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ]
            case KEY_SymbolBracketRight:
                key = Qt::Key_BracketRight;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // |
            case KEY_SymbolBar:
                key = Qt::Key_Bar;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // "\"
            case KEY_SymbolBackslash:
                key = Qt::Key_Backslash;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ;
            case KEY_SymbolSeicolon:
                key = Qt::Key_Semicolon;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // "
            case KEY_SymbolQuoteDbl:
                key = Qt::Key_QuoteDbl;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // '
            case KEY_SymbolApostrophe:
                key = Qt::Key_Apostrophe;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // <
            case KEY_SymbolLess:
                key = Qt::Key_Less;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // >
            case KEY_SymbolGreater:
                key = Qt::Key_Greater;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;


            // 《
            case KEY_SymbolLessZh:
                key = Qt::Key_Less;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 》
            case KEY_SymbolGreaterZh:
                key = Qt::Key_Greater;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 。
            case KEY_SymbolPeriodZh:
                key = Qt::Key_Period;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // ?
            case KEY_SymbolQuestion:
                key = Qt::Key_Question;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // /
            case KEY_numberDivide:
            case KEY_SymbolSlash:
                key = Qt::Key_Slash;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, keyCodeVector.at(i));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 删除按钮
            case KEY_BackSpace:
            case KEY_numberBackSpace:
            case KEY_SymbolBackSpace:
                if(!(isChinese && isLower && !inputLineEdit->text().isEmpty()))
                {
                    key = Qt::Key_Backspace;
                    keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, QString());
                    QCoreApplication::sendEvent(focusWidget, &keyEvent);

                    keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                    QCoreApplication::sendEvent(focusWidget, &keyEvent);
                }
                else
                {
                    inputLineEdit->backspace();
                }
                matching(inputLineEdit->text());
                break;

            // 空格按钮
            case KEY_Space:
            case KEY_numberSpace:
                key = Qt::Key_Space;
                keyEvent = QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, QString(" "));
                QCoreApplication::sendEvent(focusWidget, &keyEvent);

                keyEvent = QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, QString());
                QCoreApplication::sendEvent(focusWidget, &keyEvent);
                break;

            // 中英文切换
            case KEY_en:
                /*isChinese = !isChinese;
                keyButtonVector.at(KEY_en)->setText(isChinese ? QString("中") : (isLower ? QString("en") : QString("EN")));

                if(isChinese)
                {
                    charWidget->setFixedHeight(charWidget->height() + topRowHeight);
                    topRowWidget->show();
                    resize(charWidget->width(), charWidget->height());
                }
                else if(!isChinese )
                {
                    topRowWidget->hide();
                    charWidget->setFixedHeight(charWidget->height() - topRowHeight);
                    resize(charWidget->width(), charWidget->height());
                }
                matching("");*/
                break;

            // 数字键盘进入和返回
            case KEY_number:
            case KEY_numberReturn:
                isNumber = !isNumber;
                setInputMethod();
                break;

            // 标点符号键盘进入和返回
            case KEY_symbol:
            case KEY_numberSymbol:
            case KEY_SymbolReturn:
                isSymbol = !isSymbol;
                setInputMethod();
                break;

            // 大小写锁定
            case KEY_CapsLock:
                isLower = !isLower;
                 setInputMethod();
                break;

            // 汉字前一页
            case KEY_prePage:
                prePage();
                break;

            // 汉字下一页
            case KEY_nextPage:
                nextPage();
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

}

/*******************************************************************************
* Function Name  : matching
* Description    : 匹配处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::matching(QString field)
{
//    pinyinList = pinyinMap.values(field);
//    changePage(0);
//    pageCount = 0;
}

/*******************************************************************************
* Function Name  : prePage
* Description    : 前一页处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::prePage()
{
    changePage(--pageCount);
}

/*******************************************************************************
* Function Name  : nextPage
* Description    : 下一页处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::nextPage()
{
    changePage(++pageCount);
}

/*******************************************************************************
* Function Name  : changePage
* Description    : 汉字显示选择按键处理函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputKeyBoard::changePage(int index)
{
    int count = pinyinList.size();
    int i = index * 6;
    int j = KEY_Hanzi1;

    while (j < KEY_Hanzi1 + 6)
    {
        keyButtonVector.at(j++)->setText(QString::fromUtf8(pinyinList.value(count - (++i)).toUtf8()));
    }

    if(index == 0)
    {
        keyButtonVector.at(KEY_prePage)->setEnabled(false);
    }
    else
    {
        keyButtonVector.at(KEY_prePage)->setEnabled(true);
    }

    if(pinyinList.size() > (index * 6 + 6))
    {
        keyButtonVector.at(KEY_nextPage)->setEnabled(true);
    }
    else
    {
        keyButtonVector.at(KEY_nextPage)->setEnabled(false);
    }
}

/*******************************************************************************
* Function Name  : showEvent
* Description    : 窗体显示事件
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void InputKeyBoard::showEvent(QShowEvent * event)
//{
//    // 获取应用程序焦点窗体
//    QWidget * focusWidget = QApplication::focusWidget();
//    QWidget::showEvent(event);

//    if(focusWidget)
//    {   // 恢复焦点窗体
//        focusWidget->parentWidget()->setFocus();
//        focusWidget->setFocus();
//    }

//    focusWidget = QApplication::focusWidget();
//}

bool InputKeyBoard::eventFilter(QObject *target, QEvent *event)
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

void InputKeyBoard::myMove(int iFocusWidgetX, int iFocusWidgetY, int iFocusWidgetWidth, int iFocusWidgetHeight)
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
    move(iMovX, iMovY);
}
