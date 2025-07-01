#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>

namespace ConnectionSpace
{

class Client: public QObject
{
    Q_OBJECT
public:
    Client(const QString &host, quint16 port, QObject *parent = nullptr);
    void Start();
private slots:
    void OnConnected();
    void OnReadyRead();
    void OnDisconnected();

private:
    QTcpSocket *m_socket = nullptr;
    QString m_host;
    quint16 m_port;
};
}

#endif // CLIENT_H
