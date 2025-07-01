#include "Client.h"
#include <QDir>

#include <QApplication>

namespace ConnectionSpace
{

Client::Client(const QString &host, quint16 port, QObject *parent) : QObject(parent), m_host(host),m_port(port)
{

    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &Client::OnConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::OnDisconnected);
}

void Client::Start()
{
    m_socket->connectToHost(m_host, m_port);
    if (!m_socket->waitForConnected(5000)) {
        return;
    }

    QByteArray request = "GET_DATABASE_FILE";
    m_socket->write(request);

    // 阻塞等待读取数据
    QEventLoop loop;
    connect(m_socket, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
    loop.exec();  // 执行事件循环，直到接收到数据
}


void Client::OnConnected()
{
    // QFile file(m_fileName);
    // if (file.open(QIODevice::ReadOnly)) {
    //     QByteArray fileData = file.readAll();
    //     m_socket->write(fileData);  // 发送文件数据
    //     file.close();
    //     qDebug() << "File sent to server.";
    // } else {
    //     qDebug() << "Unable to open file!";
    // }
}

void Client::OnReadyRead()
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QDir dir(currentDir);
    dir.cdUp();
    QString dataDirPath = dir.absolutePath() + "/LuipData/bak.ds";
    QString sqlDirPath = dir.absolutePath() + "/LuipData/measuring.ds";
    QFile receivedFile(dataDirPath);
    if (receivedFile.exists()) {
        receivedFile.remove();
    }
    QFile sqlFile(sqlDirPath);
    if (sqlFile.exists()) {
        sqlFile.remove();
    }
    QByteArray receivedData = m_socket->readAll();
    if (receivedFile.open(QIODevice::WriteOnly)) {
        receivedFile.write(receivedData);
        // receivedFile.rename("measuring.ds");
        receivedFile.close();
    }
    QFile res(dataDirPath);
    if (res.exists()) {
        res.rename(sqlDirPath);
    }

    m_socket->disconnectFromHost();
}

void Client::OnDisconnected()
{
}
}
