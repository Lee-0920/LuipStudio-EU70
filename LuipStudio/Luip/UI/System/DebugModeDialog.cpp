#include "DebugModeDialog.h"
#include <QGridLayout>
#include "UI/Frame/Common.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/MessageDialog.h"
#include "System/DynamicPassword.h"
#include "Setting/SettingManager.h"

using namespace Lua;
using namespace System;
using namespace Configuration;

namespace UI
{
DebugModeDialog::DebugModeDialog(QWidget *parent) : QDialog(parent)
{
    this->resize(380, 270);
    setWindowIcon(QIcon(":/img/WQIMC"));

    CNumberKeyboard *numberKey = new CNumberKeyboard(this);

    QLabel *logoLabel = new QLabel();

    logoLabel->setFixedSize(150, 75);
    logoLabel->setPixmap(QPixmap(":/img/logo"));
    logoLabel->setScaledContents(true);

    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->addStretch();
    logoLayout->addWidget(logoLabel);
    logoLayout->setSpacing(10);
    logoLayout->setContentsMargins(0, 10, 15, 0);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addLayout(logoLayout);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setPointSize(16);

    QLabel *minLabel = new QLabel();
    minLabel->setFixedSize(200, 30);
    minLabel->setFont(font);
    minLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    minLabel->setText("随机结果最小值：");

    QLabel *maxLabel = new QLabel();
    maxLabel->setFixedSize(200, 30);
    maxLabel->setFont(font);
    maxLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    maxLabel->setText("随机结果最大值：");

    m_minData = new QLineEdit();
    m_minData->setFixedSize(100, 30);
    m_minData->installEventFilter(numberKey);

    m_maxData = new QLineEdit();
    m_maxData->setFixedSize(100, 30);
    m_maxData->installEventFilter(numberKey);

    QHBoxLayout *minLayout = new QHBoxLayout();
    minLayout->addWidget(minLabel);
    minLayout->addWidget(m_minData);

    QHBoxLayout *maxLayout = new QHBoxLayout();
    maxLayout->addWidget(maxLabel);
    maxLayout->addWidget(m_maxData);

    QFont btnFont;
    btnFont.setPointSize(20);

    m_confirmBtn = new QPushButton();
    m_confirmBtn->setFixedSize(150,80);
    m_confirmBtn->setFont(btnFont);
    m_confirmBtn->setObjectName("brownButton");
    m_confirmBtn->setText("确认");
    connect(m_confirmBtn ,SIGNAL(clicked()),this,SLOT(ConfirmSlot()));

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_confirmBtn);
    btnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(minLayout);
    mainLayout->addLayout(maxLayout);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(10, 0, 0, 20);

    this->setLayout(mainLayout);
}

void DebugModeDialog::ConfirmSlot()
{
    if (!m_minData->text().isEmpty() && !m_maxData->text().isEmpty())
    {
        float min =  m_minData->text().toFloat();
        float max = m_maxData->text().toFloat();
        if (min < max)
        {
            LuaEngine::Instance()->SetLuaValue("setting.common.minRandomData", min);
            LuaEngine::Instance()->SetLuaValue("setting.common.maxRandomData", max);
        }
    }

    this->accept();
}

}

