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
#include "PT63DriveController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/PeristalticPumpInterface.h"
#include "API/Code/OpticalAcquireInterface.h"
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
PT63DriveController::PT63DriveController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new PeristalticPumpInterface(addr)),
    ISolenoidValve(new SolenoidValveInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr)),
    ITemperatureControl(new TemperatureControlInterface(addr)),
    IOpticalAcquire(new OpticalAcquireInterface(addr)),
    IExtTemperatureControl(new ExtTemperatureControlInterface(addr)),
    IExtOpticalAcquire(new ExtOpticalAcquireInterface(addr)),
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
        logger->warn("PT63DriveController::PT63DriveController() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("PT63DriveController::PT63DriveController() => %s", e.what());
    }

    connect(this, SIGNAL(BoxFanControlSignal(float)), this, SLOT(BoxFanControlSlot(float)));
}

PT63DriveController::~PT63DriveController()
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
    if (IExtTemperatureControl)
        delete IExtTemperatureControl;
    if (IExtOpticalAcquire)
        delete IExtOpticalAcquire;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool PT63DriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    IOpticalAcquire->RegisterAcquireADNotice(this);
    IOpticalAcquire->RegisterAcquireCheckLeaking(this);
    return true;
}

bool PT63DriveController::Uninit()
{
    return true;
}

void PT63DriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *PT63DriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *PT63DriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *PT63DriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

TemperatureControlInterface *PT63DriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

OpticalAcquireInterface *PT63DriveController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

ExtTemperatureControlInterface *PT63DriveController::GetIExtTemperatureControl()
{
    return IExtTemperatureControl;
}

ExtOpticalAcquireInterface *PT63DriveController::GetIExtOpticalAcquire()
{
    return IExtOpticalAcquire;
}

float PT63DriveController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float PT63DriveController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

float PT63DriveController::GetReportThermostatTemp(int index) const
{
    float temp = 0;

    if(index >= 0 && index < 2)
    {
        temp = m_thermostatTempArray[index];
    }

    return temp;
}

float PT63DriveController::GetReportEnvironmentTemp() const
{
    return m_environmentTemp;
}

Temperature PT63DriveController::GetCurrentTemperature()
{
    Temperature temp = {0, 0};

    try
    {
        temp = this->ITemperatureControl->GetTemperature();
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_temperature, sizeof(Temperature));
        }
        else
        {
            throw CommandTimeoutException(e.m_addr, e.m_code);
        }
    }
    catch (std::exception e)
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_temperature, sizeof(Temperature));
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

void PT63DriveController::OnReceive(DscpEventPtr event)
{
    switch (event->code)
    {
        case DSCP_EVENT_OMI_OPTICAL_AD_NOTICE:      //光学定量信号AD上报
        {
            Uint8 num = *(event->data);
            Uint32 adValue[num];

            for(Uint8 i = 0; i < num; i++)
            {
                adValue[i] = *((Uint32*)(event->data+1 + i*4));
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
        }
            break;

        case DSCP_EVENT_TCI_TEMPERATURE_NOTICE:     //温度上报事件
        {
            m_temperature.thermostatTemp = *((float*)event->data);
            m_temperature.environmentTemp = *((float*)(event->data+4));

            m_thermostatTempArray[0] = *((float*)(event->data));
            m_environmentTemp = *((float*)(event->data+4));
            if(event->len >= 12)
            {
                m_thermostatTempArray[1] = *((float*)(event->data+8));
            }

            emit BoxFanControlSignal(m_temperature.environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("tTemp", m_temperature.thermostatTemp);
                    (*it)->OnSignalChanged("eTemp", m_environmentTemp);
                    (*it)->OnSignalChanged("tTemp1", m_thermostatTempArray[0]);
                    (*it)->OnSignalChanged("tTemp2", m_thermostatTempArray[1]);
                }
            }
        }
            break;

        case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
        {
            double ref = *((Uint32*)(event->data));
            double mea = *((Uint32*)(event->data + 4));
            double abs = 0;
            if (0 < ref && 0 < mea)
            {
                abs = 1000 * log10(ref/mea);
            }

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("refAD", ref);
                    (*it)->OnSignalChanged("meaAD", mea);
                    (*it)->OnSignalChanged("abs", abs);
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

void PT63DriveController::StopSignalUpload()
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

void PT63DriveController::SetBoxFanEnable(bool enable)
{
    m_isEnable = enable;
    if(enable)
    {
        m_isBoxFanRunning = false;
    }
}

void PT63DriveController::BoxFanControlSlot(float temp)
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

void PT63DriveController::StartSignalUpload()
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
            logger->warn("PT63DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT63DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT63DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("PT63DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT63DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT63DriveController::ResetHandler() => %s", e.what());
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


void PT63DriveController::ClearThermostatRemainEvent()
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

void PT63DriveController::ClearPumpRemainEvent()
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

void PT63DriveController::ClearAllRemainEvent()
{
    logger->debug("{ClearAllRemainEvent}");
    EventHandler::Instance()->EmptyEvents();    //清空事件池
}


}
