/**
 * @file
 * @brief 驱动控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#include <QTime>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include "Log.h"
#include "TN67DriveController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/OpticalAcquireInterface.h"
#include "API/Code/PeristalticPumpInterface.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "LuaEngine/LuaEngine.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace OOLUA;
using namespace Lua;

namespace Controller
{

/**
 * @brief 驱动控制器构造。
 */
TN67DriveController::TN67DriveController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new PeristalticPumpInterface(addr)),
    ISolenoidValve(new SolenoidValveInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr)),
    ITemperatureControl(new TemperatureControlInterface(addr)),
    IOpticalAcquire(new OpticalAcquireInterface(addr)),
    m_isEnable(false),
    m_isBoxFanRunning(false),
    m_insideEnvironmentTemp(0),
    m_environmentTemp(0),
    m_weepingDetectEnable(false)
{
    memset(m_thermostatTempArray, 0, sizeof(m_thermostatTempArray));

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "setting.temperature.insideEnvironmentTemp", m_insideEnvironmentTemp);

        luaEngine->GetLuaValue(state, "config.system.adcDetect[1].enable", m_weepingDetectEnable);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("TN67DriveController::TN67DriveController() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("TN67DriveController::TN67DriveController() => %s", e.what());
    }

    connect(this, SIGNAL(BoxFanControlSignal(float)), this, SLOT(BoxFanControlSlot(float)));
}

TN67DriveController::~TN67DriveController()
{
    if (IPeristalticPump)
        delete IPeristalticPump;
    if (ISolenoidValve)
        delete ISolenoidValve;
    if (IOpticalMeter)
        delete IOpticalMeter;
    if (ITemperatureControl)
        delete ITemperatureControl;
    if (IOpticalAcquire)
        delete IOpticalAcquire;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool TN67DriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    IOpticalAcquire->RegisterAcquireADNotice(this);
    IOpticalAcquire->RegisterAcquireCheckLeaking(this);
    return true;
}

bool TN67DriveController::Uninit()
{
    return true;
}

void TN67DriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *TN67DriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *TN67DriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *TN67DriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

TemperatureControlInterface *TN67DriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

OpticalAcquireInterface *TN67DriveController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

float TN67DriveController::GetReportThermostatTemp(int index) const
{
    float temp = 0;

    if(index >= 0 && index <= 2)
    {
        temp = m_thermostatTempArray[index];
    }

    return temp;
}

float TN67DriveController::GetReportEnvironmentTemp() const
{
    return m_environmentTemp;
}

void TN67DriveController::OnReceive(DscpEventPtr event)
{
    switch (event->code)
    {
        case DSCP_EVENT_OMI_OPTICAL_AD_NOTICE:      //光学定量信号AD上报
        {
            Uint8 num = *(event->data);
            unsigned long *adValue = new unsigned long[num];

            for(Uint8 i = 0; i < num; i++)
            {
                adValue[i] = *((unsigned long*)(event->data+1 + i*4));
            }

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    for(Uint8 i = 0; i < num; i++)
                    {
                        String str = "mp" + std::to_string(i + 1) + "AD";
                        (*it)->OnSignalChanged(str, adValue[i]);
                    }
                }
            }

            if (adValue)
            {
              delete[] adValue;
            }
        }
            break;

        case DSCP_EVENT_TCI_TEMPERATURE_NOTICE:     //温度上报事件
        {
            m_thermostatTempArray[0] = *((float*)(event->data));
            m_environmentTemp = *((float*)(event->data+4));
            m_thermostatTempArray[1] = *((float*)(event->data+8));

            emit BoxFanControlSignal(m_environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("eTemp", m_environmentTemp);
                    (*it)->OnSignalChanged("tTemp1", m_thermostatTempArray[0]);
                    (*it)->OnSignalChanged("tTemp2", m_thermostatTempArray[1]);
                }
            }
        }
            break;

        case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
        {
            double ref1 = 0;
            double mea1 = 0;
            double abs1 = 0;

            if(event->len >= 8)
            {
                ref1 = *((unsigned long*)(event->data));
                mea1 = *((unsigned long*)(event->data + 4));
            }

            if (0 < ref1 && 0 < mea1)
            {
                abs1 = 1000 * log10(ref1/mea1);
            }

            double ref2 = 0;
            double mea2 = 0;
            double abs2 = 0;

            if(event->len >= 16)
            {
                ref2 = *((unsigned long*)(event->data + 8));
                mea2 = *((unsigned long*)(event->data + 12));
            }

            if (0 < ref2 && 0 < mea2)
            {
                abs2 = 1000 * log10(ref2/mea2);
            }

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("refAD1", ref1);
                    (*it)->OnSignalChanged("meaAD1", mea1);
                    (*it)->OnSignalChanged("abs1", abs1);
                    (*it)->OnSignalChanged("refAD2", ref2);
                    (*it)->OnSignalChanged("meaAD2", mea2);
                    (*it)->OnSignalChanged("abs2", abs2);
                }
            }
        }
            break;

    case DSCP_EVENT_DSI_CHECK_LEAKING_NOTICE:   // 漏液检测AD定时上报事件
    {
        if (m_weepingDetectEnable)
        {
            Uint16 checkLeakingValve = *((Uint16*)(event->data));
            Script *lua = LuaEngine::Instance()->GetEngine();
            Table table, itermsTable;
            itermsTable.bind_script(*lua);
            itermsTable.set_table("setting");
            itermsTable.at("ui", table);
            table.at("runStatus", itermsTable);

            Lua_func_ref func;
            if (itermsTable.safe_at("WeepingDetectHandle", func))
            {
                lua->call(func,checkLeakingValve);
            }
        }
    }
        break;

        default:
            break;
    }
}

