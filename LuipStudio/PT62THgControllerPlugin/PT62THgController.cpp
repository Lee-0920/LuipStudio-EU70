/**
 * @file
 * @brief 驱动控制器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */

#include <math.h>
#include <QTime>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>
#include "Log.h"
#include "PT62THgController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/OpticalAcquireInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "LuaEngine/LuaEngine.h"


#define ORIGINALDATA 1
#define DEBUGDATA 0

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace OOLUA;
using namespace Lua;

namespace Controller
{

/**
 * @brief PT62THg驱动控制器构造。
 */
PT62THgController::PT62THgController(DscpAddress addr)
    : BaseController(addr),
    IPeristalticPump(new PeristalticPumpInterface(addr)),
    ISolenoidValve(new SolenoidValveInterface(addr)),
    IOpticalMeter(new OpticalMeterInterface(addr)),
    IOpticalAcquire(new OpticalAcquireInterface(addr)),
    ITemperatureControl(new TemperatureControlInterface(addr)),
    m_environmentTemp(0),
    m_sendEnvTempCount(0),
    m_isStopAcquireAD(false),
    m_isOnesAcquireFinish(true)

{
    m_environmentTemp = 0;

    for (int i = 0; i < 10; i++)
    {
        m_ThermostatTemps.push_back(0);
    }

    connect(this, SIGNAL(EnvTempToLuaSignal(float)), this, SLOT(EnvTempToLuaSlot(float)));
}

PT62THgController::~PT62THgController()
{
    if (IPeristalticPump)
        delete IPeristalticPump;
    if (ISolenoidValve)
        delete ISolenoidValve;
    if (IOpticalMeter)
        delete IOpticalMeter;
    if (IOpticalAcquire)
        delete IOpticalAcquire;
    if (ITemperatureControl)
        delete ITemperatureControl;
}

/**
 * @brief 初始化 PT62THgController 环境。
 */
bool PT62THgController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    IOpticalAcquire->RegisterAcquireADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);

    return true;
}

bool PT62THgController::Uninit()
{
    return true;
}

void PT62THgController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *PT62THgController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *PT62THgController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *PT62THgController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

OpticalAcquireInterface *PT62THgController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

TemperatureControlInterface *PT62THgController::GetITemperatureControl()
{
    return ITemperatureControl;
}

float PT62THgController::GetEnvironmentSignalTemp() const
{
    return m_environmentTemp;
}

float PT62THgController::GetThermostatSignalTemp(Uint8 index) const
{
    return m_ThermostatTemps[index];
}


