#-------------------------------------------------
#
# Project created by QtCreator 2017-04-15T23:31:53
#
#-------------------------------------------------

QT       -= gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += serialport

QMAKE_CXXFLAGS += -std=c++11
CONFIG += C++11
TARGET = Communication
TEMPLATE = lib
DESTDIR = ../../bin

#DEFINES += _CS_USING_LAI_RS232
DEFINES += _CS_USING_LAI_RS485
#DEFINES += _CS_USING_LAI_UDP

INCLUDEPATH += . ./eCek ../Luip ../
INCLUDEPATH += ../Lib/log4cpp/include
INCLUDEPATH += ../Lib/lua/include
INCLUDEPATH += ../Lib/oolua/include

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0

win32 {
    DEFINES += _CS_X86_WINDOWS
    DEFINES += BIL_EXPORT

    DEFINES += "_CS_SERIAL_PORT=\"\\\"COM10\\\"\""

    Debug:LIBS   += -L$$PWD/../Lib/oolua/lib/windows/ -looluaD

    Release:LIBS += -L$$PWD/../Lib/oolua/lib/windows/ -loolua

    LIBS   += -lws2_32 \
              -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp \
              -L$$PWD/../Lib/lua/lib/windows/ -llua \
              -L$$PWD/../../bin/ -lLuaEngine
}
unix {
     CONFIG += arm
#    CONFIG += x86

     DEFINES += "_CS_SERIAL_PORT=\"\\\"ttymxc2\\\"\""

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-x86 -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua\
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../../bin/ -lLuaEngine
    }
    arm {
        DEFINES += _CS_ARM_LINUX
        LIBS += -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../../bin/ -lLuaEngine
    }
}
android {
        CONFIG += android

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_ANDROID
        LIBS += -L$$PWD/../Lib/log4cpp/lib/android -llog4cpp
}

SOURCES += eCek/Common/MessageDigest.c \
    eCek/Common/SafeHeap.c \
    eCek/Container/Queue.c \
    eCek/DNCP/App/DscpController.c \
    eCek/DNCP/App/DscpDevice.c \
    eCek/DNCP/Base/Frame.c \
    eCek/DNCP/Lai/LaiRS485Master.cpp \
    eCek/DNCP/Lai/LaiSerial.cpp \
    eCek/DNCP/Lai/LaiTcpClient.cpp \
    eCek/DNCP/Lai/LaiTcpServer.cpp \
    eCek/DNCP/Lai/Lai.c \
    eCek/DNCP/Ll/Dslp.c \
    eCek/DNCP/Ll/Ll.c \
    eCek/DNCP/Ll/Llci.c \
    eCek/DNCP/Net/Net.c \
    eCek/DNCP/Net/NetPack.c \
    eCek/DNCP/Net/Trp.c \
    eCek/DNCP/Net/TrpTail.c \
    eCek/DNCP/Port/OS/DscpScheduler.cpp \
    eCek/Common/Port/Cpp11/System.cpp \
    eCek/Tracer/Trace.c \
    Stack/DncpMcStack.cpp \
    Stack/DncpStack.cpp \
    AsyncCall.cpp \
    AsyncScheduler.cpp \
    AsyncTask.cpp \
    CallingQueue.cpp \
    EventHandler.cpp \
    SyncCaller.cpp \
    CommunicationLibrary.cpp \
    Dscp/DscpStatus.cpp \
    Dscp/DscpPacket.cpp \
    Log.cpp \
    CommunicationException.cpp \
    CommunicationProxy.cpp
HEADERS += Dscp/DscpAddress.h \
    Dscp/DscpCommand.h \
    Dscp/DscpEvent.h \
    Dscp/DscpPacket.h \
    Dscp/DscpRespond.h \
    Dscp/DscpStatus.h \
    eCek/Common/MessageDigest.h \
    eCek/Common/Readme.h \
    eCek/Common/SafeHeap.h \
    eCek/Common/System.h \
    eCek/Common/Types.h \
    eCek/Common/Utils.h \
    eCek/Container/ByteFifo.h \
    eCek/Container/Queue.h \
    eCek/DNCP/App/AppDefine.h \
    eCek/DNCP/App/Dscp.h \
    eCek/DNCP/App/DscpController.h \
    eCek/DNCP/App/DscpDevice.h \
    eCek/DNCP/App/DscpSysDefine.h \
    eCek/DNCP/Base/Frame.h \
    eCek/DNCP/Lai/IConnectStatusNotifiable.h \
    eCek/DNCP/Lai/Lai.h \
    eCek/DNCP/Lai/LaiRS485Master.h \
    eCek/DNCP/Lai/LaiSerial.h \
    eCek/DNCP/Lai/LaiTcpClient.h \
    eCek/DNCP/Lai/LaiTcpServer.h \
    eCek/DNCP/Lai/LaiTokenManager.h \
    eCek/DNCP/Ll/Dslp.h \
    eCek/DNCP/Ll/LinkFrame.h \
    eCek/DNCP/Ll/Ll.h \
    eCek/DNCP/Ll/Llci.h \
    eCek/DNCP/Net/Net.h \
    eCek/DNCP/Net/NetPack.h \
    eCek/DNCP/Net/Trp.h \
    eCek/DNCP/Net/TrpTail.h \
    eCek/DNCP/Port/OS/DscpScheduler.h \
    eCek/Tracer/Trace.h \
    eCek/Readme.h \
    Stack/DncpMcStack.h \
    Stack/DncpStack.h \
    Stack/IDscpReceivable.h \
    AsyncCall.h \
    AsyncScheduler.h \
    AsyncTask.h \
    CallingQueue.h \
    CommunicationException.h \
    EventHandler.h \
    IAsyncCallable.h \
    IEventReceivable.h \
    SyncCaller.h \
    CommunicationLibrary.h \
    Log.h \
    LuipShare.h \
    CommunicationProxy.h

win32 {
  SOURCES += \
    eCek/DNCP/Lai/LaiUdp.c
  HEADERS  += \
    eCek/DNCP/Lai/LaiUdp.h
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}
