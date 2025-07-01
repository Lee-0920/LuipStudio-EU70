#ifndef __TREASURE_SYSTEM_LOCK_H__
#define __TREASURE_SYSTEM_LOCK_H__

#include <QMutex>
#include <QMutexLocker>


namespace Treasure
{

class MutexTryLock
{
public:
    MutexTryLock(QMutex *mutex, int timeout = 0)
    {
        m_mutex = mutex;
        m_isLock = m_mutex->tryLock(timeout);
    }

    ~MutexTryLock()
    {
        UnLock();
    }

    bool IsLock()
    {
        return m_isLock;
    }

    void UnLock()
    {
        if (m_isLock)
        {
            m_mutex->unlock();
            m_isLock = false;
        }
    }

private:
    std::atomic_bool m_isLock;
    QMutex* m_mutex;
};

}

#endif
