/**
 * @file
 * @brief 控制网络。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */


#if !defined(CONTROLLER_MANAGER_H_)
#define CONTROLLER_MANAGER_H_

#include <memory>
#include <map>
#include <vector>
#include <QObject>
#include "oolua.h"
#include "System/Types.h"
#include "ControllerPlugin/ControllerPlugin.h"
#include "ControllerPlugin/BaseController.h"
#include "MainController.h"

using System::String;

namespace Controller
{

class ControllerManager: public QObject, public IEventReceivable
{
    Q_OBJECT
public:
    ~ControllerManager();
    static ControllerManager* Instance();
    void Init();
    void Uninit();
    void LoadPlugin();
    bool StopControllers();
    void AddController(String name, String text, BaseController* controller);
    BaseController* GetController(String name);
    void DeviceReset();
    void DeviceConnectCheck();
    bool IsAllConnected();
    void SetUpdateFlag(bool isUpdate);
    bool GetUpdateFlag();
    void SetParamSettingFlag(bool isSetting);
    bool GetParamSettingFlag();
    void SetInitOverFlag(bool state);
    bool GetInitOverFlag();
    void ControllerConnectStatusChanged(Uint8 pollingAddr, bool status);    
    std::map<String,BaseController*> *GetControllerMap(void);
    String GetControllerText(DscpAddress addr);
    void SetResetNotice();
    void StartSignalUpload(DscpEventPtr event);
    void StartAllSignalUpload();
    void StopSignalUpload(DscpAddress addr);
    void StopAllSignalUpload();

    void RegisterSignalNotifiable(ISignalNotifiable *handle);

    virtual void OnReceive(DscpEventPtr event);

    MainController *MC;

private:
    static std::unique_ptr<ControllerManager> m_instance;
    std::map<String, BaseController*> m_controllerMap;
    std::map<String, String> m_TextMap;
    ControllerPlugin *m_controllerPlugin;
    bool m_isUpdate;
    bool m_isParamSetting;
    bool m_isInitOver;

    ControllerManager();
};

}

#endif  //CONTROLLER_MANAGER_H_
