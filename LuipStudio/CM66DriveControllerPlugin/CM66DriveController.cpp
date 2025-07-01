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
#include "CM66DriveController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/TitrationOperateInterface.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "LuaEngine/LuaEngine.h"

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
CM66DriveController::CM66DriveController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new PeristalticPumpInterface(addr)),
    ISolenoidValve(new SolenoidValveInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr)),
    ITemperatureControl(new TemperatureControlInterface(addr)),
    ITitrationoperate(new TitrationOperateInterface(addr)),
    m_thermostatTemp(0),m_environmentTemp(0),m_sendTempCount(0)
{
    connect(this, SIGNAL(TempToLuaSignal(float, float)), this, SLOT(TempToLuaSlot(float, float)));
}

CM66DriveController::~CM66DriveController()
{
    if (IPeristalticPump)
        delete IPeristalticPump;
    if (ISolenoidValve)
        delete ISolenoidValve;
    if (IOpticalMeter)
        delete IOpticalMeter;
    if (ITemperatureControl)
        delete ITemperatureControl;
    if (ITitrationoperate)
        delete ITitrationoperate;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool CM66DriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    ITitrationoperate->RegisterAcquireElectrodeADNotice(this);
    return true;
}

bool CM66DriveController::Uninit()
{
    return true;
}

void CM66DriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *CM66DriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *CM66DriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *CM66DriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

TemperatureControlInterface *CM66DriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

TitrationOperateInterface *CM66DriveController::GetITitrationOperate()
{
    return ITitrationoperate;
}

float CM66DriveController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float CM66DriveController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

Temperature CM66DriveController::GetCurrentTemperature()
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

void CM66DriveController::OnReceive(DscpEventPtr event)
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
            m_temperature.thermostatTemp = *((float*)event->data);
            m_temperature.environmentTemp = *((float*)(event->data+4));

            m_thermostatTemp = m_temperature.thermostatTemp;
            m_environmentTemp = m_temperature.environmentTemp;
            emit TempToLuaSignal(m_thermostatTemp, m_environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("tTemp", m_temperature.thermostatTemp);
                    (*it)->OnSignalChanged("eTemp", m_temperature.environmentTemp);
                }
            }
        }
            break;

        case DSCP_EVENT_TOI_ELECTRODE_SIGNAL_NOTICE:   // 信号AD定时上报事件。
        {
            unsigned long value = *((unsigned long*)(event->data));

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                     (*it)->OnSignalChanged("elcAD", value);
                }
            }
        }
            break;

        default:
            break;
    }
}

void CM66DriveController::StopSignalUpload()
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
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }

        //重置信号AD上传周期
        try
        {
            this->ITitrationoperate->SetAcquireElectrodeADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }
    }
}

bool CM66DriveController::ReInitInstrument()
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
        logger->warn("CM66DriveController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    catch(std::exception e)
    {
        logger->warn("CM66DriveController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    return ret;
}

void CM66DriveController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        this->ReInitInstrument();
    }

    if (this->GetConnectStatus())
    {
        //重置光学定量AD上传周期
        try
        {

            this->IOpticalMeter->SetOpticalADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }

        //重置电极AD上传周期
        try
        {
            this->ITitrationoperate->SetAcquireElectrodeADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("CM66DriveController::ResetHandler() => %s", e.what());
        }
    }
}

void CM66DriveController::ClearThermostatRemainEvent()
{
    try
    {
        DscpAddress addr = ITemperatureControl->GetAddress();
        for (int i = 0; i < 10; i++)
        {
            DscpEventPtr event = EventHandler::Instance()->Expect(addr,DSCP_EVENT_TCI_THERMOSTAT_RESULT, 200);

            if (event != nullptr)
            {
                Uint8 result = (int)(*event->data);
                float temp = *((float*)(event->data + 1));
                logger->debug("{ClearThermostatRemainEvent} result = %d，temp = %.1f", result, temp);
            }
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

void CM66DriveController::ClearAllRemainEvent()
{
    EventHandler::Instance()->EmptyEvents();    //清空事件池
}

void  CM66DriveController::TempToLuaSlot(float tTemp, float eTemp)
{
    m_sendTempCount++;

    if (m_sendTempCount >= 2)
    {
        m_sendTempCount = 0;

        lua_State * newState = LuaEngine::Instance()->GetThreadState();

        OOLUA::Lua_function call(newState);
        OOLUA::Lua_func_ref recvEevTemp;

        try
        {
            if(OOLUA::get_global(newState, "RecvEevTemp", recvEevTemp))
            {
                call(recvEevTemp, tTemp, eTemp);
            }
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("CM66DriveController::EnvTempToLuaSlot() => %s", e.what());
        }
        catch(std::exception e)
        {
            logger->warn("CM66DriveController::EnvTempToLuaSlot() => %s", e.what());
        }
    }
}

TitrationData CM66DriveController::GetPacketTitrationData(long timeout)
{
    TitrationData resultData = {0,0};
    TitrationData titrationData[60] = {0,0};
    Uint32 number = 0;

    try
    {
        number = ITitrationoperate->ExpectTitrationData(timeout, titrationData);
    }
    catch (ExpectEventTimeoutException e)       // 读初始AD值事件上报等待超时;
    {
        ITitrationoperate->StopTitration();     // 停止滴定;

        throw ExpectEventTimeoutException(e.m_addr, e.m_code);
    }

    Uint32 sumData = 0;
    Uint32 sumStep = 0;
    for(Uint32 i = 0; i < number; i++)
    {
        sumData += titrationData[i].electrodeAD;
        sumStep += titrationData[i].titrationStep;
    }

    Uint32 meanData = sumData/number;
    Uint32 meanStep = sumStep/number;

    resultData.electrodeAD = meanData;
    resultData.titrationStep = meanStep;
    //qDebug()<<"GetPacketTitrationData electrodeAD = "<<resultData.electrodeAD<<" titrationStep = "<<resultData.titrationStep;
    return resultData;
}

}
