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
#include "ReactController.h"
#include "Communication/CommunicationException.h"
#include "API/Code/OpticalAcquireInterface.h"
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
ReactController::ReactController(DscpAddress addr)
    : BaseController(addr),
    IOpticalAcquire(new OpticalAcquireInterface(addr)),
    ISolenoidValve(new RCSolenoidValveInterface(addr)),
    ITemperatureControl(new RCTemperatureControlInterface(addr)),
    m_environmentTemp(0),m_sendEnvTempCount(0),m_scanLen(0)
{
    connect(this, SIGNAL(EnvTempToLuaSignal(float)), this, SLOT(EnvTempToLuaSlot(float)));
}

ReactController::~ReactController()
{
    if (IOpticalAcquire)
        delete IOpticalAcquire;
    if (ITemperatureControl)
        delete ITemperatureControl;
    if (ISolenoidValve)
        delete ISolenoidValve;
}

/**
 * @brief 初始化 Controller 环境。
 */
bool ReactController::Init()
{
    IOpticalAcquire->RegisterAcquireADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);

    memset(&m_temperature, 0, sizeof(m_temperature));

    return true;
}

bool ReactController::Uninit()
{
    return true;
}

void ReactController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

OpticalAcquireInterface *ReactController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

RCTemperatureControlInterface *ReactController::GetITemperatureControl()
{
    return ITemperatureControl;
}

RCSolenoidValveInterface *ReactController::GetISolenoidValve()
{
    return ISolenoidValve;
}

float ReactController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float ReactController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

RCTemperature ReactController::GetCurrentTemperature()
{
    RCTemperature temp = {0, 0};

    try
    {
        temp = this->ITemperatureControl->GetTemperature();
    }
    catch (CommandTimeoutException e)  // 命令应答超时异常。
    {
        if(this->GetConnectStatus())
        {
            memcpy(&temp, &m_temperature, sizeof(RCTemperature));
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
            memcpy(&temp, &m_temperature, sizeof(RCTemperature));
        }
        else
        {
            throw std::exception();
        }
    }

    return temp;
}

void ReactController::OnReceive(DscpEventPtr event)
{
    switch (event->code)
    {
        case DSCP_EVENT_TCI_TEMPERATURE_NOTICE:     //温度上报事件
        {
            float temp[4] = {0};
            temp[0] = *((float*)event->data);

            if(event->len >= 4)
            {
                temp[1] = *((float*)(event->data+4));
            }
            if(event->len >= 8)
            {
                 temp[2] = *((float*)(event->data+8));
            }
            if(event->len >= 12)
            {
                 temp[3] = *((float*)(event->data+12));
            }
//            qDebug("%.3f, %.3f, %.3f, %.3f",temp[0],temp[1],temp[2],temp[3]);
            m_temperature.thermostatTemp = temp[0];
            m_temperature.environmentTemp = temp[1];

//            m_environmentTemp = m_temperature.environmentTemp;
//            emit EnvTempToLuaSignal(m_environmentTemp);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
//                    if(event->addr.a4 = 16)
//                    {

//                    }
//                    else
                    {
                        (*it)->OnSignalChanged("rTemp1", temp[0]);
                        (*it)->OnSignalChanged("rTemp2", temp[1]);
                        (*it)->OnSignalChanged("rTemp3", temp[2]);
                        (*it)->OnSignalChanged("rTemp4", temp[3]);
                    }
                }
            }
        }
            break;

        case DSCP_EVENT_OAI_SIGNAL_AD_NOTICE:   // 光学测量信号AD定时上报事件
        {
            double ref = *((float*)(event->data));
            double mea = *((float*)(event->data + 4));
            double abs = 0;
            if (0 < ref && 0 < mea)
            {
                abs = 1000 * log10(ref/mea);
            }

//            qDebug() << event->addr.ToString().c_str();
            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
//                    if(event->addr.a4 = 16)
//                    {
//                        (*it)->OnSignalChanged("exMea", mea);
//                    }
//                    else
                    {
                        (*it)->OnSignalChanged("ref", ref);
                        (*it)->OnSignalChanged("mea", mea);
                        (*it)->OnSignalChanged("abs", abs);
                    }
                }
            }

            m_scanData[m_scanLen] = mea;
            m_scanDataRef[m_scanLen] = ref;
            m_MeaTemp[m_scanLen] = m_temperature.thermostatTemp;  //温度补偿跟踪
            m_RefTemp[m_scanLen] = m_temperature.environmentTemp;  //环境温度跟踪
            m_scanLen++;   //勿在定时器中修改索引
            if(m_scanLen > DATA_MAX_LENGTH-1)
            {
                m_scanLen = 0;
                memset(m_scanData, 0, sizeof(m_scanData));
                memset(m_scanDataRef, 0, sizeof(m_scanDataRef));
                memset(m_MeaTemp, 0, sizeof(m_MeaTemp));
                memset(m_RefTemp, 0, sizeof(m_RefTemp));
            }
