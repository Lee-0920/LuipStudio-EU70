#ifndef __TREASURE_SYSTEM_TIME__
#define __TREASURE_SYSTEM_TIME__

#include <memory>
#include <QMutex>
#include <QTimer>
#include <QDateTime>
#include <QElapsedTimer>

namespace Treasure
{
    class Time
    {
    public:
        static QDateTime ConvertDateTime(const QString& str, const QString& format = "yyyyMMddhhmmss")
        {
            return QDateTime::fromString(str, format);
        }

        /**
         * @brief 日子对齐（归零到0点）
         */
        static QDateTime ConvertDateTimeDayAligment(const QString& str, const QString& format = "yyyyMMddhhmmss")
        {
            QDateTime time = QDateTime::fromString(str, format);
            if (!time.isValid())
                return time;

            time = QDateTime(time.date(), QTime(0,0,0));
            return time;
        }

        /**
         * @brief 小时对齐
         */
        static QDateTime ConvertDateTimeHourAligment(const QString& str, const QString& format = "yyyyMMddhhmmss")
        {
            QDateTime time = QDateTime::fromString(str, format);
            if (!time.isValid())
                return time;

            int hour = time.time().hour();
            time = QDateTime(time.date(), QTime(hour,0,0));
            return time;
        }

        /**
         * @brief 分钟对齐
         */
        static QDateTime ConvertDateTimeMinuteAligment(const QString& str, const QString& format = "yyyyMMddhhmmss")
        {
            QDateTime time = QDateTime::fromString(str, format);
            if (!time.isValid())
                return time;

            int hour = time.time().hour();
            int minute = time.time().minute();
            time = QDateTime(time.date(), QTime(hour,minute,0));
            return time;
        }

    public:
        /**
         * @brief 开始计时
         */
        static QElapsedTimer StartTimer()
        {
            QElapsedTimer time;
            time.start();
            return time;
        }

        /**
         * @brief 获取毫秒耗时
         */
        static int GetTimerMSConsum(QElapsedTimer& timer, bool restart = false)
        {
            if (!timer.isValid())
                return 0;
            int consume = timer.elapsed();
            if (restart)
                timer.restart();
            return consume;

        }
    };


    class TimerLocker
    {
    public:
        TimerLocker(QTimer* timer, int msInterval = 0)
        {
            m_timer = timer;
            m_interval = msInterval;
            if (m_timer)
                m_timer->stop();
        }

        ~TimerLocker()
        {
            if (m_timer == nullptr)
                return;
            if (m_timer->isActive())
                return;

            if (m_interval > 0)
                m_timer->start(m_interval);
            else
                m_timer->start();
        }

        void Start(int interval)
        {
            m_timer->start(interval);
        }

    private:
        QTimer* m_timer;
        int     m_interval;
    };
}

#endif
