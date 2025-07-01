#include <iostream>
#include "InstrumentAgentHandler.h"
#include "SignalManager/SignalManager.h"
#include "WqimcManager.h"
#include "oolua.h"
#include "LuaException.h"
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "Common.h"
#include <QDateTime>
#include <QString>
#include "UI/Frame/UpdateWidgetManager.h"
#include <QtNetwork>
#include <QUrl>
#include <QFile>
#include "HttpClient.h"
#include "FlowManager/FlowManager.h"
#include "System/SetSystemTime.h"
#include "Controller/ControllerManager.h"
#include "StatusManager/StatusManager.h"
#include "Setting/SettingManager.h"
#include "Setting/Environment.h"
#include "System/AuthorizationManager.h"
#include "UI/System/ScreenCalibrate.h"
#include "System/Uncompress.h"
#include "FlowManager/MeasureScheduler.h"
#include "StatusManager/StatusManager.h"
#include "StatusManager/RunStatus.h"
#include <algorithm>
#include "UI/Setting/ParameterWidget.h"

using namespace std;
using namespace apache::thrift;
using namespace Interface::Wqimc::Api::Agent;
using namespace OOLUA;
using namespace Lua;
using namespace Configuration;
using namespace System;
using namespace UI;
using namespace Result;
using namespace ResultData;
using namespace Flow;
using namespace Measure;

