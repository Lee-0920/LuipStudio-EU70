#include<QDebug>
#include "Log.h"
#include "Common.h"
#include "CurrentResultManager.h"
#include "System/Types.h"
#include "LuaEngine/LuaEngine.h"
#include <QProcess>
#include "checkscreen.h"

//#include "Interface/InterfaceManager.h"

using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;

namespace Interface
{
namespace Current
{
unique_ptr<CurrentResultManager> CurrentResultManager::m_instance(nullptr);
/**
 * @brief 获取全局唯一的 CurrentResultManager 对象（单间）。
 */
CurrentResultManager* CurrentResultManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CurrentResultManager);
    }
    return m_instance.get();
}
bool CurrentResultManager::Init()
{
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isADC())
    {
        m_sampleDriveFile = "";
        m_checkDriveFile = "";
        qDebug("4-20 do not need to Init");
        return true;
    }
    m_sampleDriveFile = "/dev/spidev2.0";
    m_checkDriveFile = "/dev/spidev2.1";

    CurrentLoopPtr sampleCurrentLoop(new CurrentLoop(m_sampleDriveFile));
    m_chip = sampleCurrentLoop->GetChipType();

    CurrentLoopPtr checkCurrentLoop(new CurrentLoop(m_checkDriveFile));
    m_sampleOutputer = new CurrentResultOutputer(sampleCurrentLoop);
    m_checkOutputer = new CurrentResultOutputer(checkCurrentLoop);
    m_commonOutputer = nullptr;

    m_sampleLowLimit = 0;
    m_sampleUpLimit = 0;
    m_checkLowLimit = 0;
    m_checkUpLimit = 0;

    m_sampleCurrent = 0;
    m_checkCurrent = 0;


    float sampleConsistency = 0;
    float checkConsistency = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        Table interconnection;
        luaEngine->GetLuaValue(state, "config.interconnection", interconnection);

        // 水样4-20
        interconnection.at("sampleLowLimit", m_sampleLowLimit);
        interconnection.at("sampleUpLimit", m_sampleUpLimit);

        Table measureResult;
        luaEngine->GetLuaValue(state, "status.measure.report.measure", measureResult);
        measureResult.at("consistency", sampleConsistency);

        // 核查4-20
        bool isCheckLimit = false;
        isCheckLimit = interconnection.safe_at("checkLowLimit", m_checkLowLimit);

        if (isCheckLimit == false)
        {
            m_checkLowLimit =  m_sampleLowLimit;
            m_checkUpLimit =  m_sampleUpLimit;
            checkConsistency = sampleConsistency;
        }
        else
        {
            interconnection.safe_at("checkUpLimit", m_checkUpLimit);

            Table checkResult;
            luaEngine->GetLuaValue(state, "status.measure.report.check", checkResult);
            checkResult.at("consistency", checkConsistency);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CurrentResultManager::Init => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CurrentResultManager::Init => %s", e.what());
    }

    this->OutputSample(sampleConsistency);

    return true;
}

void CurrentResultManager::SetSampleRange(float min, float max)
{
    m_sampleLowLimit = min;
    m_sampleUpLimit = max;
}

void CurrentResultManager::SetCheckRange(float min, float max)
{
    m_checkLowLimit = min;
    m_checkUpLimit = max;
}

void CurrentResultManager::OutputCheck(float consistency)
{
}

void CurrentResultManager::OnMeasureResultUpdated()
{
}

void CurrentResultManager::OutputSample(float consistency)
{
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isADC())
    {
        return;
    }

    if (m_sampleLowLimit - consistency > PRECISE)
    {
        consistency = m_sampleLowLimit;
    }
    else if (consistency  - m_sampleUpLimit > PRECISE)
    {
        consistency = m_sampleUpLimit;
    }

    m_sampleCurrent = m_commonOutputer->ComputeCurrent(m_sampleLowLimit, m_sampleUpLimit, consistency);
    this->OutPut(OutputType::sample);

}

void CurrentResultManager::OutputSampleCurrent(float current)
{
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isADC())
    {
        return;
    }

    m_sampleCurrent = current;
    this->OutPut(OutputType::sample);

}

void CurrentResultManager::OutputCheckCurrent(float current)
{
}

void CurrentResultManager::OutPut(OutputType type)
{
    if (true == CheckScreen::Instance()->isVNC() || false == CheckScreen::Instance()->isADC())
    {
        return;
    }

    if (m_sampleOutputer && type == OutputType::sample)
    {
        m_sampleOutputer->Output(m_sampleCurrent);
    }

    if (m_checkOutputer && type == OutputType::check)
    {
        m_checkOutputer->Output(m_checkCurrent);
    }

}

String CurrentResultManager::getSampleDriveFile()
{
    return m_sampleDriveFile;
}

String CurrentResultManager::getCheckDriveFile()
{
    return m_checkDriveFile;
}


}
}
