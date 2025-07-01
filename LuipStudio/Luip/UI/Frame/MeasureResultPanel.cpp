
#include <QGroupBox>
#include <QVBoxLayout>
#include <QMap>
#include <QString>
#include "LuaEngine/LuaEngine.h"
#include "MeasureResultPanel.h"
#include "Lua/App.h"
#include "FlowManager/FlowManager.h"
#include "FlowManager/MeasureScheduler.h"
#include "Setting/SettingManager.h"

#define RESULT_MIN_SIZE             12
#define RESULT_MAX_SIZE            17
#define RESULT_UI_MAX_SIZE      150
#define RESULT_MAX_POINTSIZE    75
#define RESULT_MIN_POINTSIZE    45

using namespace Configuration;
using namespace OOLUA;
using namespace Lua;
using namespace Flow;

namespace UI
{

MeasureResultPanel::MeasureResultPanel(QList<QString> targets, QString resultType, QString unitStr, QWidget *parent) :
        QWidget(parent)
{
    if (!targets.empty())
    {
        m_resultTypeLabel  = new QLabel();
        m_resultTypeLabel->setObjectName(QStringLiteral("measureLabel"));
        m_resultTypeLabel->setFixedSize(RESULT_UI_MAX_SIZE, 30);
        QFont resultTypeLabelFont = m_resultTypeLabel->font();
        resultTypeLabelFont.setPointSize(16);
        resultTypeLabelFont.setWeight(QFont::DemiBold);
        m_resultTypeLabel->setFont(resultTypeLabelFont);
        m_resultTypeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft );
        m_resultTypeLabel->setText(resultType);

//        m_proformaResultLabel = new QLabel();
//        m_proformaResultLabel->setObjectName(QStringLiteral("proformaLabel"));
//        m_proformaResultLabel->setFixedSize(180, 30);
//        QFont proformaResultLabelFont = m_proformaResultLabel->font();
//        proformaResultLabelFont.setPointSize(16);
//        proformaResultLabelFont.setWeight(QFont::DemiBold);
//        m_proformaResultLabel->setFont(proformaResultLabelFont);
//        m_proformaResultLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight );

        QHBoxLayout *typeHLayout = new QHBoxLayout();
        typeHLayout->addWidget(m_resultTypeLabel);
        typeHLayout->addStretch();
//        typeHLayout->addWidget(m_proformaResultLabel);
//        typeHLayout->setSpacing(5);
//        typeHLayout->setContentsMargins(2, 5, 8, 0);

        QVBoxLayout *typeVLayout = new QVBoxLayout();
        typeVLayout->addLayout(typeHLayout);

        for (int i = 0; i < targets.size(); i++)
        {
            TargetIterm targetIterm;
            targetIterm.name = targets.at(i);
            targetIterm.iterm = new ResultIterm(targets.at(i), tr("0.0000"),
                                                 tr("0000-00-00 00:00:00"), unitStr, this);
            m_resultMap.append(targetIterm);
        }

        QGroupBox *meaResultGroupBox = new QGroupBox();
        meaResultGroupBox->setObjectName(QStringLiteral("meaResultGroupBox"));
        meaResultGroupBox->setFixedWidth(630);
        meaResultGroupBox->setMinimumHeight(300);
        meaResultGroupBox->setMaximumHeight(300);

        //测量预期标签
        estimateResultTimeLable = new QLabel();
        estimateResultTimeLable->setFixedSize(580,50);
        estimateResultTimeLable->setObjectName(QStringLiteral("estimateResultLabel"));
        QFont estimateResultTypeFont = estimateResultTimeLable->font();
        estimateResultTypeFont.setPointSize(16);
        estimateResultTypeFont.setWeight(QFont::DemiBold);
        estimateResultTimeLable->setFont(estimateResultTypeFont);
        estimateResultTimeLable->setAlignment(Qt::AlignCenter);
        estimateResultTimeLable->setText(tr(" "));

        QHBoxLayout *estimateResultLayout = new QHBoxLayout();
        estimateResultLayout->addStretch();
        estimateResultLayout->addWidget(estimateResultTimeLable);
        estimateResultLayout->addStretch();
        estimateResultLayout->setSpacing(0);
        estimateResultLayout->setContentsMargins(0, 0, 0, 0);

        fatalButton = new QPushButton();
        fatalButton->setObjectName("brownButton");
        fatalButton->setFixedSize(280,50);
        QFont measureFont = fatalButton->font();
        measureFont.setPointSize(15);
        fatalButton->setText(tr("故障停机，恢复请点击"));
        fatalButton->setFont(measureFont);
        fatalButton->setStyleSheet("QPushButton{background-color:red;}");

        QVBoxLayout *itermLayout = new QVBoxLayout();

        itermLayout->addStretch();
        QVector <TargetIterm> ::Iterator it;
        for(it = m_resultMap.begin(); it != m_resultMap.end(); ++it)
        {
            itermLayout->addWidget((*it).iterm);
        }
        itermLayout->addStretch();

        QVBoxLayout *resultPanelLayout = new QVBoxLayout();
        resultPanelLayout->addLayout(typeVLayout);
        resultPanelLayout->addStretch();
        resultPanelLayout->addLayout(itermLayout);
        resultPanelLayout->addStretch();
        resultPanelLayout->addLayout(estimateResultLayout);
        resultPanelLayout->addWidget(fatalButton,0, Qt::AlignCenter);
        resultPanelLayout->setSpacing(0);

        meaResultGroupBox->setLayout(resultPanelLayout);

        QHBoxLayout *meaResultGroupLayout = new QHBoxLayout();
        meaResultGroupLayout->addStretch();
        meaResultGroupLayout->addWidget(meaResultGroupBox);
        meaResultGroupLayout->addStretch();
        meaResultGroupLayout->setSpacing(0);
        meaResultGroupLayout->setContentsMargins(0, 0, 0, 0);

        this->setLayout(meaResultGroupLayout);
        connect(fatalButton, SIGNAL(clicked(bool)), this, SLOT(SlotFatalButton()));
    }
}

