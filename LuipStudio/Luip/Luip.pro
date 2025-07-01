#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T10:37:47
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = Luip
TEMPLATE = app
DESTDIR = ../../bin

CONFIG += console


QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../Lib/log4cpp/include \
               ../Lib/qwt/include \
               ../Lib/oolua/include \
               ../Lib/lua/include \
               ../Lib/libmodbus/include \
               ../Lib/boost/include \
               ../Lib/thrift/include \
               ../Communication \
               ../Communication/eCek \
               ../Lib/zlib/include \
               ../Lib/qrencode/include \
               ../HJ212Plugin \
               ../TOCDriveControllerPlugin \
               ../ReactControllerPlugin \
               ../

DEFINES += OOLUA_USE_EXCEPTIONS=1 OOLUA_STORE_LAST_ERROR=0

#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

TRANSLATIONS += Luip_EN.ts
TRANSLATIONS += Luip_CT.ts

win32 {
    DEFINES += _CS_X86_WINDOWS
    Debug:LIBS   += -L$$PWD/../Lib/qwt/lib/windows/ -lqwtd \
                    -L$$PWD/../Lib/oolua/lib/windows/ -looluaD \
                    -L$$PWD/../Lib/thrift/lib/windows/ -lthriftD \
                    -L$$PWD/../Lib/qrencode/lib/windows/ -lqrencode \
                    -L$$PWD/../Lib/zlib/lib/windows/ -lzlibD

    Release:LIBS += -L$$PWD/../Lib/qwt/lib/windows/ -lqwt \
                    -L$$PWD/../Lib/oolua/lib/windows/ -loolua \
                    -L$$PWD/../Lib/thrift/lib/windows/ -lthriftD \
                    -L$$PWD/../Lib/qrencode/lib/windows/ -lqrencode \
                    -L$$PWD/../Lib/zlib/lib/windows/ -lzlib

    LIBS += -lws2_32\
            -lDbgHelp \
            -L$$PWD/../Lib/log4cpp/lib/windows/ -llog4cpp \
            -L$$PWD/../Lib/lua/lib/windows/ -llua \
            -L$$PWD/../Lib/libmodbus/lib/windows/ -lmodbus \
            -L$$PWD/../../bin/ -lLuaEngine \
            -L$$PWD/../../bin/ -lCommunication \
            -L$$PWD/../../bin/ -lControllerPlugin \
            -L$$PWD/../../bin/ -lRecordDataPlugin \
            -L$$PWD/../../bin/ -lResultDetailPlugin \
            -L$$PWD/../../bin/ -lPrinterPlugin \
            -L$$PWD/../../bin/ -lUdpPlugin \
            -L$$PWD/../../bin/ -lHJ212Plugin \
            -L$$PWD/../../bin/ -lTOCDriveControllerPlugin \
            -L$$PWD/../../bin/ -lDataBasePlugin \
            -L$$PWD/../../bin/ -lReactControllerPlugin
}
unix {
     CONFIG += arm
#    CONFIG += x86

    x86 {
        DEFINES += _CS_X86_LINUX
        LIBS += -L$$PWD/../Lib/qwt/lib/linux-x86 -lqwt \
                -L$$PWD/../Lib/log4cpp/lib/linux-x86 -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-x86 -llua \
                -L$$PWD/../Lib/oolua/lib/linux-x86 -loolua \
                -L$$PWD/../Lib/thrift/lib/linux-x86/ -lthrift \
                -L$$PWD/../Lib/qrencode/lib/linux-x86/ -lqrencode \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin -lCommunication \
                -L$$PWD/../../bin/ -lControllerPlugin \
                -L$$PWD/../../bin/ -lRecordDataPlugin \
                -L$$PWD/../../bin/ -lResultDetailPlugin \
                -L$$PWD/../../bin/ -lPrinterPlugin \
                -L$$PWD/../Lib/zlib/lib/linux-arm/ -lzlib \
                -L$$PWD/../../bin/ -lUdpPlugin \
                -L$$PWD/../../bin/ -lHJ212Plugin \
                -L$$PWD/../../bin/ -lTOCDriveControllerPlugin \
                -L$$PWD/../../bin/ -lDataBasePlugin \
                -L$$PWD/../../bin/ -lReactControllerPlugin
    }
    arm {

        DEFINES += _CS_ARM_LINUX
        DEFINES += _CS_USING_MODBUS
        DEFINES += _DEV_USING_CONTROL
        DEFINES += _DEV_USING_CURRENT
        LIBS += -L$$PWD/../Lib/qwt/lib/linux-arm -lqwt \
                -L$$PWD/../Lib/libmodbus/lib/linux-arm/ -lmodbus \
                -L$$PWD/../Lib/log4cpp/lib/linux-arm -llog4cpp \
                -L$$PWD/../Lib/lua/lib/linux-arm -llua \
                -L$$PWD/../Lib/oolua/lib/linux-arm -loolua \
                -L$$PWD/../Lib/thrift/lib/linux-arm/ -lthrift \
                -L$$PWD/../Lib/qrencode/lib/linux-arm/ -lqrencode \
                -L$$PWD/../../bin/ -lLuaEngine \
                -L$$PWD/../../bin -lCommunication \
                -L$$PWD/../../bin/ -lControllerPlugin \
                -L$$PWD/../../bin/ -lRecordDataPlugin \
                -L$$PWD/../../bin/ -lResultDetailPlugin \
                -L$$PWD/../../bin/ -lPrinterPlugin \
                -L$$PWD/../Lib/zlib/lib/linux-arm/ -lzlib \
                -L$$PWD/../../bin/ -lUdpPlugin \
                -L$$PWD/../../bin/ -lHJ212Plugin \
                -L$$PWD/../../bin/ -lTOCDriveControllerPlugin \
                -L$$PWD/../../bin/ -lDataBasePlugin \
                -L$$PWD/../../bin/ -lReactControllerPlugin
    }
}
android {
        CONFIG += android

        DEFINES += _CS_ARM_LINUX
        DEFINES -= _CS_USING_MODBUS
        DEFINES -= _DEV_USING_CONTROL
        DEFINES -= _DEV_USING_CURRENT
        DEFINES += _CS_ANDROID
        DEFINES += "_CS_APP_ROOT=\"\\\"/sdcard/LuipStudio\"\""  # Default: executable file path
        DEFINES += errno=0
        LIBS += -L$$PWD/../Lib/log4cpp/lib/android -llog4cpp-larm \
                -L$$PWD/../Lib/qwt/lib/android -lqwt \
                -L$$PWD/../Lib/libmodbus/lib/android/ -lmodbus
}


