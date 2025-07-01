#include "Server.h"
#include <QDir>
#include <QApplication>
#include <QFile>
#include "Log.h"
#include <QThread>
#include "DataBasePlugin/DataBaseManager.h"
#include "Setting/Environment.h"
namespace ConnectionSpace
{
TcpServer::TcpServer(QObject *parent): QTcpServer(parent) {
    if (!this->listen(QHostAddress::Any, m_port)) {
        qCritical() << "Server could not start!";
        return;
    }
    qDebug() << "Server started, listening on port" << m_port;

    connect(this, &QTcpServer::newConnection, this, &TcpServer::OnNewConnection);
}

ServerThread::ServerThread(QObject *parent) : QThread(parent)
{
//    if (m_server.listen(QHostAddress::Any, port)) {
//        qDebug() << "Server started, listening on port" << port;
//        connect(&m_server, &QTcpServer::newConnection, this, &Server::OnNewConnection);
//    } else {
//        qCritical() << "Server could not start";
//    }
}

void ServerThread::run()
{
    m_server = new TcpServer();

    // 启动事件循环
    exec();
}


void TcpServer::OnNewConnection()
{
    qDebug() << "new con";
    QTcpSocket *clientSocket = this->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::OnReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

void TcpServer::OnReadyRead()
{
    QTcpSocket *clientSocket = static_cast<QTcpSocket *>(sender());
    QByteArray request = clientSocket->readAll();
    QString requestString = QString::fromUtf8(request);
     if (requestString.startsWith("GET_DATABASE_FILE")) {
          SendFileToClient(clientSocket);
     }else if(requestString.startsWith("DATABASE_ARCHIVE")){
        auto archivePath = QString::fromStdString(Configuration::Environment::Instance()->GetDataBaseArchivedPath());
        auto timeStr = QDateTime::currentDateTime().toString("yyMMdd");
        auto bakPath = archivePath + "/archive" + timeStr + ".bak";
        DataBaseSpace::DataBaseManager::Instance()->DataBaseArchived(bakPath);
     }
     // 处理文件上传
     else if (requestString.startsWith("UPLOAD_FILE")) {
         QStringList parts = requestString.split("|");
         if (parts.size() < 3) {
             qWarning() << "Invalid upload request!";
             return;
         }

         QString fileName = parts[1];  // 获取文件名
         qint64 fileSize = parts[2].toLongLong();  // 获取文件大小

         // 确定文件存储路径
         auto bakPath = QString::fromStdString(Configuration::Environment::Instance()->GetDataBaseBakPath());
         QString savePath = bakPath + "/" + fileName;
         QFile delFile(savePath);
         if (delFile.exists()) {
             delFile.remove();
         }


         QFile *file = new QFile(savePath);
         if (!file->open(QIODevice::WriteOnly)) {
             qWarning() << "Failed to open file for writing!";
             delete file;
             return;
         }

         // 读取文件内容并写入
         QByteArray fileData = request.mid(request.indexOf("\n") + 1);
         file->write(fileData);

         while (clientSocket->bytesAvailable()) {
             file->write(clientSocket->readAll());
         }

         file->close();
         delete file;

         auto archivePath = QString::fromStdString(Configuration::Environment::Instance()->GetDataBaseArchivedPath());
         auto timeStr = QDateTime::currentDateTime().toString("yyMMdd");
         auto targatPath = archivePath + "/archive" + timeStr + ".bak";
         DataBaseSpace::DataBaseManager::Instance()->DataBaseArchived(targatPath, savePath);

         qDebug() << "File received and saved to:" << savePath;
         clientSocket->write("UPLOAD_SUCCESS");  // 发送成功反馈


     }

}

void TcpServer::SendFileToClient(QTcpSocket *clientSocket)
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QDir dir(currentDir);
    dir.cdUp();
    QString dataDirPath = dir.absolutePath() + "/LuipData/measuring.ds";
    QString bakPath = dir.absolutePath() + "/LuipData/bak.ds";
    bool res = BackupFile(dataDirPath, bakPath);
    if(res == false) {
        logger->debug("backup file failed!");
    }
    QFile fileToSend(bakPath);
    if (fileToSend.open(QIODevice::ReadOnly)) {
        qint64 totalSize = fileToSend.size();
        clientSocket->write(reinterpret_cast<char*>(&totalSize), sizeof(totalSize));
        clientSocket->flush();

        qint64 bytesWritten = 0;
        while (bytesWritten < totalSize) {
            QByteArray chunk = fileToSend.read(8192);
            if (chunk.isEmpty()) {
                break;
            }
            bytesWritten += clientSocket->write(chunk);
            clientSocket->flush();
        }
        clientSocket->flush();
        fileToSend.close();
        qDebug() << "File sent to client.";
    }
}

bool TcpServer::BackupFile(const QString& sourcePath, const QString& backupPath)
{
    QFile sourceFile(sourcePath);
    if (!sourceFile.exists()) {
        return false;
    }

    QFile backupFile(backupPath);
    if (backupFile.exists()) {
        backupFile.remove();
    }

    if (sourceFile.copy(backupPath)) {
        return true;
    } else {
        return false;
    }
}

}
