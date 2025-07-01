QT += core gui serialport sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = Common

TEMPLATE = lib

win32 {
    LIBS += -lws2_32
}
SOURCES += test.cpp

HEADERS  += SystemDef.h \
            Control/App.h \
            Control/SplashScreen.h \
            ControlDef.h \
            System/BitMap.h \
            DataRecord.h \
            System/CRC.h \
            System/Counter.h \
            System/Exception.h \
            System/File.h \
            System/Global.h \
            System/Debug.h \
            Control/Group.h \
            System/Lock.h \
            System/Logger.h \
            System/Math.h \
            System/Memory.h \
            System/Notifiable.h \
            System/Path.h \
            System/Picture.h \
            System/Process.h \
            System/Property.h \
            System/Queue.h \
            System/Reflection.h \
            System/String.h \
            System/Thread.h \
            System/Time.h \
            Tool/ExcelFile.h \
            Tool/Monitor.h \
            Tool/NFSClient.h \
            Tool/NetTool.h \
            Tool/Requests.h \
            Tool/Socket.h \
            Tool/SerialPortTool.h \
            Tool/SqlTable.h \
            Tool/TcpClient.h

DISTFILES += \
    CopyToProjects.py \
    README.md