//            m_timeOutLen = m_scanLen;
        }
            break;
        default:
            break;
    }
}

void ReactController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("ReactController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("ReactController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("ReactController::StartSignalUpload() => %s", e.what());
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
            logger->warn("ReactController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("ReactController::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("ReactController::StartSignalUpload() => %s", e.what());
        }
    }
}

void ReactController::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("ReactController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("ReactController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("ReactController::StopSignalUpload() => %s", e.what());
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
            logger->warn("ReactController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("ReactController::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("ReactController::StopSignalUpload() => %s", e.what());
        }
    }
}

void  ReactController::EnvTempToLuaSlot(float temp)
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
//            logger->warn("CM66DriveController::EnvTempToLuaSlot() => %s", e.what());
//        }
//        catch(std::exception e)
//        {
//            logger->warn("CM66DriveController::EnvTempToLuaSlot() => %s", e.what());
//        }
//    }
}

double ReactController::GetRefTemp(int index) const
{
    if(index < DATA_MAX_LENGTH)
    {
        return m_RefTemp[index];
    }

    return 0;
}

double ReactController::GetMeaTemp(int index) const
{
    if(index < DATA_MAX_LENGTH)
    {
        return m_MeaTemp[index];
    }

    return 0;
}

double ReactController::GetScanData(int index) const
{
    if(index < DATA_MAX_LENGTH)
    {
        return m_scanData[index];
    }

    return 0;
}

double ReactController::GetScanDataRef(int index) const
{
    if(index < DATA_MAX_LENGTH)
    {
        return m_scanDataRef[index];
    }

    return 0;
}

int ReactController::GetScanLen(void) const
{
    return m_scanLen;
}

double ReactController::GetData(void)
{
    if(m_scanLen)
    {
        return  m_scanData[m_scanLen-1];
    }
    return 0;
}

double ReactController::GetDataRef(void)
{
    if(m_scanLen)
    {
        return  m_scanDataRef[m_scanLen-1];
    }
    return 0;
}

void ReactController::ClearBuf(void)
{
    memset(m_scanData, 0, sizeof(m_scanData));
    memset(m_scanDataRef, 0, sizeof(m_scanDataRef));
    memset(m_MeaTemp, 0, sizeof(m_MeaTemp));
    memset(m_RefTemp, 0, sizeof(m_RefTemp));
    m_scanLen = 0;
}

