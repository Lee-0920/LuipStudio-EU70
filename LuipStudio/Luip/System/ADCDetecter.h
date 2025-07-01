#ifndef ADCDETECTER_H
#define ADCDETECTER_H

#include <memory>
#include <list>
#include <QThread>
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include "Setting/SettingManager.h"

using System::String;

namespace System
{
class ADCDetecter : public QThread
{
    Q_OBJECT
public:
    bool Init();

    bool Start();
    void Stop();

    friend class ADCManager;
protected:
    void run();

private:
    explicit ADCDetecter(int index);
    void Detect();

private:
    bool m_isDetected;
    String m_fileName;
    bool m_isRuning;

    int m_adcDetectIndex;

    int m_filterNum;
    int m_intervalTime;
    int m_waitTime;
    int m_limitAD;
    Configuration::CompareType m_compareType;
    OOLUA::Lua_func_ref m_detectHandleFunc;
};
}

#endif // ADCDETECTER_H