namespace Interface {
namespace Wqimc {

#define POSTLOGFILEREQUESTURL    ("http://192.168.1.53/WQIMC-WEB/instrumentFileAction!uploadLogFile.action")
#define POSTROOTFSFILEREQUESTURL ("http://192.168.1.53/WQIMC-WEB/instrumentFileAction!instrumentUploadFileToWeb.action")

void InstrumentAgentHandler::echo(std::string& _return, const std::string& data)
{
//    cout << "HANDLE: echo(" << data << ")" << endl;
    _return = data + "-REPLY";
}

bool InstrumentAgentHandler::upgrade(const std::string& type, const std::string& newVersion, const std::string& url)
{
    logger->debug("云平台: 升级类型：%s，版本号：%s, url：%s", type.c_str(), newVersion.c_str(), url.c_str());
    //判断当前仪器是否空闲
    if (!Flow::FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        return false;
    }
    return WqimcManager::Instance()->upgrade(type, newVersion, url);
}

void InstrumentAgentHandler::getSignals(std::vector<Signal> & _return)
{
//    logger->debug("云平台:获取信号");
    std::map<String, double>signalMap = SignalManager::Instance()->GetAllSignal();
    for (std::map<String, double>::iterator iter = signalMap.begin(); iter != signalMap.end(); ++iter)
    {
        Signal signal;
        signal.__set_name(iter->first);
        signal.__set_value(iter->second);
        signal.__set_format(WqimcManager::Instance()->GetSignalFormat(iter->first));
        _return.push_back(signal);
    }
}

bool InstrumentAgentHandler::setSignalUpdateCycle(const double cycleSec)
{
    logger->debug("云平台:设置信号上传周期 %f 秒", cycleSec);
    return WqimcManager::Instance()->setSignalUpdateCycle(cycleSec);
}

bool InstrumentAgentHandler::changeConfigs(const std::vector<Config> & configs)
{
   LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State *lua = luaEngine->GetThreadState();
    Lua_function call(lua);
    ConfigVector configVector;
    bool updateScheduler = false;
    for (std::vector<Config>::const_iterator iter = configs.begin(); iter != configs.end(); ++iter)
    {
        const Config &config = *iter;
        if (config.profile != "hardwareParamIterms")
        {
            Table  viewTable;
            luaEngine->GetLuaValue(lua, String("setting.ui.profile." + config.profile), viewTable);
            bool isCheckFinish = false;
            UI::DataType configType;
            oolua_ipairs(viewTable)//找到检查函数检查,类别
            {
                Table typeTable;
                OOLUA::pull(lua, typeTable);
                oolua_ipairs(typeTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(lua, itermTable);

                    String name;
                    itermTable.at("name", name);
                    if (name == config.config)//判断是不是该子项
                    {
                        itermTable.at("type", configType);
                        OOLUA::Lua_func_ref checkValueFunc;
                        if (itermTable.safe_at("checkValue", checkValueFunc))
                        {
                            String checkValueResult;
                            call(checkValueFunc, config.value);//检查值是否符合规则
                            OOLUA::pull(lua, checkValueResult);
                            if (configType == UI::DataType::Float || configType == UI::DataType::Double)
                            {
                                QString checkvalue = checkValueResult.c_str();
                                QString setValue = config.value.c_str();

                                if (qAbs(checkvalue.toFloat() - setValue.toFloat() ) > PRECISE)
                                {
                                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                                  , config.config.c_str(), config.value.c_str());
                                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                                    if (configVector.Size() > 0)
                                    {
                                        WqimcManager::Instance()->updateConfigs(configVector);
                                    }
                                    return false;
                                }
                            }
                            else if (configType == UI::DataType::IntArray)
                            {
                                if (checkValueResult != config.value && config.value != ",")//检查失败退出
                                {
                                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                                  , config.config.c_str(), config.value.c_str());
                                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                                    if (configVector.Size() > 0)
                                    {
                                        WqimcManager::Instance()->updateConfigs(configVector);
                                    }
                                    return false;
                                }
                            }
                            else
                            {
                                if (checkValueResult != config.value)//检查失败退出
                                {
                                    logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                                  , config.config.c_str(), config.value.c_str());
                                    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                                    if (configVector.Size() > 0)
                                    {
                                        WqimcManager::Instance()->updateConfigs(configVector);
                                    }
                                    return false;
                                }
                            }
                        }
                        isCheckFinish = true;
                        break;
                    }
                }
                oolua_ipairs_end()
                if (isCheckFinish)
                {
                    break;
                }
            }
            oolua_ipairs_end()
            if (isCheckFinish)
            {
                if (configType == UI::DataType::IntArray)
                {
                    QString tmpPattern("^(([0-1]?\\d|2[0-3]),)+|,$");  //检查数字是否合法
                    QRegExp tmpRx(tmpPattern);
                    if(false == tmpRx.exactMatch(config.value.c_str()))
                    {
                        logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                      , config.config.c_str(), config.value.c_str());
                        UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                        if (configVector.Size() > 0)
                        {
                            WqimcManager::Instance()->updateConfigs(configVector);
                        }
                        return false;
                    }
//                    if (config.config == "measure.timedPoint")
                    {
                        Table timedPoinTtable;
                        luaEngine->GetLuaValue(lua, String("config." + config.profile + "." + config.config), timedPoinTtable);

                        bool result = false;
                        oolua_ipairs(timedPoinTtable)
                        {
                            lua_pop(lua, 1);//把键为_i_index_的值弹出栈
                            OOLUA::push(lua, result);//把需要给键_i_index_赋值的值压人栈
                            //把栈顶的值赋给索引_oolua_array_index_指向的表（即m_timedPoinTtable）的键_i_index_
                            lua_rawseti(lua, _oolua_array_index_, _i_index_);
                        }
                        oolua_ipairs_end()
                        if (config.value != ",")
                        {
                            QString value(config.value.c_str());
                            QStringList timePointList = value.split(QRegExp("[,]"));
                            for (int index = 0; index < timePointList.size(); index++)
                            {
                                QString str = timePointList.at(index);
                                if (str != "")
                                {
                                    int point = str.toInt();
                                    if(index < 24)
                                    {
                                        timedPoinTtable.set(point + 1, true);
                                    }
                                }
                            }
                        }
                    }
                }
                else if (configType == UI::DataType::IP)
                {
                    QString tmpPattern("^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");  //检查数字是否合法
                    QRegExp tmpRx(tmpPattern);
                    if(false == tmpRx.exactMatch(config.value.c_str())
                            &&  false == QUrl::fromUserInput(config.value.c_str()).isValid())
                    {
                        logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                      , config.config.c_str(), config.value.c_str());
                        UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                        if (configVector.Size() > 0)
                        {
                            WqimcManager::Instance()->updateConfigs(configVector);
                        }
                        return false;
                    }
                    String chunk = "config." + config.profile + "." + config.config + "= \"" + config.value + "\"";
                    OOLUA::run_chunk(lua, chunk);
                }
                else if (configType == UI::DataType::String)
                {
                    String chunk = "config." + config.profile + "." + config.config + "= \"" + config.value + "\"";
                    OOLUA::run_chunk(lua, chunk);
                }
                else
                {
                    String chunk = "config." + config.profile + "." + config.config + "=" + config.value;//设置内存的值
                    OOLUA::run_chunk(lua, chunk);
                }

