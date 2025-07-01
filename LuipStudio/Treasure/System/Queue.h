/**
 * @brief 带锁的队列
 * @author huangyue
 * @date 2021-04-02
 * @note 队列有容量的限制，在Add和Get的时候，如果队列已满或队列为空，会阻塞等待
 */

#ifndef __TREASURE_SYSTEM_QUEUE__
#define __TREASURE_SYSTEM_QUEUE__

#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

namespace Treasure
{
template <typename T>
class Queue
{
public:
    Queue(int maxSize = 10)
    {
        m_maxSize = maxSize;
        m_stop = false;
    }

    /**
     * @brief 添加项（阻塞）
     * @param data 项
     */
    bool Add(T data)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notFull.wait(locker, [this] { return m_stop || ((int)m_queue.size() < m_maxSize); });
        if (m_stop)
            return false;

        m_queue.push_back(std::forward<T>(data));
        m_notEmpty.notify_one();
        return true;
    }

    bool AddUntil(T data, int msTimeout)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        if(!m_notFull.wait_for(locker,
                              std::chrono::milliseconds(msTimeout),
                              [this] { return m_stop || ((int)m_queue.size() < m_maxSize); }))
            return false;
        if ((int)m_queue.size() >= m_maxSize || m_stop)
            return false;

        m_queue.push_back(std::forward<T>(data));
        m_notEmpty.notify_one();
        return true;
    }

    /**
     * @brief 尝试添加项
     * @param data 项
     */
    bool TryAdd(T data)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        if ((int)m_queue.size() >= m_maxSize)
            return false;

        m_queue.push_back(std::forward<T>(data));
        m_notEmpty.notify_one();
        return true;
    }



public:
    /**
     * @brief 获取项（阻塞）
     * @param data 项
     */
    bool Get(T &data)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this] { return m_stop || (m_queue.size() > 0); });
        if (m_stop)
            return false;

        data = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
        return true;
    }

    bool GetUntil(T &data, int msTimeout)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        if(!m_notEmpty.wait_for(locker,
                               std::chrono::milliseconds(msTimeout),
                               [this] { return m_stop || (m_queue.size() > 0); }))
            return false;
        if ((int)m_queue.size() <= 0 || m_stop)
            return false;

        data = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
        return true;
    }

    /**
     * @brief 尝试获取项
     * @param data 项
     */
    bool TryGet(T &data)
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        if (m_queue.size() <= 0)
            return false;

        data = m_queue.front();
        m_queue.pop_front();
        m_notFull.notify_one();
        return true;
    }

    /**
     * @brief 停止
     */
    void Stop()
    {
        m_stop = true;
        m_notFull.notify_all();
        m_notEmpty.notify_all();
        m_queue.clear();
    }

    /**
     * @brief 获取队列当前大小
     */
    int GetSize()
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }

    typedef bool (*Cmp)(T,T);
    bool Find(T data, Cmp p)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        for(auto it = m_queue.begin(); it!=m_queue.end(); it++)
        {
            if (p(data, *it))
                return true;
        }
        return false;
    }

private:
    std::list<T> m_queue;               //缓冲区
    int m_maxSize;                      //同步队列最大的size
    bool m_stop;                        //停止的标志

    std::mutex m_mutex;                 //互斥量和条件变量结合起来使用
    std::condition_variable m_notEmpty; //不为空的条件变量
    std::condition_variable m_notFull;  //没有满的条件变量
};
}

#endif
