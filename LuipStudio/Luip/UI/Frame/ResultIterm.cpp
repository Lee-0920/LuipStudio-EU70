#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include "ResultIterm.h"
#include <QTextCharFormat>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

#define RESULT_WIDTH        110
#define RESULT_HIGH        80
#define TARGET_WIDTH        80
#define TARGET_HIGH        60
#define RESULT_MAX_SIZE   30
#define RESULT_FONT_WEIGHT   40
#define UNITLABLE_WIDTH        60
//边界调试，显示主界面黑框中的三个框分别在什么位置
#define BORDER_DEBUG        0

namespace UI
{

ResultIterm::ResultIterm(QString strTarget, QString strResult,
                         QString strDateTime, QString strUnit, QWidget *parent)
    : QWidget(parent)
{
    m_targetLabel  = new QLabel();
#if BORDER_DEBUG
    m_targetLabel->setStyleSheet("QLabel{"
                                "background-color:white;"
                                "}");
#else
    m_targetLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_targetLabel->setFixedSize(110, TARGET_HIGH);
    QFont measureParamFont = m_targetLabel->font();
    measureParamFont.setPointSize(25);
    measureParamFont.setWeight(QFont::DemiBold);
    m_targetLabel->setFont(measureParamFont);
    m_targetLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight );
    m_targetLabel->setText("TOC = ");

    //*TC测量类型
    m_targetTCLabel  = new QLabel();
#if BORDER_DEBUG
    m_targetTCLabel->setStyleSheet("QLabel{"
                                "background-color:white;"
                                "}");
#else
    m_targetTCLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_targetTCLabel->setFixedSize(TARGET_WIDTH, TARGET_HIGH*0.8);
    QFont targetFont = m_targetLabel->font();
    targetFont.setPointSize(20);
    targetFont.setWeight(QFont::DemiBold);
    m_targetTCLabel->setFont(targetFont);
    m_targetTCLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight );
    m_targetTCLabel->setText("TC = ");

    //*IC测量类型
    m_targetICLabel  = new QLabel();
#if BORDER_DEBUG
    m_targetICLabel->setStyleSheet("QLabel{"
                                "background-color:white;"
                                "}");
#else
    m_targetICLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_targetICLabel->setFixedSize(TARGET_WIDTH, TARGET_HIGH*0.8);
    m_targetICLabel->setFont(targetFont);
    m_targetICLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight );
    m_targetICLabel->setText("IC = ");

    m_resultLabel = new QLabel();
#if BORDER_DEBUG
    m_resultLabel->setStyleSheet("QLabel{"
                                 "background-color:red;"
                                 "}");
#else
    m_resultLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_resultLabel->setFixedSize(RESULT_WIDTH, RESULT_HIGH);
    QFont measureResultFont = m_resultLabel->font();
    measureResultFont.setWeight(RESULT_FONT_WEIGHT);
    measureResultFont.setPointSize(RESULT_MAX_SIZE);
    m_resultLabel->setFont(measureResultFont);
    m_resultLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_resultLabel->setMargin(0);
    m_resultLabel->setText(strResult);

    //*TC测量结果
    m_resultTCLabel = new QLabel();
#if BORDER_DEBUG
    m_resultTCLabel->setStyleSheet("QLabel{"
                                 "background-color:red;"
                                 "}");
#else
    m_resultTCLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_resultTCLabel->setFixedSize(RESULT_WIDTH*0.8, RESULT_HIGH/2);
    measureResultFont.setWeight(RESULT_FONT_WEIGHT*2/3);
    measureResultFont.setPointSize(RESULT_MAX_SIZE*2/3);
    m_resultTCLabel->setFont(measureResultFont);
    m_resultTCLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_resultTCLabel->setMargin(0);
    m_resultTCLabel->setText("0");

    //*IC测量结果
    m_resultICLabel = new QLabel();
#if BORDER_DEBUG
    m_resultICLabel->setStyleSheet("QLabel{"
                                 "background-color:red;"
                                 "}");
#else
    m_resultICLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_resultICLabel->setFixedSize(RESULT_WIDTH*0.8, RESULT_HIGH/2);
    m_resultICLabel->setFont(measureResultFont);
    m_resultICLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_resultICLabel->setMargin(0);
    m_resultICLabel->setText("0");

    m_unitLabel = new QLabel();
#if BORDER_DEBUG
    m_unitLabel->setStyleSheet("QLabel{"
                                 "background-color:white;"
                                 "}");
#else
    m_unitLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_unitLabel->setFixedSize(UNITLABLE_WIDTH, 35);
    QFont uintFont = m_unitLabel->font();
    uintFont.setPointSize(20);
    uintFont.setWeight(QFont::DemiBold);
    m_unitLabel->setFont(uintFont);
    m_unitLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_unitLabel->setText(strUnit);

    m_unitTCLabel = new QLabel();
#if BORDER_DEBUG
    m_unitTCLabel->setStyleSheet("QLabel{"
                                 "background-color:white;"
                                 "}");