double ReactController::NDIRResultHandle(int startIndex, int endIndex, int validCnt,
                                            int step, int increment,
                                            int filterStep, int throwNum, bool isExtra) const
{
    //    //刷新频率必须与上报频率一致
    double minDeviation, endDeviation;
//    int filterStep = 0;
    bool debugMode;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    Table measureResultTable, configSystemTable;
    luaEngine->GetLuaValue(state, "setting.measureResult", measureResultTable);
    measureResultTable.safe_at("minDeviation", minDeviation);
    measureResultTable.safe_at("endDeviation", endDeviation);
//    measureResultTable.safe_at("filterStep", filterStep);
    luaEngine->GetLuaValue(state, "config.system", configSystemTable);
    configSystemTable.safe_at("debugMode",debugMode);

    int maxIndex = 0;
    int minIndex = 0;


    //五阶中值滤波
    double filterbuffer[endIndex - startIndex] = {0};//={0};
    int length = endIndex - startIndex;
    qDebug("RC Handle %d", length);
    if(isExtra)
    {
        qDebug("IC");
        Filter(&m_scanDataRef[startIndex], &filterbuffer[0], length, filterStep, throwNum);
    }
    else
    {
        qDebug("TC");
        Filter(&m_scanData[startIndex], &filterbuffer[0], length, filterStep, throwNum);
    }

    for(int i = 0; i < length; i++) //全部取负，得到正峰
    {
//       filterbuffer[i] = 0 - filterbuffer[i];
       if(i > filterStep && i < length - filterStep)
       {
           if(debugMode)
           {
               logger->debug("Filter[%d]: %f, NDIR: %f , Env: %f",
                             i, filterbuffer[i], m_MeaTemp[startIndex+i], m_RefTemp[startIndex+i]);
           }
       }
    }

    double min = filterbuffer[filterStep*2], max = -100;
    for(int i = filterStep; i < length - filterStep ; i++) //寻找峰最大值，及其索引值
    {
       if(max < filterbuffer[i])
       {
           max = filterbuffer[i];
           maxIndex = i;
       }

       if(min > filterbuffer[i])
       {
           min = filterbuffer[i];
           minIndex = i;
       }
    }          

    if (maxIndex == 0 )
    {
        maxIndex = length/2;
        logger->warn("maxIndex error");
    }  

   logger->debug("filterStep: %d, validCnt: %d, increment: %d, filterStep: %d, throwNum: %d",
                 filterStep, validCnt, increment, filterStep ,throwNum);
   logger->debug("startDeviation: %f, endDeviation: %f", minDeviation, endDeviation);
   logger->debug("minINdex: %d, maxIndex: %d, length: %d, maxValue: %f, minValue: %f",
                 minIndex, maxIndex, length,filterbuffer[maxIndex], filterbuffer[minIndex]);

    //求峰宽，峰高
   double validHigh = filterbuffer[maxIndex]; //- filterbuffer[minIndex];

   //求峰宽系数
    int scaleNum = 500; //
   double peakWideFactor = (double)1 / scaleNum;

   //求值S1，S2
   double S1 = 0, S2 = 0, midSum = 0;

   double areaSum = 0;
   areaSum = S1 - S2;

   logger->debug("high: %f , R32", validHigh);

   return validHigh;

}

float ReactController::GetPeakTemperature(int startIndex, int endIndex, bool isExtra)
{
    int maxIndex = 0;
    int minIndex = 0;

    //五阶中值滤波
    double filterbuffer[endIndex - startIndex] = {0};//={0};
    int length = endIndex - startIndex;
    qDebug("RC Handle %d", length);
    if(isExtra)
    {
        qDebug("IC");
        Filter(&m_scanDataRef[startIndex], &filterbuffer[0], length, 5, 1);
    }
    else
    {
        qDebug("TC");
        Filter(&m_scanData[startIndex], &filterbuffer[0], length, 5, 1);
    }

    double min = filterbuffer[10], max = -100;
    for(int i = 1; i < length; i++) //寻找峰最大值，及其索引值
    {
       if(max < filterbuffer[i])
       {
           max = filterbuffer[i];
           maxIndex = i;
       }

       if(min > filterbuffer[i])
       {
           min = filterbuffer[i];
           minIndex = i;
       }
    }

    //求峰宽，峰高
   double validHigh = filterbuffer[maxIndex]; //- filterbuffer[minIndex];

   float peakTemperature;
   if(isExtra)
   {
       qDebug("IC");
       peakTemperature = m_RefTemp[startIndex+maxIndex];
   }
   else
   {
       qDebug("TC");
       peakTemperature = m_MeaTemp[startIndex+maxIndex];
   }

   return peakTemperature;
}

void ReactController::Filter(const double *buf, double *fbuf, int length, int filterStep, int throwNum)const
{
    double averBuf[filterStep*2];

    for (int i = filterStep; i < length - filterStep; ++i)
    {
//        double sum = 0;
//        for(int j =0 ;j< filterStep*2-1;j++) //滤波数组赋值
//        {
//            averBuf[j] = buf[j+i-filterStep];
//        }
//        for(int j =0 ;j< filterStep*2-1;j++) //滤波数组冒泡排序
//        {
//            for(int k =0 ;k< filterStep*2-j-1;k++)
//            {
//                if(averBuf[k+1]< averBuf[k]) //正序
//                {
//                    double temp = averBuf[k+1];
//                    averBuf[k+1] = averBuf[k];
//                    averBuf[k] = temp;
//                }
//            }
//        }

//        for(int i = 0; i<filterStep*2-1;i++)
//        {
//            if(i>=throwNum && i < filterStep*2-throwNum) //过滤前后各4个
//            {
//                sum +=  averBuf[i];
//            }
//        }
//        fbuf[i] = sum/(filterStep*2-throwNum*2);
        fbuf[i] = buf[i];

    }
}

}
