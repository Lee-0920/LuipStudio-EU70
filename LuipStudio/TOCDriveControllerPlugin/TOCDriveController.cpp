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
#include "TOCDriveController.h"
#include "API/Code/OpticalMeterInterface.h"
#include "API/Code/TemperatureControlInterface.h"
#include "API/Code/PeristalticPumpInterface.h"
#include "API/TemperatureControlInterface.h"
#include "API/Code/OpticalAcquireInterface.h"
#include "Communication/EventHandler.h"
#include "Communication/CommunicationException.h"
#include "LuaEngine/LuaEngine.h"
#include "API/Code/SolenoidValveInterface.h"
#include "NT66ResultDetailPlugin//CurveManager.h"
#include <QTimer>

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
TOCDriveController::TOCDriveController(DscpAddress addr)
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
    m_isEnableStove(true),
    m_insideEnvironmentTemp(0),
    m_ExInsideEnvironmentTemp(0),
    m_environmentTemp(0),
    m_scanLen(0),
    m_weepingDetectEnable(false),
    m_timeOutLen(0)
{
    memset(m_thermostatTempArray, 0, sizeof(m_thermostatTempArray));
    memset(m_pressureArray, 0, sizeof(m_pressureArray));
    memset(m_scanData, 0, sizeof(m_scanData));
    memset(m_MeasureTemp, 0, sizeof(m_MeasureTemp));
    memset(m_EnvTemp, 0, sizeof(m_EnvTemp));
    memset(m_pressture, 0, sizeof(m_pressture));
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        Table configSystemTable;
        luaEngine->GetLuaValue(state, "config.system", configSystemTable);
        if(!configSystemTable.safe_at("environmentTemperature",m_insideEnvironmentTemp))
        {
            luaEngine->GetLuaValue(state, "setting.temperature.insideEnvironmentTemp", m_insideEnvironmentTemp);
        }
        if(!configSystemTable.safe_at("exEnvironmentTemperature",m_ExInsideEnvironmentTemp))
        {
            luaEngine->GetLuaValue(state, "setting.temperature.insideEnvironmentTemp", m_ExInsideEnvironmentTemp);
        }

        luaEngine->GetLuaValue(state, "config.system.adcDetect[1].enable", m_weepingDetectEnable);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("TOCDriveController::TOCDriveController() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("TOCDriveController::TOCDriveController() => %s", e.what());
    }    
    connect(this, SIGNAL(BoxFanControlSignal(float)), this, SLOT(BoxFanControlSlot(float)));
    connect(this, SIGNAL(ExBoxFanControlSignal(float)), this, SLOT(ExBoxFanControlSlot(float)));   
}

TOCDriveController::~TOCDriveController()
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
bool TOCDriveController::Init()
{
    IOpticalMeter->RegisterOpticalADNotice(this);
    ITemperatureControl->RegisterTemperatureNotice(this);
    IOpticalAcquire->RegisterAcquireADNotice(this);
    ISolenoidValve->RegisterPressureNotice(this);
    IOpticalAcquire->RegisterNDIRNotice(this);
    IOpticalAcquire->RegisterAcquireCheckLeaking(this);

    return true;
}

bool TOCDriveController::Uninit()
{
    return true;
}

void TOCDriveController::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

PeristalticPumpInterface *TOCDriveController::GetIPeristalticPump()
{
    return IPeristalticPump;
}

SolenoidValveInterface *TOCDriveController::GetISolenoidValve()
{
    return ISolenoidValve;
}

OpticalMeterInterface *TOCDriveController::GetIOpticalMeter()
{
    return IOpticalMeter;
}

TemperatureControlInterface *TOCDriveController::GetITemperatureControl()
{
    return ITemperatureControl;
}

OpticalAcquireInterface *TOCDriveController::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

ExtTemperatureControlInterface *TOCDriveController::GetIExtTemperatureControl()
{
    return IExtTemperatureControl;
}

