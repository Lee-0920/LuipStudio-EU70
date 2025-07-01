#ifndef __TREASURE_TOOL_REQUESTS_H__
#define __TREASURE_TOOL_REQUESTS_H__

#include "../SystemDef.h"
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QWaitCondition>
#include <QEventLoop>

#define trRequests Treasure::Requests::Instance()

namespace Treasure
{

class Requests : public QObject
{
    TREASURE_SINGELTON(Requests)
public:
    Requests()
    {
        m_reply = NULL;
        m_accessManager = new QNetworkAccessManager();
        connect(m_accessManager, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply)
        {
            Q_UNUSED(reply)
            if (m_timer.isActive())
                m_timer.stop();
        });
        connect(&m_timer, &QTimer::timeout, this, [=]{
            m_timer.stop();
            if (m_reply->isFinished())
                return;
            m_reply->abort();
        });
    }

    ~Requests()
    {
        delete m_accessManager;
    }

public:
    QNetworkReply* Get(QString url, unsigned long time = ULONG_MAX)
    {
        QMutexLocker lock(&m_mutex);
        if (m_reply)
        {
            m_reply->deleteLater();
            m_reply = NULL;
        }

        m_timer.setInterval(time);
        m_timer.setSingleShot(true);

        QEventLoop loop;
        m_reply = m_accessManager->get(QNetworkRequest(QUrl(url)));
        connect(m_reply, SIGNAL(finished()), &loop, SLOT(quit()), Qt::DirectConnection);
        m_timer.start();
        loop.exec();
        return m_reply;
    }

private:
    QMutex m_mutex;
    QTimer m_timer;
    QNetworkReply* m_reply;
    QNetworkAccessManager *m_accessManager;
};

}

#endif
