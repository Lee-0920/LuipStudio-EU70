#if !defined(CONTROLLER_MAINCONTROLLER_H_)
#define CONTROLLER_MAINCONTROLLER_H_

#include <QObject>
#include "ControllerPlugin/API/InstrumentInfoInterface.h"
#include "ControllerPlugin/BaseController.h"

namespace Controller
{
/**
 * @brief 主控器。
 * @details
 */
class MainController: public QObject, public BaseController
{
Q_OBJECT

public:
    MainController(DscpAddress addr);
    virtual ~MainController();

    // ---------------------------------------
    // Controller 接口
    bool Init();
    void Stop();
    // ---------------------------------------
    // IEventReceivable 接口
    virtual void OnReceive(DscpEventPtr event);
public:
    // 设备接口集
     Controller::API::InstrumentInfoInterface * const IInstrumentInfo;
};
}

#endif // CONTROLNET_MAINCONTROLLER_H_