ExtOpticalAcquireInterface *TOCDriveController::GetIExtOpticalAcquire()
{
    return IExtOpticalAcquire;
}

float TOCDriveController::GetDigestTemperature() const
{
    return m_temperature.thermostatTemp;
}

float TOCDriveController::GetEnvironmentTemperature() const
{
    return m_temperature.environmentTemp;
}

float TOCDriveController::GetReportThermostatTemp(int index) const
{
    float temp = 0;

    if(index >= 0 && index < 3)
    {
        temp = m_thermostatTempArray[index];
    }

    return temp;
}

float TOCDriveController::GetReportEnvironmentTemp() const
{
    return m_environmentTemp;
}

float TOCDriveController::GetPressure(int index) const
{
    return m_pressureArray[index];
}

double TOCDriveController::GetScanData(int index) const
{
    if(index < DATA_MAX_LENGTH)
    {
        return m_scanData[index];
    }

    return 0;
}

int TOCDriveController::GetScanLen(void) const
{
    return m_scanLen;
}

//double TOCDriveController::GetData(void)
//{
//    if(m_scanLen)
//    {
////        qDebug("Test OK len %d, %.3f", m_scanLen, m_scanData[m_scanLen-1]);
//        return  m_scanData[m_scanLen-1];
//    }
//    return 0;
//}

void TOCDriveController::ClearBuf(void)
{
    memset(m_scanData, 0, sizeof(m_scanData));
    memset(m_scanDataRef, 0, sizeof(m_scanDataRef));
    memset(m_MeasureTemp, 0, sizeof(m_MeasureTemp));
    memset(m_EnvTemp, 0, sizeof(m_EnvTemp));
    memset(m_pressture, 0, sizeof(m_pressture));
    m_scanLen = 0;
}

