
#include "Log.h"
#include "HttpClient.h"
#include <QDateTime>
namespace Interface
{
namespace Wqimc
{

HttpClient::HttpClient():
    m_postMethodReply(Q_NULLPTR), m_upgradeGet(nullptr),
    m_isUpgradeGetFinish(true), m_downloadGet(nullptr),
    m_isDownloadGetFinish(true)
{
    connect(&m_qnam, &QNetworkAccessManager::authenticationRequired,
                  this, &HttpClient::AuthenticationRequiredSlot);
    connect(&m_qnam, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(ReplyFinished(QNetworkReply*)));

    m_postMethodTimer = new QTimer;
    connect(m_postMethodTimer, SIGNAL(timeout()),
             this, SLOT(PostMethodTimeoutSlot()));
}

void HttpClient::ReplyFinished(QNetworkReply* reply)
{
    switch(reply->operation())
    {
    case QNetworkAccessManager::Operation::GetOperation :
        {
            if (m_upgradeGet != nullptr && m_upgradeGet->GetReply() == reply)
            {
                HttpGetMethod* httpGetMethod = m_upgradeGet;
                m_upgradeGet = nullptr;
                m_isUpgradeGetFinish = true;
                httpGetMethod->ReplyFinished();
            }
            else if (m_downloadGet != nullptr && m_downloadGet->GetReply() == reply)
            {
                HttpGetMethod* httpGetMethod = m_downloadGet;
                m_downloadGet = nullptr;
                m_isDownloadGetFinish = true;
                httpGetMethod->ReplyFinished();
            }
            else
            {
                logger->debug("Get 请求代码错误，不存在请求队列");
                reply->deleteLater();
                reply = Q_NULLPTR;
            }

        }
        break;
    case QNetworkAccessManager::Operation::PostOperation :
        if (m_postMethodTimer->isActive())
        {
            m_postMethodTimer->stop();
        }
        m_fileSentBytes = 0;
        m_lastSentBytes = 0;
        if (reply->error())
        {
            emit PostMethodErrorSignal(reply->errorString());
        }
        else
        {
            emit PostMethodFinishedSignal();
        }
        m_postMethodReply = Q_NULLPTR;
        reply->deleteLater();
        reply = Q_NULLPTR;
        break;
     default :
        break;
    }
}

void HttpClient::AuthenticationRequiredSlot(QNetworkReply*,QAuthenticator *authenticator)
{
    authenticator->setUser("");
    authenticator->setPassword("");
}

bool HttpClient::UpgradeGetRequest(const QString urlSpec, const QObject *receiver, const char* readyReadSlot, const char* finishedSlot, const char* errorSlot)
{
    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        logger->debug("无效的 URL： %s：%s", urlSpec.toStdString().c_str(), newUrl.errorString().toStdString().c_str());
        return false;
    }
    if (m_isUpgradeGetFinish == true)
    {
        m_isUpgradeGetFinish = false;
        m_upgradeGet= new HttpGetMethod(m_qnam.get(QNetworkRequest(newUrl)));
        connect(m_upgradeGet->GetReply(), SIGNAL(readyRead()), m_upgradeGet, SLOT(ReadyReadSlot()));
        connect(m_upgradeGet->GetReply(), SIGNAL(downloadProgress(qint64, qint64)), m_upgradeGet, SLOT(DownloadProgressSlot(qint64, qint64)));
        connect(m_upgradeGet, SIGNAL(ReadyReadSignal(QByteArray)), receiver, readyReadSlot);
        connect(m_upgradeGet, SIGNAL(FinishedSignal()), receiver, finishedSlot);
        connect(m_upgradeGet, SIGNAL(ErrorSignal(QString)), receiver, errorSlot);
        m_upgradeGet->StartTimeOutCheck();
        m_upgradeGet->setParent(m_upgradeGet->GetReply());
        return true;
    }
    else
    {
        logger->debug("升级 Get 请求已满, %s 请求失败", urlSpec.toStdString().c_str());
        return false;
    }
}

bool HttpClient::DownloadGetRequest(const QString urlSpec, const QObject *receiver, const char* readyReadSlot, const char* finishedSlot, const char* errorSlot)
{
    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        logger->debug("无效的 URL： %s：%s", urlSpec.toStdString().c_str(), newUrl.errorString().toStdString().c_str());
        return false;
    }
    if (m_isDownloadGetFinish == true)
    {
        m_isDownloadGetFinish = false;
        m_downloadGet = new HttpGetMethod(m_qnam.get(QNetworkRequest(newUrl)));
        connect(m_downloadGet->GetReply(), SIGNAL(readyRead()), m_downloadGet, SLOT(ReadyReadSlot()));
        connect(m_downloadGet->GetReply(), SIGNAL(downloadProgress(qint64, qint64)), m_downloadGet, SLOT(DownloadProgressSlot(qint64, qint64)));
        connect(m_downloadGet, SIGNAL(ReadyReadSignal(QByteArray)), receiver, readyReadSlot);
        connect(m_downloadGet, SIGNAL(FinishedSignal()), receiver, finishedSlot);
        connect(m_downloadGet, SIGNAL(ErrorSignal(QString)), receiver, errorSlot);
        m_downloadGet->StartTimeOutCheck();
        m_downloadGet->setParent(m_downloadGet->GetReply());
        return true;
    }
    else
    {
        logger->debug("下载 Get 请求已满, %s 请求失败", urlSpec.toStdString().c_str());
        return false;
    }
}

