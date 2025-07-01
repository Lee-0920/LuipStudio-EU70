#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QDialog>
#include "HttpGetMethod.h"

namespace Interface
{
namespace Wqimc
{
class RemoveZip: public QObject
{
public:
    RemoveZip(QString fileName):
        m_fileName(fileName)
    {

    }

    ~RemoveZip()
    {
        if (QFile::exists(m_fileName))
        {
            QFile::remove(m_fileName);
        }
    }
private:
    QString m_fileName;
};


class HttpClient: public QObject
{
    Q_OBJECT
public:
    explicit HttpClient();
    bool UpgradeGetRequest(const QString urlSpec, const QObject *receiver, const char* readyReadSlot, const char* finishedSlot, const char* errorSlot);
    bool DownloadGetRequest(const QString urlSpec, const QObject *receiver, const char* readyReadSlot, const char* finishedSlot, const char* errorSlot);
    bool PostFileRequest(const QString urlSpec, const QString fileName, const QString filePath, bool isDeletePostFile = false);

private slots:
    void AuthenticationRequiredSlot(QNetworkReply*,QAuthenticator *);
    void ReplyFinished(QNetworkReply*reply);

    void UploadProgressSlot(qint64 bytesSent, qint64 bytesTotal);
    void PostMethodTimeoutSlot();
signals:
    void PostMethodFinishedSignal();
    void PostMethodErrorSignal(const QString message);

private:
     QNetworkAccessManager m_qnam;

     QNetworkReply *m_postMethodReply;
     QTimer *m_postMethodTimer;
     qint64 m_fileSentBytes;
     qint64 m_lastSentBytes;

     HttpGetMethod* m_upgradeGet;
     bool m_isUpgradeGetFinish;
     HttpGetMethod* m_downloadGet;
     bool m_isDownloadGetFinish;
};
}
}
#endif // HTTPCLIENT_H
