/**
 * @brief 线程
 * @author huangyue
 * @date 2021-04-02
 * @note
 */


#ifndef __TREASURE_SYSTEM_THREAD__
#define __TREASURE_SYSTEM_THREAD__

#include <thread>
#include <atomic>
#include <QMap>
#include <QMutex>

#include "Global.h"

namespace Treasure
{
    class Thread
    {
    public:
        Thread()
        {
            m_requestInterruption = false;
            m_isStarted = false;
            m_thread = NULL;
        }

        ~Thread()
        {
            if (m_thread)
            {
                delete m_thread;
                m_thread = NULL;
            }
        }

    public:
        /**
         * @brief 创建线程
         * @param func 函数
         * @param arg  参数集合
         */
        template<typename T, typename... ARGS>
        static Thread* CreatThread(T&& func, ARGS&&... arg)
        {
            Thread* thread = new Thread();
            thread->Start(func, arg...);
            return thread;
        }

        /**
         * @brief 启动线程
         * @param func 函数
         * @param arg  参数集合
         */
        template<typename T, typename... ARGS>
        void Start(T&& func, ARGS&&... arg)
        {
            m_thread = new std::thread(func, arg...);
            m_isStarted = true;
        }

        //等待
        void Wait()
        {
            if (!m_isStarted)
                return;
            if (m_thread)
            {
                m_thread->join();
                m_isStarted = false;
            }
        }

        //脱离主线程
        void Detach()
        {
            if (!m_isStarted)
                return;
            if (m_thread)
            {
                m_thread->detach();
                m_isStarted = false;
            }
        }

        //请求终止
        void RequestInterruption()
        {
            m_requestInterruption = true;
        }

        //是否请求终止
        bool IsRequestInterruption() const
        {
            return m_requestInterruption;
        }

        /**
         * @brief 休眠（毫秒）
         * @param msTime 总时长
         * @param partTime 片段时长（每个片段会检查线程是否请求终止）
         */
        void Sleep(int msTime, int partTime)
        {
            int sleepTime = partTime;
            int cnt = msTime / sleepTime;
            while(cnt--)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                if (IsRequestInterruption())
                    return;
            }
        }

    private:
        std::atomic_bool m_requestInterruption;
        std::atomic_bool m_isStarted;
        std::thread*     m_thread;

    };

    //线程智能指针
    typedef std::shared_ptr<Thread> ThreadPtr;

    //全局线程管理
    #define trThreads Treasure::Global<Treasure::ThreadManager>::Instance()

    class ThreadManager
    {
    public:
        /**
         * @brief 创建线程
         * @param key 关键字
         * @param func 函数
         * @param arg  参数集合
         */
        template<typename T, typename... ARGS>
        Thread* CreatThread(QString key, T&& func, ARGS&&... arg)
        {
            QMutexLocker lock(&m_mutex);
            if (m_threads.contains(key))
                return NULL;

            Thread* thread = new Thread();
            m_threads.insert(key, thread);

            thread->Start(func, arg...);
            return thread;
        }

        /**
         * @brief 添加线程
         * @param key 关键字
         * @param thread 线程
         */
        bool Add(QString key, Thread* thread)
        {
            QMutexLocker lock(&m_mutex);
            if (m_threads.contains(key))
                return false;

            m_threads.insert(key, thread);
            return true;
        }

        /**
         * @brief 删除线程
         * @param key 关键字
         */
        void Remove(QString key)
        {
            QMutexLocker lock(&m_mutex);
            if (m_threads.contains(key))
            {
                m_threads.remove(key);
            }
        }

        /**
         * @brief 获取
         * @param key 关键字
         */
        Thread* Get(QString key)
        {
            QMutexLocker lock(&m_mutex);
            if (m_threads.contains(key))
                return m_threads[key];
            return NULL;
        }

        //数量
        int Count()
        {
            QMutexLocker lock(&m_mutex);
            return m_threads.count();
        }

    private:
        QMutex m_mutex;
        QMap<QString, Thread*> m_threads;
    };


}

#endif
