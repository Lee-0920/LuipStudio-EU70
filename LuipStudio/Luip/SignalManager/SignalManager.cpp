#include <vector>
#include "Log.h"
#include "oolua.h"
#include "ControllerPlugin/BaseController.h"
#include "Controller/ControllerManager.h"
#include "LuaEngine/LuaEngine.h"
#include "SignalManager.h"
#include "float.h"

using namespace std;
using namespace OOLUA;
using namespace Controller;
using namespace Lua;

std::unique_ptr<SignalManager> SignalManager::m_instance(nullptr);

SignalManager::SignalManager()
{

}

SignalManager::~SignalManager()
{

}

SignalManager *SignalManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new SignalManager);
    }

    return m_instance.get();

}

void SignalManager::Register(IUpdateSignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void SignalManager::UpdateSignal(void)
{
    for(std::list<IUpdateSignalNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnUpdateSignal();
        }
    }
}

void SignalManager::Init()
{
    ControllerManager::Instance()->RegisterSignalNotifiable(this);
}

void SignalManager::OnSignalChanged(System::String name, double value)
{
    QWriteLocker locker(&m_signalMapLock);

    std::map<String, double>::iterator it = m_signalMap.find(name);
    if(it != m_signalMap.end())
    {
        m_signalMap[name] = value;
    }
    else
    {
        m_signalMap.insert(make_pair(name, value));
    }

    QReadLocker locker2(&m_focusMapMapLock);

    std::map<String, bool>::iterator i = m_focusMap.find(name);
    if(i != m_focusMap.end() && i->second == true)
    {
        logger->debug("%s = %f", name.c_str(), value);
    }
}

double SignalManager::GetSignal(String name)
{
    QReadLocker locker(&m_signalMapLock);

    double ret = DBL_MIN;
    std::map<String, double>::iterator it = m_signalMap.find(name);
    if(it != m_signalMap.end())
    {
       ret = m_signalMap[name];
    }
    return ret;
}

std::map<String, double> SignalManager::GetAllSignal(void)
{
    QReadLocker locker(&m_signalMapLock);

    return m_signalMap;
}

void SignalManager::FocusSignal(String name, bool isFocus)
{
    QWriteLocker locker(&m_focusMapMapLock);

    std::map<String, bool>::iterator it = m_focusMap.find(name);
    if(it != m_focusMap.end())
    {
        m_focusMap[name] = isFocus;
    }
    else
    {
        m_focusMap.insert(make_pair(name, isFocus));
    }
}