float PT62THgController::GetCurrEnvironmentTemp()
{
    float temp;

    try
    {
        temp = this->ITemperatureControl->GetEnvironmentTemperature();
        m_environmentTemp = temp;
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            temp = m_environmentTemp;
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
            temp = m_environmentTemp;
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

float PT62THgController::GetCurrThermostatTemp(Uint8 index)
{
    float temp;

    try
    {
        temp = this->ITemperatureControl->GetThermostatTemperature(index);
        m_ThermostatTemps[index] = temp;
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            temp = m_ThermostatTemps[index];
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
            temp = m_ThermostatTemps[index];
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

bool SortForOpticalAD(const OpticalAD &ad1, const OpticalAD &ad2)
{
    return ad1.measure < ad2.measure;   //升序排列
}

void PT62THgController::SlideFilter(std::vector<float> &ads)
{
    std::vector<float>::iterator itTemp;
    std::vector<float> temp;

    for (std::vector<float>::iterator it = ads.begin(); it != ads.end(); it++)
    {
        itTemp = it;

        float adsBuff = 0;

        int count = 0;

        for (int i = 0; i < 10; i++)
        {
           if (it != ads.end())
           {
               adsBuff += (*it);
               it++;
               count++;
           }
           else
           {
               break;
           }
        }

        if (count < 10)
        {
           break;
        }

        it = itTemp;

        temp.push_back(adsBuff/count);
    }

    ads.clear();
    ads = temp;

    #if DEBUGDATA
        int cnt = 0;
        for (std::vector<float>::iterator itAds = ads.begin(); itAds != ads.end(); itAds++)
        {
            logger->debug("%4d ads = %f", ++cnt, (*itAds));
        }
    #endif
}
OpticalResult PT62THgController::StartAcquireAD(float samplePeriod, int acqTimes, Algorithms algorithms)
{
    vector<OpticalAD> ad;
    OpticalResult result = {(int)OpticalResultType::Finished,0,0};

    m_isStopAcquireAD =false;

    long timeout = samplePeriod * 1000 + 1000;

    for (int i = 0; i < acqTimes; i++)
    {
        if (m_isStopAcquireAD)
        {
            m_isStopAcquireAD =false;
            result.type = (int)OpticalResultType::Stopped;
            return result;
        }

        m_isOnesAcquireFinish = false;
        IOpticalAcquire->ExpectADAcquirer(timeout, ad);
        m_isOnesAcquireFinish = true;
    }

    #if ORIGINALDATA
        if (!ad.empty())
        {
            logger->debug("原始数据");
            for (int i = 0; i < (int)ad.size(); i++)
            {
                logger->debug("refAD = %d, meaAD = %d", ad[i].reference, ad[i].measure);
            }
        }
    #endif

    if (algorithms == Algorithms::Peak)
    {
        vector<float> ads;

        if (!ad.empty())
        {
            #if DEBUGDATA
                logger->debug("原始绝对吸光值");
            #endif

            for (int i = 0; i < (int)ad.size(); i++)
            {
                float val = (float)ad[i].reference/ad[i].measure;
                ads.push_back(std::log10(val));

                #if DEBUGDATA
                    logger->debug("%4d ads = %f",i, ads[i]);
                #endif
            }
        }

        #if DEBUGDATA
            logger->debug("平滑滤波后绝对吸光值");
        #endif

        SlideFilter(ads);

        int point = 0;
        int size = ads.size();
        float maxAds = ads[0];

        for (int i = 0; i < size; i++)
        {
            if (ads[i] - maxAds > 0.000001)
            {
                maxAds = ads[i];
                point = i;
            }
        }

        #if DEBUGDATA
            logger->debug("point = %d, max ads = %f", point +1, maxAds);
            logger->debug("峰值数据");
        #endif

        vector<OpticalAD> peakVal;

        if (point - 10 >= 0)
        {
            for (int i = point - 10; i < point; i++)
            {
                peakVal.push_back(ad[i]);

                #if DEBUGDATA
                    logger->debug("%4d ref = %d, mea = %d",i + 1, ad[i].reference, ad[i].measure);
                #endif
            }
        }
        else
        {
            for (int i = 0; i < point; i++)
            {
                peakVal.push_back(ad[i]);

                #if DEBUGDATA
                    logger->debug("%4d ref = %d, mea = %d",i + 1, ad[i].reference, ad[i].measure);
                #endif
            }
        }

        peakVal.push_back(ad[point]);

        #if DEBUGDATA
           logger->debug("%4d ref = %d, mea = %d (max)", point + 1, ad[point].reference, ad[point].measure);
        #endif

        if (point + 10 <= size)
        {
            for (int i = point + 1; i <= point + 10; i++)
            {
                peakVal.push_back(ad[i]);

                #if DEBUGDATA
                    logger->debug("%4d ref = %d, mea = %d",i + 1, ad[i].reference, ad[i].measure);
                #endif
            }
        }
        else
        {
            for (int i = point + 1; i < size; i++)
            {
                peakVal.push_back(ad[i]);

                #if DEBUGDATA
                    logger->debug("%4d ref = %d, mea = %d",i + 1, ad[i].reference, ad[i].measure);
                #endif
            }
        }

        if (!peakVal.empty())
        {
            std::sort(peakVal.begin(), peakVal.end(), SortForOpticalAD);

            for(int i = 0; i < 3; i++)
            {
                  peakVal.erase(peakVal.begin());
            }

            for(int i = 0; i < 2; i++)
            {
                peakVal.erase(peakVal.end() - 1);
            }

            Uint64 ref = 0;
            Uint64 mea = 0;

            int size = peakVal.size();

            for (int i = 0; i < size; i++)
            {
                ref += peakVal[i].reference;
                mea += peakVal[i].measure;
            }

            result.reference = ref/size;
            result.measure = mea/size;
        }
    }
    else if (algorithms == Algorithms::HalfArea)
    {
        vector<float> ads;

        #if DEBUGDATA
            logger->debug("初始值吸光度 = %f", m_intAbs);
        #endif

        if (!ad.empty())
        {
            #if DEBUGDATA
                logger->debug("原始相对吸光值");
            #endif

            for (int i = 0; i < (int)ad.size(); i++)
            {
                float val = (float)ad[i].reference/ad[i].measure;
                ads.push_back(std::log10(val) - m_intAbs);

                #if DEBUGDATA
                    logger->debug("%4d ads = %f",i, ads[i]);
                #endif
            }
        }

        #if DEBUGDATA
            logger->debug("平滑滤波后相对对吸光值");
        #endif

        SlideFilter(ads);

        int point = 0;
        int size = ads.size();
        float maxAds = ads[0];

        for (int i = 0; i < size; i++)
        {
            if (ads[i] - maxAds > 0.000001)
            {
                maxAds = ads[i];
                point = i;
            }
        }

        #if DEBUGDATA
            logger->debug("point = %d, max ads = %f", point +1, maxAds);
        #endif

        float halfAreaAds = 0;

        for (int i = 0; i < point + 1; i++)
        {
            halfAreaAds += ads[i];
        }

        #if DEBUGDATA
            logger->debug("半峰面积，总相对吸光度 = %f", halfAreaAds);
        #endif

        result.reference = 1;
        result.measure = 1;
    }
    else if (algorithms == Algorithms::Average)
    {
        if (!ad.empty())
        {
            std::sort(ad.begin(), ad.end(), SortForOpticalAD);

            for(int i = 0; i < 5; i++)
            {
                ad.erase(ad.begin());
            }

            for(int i = 0; i < 5; i++)
            {
                ad.erase(ad.end() - 1);
            }

            Uint64 ref = 0;
            Uint64 mea = 0;

            int size = ad.size();
            for (int i = 0; i < size; i++)
            {
                ref += ad[i].reference;
                mea += ad[i].measure;
            }

            result.reference = ref/size;
            result.measure = mea/size;
        }
    }

    logger->debug("result refAD = %d, meaAD = %d", result.reference, result.measure);

    return result;
}

void PT62THgController::StopAcquireAD()
{
    m_isStopAcquireAD = true;

    if (m_isOnesAcquireFinish)
    {
        IOpticalAcquire->StopAcquirer();
    }
    else
    {
        int i = 300;
        while(i--)
        {
          QThread::msleep(10);

          if (m_isOnesAcquireFinish)
          {
               IOpticalAcquire->StopAcquirer();
               break;
          }
        }
    }
}

void PT62THgController::SetInitAD(Uint32 refAD, Uint32 meaAD)
{
    m_intAbs = std::log10((float)refAD/meaAD);
}

bool PT62THgController::ReInitInstrument()
{
    bool ret = true;

    lua_State * newState = LuaEngine::Instance()->GetThreadState();

    OOLUA::Lua_function call(newState);
    OOLUA::Lua_func_ref initInstrument;

    try
    {
        OOLUA::get_global(newState, "InitInstrument", initInstrument);
        call(initInstrument);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("PT62THgController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    catch(std::exception e)
    {
        logger->warn("PT62THgController::ReInitInstrument() => %s", e.what());
        ret = false;
    }
    return ret;
}


void PT62THgController::StartSignalUpload()
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
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.what());
        }
    }

    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.what());
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
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StartSignalUpload() => %s", e.what());
        }
    }
}