MeasureResultPanel::~MeasureResultPanel()
{

}

void MeasureResultPanel::SetResultInfo(QString target, QString result, QString dateTime, QString resultType)
{
    QVector <TargetIterm> ::Iterator it;
    for(it = m_resultMap.begin(); it != m_resultMap.end(); ++it)
    {
        if ((*it).name == target)
        {
            QFont font;
            font.setPointSize(14);
            int size = 14;

            QFontMetrics fontWidth(font);
            int textWidth = fontWidth.width(QObject::tr(resultType.toStdString().c_str()));

            while(size > RESULT_MIN_SIZE && textWidth > RESULT_UI_MAX_SIZE)
            {
                size = size - 1;
                font.setPointSize(size);

                QFontMetrics fontWidth(font);
                textWidth = fontWidth.width(QObject::tr(resultType.toStdString().c_str()));
            }

            if (size > RESULT_MAX_SIZE)
            {
                size = RESULT_MAX_SIZE;
            }

            font.setPointSize(size);
            font.setWeight(QFont::DemiBold);
            m_resultTypeLabel->setFont(font);
            m_resultTypeLabel->setText(QObject::tr(resultType.toStdString().c_str()));

            ResultIterm* iterm = (*it).iterm;

            if(it != m_resultMap.begin())
            {
                iterm->SetMeasureResultFont(RESULT_MIN_POINTSIZE);
            }
            else
            {
                iterm->SetMeasureResultFont(RESULT_MAX_POINTSIZE);
            }

            iterm->SetMeasureTarget(target);
            iterm->SetMeasureResult(result);
            iterm->SetMeasureTime(dateTime);
        }
    }
}

void MeasureResultPanel::SetResultInfoTC(QString target, QString result)
{
    QVector <TargetIterm> ::Iterator it;
    for(it = m_resultMap.begin(); it != m_resultMap.end(); ++it)
    {
        ResultIterm* iterm = (*it).iterm;
        iterm->SetMeasureTargetTC(target);
        iterm->SetMeasureResultTC(result);
        break;
    }
}

void MeasureResultPanel::SetResultInfoIC(QString target, QString result)
{
    QVector <TargetIterm> ::Iterator it;
    for(it = m_resultMap.begin(); it != m_resultMap.end(); ++it)
    {
        ResultIterm* iterm = (*it).iterm;
        iterm->SetMeasureTargetIC(target);
        iterm->SetMeasureResultIC(result);
        break;
    }
}

void MeasureResultPanel::SetProformaResultInfo(QString result)
{
//    if (result.isEmpty())
//    {
//        m_proformaResultLabel->hide();
//    }
//    else
//    {
//        QString strInfo = QString("反应值: %1").arg(result);
//        m_proformaResultLabel->setText(strInfo);
//        m_proformaResultLabel->show();
//    }
}

void MeasureResultPanel::ShowFatalWarning()
{
    this->fatalButton->setVisible(true);
    this->estimateResultTimeLable->setVisible(false);
}

void MeasureResultPanel::SlotFatalButton()
{
     this->fatalButton->setVisible(false);
     this->estimateResultTimeLable->setVisible(true);

    LuaEngine* luaEngine = LuaEngine::Instance();
    luaEngine->SetLuaValue("status.measure.isFatalError", false);
    SettingManager::Instance()->MeasureStatusSave();
    MeasureScheduler::Instance()->Update();
}

}
