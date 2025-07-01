#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <memory>
#include <QObject>
#include <QReadWriteLock>
#include <map>
#include "System/Types.h"
#include "ControllerPlugin/ISignalNotifiable.h"
#include "IUpdateSignalNotifiable.h"

using Controller::ISignalNotifiable;

class SignalManager : public ISignalNotifiable
{
public:
    ~SignalManager();

    static SignalManager* Instance();
    void Register(IUpdateSignalNotifiable *handle);
    void Init();
    void OnSignalChanged(String name, double value);
    double GetSignal(String name);
    void UpdateSignal(void);
    std::map<String, double> GetAllSignal(void);
    void FocusSignal(String name, bool isFocus);

private:
    std::list<IUpdateSignalNotifiable*> m_notifise;
    static std::unique_ptr<SignalManager> m_instance;
    std::map<String, double> m_signalMap;
    std::map<String, bool> m_focusMap;
    SignalManager();

    QReadWriteLock m_signalMapLock;
    QReadWriteLock m_focusMapMapLock;
};

#endif // SIGNALMANAGER_H