double TOCDriveController::NDIRResultHandle(int startIndex, int endIndex, int validCnt,
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

    //五阶中值滤波
    double filterbuffer[endIndex - startIndex] = {0};//={0};
    int length = endIndex - startIndex;
    if(isExtra)
    {
      Filter(&m_scanDataRef[startIndex], &filterbuffer[0], length, filterStep, throwNum);
    }
    else
    {
        Filter(&m_scanData[startIndex], &filterbuffer[0], length, filterStep, throwNum);
    }

    for(int i = 0; i < length; i++) //全部取负，得到正峰
    {
       filterbuffer[i] = 0 - filterbuffer[i];
       if(i > 20+filterStep*2 && i < length - 20-filterStep*2)
       {
           if(debugMode)
           {
               logger->debug("Filter[%d]: %f, NDIR: %f , Env: %f",
                             i, filterbuffer[i], m_MeasureTemp[startIndex+i], m_EnvTemp[startIndex+i]);
           }
       }
    }

    double min = filterbuffer[50], max = -100;
    for(int i = 20+filterStep*2; i < length - 50-filterStep*2 ; i++) //寻找峰最大值，及其索引值
    {
       if(max < filterbuffer[i]
               && (fabs(filterbuffer[i] - filterbuffer[i-10]) < 0.1)
               && (fabs(filterbuffer[i] - filterbuffer[i+10]) < 0.1))
       {
           max = filterbuffer[i];
           maxIndex = i;
       }
    }

    if (maxIndex == 0 )
    {
        maxIndex = length/2;
        logger->warn("maxIndex error");
    }

    int leftIndex = 20+filterStep*2, rightIndex = length - 30- filterStep*2;
    //从峰最低点往后寻找起始点, 连续十个点的极差值小于偏差范围则认为稳定
    double divation;
    bool isFinish = false;
    int finishCnt = 0;
    int endValidCnt = 0;
    float decimal = (int)((float)validCnt*0.8*10)%10;
    if(decimal >= 5)
    {
        endValidCnt = (int)((float)validCnt*0.8)+1;
    }
    else
    {
        endValidCnt = (int)((float)validCnt*0.8);
    }

    int tempIndex = maxIndex;
    for(int i = maxIndex - 50; i > 50+validCnt*increment; i -= increment)
    {

       min = filterbuffer[i];
       max = filterbuffer[i-validCnt*increment];

       divation = fabs(max - min);
    //           qDebug("divation: %f", divation);
       if(divation <= minDeviation*1.5 && i < maxIndex - increment*10)
       {
           for(int j = i; j > i - validCnt*increment; j -= increment)
           {
               double subMin = filterbuffer[j];
               double subMax = filterbuffer[j-step];

               divation = fabs(subMax - subMin);
               qDebug("divation-: %f", divation);
               if(divation <= minDeviation)
               {
                   finishCnt++;
               }
               if(maxIndex == 20)  //数据异常
               {
                   break;
               }
               if(finishCnt >= endValidCnt)
               {
                   leftIndex = j + increment*6;
                   break;
               }
           }

       }

       if(finishCnt >= endValidCnt)
       {
           qDebug("finish: %d, minDeviation: %f", leftIndex, minDeviation);
           break;
       }
       else
       {
           finishCnt = 0;
       }
    }
    if(finishCnt < endValidCnt)
    {
      leftIndex = 20+filterStep*2;
    }

   logger->debug("filterStep: %d, validCnt: %d, increment: %d, filterStep: %d, throwNum: %d",
                 filterStep, validCnt, increment, filterStep ,throwNum);
   logger->debug("startDeviation: %f, endDeviation: %f", minDeviation, endDeviation);
   logger->debug("leftIndex: %d, rightIndex: %d , length: %d, maxIndex: %d, maxVlue: %f, minDeviation: %f",
                 leftIndex, rightIndex, length, maxIndex, filterbuffer[maxIndex], minDeviation);

    //求峰宽，峰高
   double validHigh = filterbuffer[maxIndex] -
           ((filterbuffer[leftIndex-1] + filterbuffer[leftIndex] + filterbuffer[leftIndex+1])/3);

   //求峰宽系数
    int scaleNum = 500; //
   double peakWideFactor = (double)1 / scaleNum;

   //求值S1，S2
   double S1 = 0, S2 = 0, midSum = 0;

   double areaSum = 0;
   areaSum = S1 - S2;

   validHigh = validHigh*1000;

   logger->debug("left: %f, max: %f", filterbuffer[leftIndex], filterbuffer[maxIndex]);
   logger->debug("high: %f , R30", validHigh);

   return validHigh;


}