                OOLUA::Lua_func_ref saveFile;//写入到文件
                viewTable.at("saveFile", saveFile);
                call(saveFile, false);
                Api::Manager::Config  setConfig;
                setConfig.__set_profile(config.profile);
                setConfig.__set_config(config.config);
                setConfig.__set_value(config.value);
                configVector.Push(setConfig);
            }
            if(config.profile == "scheduler")
            {
                updateScheduler = true;
            }
            logger->debug("云平台:修改设置参数profile：%s， config：%s， value：%s， status: %d", config.profile.c_str()
                          , config.config.c_str(), config.value.c_str(), isCheckFinish);
        }
        else
        {
           /*
            Table viewTable;
            luaEngine->GetLuaValue(lua, "setting.ui.profile.hardwareParamIterms", viewTable);
            bool isUpdater = false;
            oolua_ipairs(viewTable)//类别
            {
                Table typeTable;
                OOLUA::pull(lua, typeTable);
                OOLUA::Lua_func_ref hardwareCategorySet;
                OOLUA::Lua_func_ref hardwareCategoryGet;

                typeTable.at("set", hardwareCategorySet);
                typeTable.at("get", hardwareCategoryGet);

                oolua_ipairs(typeTable)//子项
                {
                    Table itermTable;
                    OOLUA::Lua_func_ref hardwareItemSet;
                    OOLUA::Lua_func_ref hardwareItemGet;
                    OOLUA::Lua_func_ref checkValueFunc;

                    OOLUA::pull(lua, itermTable);
                    itermTable.at("set", hardwareItemSet);
                    itermTable.at("get", hardwareItemGet);
                    itermTable.at("checkValue", checkValueFunc);

                    String name;
                    itermTable.at("name", name);
                    if (name == config.config)//判断是不是该子项
                    {
                        String checkValueResult;
                        call(checkValueFunc, config.value);//检查值是否符合规则
                        OOLUA::pull(lua, checkValueResult);
                        if (checkValueResult != config.value)//检查失败退出
                        {
                            logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] value格式错误。", config.profile.c_str()
                                          , config.config.c_str(), config.value.c_str());
                            UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                            if (configVector.Size() > 0)
                            {
                                WqimcManager::Instance()->updateConfigs(configVector);
                            }
                            return false;
                        }

                        float value =  QString(config.value.c_str()).toFloat();//设置该值到下位机
                        call(hardwareItemSet, value);
                        call(hardwareCategorySet);
                        String str;
                        bool ret = false;
                        OOLUA::pull(lua, str);
                        OOLUA::pull(lua, ret);
                        if (ret == false)//设置失败退出
                        {
                            logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] 通信异常。", config.profile.c_str()
                                          , config.config.c_str(), config.value.c_str());
                            UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                            if (configVector.Size() > 0)
                            {
                                WqimcManager::Instance()->updateConfigs(configVector);
                            }
                            return false;
                        }
//                        QThread::msleep(250);
                        call(hardwareCategoryGet);//检查下位机的值是否等于设置的值
                        call(hardwareItemGet);
                        float result;
                        OOLUA::pull(lua, result);
                        if (qAbs(result - value) > PRECISE)//检查失败退出
                        {
                            logger->debug("云平台:设置参数profile：[%s]， config：[%s]， value：[%s] 无法设置到下位机。", config.profile.c_str()
                                          , config.config.c_str(), config.value.c_str());
                            UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
                            if (configVector.Size() > 0)
                            {
                                WqimcManager::Instance()->updateConfigs(configVector);
                            }
//                            return false;
                        }
                        else
                        {
                            Api::Manager::Config  setConfig;
                            setConfig.__set_profile(config.profile);
                            setConfig.__set_config(config.config);
                            setConfig.__set_value(config.value);
                            configVector.Push(setConfig);
                            logger->debug("云平台:修改设置参数profile：%s， config：%s， value：%s 成功", config.profile.c_str()
                                          , config.config.c_str(), config.value.c_str());
                        }
                        isUpdater = true;
                        break;
                    }
                }
                oolua_ipairs_end()
                if (isUpdater)
                {
                    break;
                }
            }
            oolua_ipairs_end()*/

            WqimcManager::Instance()->ChangeHardwareParam(config);
        }
    }
    if(updateScheduler == true)
    {
        MeasureScheduler::Instance()->Update();
    }

    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::WqimcChangeParam);
    return true;
}

bool InstrumentAgentHandler::changeSystemTime(const int64_t time)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(time);
    logger->debug("云平台:设置系统时间 %lld", time);
    System::SetSystemTime::SetTime(dateTime);
    return true;
}

bool InstrumentAgentHandler::restore(const Interface::Wqimc::Api::Agent::RoleType::type privilege, const std::string& profileName)
{
    lua_State *lua = LuaEngine::Instance()->GetThreadState();
    Lua_function call(lua);
    Lua_func_ref defaultRestore;

    if (profileName != "")
    {
        string tableName = "setting.ui.profile." + profileName + ".defaultRestore";
        LuaEngine::Instance()->GetLuaValue(lua, tableName, defaultRestore);
    }
    else
    {
        LuaEngine::Instance()->GetLuaValue(lua, "setting.ui.factoryTime.defaultRestore", defaultRestore);
    }
    UI::RoleType userType = (UI::RoleType)privilege;
    call(defaultRestore, userType);
    logger->debug("云平台:一键恢复出厂设置，命令权限：%d ,表名：%s", privilege, profileName.c_str());
    UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::ChangeConfigParam);
    return true;
}

bool InstrumentAgentHandler::reboot()
{
    logger->debug("云平台:重启应用程序");
    WqimcManager::Instance()->reboot();
    return true;
}