void TN67DriveController::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        //重置光学定量AD上传周期
        try
        {

            this->IOpticalMeter->SetOpticalADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.what());
        }

        //重置信号AD上传周期
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.what());
        }
    }
}

void TN67DriveController::SetBoxFanEnable(bool enable)
{
    m_isEnable = enable;
    if(enable)
    {
        m_isBoxFanRunning = false;
    }
}

void TN67DriveController::BoxFanControlSlot(float temp)
{
   if (m_isEnable && m_insideEnvironmentTemp > 0.1)
   {
        if (temp - m_insideEnvironmentTemp > 0.1 && m_isBoxFanRunning == false)
        {
           ITemperatureControl->BoxFanSetOutput(1); //打开箱风扇
           m_isBoxFanRunning = true;
    //       logger->debug("打开机箱风扇");
        }
        else if (m_insideEnvironmentTemp - temp > 2 && m_isBoxFanRunning == true)
        {
            ITemperatureControl->BoxFanSetOutput(0); //关机箱风扇
           m_isBoxFanRunning = false;
    //       logger->debug("关闭机箱风扇");
        }
    }
}

bool TN67DriveController::ReInitInstrument()
{
    bool ret = true;

    lua_State * newState = LuaEngine::Instance()->GetThreadState();

    OOLUA::Lua_function call(newState);
    OOLUA::Lua_func_ref initInstrument;

    try
    {
        if(OOLUA::get_global(newState, "InitInstrument", initInstrument))
        {
            call(initInstrument);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("TN67DriveController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    catch(std::exception e)
    {
        logger->warn("TN67DriveController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    return ret;
}

void TN67DriveController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        //重置光学定量AD上传周期
        try
        {

            this->IOpticalMeter->SetOpticalADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TN67DriveController::ResetHandler() => %s", e.what());
        }

        //重置电极AD上传周期
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("DriveController::ResetHandler() => %s", e.what());
        }
    }
}



void TN67DriveController::ClearThermostatRemainEvent()
{
    logger->debug("{ClearThermostatRemainEvent}");
    try
    {
        DscpAddress addr = ITemperatureControl->GetAddress();
        for (int i = 0; i < 10; i++)
        {
            EventHandler::Instance()->Expect(addr,DSCP_EVENT_TCI_THERMOSTAT_RESULT, 200);
        }
    }
    catch(CommandTimeoutException e)
    {
    }
    catch(ExpectEventTimeoutException e)
    {
    }
    catch (std::exception e)
    {

    }
}

void TN67DriveController::ClearPumpRemainEvent()
{
    logger->debug("{ClearPumpRemainEvent}");
    try
    {
        DscpAddress addr = IPeristalticPump->GetAddress();
        for (int i = 0; i < 10; i++)
        {
            EventHandler::Instance()->Expect(addr,DSCP_EVENT_PPI_PUMP_RESULT, 200);
        }
    }
    catch(CommandTimeoutException e)
    {
    }
    catch(ExpectEventTimeoutException e)
    {
    }
    catch (std::exception e)
    {

    }
}

void TN67DriveController::ClearAllRemainEvent()
{
    logger->debug("{ClearAllRemainEvent}");
    EventHandler::Instance()->EmptyEvents();    //清空事件池
}

}