#else
    m_unitTCLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_unitTCLabel->setFixedSize(UNITLABLE_WIDTH, 35);
    m_unitTCLabel->setFont(uintFont);
    m_unitTCLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_unitTCLabel->setText(strUnit);

    m_unitICLabel = new QLabel();
#if BORDER_DEBUG
    m_unitICLabel->setStyleSheet("QLabel{"
                                 "background-color:white;"
                                 "}");
#else
    m_unitICLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_unitICLabel->setFixedSize(UNITLABLE_WIDTH, 35);
    m_unitICLabel->setFont(uintFont);
    m_unitICLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_unitICLabel->setText(strUnit);

    QString strDate = "";
    QString strTime = "";

    DateTimeStringAnalysis(strDateTime, strDate, strTime);

    m_dateLabel = new QLabel();
#if BORDER_DEBUG
    m_dateLabel->setStyleSheet("QLabel{"
                                 "background-color:white;"
                                 "}");
#else
    m_dateLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_dateLabel->setFixedSize(180,30);
    QFont dateFont = m_dateLabel->font();
    dateFont.setPointSize(20);
    dateFont.setWeight(QFont::DemiBold);
    m_dateLabel->setFont(dateFont);
    m_dateLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    m_dateLabel->setText(strDate);

    m_timeLabel = new QLabel();
#if BORDER_DEBUG
    m_timeLabel->setStyleSheet("QLabel{"
                                 "background-color:white;"
                                 "}");
#else
    m_timeLabel->setObjectName(QStringLiteral("measureLabel"));
#endif
    m_timeLabel->setFixedSize(150,30);
    m_timeLabel->setFont(dateFont);
    m_timeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_timeLabel->setText(strTime);

    QHBoxLayout *targetLayout = new QHBoxLayout();
    targetLayout->addStretch();
//    targetLayout->addSpacing(7);
    targetLayout->addWidget(m_targetLabel);
//    targetLayout->addWidget(m_unitLabel);
//    targetLayout->addSpacing(10);
    targetLayout->addWidget(m_resultLabel);
    targetLayout->addStretch();
    targetLayout->addWidget(m_unitLabel);
    targetLayout->addStretch();
    targetLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *targetTCLayout = new QHBoxLayout();
    targetTCLayout->addStretch();
    targetTCLayout->addWidget(m_targetTCLabel);
//    targetTCLayout->addSpacing(10);
    targetTCLayout->addWidget(m_resultTCLabel);
    targetTCLayout->addStretch();
    targetTCLayout->addWidget(m_unitTCLabel);
    targetTCLayout->addStretch();
    targetTCLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *targetICLayout = new QHBoxLayout();
    targetICLayout->addStretch();
    targetICLayout->addWidget(m_targetICLabel);
//    targetICLayout->addSpacing(10);
    targetICLayout->addWidget(m_resultICLabel);
    targetICLayout->addStretch();
    targetICLayout->addWidget(m_unitICLabel);
    targetICLayout->addStretch();
    targetICLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Plain);
    line->setLineWidth(10);
//    line->setFixedHeight(150);
    QPalette palette = line->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    line->setPalette(palette);

    QFrame *lineLeft = new QFrame();
    lineLeft->setFrameShape(QFrame::VLine);
    lineLeft->setFrameShadow(QFrame::Plain);
    lineLeft->setLineWidth(10);
//    lineLeft->setFixedHeight(150);
    lineLeft->setPalette(palette);

    QFrame *lineRight = new QFrame();
    lineRight->setFrameShape(QFrame::VLine);
    lineRight->setFrameShadow(QFrame::Plain);
    lineRight->setLineWidth(10);
//    lineRight->setFixedHeight(150);
    lineRight->setPalette(palette);

    QFrame *lineTop = new QFrame();
    lineTop->setFrameShape(QFrame::HLine);
    lineTop->setFrameShadow(QFrame::Plain);
    lineTop->setLineWidth(10);
    lineTop->setPalette(palette);

    QFrame *lineBottom = new QFrame();
    lineBottom->setFrameShape(QFrame::HLine);
    lineBottom->setFrameShadow(QFrame::Plain);
    lineBottom->setLineWidth(10);
    lineBottom->setPalette(palette);

    QFrame *lineMidHLine = new QFrame();
    lineMidHLine->setFrameShape(QFrame::HLine);
    lineMidHLine->setFrameShadow(QFrame::Plain);
    lineMidHLine->setLineWidth(10);
    lineMidHLine->setPalette(palette);

    QVBoxLayout *subTargetLayout = new QVBoxLayout();
    subTargetLayout->addStretch();
    subTargetLayout->addLayout(targetTCLayout);
    subTargetLayout->addSpacing(6);
//    subTargetLayout->addWidget(lineMidHLine);
    subTargetLayout->addSpacing(6);
    subTargetLayout->addLayout(targetICLayout);
    subTargetLayout->addStretch();
    subTargetLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainTargetLayout = new QHBoxLayout();
