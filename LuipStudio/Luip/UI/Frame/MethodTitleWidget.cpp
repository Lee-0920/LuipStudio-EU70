#include "Log.h"
#include <QSignalMapper>
#include "MethodTitleWidget.h"
#include "UI/Frame/ChangePasswordDialog.h"
#include "System/LockScreen/ScreenProtection.h"
#include "LuaEngine/LuaEngine.h"
#include "checkscreen.h"
#include "System/FileDetecter.h"
#include "UI/Frame/MessageDialog.h"

using namespace System;
using namespace System::Screen;
using namespace Lua;

namespace UI
{
MethodTitleWidget::MethodTitleWidget(QWidget *parent)
    : QWidget(parent)
{
#ifdef _CS_ARM_LINUX
    Screenprotection::Instance()->Register(this);
#endif
    mainLayout = new QVBoxLayout();
    setLayout(mainLayout);
    this->SetupUIWidget();
    this->TranslateLanguage();
}

void MethodTitleWidget::SetupUIWidget()
{
    m_closeButton = new ToolButton(":/toolWidget/methodQuit");
    m_closeButton->setFixedSize(90,90);
    m_closeButton->setText("退出");

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(115, 115, 140));

    this->setPalette(palette);
    this->setAutoFillBackground(true);

    connect(m_closeButton, SIGNAL(clicked()), this, SIGNAL(MethodQuit()));

    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addStretch();
    titleLayout->addWidget(m_closeButton, 0, Qt::AlignBottom);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 0, 5, 0);

    QStringList stringList;

    stringList<<":/toolWidget/methodSetting"<<":/toolWidget/methodInfo";

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(int i=0; i<stringList.size(); i++)
    {
        ToolButton *toolButton = new ToolButton(stringList.at(i));
        m_buttonList.append(toolButton);
        connect(toolButton, SIGNAL(clicked()), signalMapper, SLOT(map()));
        signalMapper->setMapping(toolButton, QString::number(i, 10));

        buttonLayout->addWidget(toolButton, 0, Qt::AlignBottom);
    }
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(SlotTurnPage(QString)));
//    buttonLayout->addStretch();
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(15, 0, 15, 0);

    QHBoxLayout *buttonContextLayout = new QHBoxLayout();
    buttonContextLayout->addLayout(buttonLayout);
    buttonContextLayout->addStretch();
    buttonContextLayout->addLayout(titleLayout);


//    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(buttonContextLayout);
    mainLayout->setSpacing(0);

#ifdef _CS_ARM_LINUX
    mainLayout->setContentsMargins(0, 0, 0, 0);
#endif

#ifdef _CS_X86_WINDOWS
    mainLayout->setContentsMargins(0, 0, 0, 0);
#endif

    setFixedHeight(100);

    ChangeBottomStatus();

    connect(this, SIGNAL(ScreenAlreadyLock()), this, SIGNAL(MethodQuit()));
}

void MethodTitleWidget::OnLockScreen()
{
    if (!CheckScreen::Instance()->isVNC())
    {
        emit ScreenAlreadyLock();
    }
}

void MethodTitleWidget::TranslateLanguage()
{
    m_buttonList.at(0)->setText(tr("设置"));
    m_buttonList.at(1)->setText(tr("历史方法"));
}

void MethodTitleWidget::SlotTurnPage(QString currentPage)
{
    bool ok;
    int currentIndex = currentPage.toInt(&ok, 10);

    for(int i = 0; i < m_buttonList.count(); i++)
    {
        ToolButton *toolButton = m_buttonList.at(i);
        if(currentIndex == i)
        {
            toolButton->SetMousePress(true);
        }
        else
        {
            toolButton->SetMousePress(false);
        }
    }

    emit MethodTurnPage(currentIndex);
}

void MethodTitleWidget::ChangeBottomStatus()
{

}

}
