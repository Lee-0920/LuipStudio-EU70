#include "UI/Frame/LoginDialog.h"
#include "UI/Setting/ParameterWidget.h"
#include "UIProxy.h"
#include <QColor>
#include "UI/Frame/IUpdateWidgetNotifiable.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "UI/Frame/MethodViewWidget.h"

using namespace OOLUA;
using namespace std;
using namespace UI;
using Qt::GlobalColor;

OOLUA_PROXY(,RoleType)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(None)
        OOLUA_ENUM(General)
        OOLUA_ENUM(Maintain)
        OOLUA_ENUM(Administrator)
        OOLUA_ENUM(Super)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(RoleType)

OOLUA_PROXY(,DataType)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(Bool)
        OOLUA_ENUM(Option)
        OOLUA_ENUM(Int)
        OOLUA_ENUM(Float)
        OOLUA_ENUM(IP)
        OOLUA_ENUM(IntArray)
        OOLUA_ENUM(Double)
        OOLUA_ENUM(String)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(DataType)

OOLUA_PROXY(,GlobalColor)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(black)
        OOLUA_ENUM(white)
        OOLUA_ENUM(darkGray)
        OOLUA_ENUM(gray)
        OOLUA_ENUM(lightGray)
        OOLUA_ENUM(red)
        OOLUA_ENUM(green)
        OOLUA_ENUM(blue)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(GlobalColor)

OOLUA_PROXY(,UpdateEvent)
    OOLUA_TAGS(
        Register_class_enums
    )
    OOLUA_ENUMS(
        OOLUA_ENUM(ChangeConfigParam)
        OOLUA_ENUM(ChangeInterconnectionParam)
        OOLUA_ENUM(ClearMeasureData)
        OOLUA_ENUM(ChangeMeasureParam)
        OOLUA_ENUM(ChangeCalibrateCurve)
        OOLUA_ENUM(NewCalibrateCurve)
        OOLUA_ENUM(UpdatePumpFactor)
        OOLUA_ENUM(ChangeSystemParam)
        OOLUA_ENUM(ChangeMeaModeOnHome)
        OOLUA_ENUM(ModbusChangeParam)
        OOLUA_ENUM(ChangeAutoMeasure)
        OOLUA_ENUM(WqimcChangeParam)
        OOLUA_ENUM(AutoPumpCheck)
        OOLUA_ENUM(FatalErrorOccurrence)
        OOLUA_ENUM(PrintNewMeasureData)
        OOLUA_ENUM(ShowNewProformaData)
        OOLUA_ENUM(PeakStatusChanged)
        OOLUA_ENUM(BaseLineStatusChanged)
        OOLUA_ENUM(MulCalibrationTip)
    )
OOLUA_PROXY_END
OOLUA_EXPORT_NO_FUNCTIONS(UpdateEvent)

OOLUA_PROXY(,UpdateWidgetManager)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_SFUNC(Instance)
    OOLUA_MFUNC(Update)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(UpdateWidgetManager, Update)
OOLUA_EXPORT_FUNCTIONS_CONST(UpdateWidgetManager)

OOLUA_PROXY(,MethodViewWidget)
    OOLUA_TAGS(
        No_public_constructors
    )
    OOLUA_SFUNC(Instance)
    OOLUA_MFUNC(MethodSaveForModbus)
    OOLUA_MFUNC(MethodApplyForModbus)
    OOLUA_MFUNC(MethodDelectForModbus)

OOLUA_PROXY_END
OOLUA_EXPORT_FUNCTIONS(MethodViewWidget, MethodSaveForModbus, MethodApplyForModbus, MethodDelectForModbus)
OOLUA_EXPORT_FUNCTIONS_CONST(MethodViewWidget)

namespace UI
{
Script* UIProxy::m_lua;

void UIProxy::Proxy(Script * lua)
{
    m_lua = lua;
    m_lua->register_class<RoleType>();
    m_lua->register_class<DataType>();
    m_lua->register_class<GlobalColor>();
    m_lua->register_class<UpdateEvent>();
    m_lua->register_class<UpdateWidgetManager>();
    m_lua->register_class_static<UpdateWidgetManager>("Instance",
             &OOLUA::Proxy_class<UpdateWidgetManager>::Instance);
    m_lua->register_class<MethodViewWidget>();
    m_lua->register_class_static<MethodViewWidget>("Instance",
             &OOLUA::Proxy_class<MethodViewWidget>::Instance);
}
}