SOURCES += main.cpp\
    Communication/CommunicationManager.cpp \
    ConnectionManager/Client.cpp \
    ConnectionManager/Server.cpp \
    Controller/ControllerManager.cpp \
    Log.cpp \
    Setting/Environment.cpp \
    Setting/SettingManager.cpp \
    Setting/SettingManagerProxy.cpp \
    ResultManager/RecordFields.cpp \
    ResultManager/ResultManager.cpp \
    ResultManager/ResultManagerProxy.cpp \
    System/CopyFile.cpp \
    System/FileDetecter.cpp \
    AlarmManager/AlarmInfoFile.cpp \
    AlarmManager/AlarmManager.cpp \
    AlarmManager/AlarmManagerProxy.cpp \
    StatusManager/StatusManager.cpp \
    StatusManager/StatusManagerProxy.cpp \
    System/NFSManager.cpp \
    UI/Data/MeasureData.cpp \
    UI/Frame/NumberKeyboard.cpp \
    UI/UserChangeManager/UserChangeManager.cpp \
    UI/Frame/MQtableWidget.cpp \
    UI/Frame/QMyEdit.cpp \
    UI/Frame/MessageDialog.cpp \
    UI/Data/CalibrateCurve.cpp \
    UI/Data/TextfileParser.cpp \
    UI/Data/WarnMangerWidget.cpp \
    UI/Data/RunLogWidget.cpp \
    Controller/ControllerManagerProxy.cpp \
    UI/Frame/LoginDialog.cpp \
    UI/Frame/DropShadowWidget.cpp \
    UI/Frame/TitleWidget.cpp \
    UI/Frame/ToolButton.cpp \
    UI/Frame/ChangePasswordDialog.cpp \
    UI/Frame/BottomWidget.cpp \
    UI/Frame/MeasureProgress.cpp \
    UI/Frame/QclickLabel.cpp \
    UI/Home/RunStatusWindow.cpp \
    UI/Frame/MeasureResultPanel.cpp \
    UI/Data/MeasureDataWindow.cpp \
    UI/Frame/MainWindow.cpp \
    UI/Singal/SignalWidget.cpp \
    UI/Singal/SingalWindow.cpp \
    SignalManager/SignalManager.cpp \
    SignalManager/SignalManagerProxy.cpp \
    UI/Setting/MachineParamWindow.cpp \
    UI/UIProxy.cpp \
    UI/Maintain/RemoteMaintainWindow.cpp \
    UI/Maintain/ReagentManagerWidget.cpp \
    UI/Maintain/MaintenanceWidget.cpp \
    UI/Maintain/UseResourceWidget.cpp \
    UI/Maintain/PipeManngerWidget.cpp \
    UI/Maintain/HardwareCheckWidget.cpp \
    UI/System/SystemInfoWindow.cpp \
    UI/System/InstrumentInformationWidget.cpp \
    UI/System/DeviceInformation.cpp \
    UI/System/NetSet.cpp \
    UI/System/SystemTime.cpp \
    UI/System/FactoryTime.cpp \
    UI/System/ScreenCalibrate.cpp \
    System/Watchdogclient.cpp \
    System/LockScreen/BanklightControl.cpp \
    System/LockScreen/Buzzer.cpp \
    System/LockScreen/ScreenEventFilter.cpp \
    System/LockScreen/ScreenProtection.cpp \
    UI/Data/WaveData.cpp \
    FlowManager/FlowManager.cpp \
    FlowManager/MeasureScheduler.cpp \
    UI/Maintain/IntellectCheckWidget.cpp \
    UI/Maintain/CommunicationTestWidget.cpp \
    UI/Maintain/HardwareTestWidget.cpp \
    UI/Maintain/UpdateWidget.cpp \
    UpdateManager/ProgramFileParser.cpp \
    UpdateManager/ProgramSegment.cpp \
    UpdateManager/MasterUpdate.cpp \
    FlowManager/CFlow.cpp \
    FlowManager/UpdateFlow.cpp \
    UpdateManager/DeviceUpdator.cpp \
    System/MessageDigest.c \
    UpdateManager/UpdateManager.cpp \
    FlowManager/FlowManagerProxy.cpp \
    Interface/Control/RelayControl.cpp \
    Interface/Control/WaterCollector.cpp \
    Interface/Current/CurrentLoop.cpp \
    Interface/Current/CurrentResultManager.cpp \
    Interface/Current/CurrentResultOutputer.cpp \
    Interface/CCEP/CCEPInterface.cpp \
    Interface/ExternalInterfaceProxy.cpp \
    Interface/Modbus/SerialModbus.cpp \
    Interface/Modbus/TCPModbus.cpp \
    Interface/Modbus/ModbusManager.cpp \
    System/Hash/md5.cpp \
    UI/Setting/ParameterWidget.cpp \
    ResultManager/MeasureRecordFile.cpp \
    ResultManager/CalibrateRecordFile.cpp \
    ResultManager/RecordFile.cpp \
    System/IO/LuipFile.cpp \
    UI/Frame/UpdateWidgetManager.cpp \
    Communication/DevCmdTable.cpp \
    Communication/DeviceInfo.cpp \
    Communication/InstrumentInfomation.cpp \
    Controller/MainController.cpp \
    Lua/App.cpp \
    Lua/LoadScript.cpp \
    Lua/AppProxy.cpp \
    Interface/Wqimc/Api/Agent/InstrumentAgent.cpp \
    Interface/Wqimc/Api/Agent/InstrumentAgent_constants.cpp \
    Interface/Wqimc/Api/Agent/InstrumentAgent_types.cpp \
    Interface/Wqimc/Api/Manager/InstrumentManager.cpp \
    Interface/Wqimc/Api/Manager/InstrumentManager_constants.cpp \
    Interface/Wqimc/Api/Manager/InstrumentManager_types.cpp \
    Interface/Wqimc/CloudConnectionService.cpp \
    Interface/Wqimc/InstrumentAgentHandler.cpp \
    Interface/Wqimc/DuplexThrift/CallFramedTransport.cpp \
    Interface/Wqimc/DuplexThrift/DuplexClientService.cpp \
    Interface/Wqimc/WqimcManager.cpp \
    ResultDetailManager/ResultDetailManager.cpp \
    ResultDetailManager/ResultDetailManagerProxy.cpp \
    UI/Setting/AutoMeasureWidget.cpp \
    UI/Frame/ResultIterm.cpp \
    Interface/Wqimc/ConfigVector.cpp \
    Interface/Wqimc/ReagentRemainVector.cpp \
    Interface/Wqimc/HttpClient.cpp \
    System/SetSystemTime.cpp \
    System/Uncompress.cpp \
    System/AuthorizationManager.cpp \
    UI/Frame/PushButton.cpp \
    ResultManager/OperateRecordData.cpp \
    Interface/Wqimc/HttpGetMethod.cpp \
    UI/Frame/RowHiddenHandler.cpp \
    System/DynamicPassword.cpp \
    System/ADC.cpp \
    UI/Maintain/CombineOpsManagerWidget.cpp \
    System/Translate.cpp \
    UI/System/SetSNDialog.cpp \
    PrinterManager/PrinterManager.cpp \
    PrinterManager/PrinterManagerProxy.cpp \
    UI/Data/MeasureDataPrintWidget.cpp \
    System/ADCManager.cpp \
    System/ADCDetecter.cpp \
    Interface/HJ212/HJ212Interface.cpp \
    UI/Frame/InputKeyBoard.cpp \
    UI/Frame/ReagentAuthorizationConfigDialog.cpp \
    UI/Frame/ReagentAuthorizationDialog.cpp \
    UI/Data/CurveParamWidget.cpp \
    outage.cpp \
    Interface/Current/currenttest.cpp \
    checkscreen.cpp \
    HJ212Manager/HJ212Manager.cpp \
    Interface/CCEP/CCEPTcpInterface.cpp \
    UI/Maintain/MaintenanceCombineWidget.cpp \
    UI/Singal/PeakShapeWidget.cpp \
    UI/Maintain/AutoTempCalibrateDialog.cpp \
    UI/Maintain/ExAutoTempCalibrateDialog.cpp \
    UI/System/DebugModeDialog.cpp \
    UI/Data/LogPrintWidget.cpp \
    UI/Data/CalibrateDataPrintWidget.cpp \
    UI/Frame/MethodTitleWidget.cpp \
    UI/Frame/MethodViewWidget.cpp \
    UI/Frame/MethodWidget.cpp \
    UI/Frame/MethodWindow.cpp \
    UI/Frame/MethodDetailWidget.cpp \
    UI/Data/MeasureDataDetailWidget.cpp \
    UI/UserManager/AccessLevelWidget.cpp \
    UI/UserManager/UserEditWidget.cpp \
    UI/UserManager/UserInfoWidget.cpp \
    UI/UserManager/UserManagerTitleWidget.cpp \
    UI/UserManager/UserManagerWindow.cpp \
    UI/UserManager/LevelEditWidget.cpp \
    UI/Frame/ProLicenseDialog.cpp \
    System/ProLicenseManager.cpp \
    UI/Frame/MultiSelectComboBox.cpp \
    UI/Frame/SignatureDialog.cpp

