//#include "Test.h"
//#include <QApplication>


//#include <QNetworkReply>
//#include <QNetworkRequest>
//#include <QNetworkAccessManager>

//#include "Treasure/Tool/NetTool.h"

//#include "UdpManager.h"
//using namespace UdpSpace;

//int main(int argc, char *argv[])
//{
//    QApplication app(argc, argv);

//    trLogger->ShowQDebug(true);
//    qDebug("Start UDP");

//    DeviceInfo info;
//    info.port = 5900;
//    info.password = "";
//    info.name = "COD";
//    UdpManager::Instance()->InitByClient(NULL, info);
////    UdpManager::Instance()->InitByServer(NULL);

////    for(int i = 0; i < 10; i++)
////    {
////        UdpManager::Instance()->ClearBeforeSendSerach();
////        if (!UdpManager::Instance()->SendSearch())
////        {
////            qDebug("Send search failed.");
////            QThread::sleep(5);
////            continue;
////        }

////        QThread::sleep(5);
////        auto array = UdpManager::Instance()->GetDevices();
////        foreach(auto item, array)
////        {
////            qDebug("IP:%s PORT:%d NAME:%s", item.ip.toStdString().c_str(),
////                   item.port, item.name.toStdString().c_str());
////        }
////    }

//    app.exec();
//    return 0;
//}