void PT62THgController::StopSignalUpload()
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
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.what());
        }
    }

    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.what());
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
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("PT62THgController::StopSignalUpload() => %s", e.what());
        }
    }
}

void PT62THgController::OnReceive(DscpEventPtr event)
{
    switch (event->code)
    {
        case DSCP_EVENT_OMI_OPTICAL_AD_NOTICE:     //定量点光信号AD定时上报事件
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
            m_environmentTemp = *((float*)event->data);

            emit EnvTempToLuaSignal(m_environmentTemp);

            Uint8 num = *(event->data+4);

            for(Uint8 i = 0; i < num; i++)
            {
                m_ThermostatTemps[i] = *((float*)(event->data+5 + i*4));
            }

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("eTemp", m_environmentTemp);
                    (*it)->OnSignalChanged("tTemp", m_ThermostatTemps[0]);

                    for(Uint8 i = 1; i < num; i++)
                    {
                        String str = "temp" + std::to_string(i);
                        (*it)->OnSignalChanged(str, m_ThermostatTemps[i]);
                    }
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

        default:
            break;
    }
}

void PT62THgController::EnvTempToLuaSlot(float temp)
{
    m_sendEnvTempCount++;

    if (m_sendEnvTempCount >= 30)
    {
        m_sendEnvTempCount = 0;

        lua_State * newState = LuaEngine::Instance()->GetThreadState();

        OOLUA::Lua_function call(newState);
        OOLUA::Lua_func_ref recvEevTemp;

        try
        {
            OOLUA::get_global(newState, "RecvEevTemp", recvEevTemp);
            call(recvEevTemp, temp);
        }
        catch(OOLUA::Exception e)
        {
            logger->warn("PT62THgController::EnvTempToLuaSlot() => %s", e.what());
        }
        catch(std::exception e)
        {
            logger->warn("PT62THgController::EnvTempToLuaSlot() => %s", e.what());
        }
    }
}

void PT62THgController::ClearThermostatRemainEvent()
{
    try
    {
        DscpAddress addr = ITemperatureControl->GetAddress();
        for (int i = 0; i < 10; i++)
        {
            DscpEventPtr event = EventHandler::Instance()->Expect(addr,DSCP_EVENT_TCI_THERMOSTAT_RESULT, 200);

            if (event != nullptr)
            {
                Uint8 index = *event->data;
                Uint8 result = *(event->data + 1);
                float temp = *((float*)(event->data + 2));
                logger->debug("{ClearThermostatRemainEvent} index = %d，result = %d，temp = %.1f",index, result, temp);
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

}
