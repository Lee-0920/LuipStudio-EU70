#include "MeasureProgress.h"
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QTime>
#include "Log.h"

namespace UI
{

MeasureProgress::MeasureProgress(int actionflag, QProgressBar *parent) :
        QProgressBar(parent),
        m_progressName(" "),
        m_remaintime(0),
        m_fulltime(0),
        m_progressflag(actionflag),
        m_startFlag(false),
        m_progressValue(0)
{
    m_threadStr = QStringLiteral("%1").arg(quintptr(QThread::currentThreadId()));

    curDateTimer = new QTimer(this);
    connect(curDateTimer, SIGNAL(timeout()), this, SLOT(slotUpdataDateTime()));

    connect(this, SIGNAL(UpdateInfoSignal(QString,int)), this, SLOT(SlotUpdateInfo(QString,int)));
    connect(this, SIGNAL(UpdateProgress()), this, SLOT(repaint()));
    connect(this, SIGNAL(SetDelayTimeSignal(int)), this, SLOT(SetDelayTimeSlot(int)));
}

MeasureProgress::~MeasureProgress()
{
    if(curDateTimer->isActive())
    {
        curDateTimer->stop();
    }
}

void MeasureProgress::setupUi(QWidget *Form)
{
    (void)Form;
} // setupUi

void MeasureProgress::retranslateUi(QWidget *Form)
{
    (void)Form;
}

void MeasureProgress::OnUpdateInfo(QString title, int time)
{
    emit UpdateInfoSignal(title, time);
}

void MeasureProgress::SlotUpdateInfo(QString title, int time)
{
    QMutexLocker locker(&m_updateMutex);

    QString  timeStr;

    if (time > 0)
    {
        timeStr = this->SetRemainTime(time , time , m_progressflag);
        this->StartTimer();
    }
    else
    {
        timeStr = this->SetRemainTime(0 , 0 , m_progressflag);
        this->StopTimer();
    }

    int rate = this->ProgressRate(m_remaintime, m_fulltime);
    this->setValue(rate);
    this->SetProgressContent(title, timeStr, rate);

    emit UpdateProgress();
}

void MeasureProgress::paintEvent(QPaintEvent* event)
{
    QString nameStr;
    QString timeStr;
    int rate;

    GetProgressContent(nameStr, timeStr, rate);

    if (nameStr.isNull() || nameStr.isEmpty() ||
        timeStr.isNull() || timeStr.isEmpty())
    {
        return;
    }

    QProgressBar::paintEvent(event);
    QFont listFont;
    listFont.setPointSize(13);

    QPainter paint(this);
    paint.setFont(listFont);
    paint.setPen(Qt::black);
    paint.setBrush(QBrush(Qt::blue));

    int maxWidth = 100;
    if (m_progressflag == 0)
    {
        maxWidth = 130;
    }

    QRectF rectName(5,1,maxWidth,36);

    QFontMetrics fontWidth(listFont);
    int nameWidth = fontWidth.width(nameStr);
    if(nameWidth >= maxWidth)
    {
        int textSize = 13;
        while(1)
        {
            textSize--;
            listFont.setPointSize(textSize);
            paint.setFont(listFont);
            QFontMetrics fontWidth(listFont);
            nameWidth = fontWidth.width(nameStr);
//            qDebug("textSize = %d, nameWidth = %d, maxWidth = %d",textSize,nameWidth,maxWidth);
            if(nameWidth <= maxWidth)
            {
                break;
            }
            else if (9 >= textSize)
            {
                int len = nameStr.length();
                int top = len/2 + 1;
                int bottom = len - top;
                if (top > 0 && bottom >= 0)
                {
                    nameStr = nameStr.left(top) + "\n" + nameStr.right(bottom);
                }
//                nameStr = fontWidth.elidedText(nameStr, Qt::ElideRight, maxWidth);
                break;
            }
        }
    }
    paint.drawText(rectName, nameStr, QTextOption(Qt::AlignLeft|Qt::AlignVCenter));

//    QRectF rectTime(maxWidth+6,1,width()-(maxWidth+11),25);
    QRectF rectTime(maxWidth-4,1,width()-(maxWidth),36);

    //时间显示不缩小
    listFont.setPointSize(13);
    paint.setFont(listFont);
    paint.drawText(rectTime, timeStr, QTextOption(Qt::AlignRight|Qt::AlignVCenter));

//    qDebug("nameStr = %s, tiemStr = %s", nameStr.toStdString().c_str(), timeStr.toStdString().c_str());
}

void MeasureProgress::SetDelayTime(int time)
{
    emit SetDelayTimeSignal(time);
}

void MeasureProgress::SetProgressContent(QString name, const QString time, int rate)
{
    QWriteLocker locker(&m_progressContentLock);

    m_progressName = name;
    m_progressTime = time;
    m_progressValue = rate;
}

void MeasureProgress::GetProgressContent(QString &name, QString &time, int &rate)
{
    QReadLocker  locker(&m_progressContentLock);

    name = m_progressName;
    time = m_progressTime;
    rate = m_progressValue;
}
QString MeasureProgress::SetRemainTime(int remain , int fulltime , int flag)
{
    QString progressTimeStr;
    this->m_remaintime = remain;
    this->m_fulltime = fulltime;
    this->m_progressflag = flag;

    int hour = this->m_remaintime/3600;
    int temp = this->m_remaintime%3600;
    int min = temp/60;
    int second = temp%60;

    QString theHour;
    QString theMin;
    QString theSecond;

    if(hour < 10)
    {
       theHour = "0" + QString::number(hour);
    }
    else
    {
        theHour = QString::number(hour);
    }

    if(min < 10)
    {
        theMin = "0" + QString::number(min);
    }
    else
    {
        theMin = QString::number(min);
    }

    if(second < 10)
    {
        theSecond = "0" + QString::number(second);
    }
    else
    {
        theSecond = QString::number(second);
    }

    if (flag == 1)
    {
        progressTimeStr = theHour + ":" + theMin + ":" + theSecond;
    }
    else
    {
        progressTimeStr = theMin + ":" + theSecond;
    }

    return progressTimeStr;
}

int MeasureProgress::ProgressRate(int remain, int fulltime)
{
    int rate =0;

    if(fulltime!=0)
    {
        rate = (fulltime - remain) * 100 / (fulltime);
    }
    return rate;
}

void MeasureProgress::StartTimer()
{
    m_startFlag = true;

    if (!curDateTimer->isActive())
    {
        curDateTimer->start(1000);
    }
}

void MeasureProgress::StopTimer()
{
    m_startFlag = false;

    if (curDateTimer->isActive())
    {
        curDateTimer->stop();
    }
}

void MeasureProgress::slotUpdataDateTime()
{
    QMutexLocker locker(&m_updateMutex);

    if(this->m_startFlag)
    {

        if(this->m_remaintime > 0)
        {
           if(this->m_startFlag)
           {
               this->m_remaintime--;
           }
        }
        else
        {
           this->m_remaintime = 0;
        }

        QString timeStr = SetRemainTime(this->m_remaintime , m_fulltime , m_progressflag);

        int rate = this->ProgressRate(m_remaintime, m_fulltime);
        this->SetProgressContent(m_progressName, timeStr, rate);
        this->setValue(rate);
        emit UpdateProgress();
    }
}

void MeasureProgress::SetDelayTimeSlot(int time)
{
    QString timeStr = this->SetRemainTime(this->m_remaintime + time ,
                        this->m_fulltime + time ,
                         m_progressflag);

    int rate = this->ProgressRate(m_remaintime, m_fulltime);
    this->setValue(rate);
    this->SetProgressContent(m_progressName, timeStr, rate);
}

}