bool TOCDriveController::IsReachSteady(int num, int validCnt, int step, int increment, int filterStep, int throwNum, int index) const
{
    if(m_scanLen < num )
    {
        return false;
    }
    //    //刷新频率必须与上报频率一致
    double minDeviation, endDeviation, deviation;
    bool debugMode;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    Table measureResultTable, configSystemTable;
    luaEngine->GetLuaValue(state, "setting.measureResult", measureResultTable);
    measureResultTable.safe_at("deviation", deviation);
    measureResultTable.safe_at("minDeviation", minDeviation);
    measureResultTable.safe_at("endDeviation", endDeviation);
    luaEngine->GetLuaValue(state, "config.system", configSystemTable);
    configSystemTable.safe_at("debugMode",debugMode);

    int maxIndex = 0;

    //五阶中值滤波
    double filterbuffer[num] = {0};//={0};
    int length = num;
    Filter(&m_scanData[m_scanLen-length], &filterbuffer[0], length, filterStep, throwNum);

    for(int i = 0; i < length; i++) //全部取负，得到正峰
    {
       filterbuffer[i] = 0 - filterbuffer[i];
    }

    //终点判定
    double divation, max, min;
    bool isFinish = false;
    int finishCnt = 0;
    int tempIndex = length - step;

    //极差值判定方式
    if(index == 0)
    {
        double checkDeviation;
        if(m_scanLen < num)
        {
            qDebug("NDIR len error");
            return false;
        }

        for(int i = 0; i < length; i++) //全部取负，得到正峰
        {
           filterbuffer[i] = 0 - filterbuffer[i];
        }

        //倒序寻找并比较
        int minIndex = m_scanLen - 1;  //减去数组未装填的指针地址
        int endIndex = minIndex - num;  //连续10个数据在波动范围内则认为稳定

        double min = filterbuffer[filterStep*2];
        double max = filterbuffer[filterStep*2];
        double k1,k2;
        for(int i = filterStep*2; i < length - filterStep*2; i++)
        {
            if(min > filterbuffer[i])
            {
                min = filterbuffer[i];
            }
            if(max < filterbuffer[i])
            {
                max = filterbuffer[i];
            }
//            logger->debug("check fbuf[%d]:%f", i, fbuf[i]);
        }
        k1 = filterbuffer[num/2] - filterbuffer[filterStep*2];
        k2 = filterbuffer[num- filterStep*2-1] - filterbuffer[num/2];
        checkDeviation = fabs(max - min);
        qDebug("check: %f, deviation:%f , max: %f, min: %f, k1: %f, k2:%f , sub: %f",
               checkDeviation, deviation, max, min, k1, k2, k2-k1);
        if(checkDeviation < deviation && checkDeviation > 0.000001)
        {
//            logger->debug("check divation: %f ", divation);
            return true;
        }
        else if(abs(k2 - k1) < 0.0001 && checkDeviation < deviation*3)
        {
             qDebug("k1: %f, k2:%f , sub: %f", k1, k2, k2-k1);
             return true;
        }
        return false;
    }
    else //连续判定方式
    {
        for(int i = length - step; i > step; i -= increment)
        {
           min = filterbuffer[i];
           max = filterbuffer[i-step];
           divation = fabs(max - min);
           if(divation <= minDeviation)
           {
               finishCnt++;
               if(abs(tempIndex-i)>increment*2 && finishCnt > 1) //判断是否为连续
               {
                   finishCnt = 0;
               }
               tempIndex = i;
           }

           if(finishCnt >= validCnt)
           {
               qDebug("finish: %f", minDeviation);
               return true;
           }
        }

        return false;
    }
}

Temperature TOCDriveController::GetCurrentTemperature()
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

