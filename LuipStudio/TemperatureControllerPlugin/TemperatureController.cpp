/**
 * @file
 * @brief 温度控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include <QDebug>
#include "Log.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "TemperatureController.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

/**
 * @brief 温度控制器构造。
 */
TemperatureController::TemperatureController(DscpAddress addr)
    : BaseController(addr),
    ITemperatureControl(new TemperatureControlInterface(addr))
{

}


TemperatureController::~TemperatureController()
{
    if (ITemperatureControl)
        delete ITemperatureControl;
}

/**
 * @brief 初始化 Controller 环境。
 */
bool TemperatureController::Init()
{
    ITemperatureControl->RegisterTemperatureNotice(this);

    memset(&m_temperature, 0, sizeof(m_temperature));

    return true;
}

bool TemperatureController::Uninit()
{
    return true;
}

float TemperatureController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float TemperatureController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

Temperature TemperatureController::GetCurrentTemperature()
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

void TemperatureController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

TemperatureControlInterface *TemperatureController::GetITemperatureControl()
{
    return ITemperatureControl;
}

void TemperatureController::OnReceive(DscpEventPtr event)
{
    m_temperature.thermostatTemp = *((float*)event->data);
    m_temperature.environmentTemp = *((float*)(event->data+4));

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

void TemperatureController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TemperatureController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TemperatureController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TemperatureController::StartSignalUpload() => %s", e.what());
        }
    }
}

void TemperatureController::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->ITemperatureControl->SetTemperatureNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TemperatureController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TemperatureController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TemperatureController::StopSignalUpload() => %s", e.what());
        }
    }
}
}
    