bool InstrumentAgentHandler::execute(const Operation& op)
{
    String params;
    for (std::vector<String>::const_iterator iter = op.params.begin(); iter != op.params.end(); iter++)
    {
        params += *iter;
        params += ",";
    }
    logger->debug("云平台: 操作类：%s，操作项：%s, 参数：%s", op.suit.c_str(), op.name.c_str(), params.c_str());

    lua_State *lua = LuaEngine::Instance()->GetThreadState();
    LuaEngine* luaEngine = LuaEngine::Instance();
    Lua_function call(lua);

    Table itermsTable;
    luaEngine->GetLuaValue(lua, "setting.ui.operation", itermsTable);
    bool  isMatching = false;
    oolua_pairs(itermsTable)//界面
    {
        Table  viewTable;
        OOLUA::pull(lua, viewTable);
        String suit;
        viewTable.at("name", suit);
        if (suit == op.suit)
        {
            if (suit == "maintain")
            {
                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(lua, itermTable);
                    String name;
                    itermTable.at("name", name);
                    if (name == op.name)
                    {
                        OOLUA::Lua_func_ref createFlow;
                        itermTable.at("createFlow", createFlow);
                        //判断当前仪器是否空闲
                        if (!Flow::FlowManager::Instance()->IsFlowEnable())  // 空闲状态
                        {
                            return false;
                        }

                        if (!Flow::FlowManager::Instance()->IsComValid())
                        {
                            return false;
                        }

                        if (!Flow::FlowManager::Instance()->IsAuthorize())  // 运行权限
                        {
                            return false;
                        }

                        qDebug("%d", call(createFlow));
                        Flow::FlowManager::Instance()->StartFlow();
                        logger->debug("云平台: 操作类：%s，操作项：%s 启动成功", op.suit.c_str(), op.name.c_str());
                        isMatching = true;
                        break;
                    }
                }
                oolua_ipairs_end()
            }
            else if (suit == "liquidOperator")
            {
                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(lua, itermTable);
                    String name;
                    itermTable.at("name", name);
                    if (name == op.name)
                    {
                        if ( op.params.size() < 2)
                        {
                            logger->debug("云平台: 管道操作 参数无效 参数不足两个");
                            return false;
                        }

                        QRegExp integerRx("^[0-9]+$");
                        QRegExp decimalRx("^[0-9]*(\\.[0-9]*)?$");
                        QString param1, param2;
                        param1 = op.params.at(0).c_str();
                        param2 = op.params.at(1).c_str();

                        if (false == integerRx.exactMatch(param1) ||
                                false == decimalRx.exactMatch(param2))
                        {
                            logger->debug("云平台: 管道操作 type[%s], vol[%s] 参数无效 ", param1.toLatin1().data(), param2.toLatin1().data());
                            return false;
                        }

                        int type = QString(op.params.at(0).c_str()).toInt();
                        float vol = QString(op.params.at(1).c_str()).toFloat();
                        OOLUA::Lua_func_ref checkvalueFunc;
                        viewTable.at("checkvalue", checkvalueFunc);

                        String ret = op.params.at(1);
                        call(checkvalueFunc, type, ret);
                        String result;
                        OOLUA::pull(lua, result);
                        QString checkvalue = result.c_str();
                        if (qAbs(checkvalue.toFloat() - vol) > PRECISE)
                        {
                            logger->debug("云平台: 管道操作 type[%d], vol[%s] 参数无效,体积设为 %s", type, ret.c_str(), result.c_str());
                            return false;
                        }
                        OOLUA::Lua_func_ref createFlow;
                        itermTable.at("createFlow", createFlow);
                        //判断当前仪器是否空闲
                        if (!Flow::FlowManager::Instance()->IsFlowEnable())  // 空闲状态
                        {
                            return false;
                        }

                        if (!Flow::FlowManager::Instance()->IsComValid())
                        {
                            return false;
                        }
                        qDebug("%d, type = %d, vol = %f", call(createFlow, type, vol), type, vol);
                        Flow::FlowManager::Instance()->StartFlow();
                        isMatching = true;
                        break;
                    }
                }
                oolua_ipairs_end()
            }
            else if (suit == "hardwareTest")
            {
                oolua_ipairs(viewTable)//类别
                {
                    Table typeTable;
                    OOLUA::pull(lua, typeTable);
                    oolua_ipairs(typeTable)//子项
                    {
                        Table itermTable;
                        OOLUA::pull(lua, itermTable);

                        String name;
                        itermTable.at("name", name);
                        if (name == op.name)
                        {
                            if (Controller::ControllerManager::Instance()->IsAllConnected() == false ||
                                (Measure::StatusManager::Instance()->IsIdleStatus() == false &&
                                 Measure::StatusManager::Instance()->GetStatus().name != QObject::tr("HardwareTest").toStdString()))
                            {
                                logger->debug("云平台：硬件测试：当前状态不允许启动");
                                return false;
                            }
                            String action = op.params.at(0);
                            OOLUA::Lua_func_ref createFlow;
                            itermTable.at("createFlow", createFlow);
                            bool result;
                            if (action == "1")
                            {
                                call(createFlow, true);
                            }
                            else if (action == "0")
                            {
                                call(createFlow, false);
                            }
                            else
                            {
                                logger->debug("云平台：硬件测试：param[%s] 参数错误");
                                return false;
                            }
                            OOLUA::pull(lua, result);

                            if(result == true)
                            {
                                String message = name + ',' + action + ',';
                                UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::StartHardwareTestSuccess, message);
                                isMatching = true;
                                break;
                            }
                            else
                            {
                                logger->debug("云平台: 硬件测试 操作项：%s 命令：%s 启动不成功", op.name.c_str(), params.c_str());
                                isMatching = false;
                                return false;
                            }
                        }
                    }
                    oolua_ipairs_end()
                    if (isMatching == true)
                    {
                        break;
                    }
                }
                oolua_ipairs_end()
            }
            else if (suit == "combineOperator")
            {
                oolua_ipairs(viewTable)//子项
                {
                    Table itermTable;
                    OOLUA::pull(lua, itermTable);
                    String name;
                    itermTable.at("name", name);
                    if (name == op.name)
                    {
                        if ( op.params.size() < 1)
                        {
                            logger->debug("云平台: 组合操作 无参数");
                            return false;
                        }


                        QRegExp decimalRx("^[0-9]*(\\.[0-9]*)?$");
                        QString param1;
                        param1 = op.params.at(0).c_str();

                        if (false == decimalRx.exactMatch(param1))
                        {
                            logger->debug("云平台: 组合操作 vol[%s] 参数无效 ", param1.toLatin1().data());
                            return false;
                        }

                        int type = 0;
                        itermTable.at("mode", type);
                        float vol = QString(op.params.at(0).c_str()).toFloat();
                        OOLUA::Lua_func_ref checkvalueFunc;
                        viewTable.at("checkvalue", checkvalueFunc);

                        String volStr = op.params.at(0);
                        call(checkvalueFunc, type, volStr);
                        String result;
                        OOLUA::pull(lua, result);
                        QString checkvalue = result.c_str();
                        if (qAbs(checkvalue.toFloat() - vol) > PRECISE)
                        {
                            logger->debug("云平台: 管道操作 type[%d], vol[%s] 参数无效,体积设为 %s", type, volStr.c_str(), result.c_str());
                            return false;
                        }
                        OOLUA::Lua_func_ref createFlow;
                        itermTable.at("createFlow", createFlow);
                        //判断当前仪器是否空闲
                        if (!Flow::FlowManager::Instance()->IsFlowEnable())  // 空闲状态
                        {
                            return false;
                        }

                        if (!Flow::FlowManager::Instance()->IsComValid())
                        {
                            return false;
                        }
                        qDebug("%d, type = %d, vol = %f", call(createFlow, type, vol), type, vol);
                        Flow::FlowManager::Instance()->StartFlow();
                        isMatching = true;
                        break;
                    }
                }
                oolua_ipairs_end()
            }
        }
        if (isMatching == true)
        {
            break;
        }
    }
    oolua_pairs_end()
    return isMatching;
}

