/**
 * @brief 调试
 * @author huangyue
 * @date 2021-02-05
 * @note 说明: 调试的接口都使用trDebug调用，比如trDebug->StartTimer
 *        -----------------------------------------------------------
 *            功能   |   使用
 *        -----------------------------------------------------------
 *            计时     StartTimer/GetTimerMSConsum/PrintTimerMSConsum
 *
 */

#ifndef __TREASURE_SYSTEM_DEBUG__
#define __TREASURE_SYSTEM_DEBUG__

#include <QString>
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "Global.h"

namespace Treasure
{

/**
 * @brief 全局调试工具，使用trDebug就可以调用Debug的接口
 */
#define trDebug Treasure::Global<Treasure::Debug>::Instance()


class Debug
{
public:
    /**
     * @brief 开始计时
     */
    void StartTimer()
    {
        if (!m_timer.isValid())
            m_timer.start();
        else
            m_timer.restart();
    }


    /**
     * @brief 获取毫秒耗时
     * @param restart 重启计时
     */
    int GetTimerMSConsum(bool restart = false)
    {
        if (!m_timer.isValid())
            return -1;
        int consume = m_timer.elapsed();
        if (restart)
            StartTimer();
        return consume;
    }


    /**
     * @brief 打印毫秒耗时
     * @param restart 重启计时
     * @param desc 描述
     * @return 耗时
     */
    int PrintTimerMSConsum(bool restart = false, QString desc = "")
    {
        int consume = GetTimerMSConsum(restart);

        desc += " Consum time:%d ms.";
        qDebug(desc.toStdString().c_str(), consume);
        return consume;
    }

    /**
     * @brief 打印当前线程ID
     * @param desc 描述
     */
    void PrintCurrentThreadId(QString desc = "")
    {
        QString pre = desc.isNull() ? "" : desc + " ";
#ifdef _WIN32
        qDebug("%sThread Id is: %d", pre.toStdString().c_str(), (int)QThread::currentThreadId());
#else
        qDebug("%sThread Id is: %d", pre.toStdString().c_str(), QThread::currentThreadId());
#endif
    }

    /**
     * @brief 打印
     */
    void Print(QString desc)
    {
        qDebug() << desc;
    }

    void Print(std::string desc)
    {
        qDebug() << desc.c_str();
    }



private:
    QElapsedTimer m_timer;
};





}

#endif
