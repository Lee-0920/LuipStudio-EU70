/**
 * @file
 * @brief 反应堆控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include "Log.h"
#include "Communication/EventHandler.h"
#include "OutputController.h"
#include "Communication/CommunicationException.h"
#include "API/Code/IOControlInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "LuaEngine/LuaEngine.h"
#include <QDebug>

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace OOLUA;
using namespace Lua;

namespace Controller
{

/**
 * @brief 单泵液路控制器构造。
 */
OutputController::OutputController(DscpAddress addr)
    : BaseController(addr),    
    ITemperatureControl(new OCTemperatureControlInterface(addr)),
    IOutputControl(new IOControlInterface(addr)),
    m_environmentTemp(0),m_sendEnvTempCount(0)
{
    connect(this, SIGNAL(EnvTempToLuaSignal(float)), this, SLOT(EnvTempToLuaSlot(float)));
}

OutputController::~OutputController()
{
    if (ITemperatureControl)
        delete ITemperatureControl;
    if (IOutputControl)
        delete IOutputControl;
}

/**
 * @brief 初始化 Controller 环境。
 */
bool OutputController::Init()
{
    ITemperatureControl->RegisterTemperatureNotice(this);
    IOutputControl->RegisterTriggerNotice(this);
    memset(&m_temperature, 0, sizeof(m_temperature));

    return true;
}

bool OutputController::Uninit()
{
    return true;
}

void OutputController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

OCTemperatureControlInterface *OutputController::GetITemperatureControl()
{
    return ITemperatureControl;
}

IOControlInterface *OutputController::GetIOutputControl()
{
    return IOutputControl;
}

float OutputController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float OutputController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

OCTemperature OutputController::GetCurrentTemperature()
{
    OCTemperature temp = {0, 0};

    try
    {
        temp = this->ITemperatureControl->GetTemperature();
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_temperature, sizeof(OCTemperature));
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
            memcpy(&temp, &m_temperature, sizeof(OCTemperature));
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

void OutputController::OnReceive(DscpEventPtr event)
{
    switch (event->code)
    {
        case DSCP_EVENT_TCI_TEMPERATURE_NOTICE:     //温度上报事件
        {
            m_temperature.environmentTemp = *((float*)event->data);

            m_environmentTemp = m_temperature.environmentTemp;
            emit EnvTempToLuaSignal(m_environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {                    
                    (*it)->OnSignalChanged("oTemp", m_temperature.environmentTemp);
                }
            }
        }
            break;

//        case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
//        {
//            double ref0 = *((unsigned short*)(event->data));
//            double mea0 = *((unsigned short*)(event->data + 2));
//            double mea = *((unsigned short*)(event->data + 4)); //相反的
//            double ref = *((unsigned short*)(event->data + 6));
//            double abs = 0;
//            if (0 < ref && 0 < mea)
//            {
//                abs = 1000 * log10(ref/mea);
//            }

//            if(!m_notifise.empty())
//            {
//                list<ISignalNotifiable*>::iterator it;
//                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
//                {
//                    (*it)->OnSignalChanged("refAD", ref);
//                    (*it)->OnSignalChanged("meaAD", mea);
//                    (*it)->OnSignalChanged("abs", abs);
//                }
//            }
//        }
//            break;
        case DSCP_EVENT_IOI_TRIGGER:
            qDebug("Trigger");
            break;
        default:
            break;
    }
}

void OutputController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {

        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.what());
        }
    }

    if (this->GetConnectStatus())
    {
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OutputController::StartSignalUpload() => %s", e.what());
        }
    }
}

void OutputController::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {

        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.what());
        }
    }

    if (this->GetConnectStatus())
    {
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OutputController::StopSignalUpload() => %s", e.what());
        }
    }
}

void  OutputController::EnvTempToLuaSlot(float temp)
{
//    m_sendEnvTempCount++;

//    if (m_sendEnvTempCount >= 60)
//    {
//        m_sendEnvTempCount = 0;

//        lua_State * newState = LuaEngine::Instance()->GetThreadState();

//        OOLUA::Lua_function call(newState);
//        OOLUA::Lua_func_ref recvEevTemp;

//        try
//        {
//            if(OOLUA::get_global(newState, "RecvEevTemp", recvEevTemp))
//            {
//                call(recvEevTemp, temp);
//            }
//        }
//        catch(OOLUA::Exception e)
//        {
//            logger->warn("OutputController::EnvTempToLuaSlot() => %s", e.what());
//        }
//        catch(std::exception e)
//        {
//            logger->warn("OutputController::EnvTempToLuaSlot() => %s", e.what());
//        }
//    }
}

}