bool InstrumentAgentHandler::diagnose(const std::vector<Diagnosis> & diags)
{
    logger->debug("云平台： 诊断");
    lua_State *lua = LuaEngine::Instance()->GetThreadState();
    LuaEngine* luaEngine = LuaEngine::Instance();
    Lua_function call(lua);
    if (!Flow::FlowManager::Instance()->IsFlowEnable())  // 空闲状态
    {
        return false;
    }
    Table itermsTable;
    luaEngine->GetLuaValue(lua, "setting.ui.diagnosis", itermsTable);

    oolua_pairs(itermsTable)//界面
    {
        Table  viewTable;
        OOLUA::pull(lua, viewTable);
        String suit;
        viewTable.at("name", suit);
        if (suit == diags.at(0).suit)
        {
            if (suit == "smartDetect")
            {
                for (std::vector<Diagnosis>::const_iterator iter = diags.begin(); iter != diags.end(); ++iter)
                {
                    Diagnosis diagnosis = *iter;
                    if (diagnosis.suit != suit)//只能启动同一个界面的诊断
                    {
                        continue;
                    }
                    oolua_ipairs(viewTable)//子项
                    {
                        Table itermTable;
                        OOLUA::pull(lua, itermTable);
                        String name;
                        itermTable.at("name", name);
                        if (name == diagnosis.name)
                        {
                             Lua_func_ref createFlow;
                             itermTable.at("createFlow", createFlow);
                             call(createFlow);
                             logger->debug("云平台: 诊断类：%s，诊断项：%s", diagnosis.suit.c_str(), diagnosis.name.c_str());
                        }
                    }
                    oolua_ipairs_end()
                }
                UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::StartIntellectCheck);
                break;
            }
            else if (suit == "communicationCheck")
            {
                for (std::vector<Diagnosis>::const_iterator iter = diags.begin(); iter != diags.end(); ++iter)
                {
                    Diagnosis diagnosis = *iter;
                    if (diagnosis.suit != suit)//只能启动同一个界面的诊断
                    {
                        continue;
                    }
                    oolua_ipairs(viewTable)//子项
                    {
                        Table typeTable;
                        OOLUA::pull(lua, typeTable);
                        oolua_ipairs(typeTable)//子项
                        {
                            Table itermTable;
                            OOLUA::pull(lua, itermTable);
                            String name;
                            itermTable.at("name", name);
                            if (name == diagnosis.name)
                            {
                                 Lua_func_ref createFlow;
                                 itermTable.at("createFlow", createFlow);
                                 call(createFlow);
                                 logger->debug("云平台: 诊断类：%s，诊断项：%s", diagnosis.suit.c_str(), diagnosis.name.c_str());
                            }
                        }
                        oolua_ipairs_end()
                    }
                    oolua_ipairs_end()
                }
                UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::StartCommunicationTest);
                break;
            }
        }
    }
    oolua_pairs_end()
    return true;
}

