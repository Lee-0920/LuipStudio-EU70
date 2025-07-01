//#include <QApplication>
//#include <QThread>
//#include <QPushButton>

//#include "ControlDef.h"
//#include "SystemDef.h"
//#include "Tool/SerialPortTool.h"
//#include "Tool/Socket.h"
//#include "Tool/Monitor.h"
//#include "Control/App.h"
//#include "Tool/NetTool.h"

//using namespace Treasure;

////void Check(SerialPortTool& serial)
////{
////    if (serial.Open("COM5"))
////    {
////        std::string buffer = "test serial port tool\n";
////        serial.Send(buffer.c_str(), buffer.length());

////        qDebug("send data:%s",buffer.c_str());
////        QString data = serial.Recieve(5000);
////        qDebug("recieve data:%s",data.toStdString().c_str());
////    }
////}


////class Test
////{
////public:
////    template<typename T>
////    bool Check(T& obj)
////    {
////        return false;
////    }
////};

////class TEST2 : public Test
////{
////public:
////    bool Check(int& a)
////    {
////        return true;
////    }

////    int a = 0;
////};

////static void ThreadFunc(TEST2* test)
////{
////    Thread* thread = trThreads->Get("test");
////    while(thread == NULL || !thread->IsRequestInterruption())
////    {
////        qDebug("test: %d", test->a++);

////        QThread::sleep(3);
////    }
////}


//int main(int argc, char *argv[])
//{
//    App a(argc, argv);
//    NetTool::GetSystemTime();

//    a.exec();

////    (void)argc;
////    (void)argv;
////    char rec[1024];
////    QString buff = "QN=11;CN=2011;CP=&&user=yaron;pw=123456;user=huang;pw=11123&&";

//////    int sub = trString::SubString(buff, "ff=", "&&", 0).toUInt();

////    Socket::Init();
////    Socket client;
////    bool check = client.Connect("172.20.67.1",6800);
////    if (check)
////    {
////        int leng = client.Send(buff.toStdString().c_str(), buff.length());

////        leng = client.Recieve(rec, 1024);
////    }
////    //    TEST2 test;
//////    test.a = 100;

//////    Thread* thread = trThreads->CreatThread("test", ThreadFunc, &test);
//////    QThread::sleep(15);
//////    thread->RequestInterruption();
//////    thread->Wait();
//////    qDebug("MAIN test: %d", test.a++);


//    return 0;
//}
