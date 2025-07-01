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
#include "DriveController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/ExtraTemperatureControlInterface.h"
//#include "API/Code/OpticalAcquireInterface.h"
#include "API/Code/ElectrodeAcquireInterface.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"


using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

/**
 * @brief 驱动控制器构造。
 */
DriveController::DriveController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new PeristalticPumpInterface(addr)),
    ISolenoidValve(new SolenoidValveInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr)),
    ITemperatureControl(new TemperatureControlInterface(addr)),
    IExtraTemperatureControl(new ExtraTemperatureControlInterface(addr)),
    IElectrodeAcquire(new ElectrodeAcquireInterface(addr))
{

}

DriveController::~DriveController()
{
    if (IPeristalticPump)
        delete IPeristalticPump;
    if (ISolenoidValve)
        delete ISolenoidValve;
    if (IOpticalMeter)
        delete IOpticalMeter;
    if (ITemperatureControl)
        delete ITemperatureControl;
    if (IExtraTemperatureControl)
        delete IExtraTemperatureControl;
    if (IElectrodeAcquire)
        delete IElectrodeAcquire;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool DriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    IExtraTemperatureControl->RegisterTemperatureNotice(this);
    IElectrodeAcquire->RegisterAcquireElectrodeADNotice(this);
    return true;
}

bool DriveController::Uninit()
{
    return true;
}

void DriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *DriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *DriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *DriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

TemperatureControlInterface *DriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

ExtraTemperatureControlInterface *DriveController::GetIExtraTemperatureControl()
{
    return IExtraTemperatureControl;
}


ElectrodeAcquireInterface *DriveController::GetIElectrodeAcquire()
{
    return IElectrodeAcquire;
}

float DriveController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float DriveController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

Temperature DriveController::GetCurrentTemperature()
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

float DriveController::GetMeasureCellTemperature() const
{
    return m_extraTemperature.thermostatTemp;
}

Temperature DriveController::GetCurrentExtraTemperature()
{
    Temperature temp = {0, 0};

    try
    {
        temp = this->IExtraTemperatureControl->GetTemperature();
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_extraTemperature, sizeof(Temperature));
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
            memcpy(&temp, &m_extraTemperature, sizeof(Temperature));
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

// 五阶中值滤波
void DriveController::MedianFilter(const float *buf, float *fbuf, int length)
{
   for (int i = 2; i < length - 2; ++i)
   {
      float window[5];
      for(int j = 0; j < 5; ++j)
      {
         window[j] = buf[i - 2 + j];
      }
      for(int j = 0; j < 3; ++j)
      {
         int min = j;
         for(int k = j + 1; k < 5; ++k)
         {
            if(window[k] < window[min])
            {
               min = k;
            }
         }
         const float temp = window[j];
         window[j] = window[min];
         window[min] = temp;
      }
      fbuf[i] = window[2];
   }

   for (int i = 1; i > -1; i--)
   {
       if(fabs(buf[i] - fbuf[i+1]) > 2*fabs(fbuf[i+1] - fbuf[i+2]))
       {
           fbuf[i] = fbuf[i+1] - (fbuf[i+2] - fbuf[i+1]);
       }
   }

   for (int i = length - 2; i < length; i++)
   {
       if(fabs(buf[i] - fbuf[i-1]) > 2*fabs(fbuf[i-1] - fbuf[i-2]))
       {
           fbuf[i] = fbuf[i-1] - (fbuf[i-2] - fbuf[i-1]);
       }
   }
}

CurveData* DriveController::GetCurveData(float beginVoltage, float endVoltage, float resolution)
{
    int maxPackSize = 50;
    const int length = floor((endVoltage - beginVoltage)/resolution)-1;

    if(length <= 0)
    {
        return nullptr;
    }

    float buffer[length];// = {0};
    memset(&buffer[0], 0,  sizeof(float)*length);

    CurveData *curveData = new CurveData(beginVoltage, endVoltage, length);
    logger->debug("curve data length = %d", length);
    // 生成曲线数据
    /*double u = 600;
    double s = 80;
    for(int i = 0; i < length; i++)
    {
        curveData->SetData(i,sin(i*M_PI/2000)+(1000/(pow(2*M_PI,0.5)*s))*exp(-(pow((i-u),2)/(2*s*s))));
    }*/

    if(length > maxPackSize)
    {
        int i = 0;
        for(i =0; i< length/maxPackSize; i++)
        {
                this->IElectrodeAcquire->GetElectrodeScanData(&buffer[i*maxPackSize], (Uint16)i*maxPackSize, (Uint16)maxPackSize);
        }
        if(length - i*maxPackSize > 0)
        {
            this->IElectrodeAcquire->GetElectrodeScanData(&buffer[i*maxPackSize], i*maxPackSize, length - i*maxPackSize);
        }
    }
    else
    {
        this->IElectrodeAcquire->GetElectrodeScanData(&buffer[0], 0, length);
    }

    // 中值滤波
    float filterbuffer[length];//={0};
    memcpy(&filterbuffer[0], &buffer[0],  sizeof(float)*length);
    MedianFilter(&buffer[0], &filterbuffer[0], length);

    curveData->AddData(&filterbuffer[0], length);

    return curveData;
}

void DriveController::OnReceive(DscpEventPtr event)
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

        case DSCP_EVENT_ETCI_TEMPERATURE_NOTICE:     //附加温度上报事件
        {
            m_extraTemperature.thermostatTemp = *((float*)event->data);
            m_extraTemperature.environmentTemp = *((float*)(event->data+4));

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("mTemp", m_extraTemperature.thermostatTemp);
                }
            }
        }
            break;

        /*case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
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
                    (*it)->OnSignalChanged("refAD", ref);
                    (*it)->OnSignalChanged("meaAD", mea);
                    (*it)->OnSignalChanged("abs", abs);
                }
            }
        }
            break;*/

        case DSCP_EVENT_EAI_SIGNAL_AD_NOTICE:   // 电极信号AD定时上报事件
        {
            float current = *((float*)(event->data));

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                     (*it)->OnSignalChanged("current", current);
                }
            }
        }
            break;

        default:
            break;
    }
}

void DriveController::StopSignalUpload()
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

        //重置附加温度上传周期
        try
        {
            this->IExtraTemperatureControl->SetTemperatureNotifyPeriod(0);
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

        //重置电极AD上传周期
        try
        {
            this->IElectrodeAcquire->SetAcquireElectrodeADNotifyPeriod(0);
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

void DriveController::StartSignalUpload()
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
            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
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

        //重置附加温度上传周期
        try
        {
            this->IExtraTemperatureControl->SetTemperatureNotifyPeriod(1);
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

        //重置电极AD上传周期
        try
        {
            this->IElectrodeAcquire->SetAcquireElectrodeADNotifyPeriod(1);
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