bool InstrumentAgentHandler::stop()
{
    logger->debug("云平台: 停止流程");

    if(StatusManager::Instance()->GetStatus().name == QObject::tr("HardwareTest").toStdString())
    {
        UpdateWidgetManager::Instance()->Update(UI::UpdateEvent::StopHardwareTest);
    }

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Configuration::MeasureMode autoMode;
    luaEngine->GetLuaValue(state, "config.scheduler.measure.mode", autoMode);
    if (autoMode == Configuration::MeasureMode::Continous)
    {
        autoMode = Configuration::MeasureMode::Trigger;
        luaEngine->SetLuaValue("config.scheduler.measure.mode", (int)autoMode);
        SettingManager::Instance()->SchedulerSave();
        UpdateWidgetManager::Instance()->Update(UpdateEvent::ChangeAutoMeasure);
    }
    return Flow::FlowManager::Instance()->StopFlow();
}

bool InstrumentAgentHandler::setAuthorization(const Authorization& authorization)
{
    logger->debug("云平台:设置授权信息:name %s, expirationDate %s", authorization.name.c_str(),
                  QDateTime::fromMSecsSinceEpoch(authorization.expirationDate).toString("yyyy-MM-dd hh:mm:ss").toLatin1().data());

    Uint32 maxTime = QDateTime::fromString("2038-01-01 00:00:00","yyyy-MM-dd hh:mm:ss").toTime_t();
    Uint32 expirationDate = QDateTime::fromMSecsSinceEpoch(authorization.expirationDate).toTime_t();
    if (expirationDate > maxTime)
    {
        return false;
    }

    AuthorizationManager::Instance()->SetAuthorization(expirationDate);

    return true;
}

bool InstrumentAgentHandler::calibrationTouchScreen()
{
    bool ret = false;
    QFile file("/usr/local/tslib/bin/ts_calibrate");
    if (file.exists())
    {
        ret = WqimcManager::Instance()->calibrationTouchScreen();
    }
    else
    {
        logger->warn("校准程序不存在");
    }
    if (ret)
    {
        logger->debug("云平台:启动触摸屏校准");
    }
    else
    {
        logger->debug("云平台:启动触摸屏校准失败");
    }
    return ret;
}

bool InstrumentAgentHandler::reintateFactorypattern()
{
    logger->debug("云平台:恢复出厂模式");
    String path = Configuration::Environment::Instance()->GetAppPath() + "/SP/Config/ProducerAuthorization.ls";
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref SaveEncryptionFile;
    luaEngine->GetLuaValue(state, "Serialization.SaveEncryptionFile", SaveEncryptionFile);

    if (QFile::exists(path.c_str()))
    {
        Table producerAuthorization;
        luaEngine->GetLuaValue(state, "config.producerAuthorization", producerAuthorization);
        producerAuthorization.set("enable", true);
        call(SaveEncryptionFile, producerAuthorization, path, "config.producerAuthorization");

        UpdateWidgetManager::Instance()->Update(UpdateEvent::RecoverInFactoryMode, "InstrumentAgentHandler");
    }
    return true;
}

