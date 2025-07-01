#include "ADCDetecter.h"
#include <vector>
#include <QDebug>
#include <QFile>
#include "Log.h"
#include "Common.h"
#include "ADC.h"
#include<fstream>

using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;
using Configuration::CompareType;

namespace System
{

bool AscendingSort(const int &v1, const int &v2);

ADCDetecter::ADCDetecter(int index):m_isDetected(false),m_isRuning(false),m_adcDetectIndex(index)
{
    m_filterNum = 10;
    m_intervalTime = 100;
    m_waitTime = 1000;
    m_limitAD = 4095;
    m_compareType = CompareType::EQ;
}

bool ADCDetecter::Init()
{
#ifdef _DEV_USING_CONTROL
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        String handleTablePath = QString("setting.adc[%1]").arg(m_adcDetectIndex).toStdString();

        Table handleTable;
        luaEngine->GetLuaValue(state, handleTablePath, handleTable);

        handleTable.safe_at("fileName", m_fileName);

        QFile file(QString(m_fileName.c_str()));
        if (!file.exists())
        {
            logger->debug("%s is not exists", m_fileName.c_str());
            return false;
        }

        handleTable.safe_at("filterNum", m_filterNum);
        handleTable.safe_at("intervalTime", m_intervalTime);
        handleTable.safe_at("waitTime", m_waitTime);
        handleTable.safe_at("limitAD", m_limitAD);
        handleTable.safe_at("compareType", m_compareType);
        handleTable.safe_at("detectHandle", m_detectHandleFunc);

        return true;
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ADCDetecter::Init => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ADCDetecter::Init => %s", e.what());
    }
#endif
    return false;
}


bool ADCDetecter::Start()
{
    system("echo 23 > /sys/class/gpio/export");
    system("echo in > /sys/class/gpio/gpio23/direction");
    m_isRuning = true;
    start();
    return true;
}

void ADCDetecter::Stop()
{
    m_isRuning = false;
}

void ADCDetecter::run()
{
    logger->debug("ADCDetecter %d Thread ID = %d", m_adcDetectIndex, QThread::currentThreadId());
    while(m_isRuning)
    {
//        this->Detect();
        using namespace std;
        string c;
        fstream inFile;
        inFile.open("/sys/class/gpio/gpio23/value");
        inFile >> c;

        if(c == "0")
        {
            try
            {
                lua_State * newState = LuaEngine::Instance()->GetThreadState();
                OOLUA::Lua_function call(newState);

                call(m_detectHandleFunc);
            }
            catch(OOLUA::Exception e)
            {
                logger->warn("ADCManager::Detect => %s", e.what());
            }
            catch(std::exception e)
            {
                logger->warn("ADCManager::Detect => %s", e.what());
            }
        }
        QThread::msleep(m_waitTime);
    }
}

void ADCDetecter::Detect()
{
#ifdef _DEV_USING_CONTROL

    int i = m_filterNum;
    std::vector<Uint16> adValues;

    while(i--)
    {
        ADC adc(m_fileName);
        int val = adc.ReadValue();
        adValues.push_back(val);
//        logger->debug("ADC %d val = %d", m_adcDetectIndex, val);
//        qDebug("ADC %d val = %d", m_adcDetectIndex, val);
        QThread::msleep(m_intervalTime);
    }

    if (!adValues.empty())
    {
        std::sort(adValues.begin(), adValues.end(), AscendingSort);

        adValues.erase(adValues.begin());
        adValues.pop_back();

        Uint16 total = 0;
        for (std::vector<Uint16>::iterator iter = adValues.begin(); iter != adValues.end(); iter++)
        {
            total += *iter;
        }

        Uint16 avg = total/adValues.size();

//        logger->debug("ADC %d avg = %d, limitAD = %d", m_adcDetectIndex, avg, m_limitAD);
//        qDebug("ADC %d avg = %d, limitAD = %d", m_adcDetectIndex, avg, m_limitAD);

        bool check = false;
        switch(m_compareType)
        {
            case CompareType::EQ:
                if(avg == m_limitAD)
                {
                    check = true;
                }
                break;
            case CompareType::NE:
                if(avg != m_limitAD)
                {
                    check = true;
                }
                break;
            case CompareType::LE:
                if(avg <= m_limitAD)
                {
                    check = true;
                }
                break;
            case CompareType::LT:
                if(avg < m_limitAD)
                {
                    check = true;
                }
                break;
            case CompareType::GE:
                if(avg >= m_limitAD)
                {
                    check = true;
                }
                break;
            case CompareType::GT:
                if(avg > m_limitAD)
                {
                    check = true;
                }
                break;
            default:
                break;
        }

        if (check)
        {
            logger->debug("ADC %d Detect ==> avg = %d, limitAD = %d", m_adcDetectIndex, avg, m_limitAD);
            qDebug("ADC %d Detect ==> avg = %d, limitAD = %d", m_adcDetectIndex, avg, m_limitAD);

            if (!m_isDetected)
            {
                m_isDetected = true;

                try
                {
                    lua_State * newState = LuaEngine::Instance()->GetThreadState();
                    OOLUA::Lua_function call(newState);

                    call(m_detectHandleFunc);
                }
                catch(OOLUA::Exception e)
                {
                    logger->warn("ADCManager::Detect => %s", e.what());
                }
                catch(std::exception e)
                {
                    logger->warn("ADCManager::Detect => %s", e.what());
                }
            }
        }
        else
        {
            if (m_isDetected)
            {
                m_isDetected = false;
            }
        }
    }
#endif
}

bool AscendingSort(const int &v1, const int &v2)
{
    return v1 < v2;   //升序排列
}

}


