/**
 * @file
 * @brief 液路控制器。
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
#include "LiquidController.h"
#include "Communication/CommunicationException.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

/**
 * @brief 液路控制器构造。
 */
LiquidController::LiquidController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new LCPeristalticPumpInterface(addr)),
    ISolenoidValve(new LCSolenoidValveInterface(addr)),
    ITemperatureControl(new LCTemperatureControlInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr))
{

}

LiquidController::~LiquidController()
{
    if (IPeristalticPump)
        delete IPeristalticPump;
    if (ISolenoidValve)
        delete ISolenoidValve;
    if (IOpticalMeter)
        delete IOpticalMeter;         
    if (ITemperatureControl)
        delete ITemperatureControl;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool LiquidController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    return true;
}

bool LiquidController::Uninit()
{
    return true;
}

void LiquidController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

LCPeristalticPumpInterface *LiquidController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

LCSolenoidValveInterface *LiquidController::GetISolenoidValve()
{
    return ISolenoidValve;
}

LCTemperatureControlInterface *LiquidController::GetITemperatureControl()
{
    return ITemperatureControl;
}

OpticalMeterInterface *LiquidController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

void LiquidController::StartSignalUpload()
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
            logger->warn("LiquidController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("LiquidController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("LiquidController::StartSignalUpload() => %s", e.what());
        }
    }
}

void LiquidController::StopSignalUpload()
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
            logger->warn("LiquidController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("LiquidController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("LiquidController::StopSignalUpload() => %s", e.what());
        }
    }
}

void LiquidController::OnReceive(DscpEventPtr event)
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

//                (*it)->OnSignalChanged(Format("mp%dAD", i + 1), adValue[i]);
            }
        }
    }

    if (adValue)
    {
      delete[] adValue;
    }
}

}