HEADERS  += \
    ConnectionManager/Client.h \
    ConnectionManager/Server.h \
    System/Exception.h \
    System/NFSManager.h \
    System/Types.h \
    System/Uuid.h \
    Communication/CommunicationManager.h \
    Controller/ControllerManager.h \
    Log.h \
    Setting/Environment.h \
    Setting/SettingManager.h \
    Setting/SettingManagerProxy.h \
    ResultManager/IResultNotifiable.h \
    ResultManager/RecordFields.h \
    ResultManager/ResultManager.h \
    ResultManager/ResultManagerProxy.h \
    System/CopyFile.h \
    System/FileDetecter.h \
    AlarmManager/Alarm.h \
    AlarmManager/AlarmInfoFile.h \
    AlarmManager/AlarmManager.h \
    AlarmManager/AlarmManagerProxy.h \
    AlarmManager/IAlarmNotifiable.h \
    StatusManager/IStatusNotifiable.h \
    StatusManager/RunAction.h \
    StatusManager/RunStatus.h \
    StatusManager/StatusManager.h \
    StatusManager/StatusManagerProxy.h \
    System/IO/Exception.h \
    UI/Data/MeasureData.h \
    UI/Frame/NumberKeyboard.h \
    UI/UserChangeManager/IUserChangeNotifiable.h \
    UI/UserChangeManager/UserChangeManager.h \
    UI/Frame/MQtableWidget.h \
    UI/Frame/QMyEdit.h \
    UI/Frame/Common.h \
    UI/Frame/MessageDialog.h \
    UI/Data/CalibrateCurve.h \
    UI/Data/TextfileParser.h \
    UI/Data/WarnMangerWidget.h \
    UI/Data/RunLogWidget.h \
    Controller/ControllerManagerProxy.h \
    UI/Frame/LoginDialog.h \
    UI/Frame/DropShadowWidget.h \
    UI/Frame/TitleWidget.h \
    UI/Frame/ToolButton.h \
    System/LockScreen/ILockScreenNotifiable.h \
    UI/Frame/ChangePasswordDialog.h \
    UI/Frame/BottomWidget.h \
    UI/Frame/MeasureProgress.h \
    UI/Frame/QclickLabel.h \
    UI/Home/RunStatusWindow.h \
    UI/Frame/MeasureResultPanel.h \
    UI/Data/MeasureDataWindow.h \
    UI/Frame/MainWindow.h \
    UI/Singal/SignalWidget.h \
    UI/Singal/SingalWindow.h \
    SignalManager/SignalManager.h \
    SignalManager/SignalManagerProxy.h \
    UI/Setting/MachineParamWindow.h \
    UI/UIProxy.h \
    UI/Maintain/RemoteMaintainWindow.h \
    UI/Maintain/ReagentManagerWidget.h \
    UI/Maintain/MaintenanceWidget.h \
    UI/Maintain/UseResourceWidget.h \
    UI/Maintain/PipeManngerWidget.h \
    UI/Maintain/HardwareCheckWidget.h \
    UI/System/SystemInfoWindow.h \
    UI/System/InstrumentInformationWidget.h \
    UI/System/DeviceInformation.h \
    UI/System/NetSet.h \
    UI/System/SystemTime.h \
    UI/System/FactoryTime.h \
    UI/System/ScreenCalibrate.h \
    System/Watchdogclient.h \
    System/LockScreen/BanklightControl.h \
    System/LockScreen/Buzzer.h \
    System/LockScreen/ScreenEventFilter.h \
    System/LockScreen/ScreenProtection.h \
    UI/Data/WaveData.h \
    FlowManager/FlowManager.h \
    FlowManager/IFlowNotifiable.h \
    FlowManager/MeasureScheduler.h \
    FlowManager/IScheduleNotifiable.h \
    UI/Maintain/IntellectCheckWidget.h \
    UI/Maintain/CommunicationTestWidget.h \
    UI/Maintain/HardwareTestWidget.h \
    UI/Maintain/UpdateWidget.h \
    UpdateManager/ProgramFileParser.h \
    UpdateManager/ProgramSegment.h \
    UpdateManager/MasterUpdate.h \
    SignalManager/IUpdateSignalNotifiable.h \
    FlowManager/CFlow.h \
    FlowManager/UpdateFlow.h \
    UpdateManager/IUpdateProgress.h \
    UpdateManager/DeviceUpdator.h \
    System/MessageDigest.h \
    UpdateManager/UpdateManager.h \
    FlowManager/FlowManagerProxy.h \
    Interface/Control/RelayControl.h \
    Interface/Control/WaterCollector.h \
    Interface/Current/CurrentLoop.h \
    Interface/Current/CurrentResultManager.h \
    Interface/Current/CurrentResultOutputer.h \
    Interface/CCEP/CCEPInterface.h \
    Interface/ExternalInterfaceProxy.h \
    Interface/Modbus/SerialModbus.h \
    Interface/Modbus/TCPModbus.h \
    Interface/Modbus/ModbusManager.h \
    Interface/Modbus/IFunCodeNotifiable.h \
    System/Hash/md5.h \
    UI/Setting/ParameterWidget.h \
    ResultManager/MeasureRecordFile.h \
    ResultManager/CalibrateRecordFile.h \
    ResultManager/RecordFile.h \
    System/IO/LuipFile.h \
    UI/Frame/UpdateWidgetManager.h \
    UI/Frame/IUpdateWidgetNotifiable.h \
    Communication/DevCmdTable.h \
    Communication/DeviceInfo.h \
    Communication/InstrumentInfomation.h \
    Controller/MainController.h \
    Lua/App.h \
    Lua/LoadScript.h \
    Lua/AppProxy.h \
    Interface/Wqimc/Api/Agent/InstrumentAgent.h \
    Interface/Wqimc/Api/Agent/InstrumentAgent_constants.h \
    Interface/Wqimc/Api/Agent/InstrumentAgent_types.h \
    Interface/Wqimc/Api/Manager/InstrumentManager.h \
    Interface/Wqimc/Api/Manager/InstrumentManager_constants.h \
    Interface/Wqimc/Api/Manager/InstrumentManager_types.h \
    Interface/Wqimc/CloudConnectionService.h \
    Interface/Wqimc/InstrumentAgentHandler.h \
    Interface/Wqimc/DuplexThrift/CallFramedTransport.h \
    Interface/Wqimc/DuplexThrift/DuplexClientService.h \
    Interface/Wqimc/DuplexThrift/FrameType.h \
    Interface/Wqimc/WqimcManager.h \
    ResultDetailManager/ResultDetailManager.h \
    ResultDetailManager/ResultDetailManagerProxy.h \
    UI/Setting/AutoMeasureWidget.h \
    UI/Frame/ResultIterm.h \
    Interface/Wqimc/ConfigVector.h \
    Interface/Wqimc/ReagentRemainVector.h \
    Interface/Wqimc/HttpClient.h \
    Interface/Wqimc/IWqimcStatusNotifiable.h \
    System/SetSystemTime.h \
    System/Uncompress.h \
    System/PlatformInfo.h \
    System/AuthorizationManager.h \
    UI/Frame/PushButton.h \
    ResultManager/OperateRecordData.h \
    Interface/Wqimc/HttpGetMethod.h \
    UI/Frame/RowHiddenHandler.h \
    System/DynamicPassword.h \
    System/ADC.h \
    UI/Maintain/CombineOpsManagerWidget.h \
    Common.h \
    System/Translate.h \
    UI/System/SetSNDialog.h \
    PrinterManager/PrinterManager.h \
    PrinterManager/PrinterManagerProxy.h \
    UI/Data/MeasureDataPrintWidget.h \
    System/ADCManager.h \
    System/ADCDetecter.h \
    Interface/HJ212/HJ212Interface.h \
    UI/Frame/InputKeyBoard.h \
    UI/Frame/ReagentAuthorizationConfigDialog.h \
    UI/Frame/ReagentAuthorizationDialog.h \
    UI/Data/CurveParamWidget.h \
    outage.h \
    Interface/Current/currenttest.h \
    UI/ControlConfig.h \
    checkscreen.h \
    HJ212Manager/HJ212Manager.h \
    Interface/CCEP/CCEPTcpInterface.h \
    UI/Maintain/MaintenanceCombineWidget.h \
    UI/Singal/PeakShapeWidget.h \
    UI/Maintain/AutoTempCalibrateDialog.h \
    UI/Maintain/ExAutoTempCalibrateDialog.h \
    UI/System/DebugModeDialog.h \
    UI/Data/LogPrintWidget.h \
    UI/Data/CalibrateDataPrintWidget.h \
    UI/Frame/MethodTitleWidget.h \
    UI/Frame/MethodViewWidget.h \
    UI/Frame/MethodWidget.h \
    UI/Frame/MethodWindow.h \
    UI/Frame/MethodDetailWidget.h \
    UI/Data/MeasureDataDetailWidget.h \
    UI/UserManager/AccessLevelWidget.h \
    UI/UserManager/LevelEditWidget.h \
    UI/UserManager/UserEditWidget.h \
    UI/UserManager/UserInfoWidget.h \
    UI/UserManager/UserManagerTitleWidget.h \
    UI/UserManager/UserManagerWindow.h \
    UI/Frame/ProLicenseDialog.h \
    System/ProLicenseManager.h \
    UI/Frame/MultiSelectComboBox.h \
    UI/Frame/SignatureDialog.h

RESOURCES += \
    UI/Resources/wqimc.qrc

DISTFILES += \
    Readme.txt \
    UI/Resources/img/checkBox.png