bool InstrumentAgentHandler::updataMeasureData(const Interface::Wqimc::Api::Agent::MeasureData& data)
{
//    logger->debug("云平台:添加测量数据");
    String name = data.target;
    OperateRecordData* operateRecordData = ResultManager::Instance()->GetMeasureOperateRecordData(name);

    if (nullptr == operateRecordData && nullptr == ResultManager::Instance()->GetMeasureRecordFile(name))
    {
        logger->debug("云平台:测量数据名称错误 %s", name.c_str());
        return false;
    }
    RecordData recordData(ResultManager::Instance()->GetMeasureRecordFile(name)->GetRecordFields()->GetFieldsSize());

    int time = QDateTime::fromMSecsSinceEpoch(data.time).toTime_t();
    float consistency = data.result;
    String resultType = data.type;
    Byte mode = data.mode;

    operateRecordData->SetRecordDataFieldValue("dateTime", recordData, time);
    operateRecordData->SetRecordDataFieldValue("consistency", recordData, consistency);
    operateRecordData->SetRecordDataFieldValue("mode", recordData, mode);

    Byte resultTypeIndex;
    if (operateRecordData->GetRecordDataFieldEnumIndex("resultType", resultType, resultTypeIndex))
    {
        operateRecordData->SetRecordDataFieldValue("resultType", recordData, resultTypeIndex);
    }

    for (vector<MeasureItem>::const_iterator iter = data.optionals.begin(); iter != data.optionals.end(); ++iter)
    {
        string fiedName;
        const MeasureItem &measureItem = *iter;
        if (operateRecordData->GetRecordFieldName( measureItem.name, fiedName))
        {
            switch(operateRecordData->GetRecordFieldType(fiedName))
            {
             case FieldType::Bool:
                 {
                     bool ret;
                     if (measureItem.value == "是")
                     {
                         ret = true;
                     }
                     else
                     {
                         ret = false;
                     }
                     operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                 }
                 break;
             case FieldType::Byte:
                 {
                     Byte ret = QString(measureItem.value.c_str()).toInt();
                     operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                 }
                 break;
             case FieldType::Int:
                 {
                     int ret = QString(measureItem.value.c_str()).toInt();
                     operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                 }
                 break;
             case FieldType::Float:
                 {
                     float ret = QString(measureItem.value.c_str()).toFloat();
                     operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                 }
                 break;
             case FieldType::Double:
                 {
                     double ret = QString(measureItem.value.c_str()).toDouble();
                     operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                 }
                 break;
             case FieldType::Time:
                {
                    int ret = QDateTime::fromString(measureItem.value.c_str(), "yyyy-MM-dd hh:mm:ss").toTime_t();
                    operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                }
                break;
            case FieldType::Enum:
                {
                    Byte ret;
                    if (operateRecordData->GetRecordDataFieldEnumIndex(fiedName, measureItem.value, ret))
                    {
                        operateRecordData->SetRecordDataFieldValue(fiedName, recordData, ret);
                    }
                }
                break;
             case FieldType::IntArray:
                 {
                 }
                 break;
            }
        }
    }
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref OnCommonMeasureResultAdded;
    luaEngine->GetLuaValue(state, "Helper.Result.OnCommonMeasureResultAdded", OnCommonMeasureResultAdded);

    call(OnCommonMeasureResultAdded, name, resultTypeIndex, time, consistency, mode);

    if(!ResultManager::Instance()->IsExistMeasureRecord(name, recordData))
    {
        ResultManager::Instance()->AddMeasureRecord(name, recordData, false);
    }
    return true;
}

void InstrumentAgentHandler::getLogFileNames(std::vector<std::string> & _return)
{
    _return = Uncompress::GetAllFilePath(Configuration::Environment::Instance()->GetAppDataPath());
    for (std::vector<std::string>::iterator iter = _return.begin(); iter != _return.end(); iter++)
    {
        QFileInfo fi(iter->c_str());
        if (fi.suffix() != "zip")
        {
            QString zipFileName = fi.absolutePath() + "/" + fi.completeBaseName() + ".zip";
            std::vector<std::string>::iterator result = std::find(_return.begin(), _return.end( ), zipFileName.toStdString());
            if (result != _return.end())
            {
                _return.erase(result);
            }
        }
    }
}

bool InstrumentAgentHandler::catchLogFile(const std::string& logFileName, const std::string& uploadPath)
{
    logger->debug("云平台：上传 %s 文件 到 %s", logFileName.c_str(), uploadPath.c_str());
    return WqimcManager::Instance()->catchLogFile(uploadPath, logFileName, true);
}

bool InstrumentAgentHandler::cdFoleder(const std::string& filePath)
{
    (void)filePath;
    return true;
}

void InstrumentAgentHandler::getFileList(std::vector<Interface::Wqimc::Api::Agent::InstrumentFile> & _return, const std::string& filePath, const bool showHiddenFile)
{
    QDir sourceDir(filePath.c_str());
    if (showHiddenFile)
    {
        sourceDir.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System);
    }
    else
    {
        sourceDir.setFilter(QDir::AllEntries | QDir::System);
    }
    QFileInfoList fileInfoList = sourceDir.entryInfoList();

    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
//        QString str = fileInfo.fileName();
        Interface::Wqimc::Api::Agent::InstrumentFile instrumentFile;
        instrumentFile.__set_name(fileInfo.fileName().toStdString());
        if(fileInfo.isDir())
        {
//            str += " Dir";
            instrumentFile.__set_isFile(false);
        }
        else
        {
//            str += " File";
            instrumentFile.__set_isFile(true);
        }
