#ifndef __TREASURE_SYSTEM_NOTIFIABLE__
#define __TREASURE_SYSTEM_NOTIFIABLE__

#include <QMap>
#include <QMutex>
#include <QString>

namespace Treasure
{


/**
 * @brief 设置状态监控
 */
class ICommunicationNotifiable
{
public:
    ICommunicationNotifiable() {}
    virtual ~ICommunicationNotifiable() {}

public:
    virtual void OnCommunitcationStatusChange(QString deviceName ,bool status) = 0;
};


template<typename NOTIFY_CLASS>
class NotifyManager
{
public:
    void Register(NOTIFY_CLASS *notice)
    {
        QMutexLocker lock(&m_notifyMutex);
        m_notifyList.append(notice);
    }

    void Deregister(NOTIFY_CLASS *notice)
    {
        QMutexLocker lock(&m_notifyMutex);
        for(auto it = m_notifyList.begin(); it != m_notifyList.end(); it++)
        {
            if((*it) == notice)
            {
                m_notifyList.erase(it);
                break;
            }
        }
    }

protected:
    QList<NOTIFY_CLASS*> m_notifyList;
    QMutex m_notifyMutex;
};



}

#endif
