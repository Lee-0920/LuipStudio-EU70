#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QThread>

namespace ConnectionSpace
{


class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(QObject *parent = nullptr);


public slots:
    void OnNewConnection();
    void OnReadyRead();
    void SendFileToClient(QTcpSocket *clientSocket);
    bool BackupFile(const QString& sourcePath, const QString& backupPath);
private:
    quint16 m_port = 20000;
};

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QObject *parent = nullptr);

protected:
    void run()override;

private:
    TcpServer* m_server;
};

}

#endif // SERVER_H
