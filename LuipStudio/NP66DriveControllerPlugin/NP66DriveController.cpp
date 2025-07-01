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
#include "NP66DriveController.h"
#include "API/Code/NP66OpticalMeterInterface.h"
#include "API/Code/NP66TemperatureControlInterface.h"
#include "API/Code/NP66OpticalAcquireInterface.h"
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
NP66DriveController::NP66DriveController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new NP66PeristalticPumpInterface(addr)),
    ISolenoidValve(new NP66SolenoidValveInterface(addr)),
    IOpticalMeter(new NP66OpticalMeterInterface(addr)),
    ITemperatureControl(new NP66TemperatureControlInterface(addr)),
    IOpticalAcquire(new NP66OpticalAcquireInterface(addr)),
    m_isEnable(false),
    m_isBoxFanRunning(false),
    m_insideEnvironmentTemp(0)
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "setting.temperature.insideEnvironmentTemp", m_insideEnvironmentTemp);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("NP66DriveController::NP66DriveController() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("NP66DriveController::NP66DriveController() => %s", e.what());
    }

    connect(this, SIGNAL(BoxFanControlSignal(float)), this, SLOT(BoxFanControlSlot(float)));
}

NP66DriveController::~NP66DriveController()
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
bool NP66DriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    IOpticalAcquire->RegisterAcquireADNotice(this);
    return true;
}

bool NP66DriveController::Uninit()
{
    return true;
}

void NP66DriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

NP66PeristalticPumpInterface *NP66DriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

NP66SolenoidValveInterface *NP66DriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

NP66OpticalMeterInterface *NP66DriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

NP66TemperatureControlInterface *NP66DriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

NP66OpticalAcquireInterface *NP66DriveController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

float NP66DriveController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float NP66DriveController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

NP66Temperature NP66DriveController::GetCurrentTemperature()
{
    NP66Temperature temp = {0, 0};

    try
    {
        temp = this->ITemperatureControl->GetTemperature();
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_temperature, sizeof(NP66Temperature));
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
            memcpy(&temp, &m_temperature, sizeof(NP66Temperature));
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

void NP66DriveController::OnReceive(DscpEventPtr event)
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
                        String str = "NP66mp" + std::to_string(i + 1) + "AD";
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

            emit BoxFanControlSignal(m_temperature.environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("NP66tTemp", m_temperature.thermostatTemp);
                    (*it)->OnSignalChanged("NP66eTemp", m_temperature.environmentTemp);
                }
            }
        }
            break;

        case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
        {
            double ref = *((unsigned long*)(event->data));
            double mea = *((unsigned long*)(event->data + 4));
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
                    (*it)->OnSignalChanged("NP66refAD", ref);
                    (*it)->OnSignalChanged("NP66meaAD", mea);
                    (*it)->OnSignalChanged("NP66abs", abs);
                }
            }
        }
            break;

        default:
            break;
    }
}

void NP66DriveController::StopSignalUpload()
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

void NP66DriveController::SetBoxFanEnable(bool enable)
{
    m_isEnable = enable;
}

void NP66DriveController::BoxFanControlSlot(float temp)
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

void NP66DriveController::StartSignalUpload()
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
            logger->warn("NP66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("NP66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("NP66DriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("NP66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("NP66DriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("NP66DriveController::ResetHandler() => %s", e.what());
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

}