//    mainTargetLayout->addWidget(lineLeft);
//    mainTargetLayout->addStretch();
    mainTargetLayout->addLayout(targetLayout);
    mainTargetLayout->addSpacing(6);
//    mainTargetLayout->addWidget(line);
    mainTargetLayout->addSpacing(6);
    mainTargetLayout->addLayout(subTargetLayout);
    mainTargetLayout->addStretch();
//    mainTargetLayout->addWidget(lineRight);
    mainTargetLayout->setContentsMargins(0, 0, 0, 0);

//    QVBoxLayout *resultLayout = new QVBoxLayout();
//    resultLayout->addStretch();
//    resultLayout->addWidget(m_resultLabel);
//    targetLayout->addStretch();
//    resultLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *dateTimeLayout = new QHBoxLayout();
    dateTimeLayout->addStretch();
    dateTimeLayout->addWidget(m_dateLabel);
    dateTimeLayout->addSpacing(7);
    dateTimeLayout->addWidget(m_timeLabel);
    dateTimeLayout->addStretch();
    dateTimeLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *itermLayout = new QVBoxLayout();
//    itermLayout->addWidget(lineTop);
    itermLayout->addLayout(mainTargetLayout);
//    itermLayout->addWidget(lineBottom);
    itermLayout->addSpacing(5);
    itermLayout->addLayout(dateTimeLayout);
    itermLayout->addStretch();
//    itermLayout->addLayout(targetLayout);
    itermLayout->setContentsMargins(0, 0, 0, 0);
    itermLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    this->setLayout(itermLayout);
}

ResultIterm::~ResultIterm()
{

}
void ResultIterm::SetMeasureTarget(QString strTarget)
{
    m_targetLabel->setText(strTarget + " = ");
}

void ResultIterm::SetMeasureTargetTC(QString strTarget)
{
    m_targetTCLabel->setText(strTarget + " = ");
}

void ResultIterm::SetMeasureTargetIC(QString strTarget)
{
    m_targetICLabel->setText(strTarget + " = ");
}

void ResultIterm::SetMeasureResult(QString strResult)
{
//    QFont typeTestFont = m_resultLabel->font();
//    int point = GetPointSize(strResult, RESULT_WIDTH-40);
//    typeTestFont.setPointSize(point);
//    typeTestFont.setWeight(80);

//    m_resultLabel->setFont(typeTestFont);
    if(strResult.toDouble() > 1000)
    {
        float consistency = strResult.toFloat();
        strResult.sprintf("%.2f",consistency/1000);
        m_resultLabel->setText(strResult);
        m_unitLabel->setText("ppm");
    }
    else
    {
        m_resultLabel->setText(strResult);
        m_unitLabel->setText("ppb");
    }

}

void ResultIterm::SetMeasureResultTC(QString strResult)
{
    if(strResult.toDouble() > 1000)
    {
        float consistency = strResult.toFloat();
        strResult.sprintf("%.2f",consistency/1000);
        m_resultTCLabel->setText(strResult);
        m_unitTCLabel->setText("ppm");
    }
    else
    {
        m_resultTCLabel->setText(strResult);
        m_unitTCLabel->setText("ppb");
    }
}

void ResultIterm::SetMeasureResultIC(QString strResult)
{
    if(strResult.toDouble() > 1000)
    {
        float consistency = strResult.toFloat();
        strResult.sprintf("%.2f",consistency/1000);
        m_resultICLabel->setText(strResult);
        m_unitICLabel->setText("ppm");
    }
    else
    {
        m_resultICLabel->setText(strResult);
        m_unitICLabel->setText("ppb");
    }

}

void ResultIterm::SetMeasureTime(QString strDateTime)
{
    QString strDate = "";
    QString strTime = "";

    DateTimeStringAnalysis(strDateTime, strDate, strTime);
    m_dateLabel->setText(strDate);
    m_timeLabel->setText(strTime);
}

void ResultIterm::DateTimeStringAnalysis(QString &dateTime, QString &date, QString &time)
{
    QStringList strList = dateTime.split(QRegExp("[ ]"));

    if (strList.size() == 2)
    {
        date = strList.at(0);
        time = strList.at(1);
    }
}


int ResultIterm::GetPointSize(QString& text, int limitWidth)
{
    QFont font = m_resultLabel->font();
    font.setPointSize(RESULT_MAX_SIZE);
    int size = RESULT_MAX_SIZE;

    QFontMetrics fontWidth(font);
    int textWidth = fontWidth.width(text);

    while(size > 20 && textWidth > limitWidth)
    {
        size = size - 1;
        font.setPointSize(size);

        QFontMetrics fontWidth(font);
        textWidth = fontWidth.width(text);
    }

    if (size > RESULT_MAX_SIZE)
    {
        size = RESULT_MAX_SIZE;
    }
    return size;
}

void ResultIterm::SetMeasureResultFont(int fontSize)
{
//    QFont measureResultFont = m_resultLabel->font();
//    measureResultFont.setPointSize(fontSize);
//    m_resultLabel->setFont(measureResultFont);
}

}