/**
 * @brief HttpClient::PostFileRequest
 * @param urlSpec
 * @param fileName 服务器里的文件名
 * @param filePath 上传的文件
 * @return
 */
bool HttpClient::PostFileRequest(const QString urlSpec, const QString fileName, const QString filePath,  bool isDeletePostFile)
{
    const QUrl newUrl = QUrl::fromUserInput(urlSpec);
    if (!newUrl.isValid())
    {
        logger->debug("无效的 URL： %s：%s", urlSpec.toStdString().c_str(), newUrl.errorString().toStdString().c_str());
        return false;
    }

    QNetworkRequest request;
    request.setUrl(newUrl);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary===ad89f3368aae8");

    QByteArray fileData;
    fileData.clear();

    QFile *file = new QFile(filePath);
    if (file->open(QFile::ReadOnly))
    {
        fileData.resize(file->bytesAvailable());
        file->read(fileData.data(), file->bytesAvailable());

        file->close();

        QByteArray data = "--==ad89f3368aae8\r\n"
                          "Content-Disposition: form-data;name=\"instrumentFile\";filename=\""+ fileName.toLatin1() +"\"\r\n"
                          "Content-Type:application/octet-stream\r\n\r\n"
                          + fileData +
                          "\r\n\r\n--==ad89f3368aae8--\r\n";

        m_fileSentBytes = 0;
        m_lastSentBytes = 0;

        m_postMethodReply = m_qnam.post(request, data);

        if (isDeletePostFile)
        {
            RemoveZip * removeZip = new RemoveZip(filePath);
            removeZip->setParent(m_postMethodReply);
        }

        connect(m_postMethodReply, &QNetworkReply::uploadProgress, this, &HttpClient::UploadProgressSlot);
        if (m_postMethodTimer->isActive())
        {
            m_postMethodTimer->stop();
        }
        m_postMethodTimer->start(60000);
        return true;
    }
    else
    {

        delete file;
        logger->debug("打开 %s 文件失败,无法上传文件到服务器", filePath.toStdString().c_str());
        return false;
    }

}

void HttpClient::PostMethodTimeoutSlot()
{
    if (m_fileSentBytes != m_lastSentBytes)
    {
        m_lastSentBytes = m_fileSentBytes;
    }
    else
    {
        if (m_postMethodReply != Q_NULLPTR)
        {
            m_postMethodReply->abort();
        }
        m_postMethodTimer->stop();
    }
}


void HttpClient::UploadProgressSlot(qint64 bytesSent, qint64 bytesTotal)
{
    (void)bytesTotal;
    m_fileSentBytes = bytesSent;
}

}
}
