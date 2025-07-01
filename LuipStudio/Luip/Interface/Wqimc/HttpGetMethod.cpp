#include "HttpGetMethod.h"

namespace Interface
{
namespace Wqimc
{
HttpGetMethod::HttpGetMethod(QNetworkReply* reply):
    m_reply(reply),m_fileReceivedBytes(0),m_lastReceivedBytes(0)
{
    connect(&m_timer, SIGNAL(timeout()),
             this, SLOT(TimeoutSlot()));
}

QNetworkReply* HttpGetMethod::GetReply()
{
    return m_reply;
}

void HttpGetMethod::StartTimeOutCheck()
{
    m_fileReceivedBytes = 0;
    m_lastReceivedBytes = 0;
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    m_timer.start(60000);
}

void HttpGetMethod::StopTimeOutCheck()
{
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    m_fileReceivedBytes = 0;
    m_lastReceivedBytes = 0;
}

void HttpGetMethod::ReplyFinished()
{
    StopTimeOutCheck();
    if (m_reply->error())
    {
        emit ErrorSignal(m_reply->errorString());
    }
    else
    {
        emit FinishedSignal();
    }
    m_reply->deleteLater();
    m_reply = Q_NULLPTR;
}

void HttpGetMethod::ReadyReadSlot()
{
    emit ReadyReadSignal(m_reply->readAll());
}

void HttpGetMethod::DownloadProgressSlot(qint64 bytesReceived, qint64 bytesTotal)
{
    (void)bytesTotal;
    m_fileReceivedBytes = bytesReceived;
}

void HttpGetMethod::TimeoutSlot()
{
    if (m_fileReceivedBytes != m_lastReceivedBytes)
    {
        m_lastReceivedBytes = m_fileReceivedBytes;
    }
    else
    {
        if (m_reply != Q_NULLPTR)
        {
            m_reply->abort();
        }
        m_timer.stop();
    }
}

}
}