void TOCDriveController::OnReceive(DscpEventPtr event)
{
    static int count = 0;
//    logger->debug("\n get event code = %d",event->code);
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

            m_environmentTemp = *((float*)(event->data+4));
            if(event->len >= 12)
            {
                m_thermostatTempArray[0] = *((float*)(event->data+8));
            }
            if(event->len >= 16)
            {
                m_thermostatTempArray[1] = *((float*)(event->data+12));
            }
            if(event->len >= 20)
            {
                m_thermostatTempArray[2] = *((float*)(event->data+16));
            }

            emit BoxFanControlSignal(m_temperature.environmentTemp);
            emit ExBoxFanControlSignal(m_thermostatTempArray[2]);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("tTemp", m_temperature.thermostatTemp);
                    (*it)->OnSignalChanged("eTemp", m_environmentTemp);
                    (*it)->OnSignalChanged("tTemp1", m_thermostatTempArray[0]);
                    (*it)->OnSignalChanged("tTemp2", m_thermostatTempArray[1]);
                    (*it)->OnSignalChanged("tTemp3", m_thermostatTempArray[2]);
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
            qDebug("mea %f, ref %f",mea, ref);

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    (*it)->OnSignalChanged("ref", ref);
                    (*it)->OnSignalChanged("mea", mea);
                    (*it)->OnSignalChanged("abs", abs);
                }
            }

            m_scanData[m_scanLen] = mea;
            m_scanDataRef[m_scanLen] = ref;
            m_MeasureTemp[m_scanLen] = m_thermostatTempArray[1];  //温度补偿跟踪
            m_EnvTemp[m_scanLen] = m_temperature.environmentTemp;  //环境温度跟踪
            m_scanLen++;   //勿在定时器中修改索引
            if(m_scanLen > DATA_MAX_LENGTH-1)
            {
                m_scanLen = 0;
                memset(m_scanData, 0, sizeof(m_scanData));
                memset(m_scanDataRef, 0, sizeof(m_scanDataRef));
                memset(m_MeasureTemp, 0, sizeof(m_MeasureTemp));
                memset(m_EnvTemp, 0, sizeof(m_EnvTemp));
            }
            m_timeOutLen = m_scanLen;
        }
            break;
        case DSCP_EVENT_SVI_PREESURE_NOTICE:      //压力传感器值上报 0-2
        {
//            logger->debug("\n event is DSCP_EVENT_SVI_PREESURE_NOTICE");
            Uint8 num = *(event->data);
            float adValue[num];

            for(Uint8 i = 0; i < num; i++)
            {
                adValue[i] = *((float*)(event->data+1 + i*4));
                m_pressureArray[i] = adValue[i];
            }

            if(!m_notifise.empty())
            {
                list<ISignalNotifiable*>::iterator it;
                for(it = m_notifise.begin(); it != m_notifise.end(); it++)
                {
                    for(Uint8 i = 0; i < num; i++)
                    {
                        String str = "pSensor" + std::to_string(i + 1);
                        (*it)->OnSignalChanged(str, adValue[i]);
                    }
                }
            }
        }
            break;
        case DSCP_EVENT_OAI_REPORT_DATA:   // NDIR测量值定时上报事件
        {
            m_scanData[m_scanLen] = *((float*)(event->data));
            m_MeasureTemp[m_scanLen] = m_thermostatTempArray[1];  //温度补偿跟踪
            m_EnvTemp[m_scanLen] = m_temperature.environmentTemp;  //环境温度跟踪
            m_pressture[m_scanLen] = m_pressureArray[2];    //载气压力跟踪
            m_scanLen++;   //勿在定时器中修改索引
            if(m_scanLen > DATA_MAX_LENGTH-1)
            {
                m_scanLen = 0;
                memset(m_scanData, 0, sizeof(m_scanData));
                memset(m_MeasureTemp, 0, sizeof(m_MeasureTemp));
                memset(m_EnvTemp, 0, sizeof(m_EnvTemp));
                memset(m_pressture, 0, sizeof(m_pressture));
            }
            m_timeOutLen = m_scanLen;

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

void TOCDriveController::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        //重置光学定量AD上传周期
        try
        {

//            this->IOpticalMeter->SetOpticalADNotifyPeriod(0);
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
//            this->ITemperatureControl->SetTemperatureNotifyPeriod(0);
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
//            this->IOpticalAcquire->SetAcquireADNotifyPeriod(0);
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

void TOCDriveController::SetBoxFanEnable(bool enable)
{
    m_isEnable = enable;
    if(enable)
    {
        m_isBoxFanRunning = false;
    }
}

void TOCDriveController::BoxFanControlSlot(float temp)
{
//    if (m_isEnable && m_insideEnvironmentTemp > 0.1)
//   {
//        if (temp - m_insideEnvironmentTemp > 0.1)
//        {
//           ITemperatureControl->BoxFanSetOutputForTOC(1, 1); //打开上机箱风扇
////           ITemperatureControl->BoxFanSetOutputForTOC(0, 1); //打开上机箱风扇
//           m_isBoxFanRunning = true;
//    //       logger->debug("打开机箱风扇");
//        }
//        else if (m_insideEnvironmentTemp - temp > 0.5)
//        {
//            ITemperatureControl->BoxFanSetOutputForTOC(1, 0); //关上机箱风扇
////            ITemperatureControl->BoxFanSetOutputForTOC(0, 0); //关下机箱风扇
//           m_isBoxFanRunning = false;
//    //       logger->debug("关闭机箱风扇");
//        }
//    }
}

void TOCDriveController::ExBoxFanControlSlot(float temp)
{
//    if (m_isEnable && m_ExInsideEnvironmentTemp > 0.1)
//   {
//        if (temp - m_ExInsideEnvironmentTemp > 0.1)
//        {
////           ITemperatureControl->BoxFanSetOutputForTOC(1, 1); //打开上机箱风扇
//           ITemperatureControl->BoxFanSetOutputForTOC(0, 1); //打开下机箱风扇
//           m_isBoxFanRunning = true;
//    //       logger->debug("打开机箱风扇");
//        }
//        else if (m_ExInsideEnvironmentTemp - temp > 0.5)
//        {
////            ITemperatureControl->BoxFanSetOutputForTOC(1, 0); //关上机箱风扇
//            ITemperatureControl->BoxFanSetOutputForTOC(0, 0); //关下机箱风扇
//           m_isBoxFanRunning = false;
//    //       logger->debug("关闭机箱风扇");
//        }
//    }
}

void TOCDriveController::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        //重置光学定量AD上传周期
        try
        {

//            this->IOpticalMeter->SetOpticalADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.what());
        }

        //重置温度上传周期
        try
        {
//            this->ITemperatureControl->SetTemperatureNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("TOCDriveController::ResetHandler() => %s", e.what());
        }

        //重置电极AD上传周期
        try
        {
//            this->IOpticalAcquire->SetAcquireADNotifyPeriod(1);
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

void TOCDriveController::ClearThermostatRemainEvent()const
{
    logger->debug("{ClearThermostatRemainEvent}");
//    try
//    {
//        DscpAddress addr = ITemperatureControl->GetAddress();
//        for (int i = 0; i < 10; i++)
//        {
//            EventHandler::Instance()->Expect(addr,DSCP_EVENT_TCI_THERMOSTAT_RESULT, 200);
//        }
//    }
//    catch(CommandTimeoutException e)
//    {
//    }
//    catch(ExpectEventTimeoutException e)
//    {
//    }
//    catch (std::exception e)
//    {

//    }
}

void TOCDriveController::ClearPumpRemainEvent()const
{
    logger->debug("{ClearPumpRemainEvent}");
//    try
//    {
//        DscpAddress addr = IPeristalticPump->GetAddress();
//        for (int i = 0; i < 10; i++)
//        {
//            EventHandler::Instance()->Expect(addr,DSCP_EVENT_PPI_PUMP_RESULT, 200);
//        }
//    }
//    catch(CommandTimeoutException e)
//    {
//    }
//    catch(ExpectEventTimeoutException e)
//    {
//    }
//    catch (std::exception e)
//    {

//    }
}

void TOCDriveController::ClearAllRemainEvent()const
{
    logger->debug("{ClearAllRemainEvent}");
    EventHandler::Instance()->EmptyEvents();    //清空事件池
}


void TOCDriveController::Filter(const double *buf, double *fbuf, int length, int filterStep, int throwNum)const
{
    double averBuf[filterStep*2];

    for (int i = filterStep; i < length - filterStep; ++i)
    {
        double sum = 0;
        for(int j =0 ;j< filterStep*2-1;j++) //滤波数组赋值
        {
            averBuf[j] = buf[j+i-filterStep];
        }
        for(int j =0 ;j< filterStep*2-1;j++) //滤波数组冒泡排序
        {
            for(int k =0 ;k< filterStep*2-j-1;k++)
            {
                if(averBuf[k+1]< averBuf[k]) //正序
                {
                    double temp = averBuf[k+1];
                    averBuf[k+1] = averBuf[k];
                    averBuf[k] = temp;
                }
            }
        }

        for(int i = 0; i<filterStep*2-1;i++)
        {
            if(i>=throwNum && i < filterStep*2-throwNum) //过滤前后各4个
            {
                sum +=  averBuf[i];
            }
        }
        fbuf[i] = sum/(filterStep*2-throwNum*2);

    }
}

}
