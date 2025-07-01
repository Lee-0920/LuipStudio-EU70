#if !defined(UI_FRAME_IUPDATEWIDGETNOTIFIABLE_H)
#define UI_FRAME_IUPDATEWIDGETNOTIFIABLE_H

#include "System/Types.h"

namespace UI
{

enum class UpdateEvent
{
    ChangeConfigParam = 0,
    ChangeInterconnectionParam = 1,
    ClearMeasureData = 2,
    ChangeMeasureParam = 3,
    ChangeCalibrateCurve = 4,
    NewCalibrateCurve = 5,
    UpdatePumpFactor = 6,
    ChangeSystemParam = 7,
    ChangeMeaModeOnHome = 8,    // 刷新主界面的测量模式
    ModbusChangeParam = 9,      // 刷新Modbus修改参数界面
    ChangeAutoMeasure = 10,
    WqimcChangeParam = 11,
    AutoPumpCheck = 12,
    StartHardwareTestSuccess = 13,
    StartIntellectCheck = 14,
    StartCommunicationTest = 15,
    StopHardwareTest = 16,
    OneKeyClearData = 17,
    RecoverInFactoryMode = 18,
    ActivateOutFactoryMode = 19,
    FatalErrorOccurrence = 20,  //严重故障
    FatalErrorClear = 21,  //严重故障清除
    FaultClear = 22,  //故障清除
    SystemInfoChanged = 23,
    PrintNewMeasureData = 24,
    ShowNewProformaData = 25,
    PeakStatusChanged = 26,
    BaseLineStatusChanged = 27,
    MulCalibrationTip = 28,//多点校准操作提示
};

/**
 * @brief 窗口更新接口。
 * @details 观察者。
 */
class IUpdateWidgetNotifiable
{
public:
    IUpdateWidgetNotifiable() {}
    virtual ~IUpdateWidgetNotifiable() {}

public:
    virtual void OnUpdateWidget(UpdateEvent event, System::String message) = 0;
};

}
#endif // UI_FRAME_IUPDATEWIDGETNOTIFIABLE_H
