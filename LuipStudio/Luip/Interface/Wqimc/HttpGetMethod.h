#ifndef HTTPGETMETHOD_H
#define HTTPGETMETHOD_H

#include <QDialog>
#include <QtNetwork>

namespace Interface
{
namespace Wqimc
{
class HttpGetMethod: public QObject
{
    Q_OBJECT
public:
    HttpGetMethod(QNetworkReply* reply);

    QNetworkReply *GetReply();
    void StartTimeOutCheck();
    void StopTimeOutCheck();
    void ReplyFinished();

public slots:
    void ReadyReadSlot();
    void DownloadProgressSlot(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void TimeoutSlot();

signals:
    void FinishedSignal();
    void ErrorSignal(const QString message);
    void ReadyReadSignal(const QByteArray &data);

private:
     QNetworkReply *m_reply;
     QTimer m_timer;
     qint64 m_fileReceivedBytes;
     qint64 m_lastReceivedBytes;
};
}
}
#endif // HTTPGETMETHOD_H
