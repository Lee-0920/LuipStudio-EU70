//#ifndef SL651_TEST_H
//#define SL651_TEST_H

//#include "Treasure/SystemDef.h"
//#include <QTcpSocket>
//#include <QTcpServer>
//#include <QNetworkReply>
//#include <QEventLoop>
//#include <QNetworkRequest>
//#include <QNetworkAccessManager>

//class ServerTest : public QObject
//{
//    Q_OBJECT
//public:
//    void Test()
//    {
////        static int a = 100;

//        auto func = [=]{
////            a++;

//            qDebug("a:%d", 1);
//         };


//        connect(this, &ServerTest::SignalTest, this, func);

//        emit SignalTest();

//    }

//    void start()
//    {
//        QString concatenated = "admin:ls123456"; //username:password

//        QByteArray data = concatenated.toLocal8Bit().toBase64();

//        QString headerData = "Basic " + data;

//        QNetworkRequest request=QNetworkRequest(QUrl("http://192.168.0.208/ISAPI/Streaming/channels/101/picture"));

//        request.setRawHeader("Authorization", headerData.toLocal8Bit());
//        request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36");
//        request.setRawHeader("Host", "192.168.0.208");
//        request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
//        request.setRawHeader("Connection", "keep-alive");
//        QNetworkAccessManager* networkAccessManager = new QNetworkAccessManager();

//        QNetworkReply * reply = networkAccessManager->get(request);
//        connect(reply, &QNetworkReply::finished, this, [=]{

//            auto error = reply->error();
//            auto msg = reply->errorString();

////            reply->deleteLater();
//            auto headList = reply->rawHeaderList();
//            auto auth = reply->rawHeader("WWW-Authenticate");

//            auto content = reply->readAll();

//            return;
//        });
//        return;
//    }

//signals:
//    void SignalTest();

//private:
//    QTcpServer* m_server;
//    QList<QTcpSocket*> m_sockets;
//};

//#endif // TEST_H