//        str += " lastModified : " + fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
//        str += " size : " + QString::number(fileInfo.size());
//        qDebug() << str;
        instrumentFile.__set_updateDate(fileInfo.lastModified().toMSecsSinceEpoch());
        instrumentFile.__set_size(fileInfo.size());
        _return.push_back(instrumentFile);
    }
}

bool InstrumentAgentHandler::downLoadInstrumentFile(const std::string& filePath, const std::string& uploadPath)
{
    logger->debug("云平台：发送 %s 文件 到 %s", filePath.c_str(), uploadPath.c_str());
    if (QFile::exists(filePath.c_str()))
    {
        return WqimcManager::Instance()->catchLogFile(uploadPath, filePath, false);
    }
    else
    {
        logger->debug("发送 %s 失败，文件不存在", filePath.c_str());
        return false;
    }
}

bool InstrumentAgentHandler::deleteInstrumentFile(const std::string& filePath)
{
    bool ret = false;
    logger->debug("云平台：删除 %s ", filePath.c_str());
    QFileInfo fi(filePath.c_str());
    if (fi.isFile())
    {
        if (fi.exists())
        {
            if (QFile::remove(filePath.c_str()))
            {
                ret = true;
                logger->debug("文件删除成功");
            }
            else
            {
                logger->debug("文件删除失败");
            }
        }
        else
        {
            logger->debug("删除失败,文件不存在");
        }
    }
    else
    {
        if (false == Uncompress::DeleteDirectory(filePath.c_str())
            && false == fi.exists())
        {
            logger->debug("文件夹删除失败");
        }
        else
        {
            ret = true;
            logger->debug("文件夹删除成功");
        }
    }
    return ret;
}

bool InstrumentAgentHandler::uploadInstrumentFile(const std::string& filePath, const std::string& url)
{
    logger->debug("云平台：接收文件 %s , url: %s", filePath.c_str(), url.c_str());
    QFileInfo fi(filePath.c_str());
    QFileInfo dir = fi.absolutePath();
    if (false == dir.exists() ||  false == dir.isDir())
    {
        logger->debug("%s 路径不存在", fi.filePath().toStdString().c_str());
        return false;
    }
    const QUrl newUrl = QUrl::fromUserInput(url.c_str());
    if (!newUrl.isValid())
    {
        logger->debug("无效的 URL： %s：%s", url.c_str(), newUrl.errorString().toStdString().c_str());
        return false;
    }
    return WqimcManager::Instance()->Download(url, filePath);
}

bool InstrumentAgentHandler::addFolder(const std::string& filePath)
{
    logger->debug("云平台：创建文件夹 %s ", filePath.c_str());
    QDir dir(filePath.c_str());
    if (!dir.exists())
    {
        return dir.mkpath(filePath.c_str());
    }
    else
    {
        logger->debug("文件夹 %s 已存在", filePath.c_str());
        return false;
    }
}

void InstrumentAgentHandler::excuteCommand(string &_return, const string &command)
{
   logger->debug("云平台:控制台命令");
    _return = "";

   (void) command;
}

bool InstrumentAgentHandler::factorypattern()
{
    logger->debug("云平台:出厂激活");
    String path = Configuration::Environment::Instance()->GetAppPath() + "/SP/Config/ProducerAuthorization.ls";
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    Lua_function call(state);

    Lua_func_ref SaveEncryptionFile;
    luaEngine->GetLuaValue(state, "Serialization.SaveEncryptionFile", SaveEncryptionFile);

    if (QFile::exists(path.c_str()))
    {
        Table producerAuthorization;
        luaEngine->GetLuaValue(state, "config.producerAuthorization", producerAuthorization);
        producerAuthorization.set("enable", false);
        call(SaveEncryptionFile, producerAuthorization, path, "config.producerAuthorization");

        UpdateWidgetManager::Instance()->Update(UpdateEvent::ActivateOutFactoryMode, "InstrumentAgentHandler");
    }

    return true;
}

bool InstrumentAgentHandler::faultClearing()
{
    logger->debug("云平台:故障清除");
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    bool isFatalError = false;
    bool isExistFatalError = false;
    Table measureTable;
    luaEngine->GetLuaValue(state, "status.measure", measureTable);
    isExistFatalError = measureTable.safe_at("isFatalError", isFatalError);

    if(isExistFatalError && isFatalError)
    {
        logger->info("清除停机故障");
        UpdateWidgetManager::Instance()->Update(UpdateEvent::FatalErrorClear, "InstrumentAgentHandler");
    }

    bool isDeviceFault = false;
    luaEngine->GetLuaValue(state, "status.measure.isDeviceFault", isDeviceFault);
    if(isDeviceFault)
    {
        logger->info("清除故障");
        UpdateWidgetManager::Instance()->Update(UpdateEvent::FaultClear, "InstrumentAgentHandler");
    }

    return true;
}
}
}
