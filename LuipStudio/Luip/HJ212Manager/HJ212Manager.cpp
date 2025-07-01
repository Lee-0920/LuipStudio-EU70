#include "Log.h"
#include "HJ212Manager.h"
#include "LuaEngine/LuaEngine.h"
#include "System/SetSystemTime.h"
#include "ResultManager/ResultManager.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "StatusManager/StatusManager.h"
#include "Setting/Environment.h"
#include "UI/Data/TextfileParser.h"
#include <QMap>
#include <QList>
#include "Interface/HJ212/HJ212Interface.h"

#define FileName  ("RunLog.txt*")
#ifdef _CS_ARM_LINUX
#define SEPARATOR ("\n")
#else
#define SEPARATOR ("\r\n")
#endif

//using namespace std;
using namespace System;
using namespace OOLUA;
using namespace Lua;
using namespace Result;
using namespace Measure;
using UI::TextfileParser;
using Configuration::Environment;
using namespace Interface::HJ212;


namespace HJ212
{

QDateTime GetDateTimeFromLog(QString str, QString pattern)
{
    QRegExp rx(pattern);
    str.indexOf(rx);
    return QDateTime::fromString(rx.cap(1), "yyyy-MM-dd hh:mm:ss");
}

QString GetContentFromLog(QString str, QString pattern)
{
    QRegExp rx(pattern);
    str.indexOf(rx);
    return rx.cap(3);
}

QStringList SetDate(QDateTime& theMinDateTime, QDateTime& theMaxDateTime, QStringList& strlist, QString pattern)
{
    QStringList newStrlist;
    for(int i = 0; i < strlist.size() - 1 ; i++)
    {
        QString m = strlist.at(i);
        if(m.contains("[INFO]",Qt::CaseSensitive))
        {
            QRegExp rx(pattern);
            m.indexOf(rx);
            QDateTime dateTime = QDateTime::fromString(rx.cap(1), "yyyy-MM-dd hh:mm:ss");
            if(rx.cap(1).size() > 1 && rx.cap(2).size() > 1 && rx.cap(3).size() > 1)
            {
                if((rx.cap(2).trimmed() == "[INFO]:")
                    && (dateTime > theMinDateTime)
                    && (dateTime < theMaxDateTime))
                {
                    newStrlist.append(m);
                }
            }
        }
    }
    return newStrlist;
}


std::unique_ptr<HJ212Manager> HJ212Manager::m_instance(nullptr);


HJ212Manager* HJ212Manager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new HJ212Manager);
    }
    return m_instance.get();
}

HJ212Manager::HJ212Manager() :QObject(nullptr), m_resultFiles(nullptr), m_isRespond(false)
{

}

HJ212Manager::~HJ212Manager()
{
    m_platform.Stop();
}

void HJ212Manager::Init()
{
    //检查是否有212初始化函数 没有则不初始化
    if (!InitHj212Script())
    {
        return;
    }

    int hj212Type = (int)HJ212Type::None;
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212Type", hj212Type);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Manager::Init() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::Init() => %s", e.what());
    }

    if (hj212Type == (int)HJ212Type::None)
    {
        logger->debug("HJ212Manager doesn't Enable.");
        return ;
    }

    if (hj212Type == (int)HJ212Type::V2005_Wry)
    {
        logger->debug("HJ212Manager Enable 2005-Wry");
        qDebug("\nHJ212 Enable 2005-Wry");
        HJ212Interface::Instance()->Init();
        QThread::msleep(200);
        return ;
    }
    else if (hj212Type == (int)HJ212Type::V2017_Dbs)
    {
        logger->debug("HJ212Manager Enable 2017-Dbs");
        qDebug("\nHJ212 Enable 2017-CCEP");
    }
    else if (hj212Type == (int)HJ212Type::V2017_Wry)
    {
        logger->debug("HJ212Manager Enable 2017-Wry");
        qDebug("\nHJ212 Enable 2017-Wry");
    }

    InitPlatformInfo();
    InitMonitoringFactors();
    InitMonitoringInfos();

    //日志
    int index = 0;
    Log::InitHJ212Logger(index);
    m_platform.InitLog(logger, Log::GetHJ212Logger(index));

    //监测因子
    QList<MonitoringSpace::MonitoringInfo> minfos = GetMonitoringInfos();
    m_platform.SetMonitoring(m_monitoringFactor, minfos);

    //平台信息
    PlatformInfo infoPlatform = GetPlatformInfo();
    HJ212CallBack callBack = GetHJ212CallBack();
    m_platform.SetPlatformInfo(infoPlatform, callBack);

    if (!m_strMeasureRecordName.isEmpty())
    {
        m_resultFiles = ResultManager::Instance()->GetMeasureRecordFile(m_strMeasureRecordName.toStdString());
        m_readerIndex = m_resultFiles->RegisterReader();
    }

    m_mainState = LuaEngine::Instance()->GetMainState();
    m_newState = LuaEngine::Instance()->CreateThread();

    connect(this, SIGNAL(OnUpdateMessageSignal(int)), this, SLOT(OnUpdateMessageSlot(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(OnUpdateLogWithBracketsSignal(bool)), this, SLOT(OnUpdateLogWithBrackesSlot(bool)), Qt::QueuedConnection);

    //设置定时上传的数据项
    QList<UploadItem> uploadItems = GetUploadItems();
    m_platform.SetUploadItems(uploadItems);

    //启动
    if (m_hj212Type != (int)HJ212Type::None)
        m_platform.Start();

    //注册
    ResultManager::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);
    StatusManager::Instance()->Register(this);

    logger->debug("HJ212Manager Init Succeed.");
}

bool HJ212Manager::IsRespond()
{
    return m_isRespond;
}

bool HJ212Manager::GetMonitoringInfo(int index, MonitoringInfo& info)
{
    QMap<QString, MonitoringInfo>::iterator iter = m_mapMonitorInfo.begin();
    for (; iter != m_mapMonitorInfo.end(); ++iter)
    {
        if (iter->index == index)
        {
            info.deviceType = iter->deviceType;
            info.index = iter->index;
            info.infoId = iter->infoId;
            info.infoType = iter->infoType;
            info.text = iter->text;
            info.unit = iter->unit;
            return true;
        }
    }
    return false;
}

MonitoringFactor HJ212Manager::GetMonitoringFactor()
{
    return m_monitoringFactor;
}

bool HJ212Manager::StartEquipmentFlow(int flowCode)
{

    bool bStart = false;
    try
    {
//        LuaEngine* luaEngine = LuaEngine::Instance();
//        lua_State * state = luaEngine->GetThreadState();

        lua_settop(m_newState ,0);//清空栈
        lua_getglobal(m_newState, "Hj212Interface");

        int status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::StartEquipmentsFlow() => get 'Hj212Interface' table failed");
            return bStart;
        }

        lua_pushstring(m_newState , "StartEquipmentFlow");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_isfunction(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::StartEquipmentsFlow() => get 'StartEquipmentFlow' function failed");
            return bStart;
        }

        lua_pushinteger(m_newState, flowCode);

        int ret = lua_resume(m_newState, m_mainState, 1);
        if (ret != 0)
        {
            logger->warn("HJ212Manager::StartEquipmentsFlow() => lua_resume call 'StartEquipmentFlow' function failed, error code:%d", ret);
            return bStart;
        }

        status = lua_isboolean(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            bStart = lua_toboolean(m_newState ,-1);
        }
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::StartEquipmentsFlow() => %s", e.what());
    }

    return bStart;
}

QList<ZeroCheckData> HJ212Manager::GetZeroCheckData(QDateTime startTime, QDateTime endTime)
{
    QList<ZeroCheckData> lstData = {};
    QDateTime DataTime;
    int cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    String flag;           //数据标识
    float processValue;     //测量信号

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetZeroCheckData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetZeroCheckData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetZeroCheckData() => get 'GetZeroCheckData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        processValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isstring(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        SpanValue = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        standardValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-6);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    if ((DataTime >= startTime)
        && (DataTime <= endTime))
    {
            ZeroCheckData data;
            data.polId = m_monitoringFactor.polId;
            data.cDataTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
            data.checkValue = checkValue;
            data.standardValue = standardValue;
            data.SpanValue = SpanValue;
            data.flag = QString::fromStdString(flag);
            data.processValue = processValue;
            lstData.append(data);
    }

    return lstData;
}

QList<AddStandardData> HJ212Manager::GetAddStandardData(QDateTime startTime, QDateTime endTime)
{
    QList<AddStandardData> lstData = {};
    QDateTime DataTime;
    QDateTime SampleDataTime;

    int cDataTime;      //采集时间
    float checkValue;       //加标回收值
    int waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    float chroma;           //母液
    float sVolume;          //加标体积
    float dVolume;          //加样杯容值
    String flag;           //数据标识

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetAddStandardData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetAddStandardData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetAddStandardData() => get 'GetAddStandardData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isstring(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-1);//取栈顶
    }

    status = lua_isnumber(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        dVolume = lua_tonumber(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        sVolume = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        chroma = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        waterValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        waterTime = lua_tointeger(m_newState ,-6);//取栈顶
    }

    status = lua_isnumber(m_newState ,-7);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-7);//取栈顶
    }

    status = lua_isinteger(m_newState ,-8);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-8);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    SampleDataTime = QDateTime::fromTime_t(waterTime);
    if ((DataTime >= startTime)
        && (DataTime <= endTime))
    {
            AddStandardData data;
            data.polId = m_monitoringFactor.polId;
            data.cDataTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
            data.checkValue = checkValue;
            data.waterTime = SampleDataTime.toString("yyyy-MM-dd hh:mm:ss");
            data.waterValue = waterValue;
            data.chroma = chroma;
            data.sVolume = sVolume;
            data.dVolume = dVolume;
            data.flag = QString::fromStdString(flag);
            lstData.append(data);
    }

    return lstData;
}

QList<SpanCheckData> HJ212Manager::GetSpanCheckData(QDateTime startTime, QDateTime endTime)
{
    QList<SpanCheckData> lstData = {};
    QDateTime DataTime;

    int cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    String flag;           //数据标识
    float processValue;     //测量信号

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetSpanCheckData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetSpanCheckData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetSpanCheckData() => get 'GetSpanCheckData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        processValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isstring(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        SpanValue = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        standardValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-6);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    if ((DataTime >= startTime)
        && (DataTime <= endTime))
    {
            SpanCheckData data;
            data.polId = m_monitoringFactor.polId;
            data.cDataTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
            data.checkValue = checkValue;
            data.standardValue = standardValue;
            data.SpanValue = SpanValue;
            data.flag = QString::fromStdString(flag);
            data.processValue = processValue;
            lstData.append(data);
    }

    return lstData;
}

RealTimeData HJ212Manager::GetRealTimeData()
{
    QDateTime DataTime;
    int cDataTime;      //采集时间
    float checkValue;       //测量值
    String flag;           //数据标识
    bool isException;
    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetRealTimeData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetRealTimeData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetRealTimeData() => get 'GetParallelData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_toboolean(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        isException = lua_isboolean(m_newState ,-1);//取栈顶
    }

    status = lua_isstring(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isinteger(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-4);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    RealTimeData data;
    data.polId = m_monitoringFactor.polId;
    data.cDataTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
    data.mDateTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
    data.value = checkValue;
    data.flag = QString::fromStdString(flag);
    data.isException = isException;
    return data;
}


QList<ParallelData> HJ212Manager::GetParallelData(QDateTime startTime, QDateTime endTime)
{
    QList<ParallelData> lstData = {};
    QDateTime DataTime;
    int cDataTime;      //采集时间
    float checkValue;       //加标回收值
    int waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    String flag;           //数据标识
    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetParallelData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetParallelData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetParallelData() => get 'GetParallelData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        waterValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isinteger(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        waterTime = lua_tointeger(m_newState ,-2);//取栈顶
    }

    status = lua_isstring(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isinteger(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-5);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    if ((DataTime >= startTime)
        && (DataTime <= endTime))
    {
            ParallelData data;
            data.polId = m_monitoringFactor.polId;
            data.cDataTime = DataTime.toString("yyyy-MM-dd hh:mm:ss");
            data.checkValue = checkValue;
            data.waterTime = waterTime;
            data.waterValue = waterValue;
            data.flag = QString::fromStdString(flag);
            lstData.append(data);
    }

    return lstData;
}
QList<TimeSliceData> HJ212Manager::GetHourData(QDateTime startTime,
                                                            QDateTime endTime)
{
    QList<TimeSliceData> lstData;
    int cDataTime;  //采集时间
    float avg;          //平均值
    double cou;         //累计值
    float min;          //最小值
    float max;          //最大值
    int mDateTime;  //测量时间;
    String flag;       //数据标识
    if (nullptr == m_resultFiles)
    {
        logger->debug("HJ212Manager::GetHourData() => Measure Record Name is null");
        return lstData;
    }

    float standardValue = GetStandardCheckConsistency();

    String fieldMode("mode");
    String fieldDatetime("dateTime");
    String fieldResultType("resultType");
    String fieldConsistency("consistency");

    QMap<int, QString> resultMark;
    resultMark.insert(0, "N");  //正常/在线
    resultMark.insert(1, "B");  //离线
    resultMark.insert(2, "M");  //维护
    resultMark.insert(3, "D");  //故障
    resultMark.insert(4, "C");  //校准
    resultMark.insert(5, "E");  //超量程

    QString mode("N");
    QDateTime dateTime;

    m_resultFiles->MoveToLast(m_readerIndex);
    while(m_resultFiles->HasPrevious(m_readerIndex) && lstData.size() < 100)
    {
        m_resultFiles->MovePrevious(m_readerIndex);

        int nTime_t = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldDatetime, nTime_t);
        dateTime = QDateTime::fromTime_t(nTime_t);

//        m_resultFiles->GetFieldCurrentRecordEnumString(m_readerIndex, m_mode, str);
//        mode = QObject::tr(str.c_str());

        System::Byte resultType;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldResultType, resultType);

        System::Byte resultMode = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldMode, resultMode);
        if (resultMark.contains(resultMode))
        {
            mode = resultMark.value((int)resultMode);
        }

        if ((dateTime >= startTime)
            && (dateTime <= endTime)
            && ((int)resultType == (int)MeasureType::RangeCheck))
        {
            bool ret = false;
            float checkValue = 0;
            ret = m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldConsistency, checkValue);
            if (ret)
            {
                TimeSliceData data;
                data.polId = m_monitoringFactor.polId;
                data.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
                data.avg= checkValue;
                data.cou= checkValue;
                data.min= checkValue;
                data.max= checkValue;
                data.mDateTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
                data.flag = mode;
                lstData.append(data);
            }
        }
    }

    return lstData;
}

QList<StandardCheckData> HJ212Manager::GetStandardCheckData(QDateTime startTime,
                                                            QDateTime endTime)
{
    QList<StandardCheckData> lstData;

    if (nullptr == m_resultFiles)
    {
        logger->debug("HJ212Manager::GetStandardCheckData() => Measure Record Name is null");
        return lstData;
    }

    float standardValue = GetStandardCheckConsistency();

    String fieldMode("mode");
    String fieldDatetime("dateTime");
    String fieldResultType("resultType");
    String fieldConsistency("consistency");

    QMap<int, QString> resultMark;
    resultMark.insert(0, "N");  //正常/在线
    resultMark.insert(1, "B");  //离线
    resultMark.insert(2, "M");  //维护
    resultMark.insert(3, "D");  //故障
    resultMark.insert(4, "C");  //校准
    resultMark.insert(5, "E");  //超量程

    QString mode("N");
    QDateTime dateTime;

    m_resultFiles->MoveToLast(m_readerIndex);
    while(m_resultFiles->HasPrevious(m_readerIndex) && lstData.size() < 100)
    {
        m_resultFiles->MovePrevious(m_readerIndex);

        int nTime_t = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldDatetime, nTime_t);
        dateTime = QDateTime::fromTime_t(nTime_t);

//        m_resultFiles->GetFieldCurrentRecordEnumString(m_readerIndex, m_mode, str);
//        mode = QObject::tr(str.c_str());

        System::Byte resultType;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldResultType, resultType);

        System::Byte resultMode = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldMode, resultMode);
        if (resultMark.contains(resultMode))
        {
            mode = resultMark.value((int)resultMode);
        }

        if ((dateTime >= startTime)
            && (dateTime <= endTime)
            && ((int)resultType == (int)MeasureType::RangeCheck))
        {
            bool ret = false;
            float checkValue = 0;
            ret = m_resultFiles->GetFieldCurrentRecordValue(m_readerIndex, fieldConsistency, checkValue);
            if (ret)
            {
                StandardCheckData data;
                data.polId = m_monitoringFactor.polId;
                data.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
                data.checkValue = checkValue;
                data.standardValue = standardValue;
                data.flag = mode;

                lstData.append(data);
            }
        }
    }

    return lstData;
}

bool HJ212Manager::GetLastestRunLog(EquipmentLog &log)
{
    bool ret = false;

    QDir dir = QDir(Environment::Instance()->GetAppDataPath().c_str());
    QStringList nameFilters;
    nameFilters << FileName;
    QFileInfoList usefulFileList = dir.entryInfoList(nameFilters, QDir::Files);

    int currentFile = 0;    //最新的文件
    int currentBlock = 0;   //最新的块
    QDateTime currentBlockLastDateTime; //最新的块开始时间
    QString currentBlockContent;

    TextfileParser* logFile = new TextfileParser(usefulFileList.at(currentFile).absoluteFilePath());

    char *s = logFile->SeekBlock(currentBlock);
    QString pattern("(.*)( .*: )(.*)");
    QString str(s);
    QStringList strlist;
    strlist = str.split(SEPARATOR);
    int currentIndex = strlist.size();
    if (strlist.size() > 0)
    {
        QString currentLog = strlist.last();
        for (currentIndex--; currentLog.isEmpty() && currentIndex >= 0; --currentIndex)
        {
            currentLog = strlist.at(currentIndex);
        }

        if (!currentLog.isEmpty())
        {
            currentBlockLastDateTime = GetDateTimeFromLog(currentLog, pattern);
            currentBlockContent = GetContentFromLog(currentLog, pattern);

            MonitoringInfo info;
            HJ212Manager::Instance()->GetMonitoringInfo((int)MonitoringInfoIndex::iEquipmentLogCode, info);

            log.cDataTime = currentBlockLastDateTime.toString("yyyy-MM-dd hh:mm:ss");
            log.content = currentBlockContent;
            log.polId = m_monitoringFactor.polId;
            log.infoId = info.infoId;
            ret = true;
        }
    }

    if(logFile != nullptr)
    {
         delete logFile;
         logFile = nullptr;
    }

    return ret;
}

QStringList HJ212Manager::GetRunLogList(QDateTime startTime, QDateTime endTime)
{
    bool isFull = false;
    int mixLimit = 100;

    QDir dir = QDir(Environment::Instance()->GetAppDataPath().c_str());
    QStringList nameFilters;
    nameFilters << FileName;
    QFileInfoList usefulFileList = dir.entryInfoList(nameFilters, QDir::Files);

    int currentFile = 0;    //最新的文件
    int currentBlock = 0;   //最新的块
    QDateTime currentBlockLastDateTime; //最新的块开始时间

    TextfileParser* logFile = new TextfileParser(usefulFileList.at(currentFile).absoluteFilePath());

    char *s = logFile->SeekBlock(currentBlock);
    QString pattern("(.*)( .*: )(.*)");
    QString str(s);
    QStringList strlist, newStrlist,usefulList;
    strlist = str.split(SEPARATOR);

    int currentIndex = strlist.size();
    QString currentLastLog = strlist.last();
    for (currentIndex--; currentLastLog.isEmpty() && currentIndex >= 0; --currentIndex)
    {
        currentLastLog = strlist.at(currentIndex);
    }

    currentBlockLastDateTime = GetDateTimeFromLog(currentLastLog, pattern);
    newStrlist = SetDate(startTime, endTime, strlist, pattern);
    int currentpage = newStrlist.size();  //最新的串

    while(currentFile < usefulFileList.size())      //  文件层
    {
        while(!isFull && currentBlock < logFile->TotalBlock() && currentBlockLastDateTime >= startTime)    // 块层
        {
            while(!isFull && currentpage > 0) // 页层
            {
                QRegExp rx(pattern);
                currentpage--; //向旧翻页搜索
                QString m = newStrlist.at(currentpage);
                m.indexOf(rx);
                if(m.size()> 0 && rx.cap(1).size() > 1 && rx.cap(2).size() > 1 && rx.cap(3).size() > 1)
                {
                    usefulList.push_back(m);
                }

                if(usefulList.size() > mixLimit)
                {
                    isFull = true;
                    break;
                }
            }

            if(isFull || currentBlockLastDateTime < startTime)
            {
                break;
            }
            else
            {
                if(currentBlock + 1 < logFile->TotalBlock())
                {
                    currentBlock++; //向旧翻块搜索

                    char *s = logFile->SeekBlock(currentBlock);
                    QString str(s);
                    strlist.clear();
                    newStrlist.clear();
                    strlist = str.split(SEPARATOR);

                    currentIndex = strlist.size();
                    QString currentLastLog = strlist.last();
                    for (currentIndex--; currentLastLog.isEmpty() && currentIndex >= 0; --currentIndex)
                    {
                        currentLastLog = strlist.at(currentIndex);
                    }

                    currentBlockLastDateTime = GetDateTimeFromLog(currentLastLog, pattern);
                    newStrlist = SetDate(startTime, endTime, strlist, pattern);
                    currentpage = newStrlist.size();  //翻块后页号重置该页最新
                }
                else
                {
                    break;
                }
            }
        }//end while block

        if(isFull || currentBlockLastDateTime < startTime)
        {
            break;
        }
        else
        {
            currentFile++;  //向旧文件搜索
            if(currentFile < usefulFileList.size())
            {
                if(logFile != nullptr)
                {
                 delete logFile;
                 logFile = nullptr;
                }
                logFile = new TextfileParser(usefulFileList.at(currentFile).absoluteFilePath());
                currentBlock = 0;  //重置为该文件最新的块
                char *s = logFile->SeekBlock(currentBlock);
                QString str(s);
                strlist.clear();
                newStrlist.clear();
                strlist = str.split(SEPARATOR);
                currentIndex = strlist.size();

                QString currentLastLog = strlist.last();
                for (currentIndex--; currentLastLog.isEmpty() && currentIndex >= 0; --currentIndex)
                {
                    currentLastLog = strlist.at(currentIndex);
                }

                currentBlockLastDateTime = GetDateTimeFromLog(currentLastLog, pattern);
                newStrlist = SetDate(startTime, endTime, strlist, pattern);
                currentpage = newStrlist.size();  //翻块后页号重置该页最新
            }
        }
    }//end while file

    if(logFile != nullptr)
    {
         delete logFile;
         logFile = nullptr;
    }

    return usefulList;
}

bool HJ212Manager::GetEquipmentMonitoringInfo(QString infoId, EquipmentParam &params)
{
    RealTimeData newData = HJ212Manager::Instance()->GetRealTimeData();
    bool isOk = false;
    if (infoId.isEmpty() || !m_mapMonitorInfo.contains(infoId))
        return isOk;

    String value;
    try
    {
//        LuaEngine* luaEngine = LuaEngine::Instance();
//        lua_State * state = luaEngine->GetThreadState();

        lua_settop(m_newState ,0);//清空栈
        lua_getglobal(m_newState, "Hj212Interface");

        int status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::GetEquipmentMonitoringInfo() => get 'Hj212Interface' table failed");
            return isOk;
        }

        lua_pushstring(m_newState , "GetEquipmentMonitoringInfo");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_isfunction(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::GetEquipmentMonitoringInfo() => get 'GetEquipmentMonitoringInfo' function failed");
            return isOk;
        }

        lua_pushstring(m_newState, infoId.toStdString().data());

        int ret = lua_resume(m_newState, m_mainState, 1);
        if (ret != 0)
        {
            logger->warn("HJ212Manager::GetEquipmentMonitoringInfo() => lua_resume call 'GetEquipmentMonitoringInfo' function failed, error code:%d", ret);
            return isOk;
        }

        status = lua_isstring(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            value = lua_tostring(m_newState ,-1);
//            qDebug("HJ212Manager::GetEquipmentMonitoringInfo() value %s", value.data());
            params.cDataTime = newData.cDataTime;
            params.infoId = infoId;
            params.polId = m_monitoringFactor.polId;
            params.valueStr = QString::fromStdString(value);
            isOk = true;
        }
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::GetEquipmentMonitoringInfo() => %s", e.what());
    }

    return isOk;
}

bool HJ212Manager::GetEquipmentMonitoringInfo(QString infoId, EquipmentStatus &params)
{
    bool ret = false;
    EquipmentParam tmp;
    ret = GetEquipmentMonitoringInfo(infoId, tmp);
    if (ret)
    {
        params.cDataTime = tmp.cDataTime;
        params.infoId = tmp.infoId;
        params.polId = tmp.polId;
        params.valueStr = tmp.valueStr;
    }

    return ret;
}

bool HJ212Manager::GetEquipmentAllParam(QList<EquipmentParam> &params)
{
    bool ret = true;
    int countParam = 0;
    QMap<QString, MonitoringInfo>::iterator iter = m_mapMonitorInfo.begin();
    for(; iter != m_mapMonitorInfo.end(); ++iter)
    {
        EquipmentParam param;
        if ((int)InfoType::Parameter != iter.value().infoType)
        {
            continue;
        }

        countParam++;
        if (GetEquipmentMonitoringInfo(iter.key(), param))
        {
            params.append(param);
        }
    }

    if (!m_mapMonitorInfo.isEmpty() && 0 == countParam)
    {
        ret = false;
    }

    return ret;
}

bool HJ212Manager::GetEquipmentAllStatus(QList<EquipmentStatus> &statuses)
{
    bool ret = true;
    int countStatus = 0;
    QMap<QString, MonitoringInfo>::iterator iter = m_mapMonitorInfo.begin();
    for(; iter != m_mapMonitorInfo.end(); ++iter)
    {
        EquipmentStatus status;
        if ((int)InfoType::Status != iter.value().infoType)
        {
            continue;
        }

        countStatus++;
        if (GetEquipmentMonitoringInfo(iter.key(), status))
        {
            statuses.append(status);
        }
    }

    if (!m_mapMonitorInfo.isEmpty() && 0 == countStatus)
    {
        ret = false;
    }

    return ret;
}


bool HJ212Manager::SetEquipmentMonitoringInfo(QString infoId, QString value)
{
    bool isOk = false;
    if (infoId.isEmpty() || value.isEmpty() || !m_mapMonitorInfo.contains(infoId))
        return isOk;

    try
    {
//        LuaEngine* luaEngine = LuaEngine::Instance();
//        lua_State * state = luaEngine->GetThreadState();

        lua_settop(m_newState ,0);//清空栈
        lua_getglobal(m_newState, "Hj212Interface");

        int status = lua_istable(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::SetEquipmentMonitoringInfo() => get 'Hj212Interface' table failed");
            return isOk;
        }

        lua_pushstring(m_newState , "SetEquipmentMonitoringInfo");
        lua_gettable(m_newState ,-2); //取栈顶下一个元素

        status = lua_isfunction(m_newState ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::SetEquipmentMonitoringInfo() => get 'SetEquipmentMonitoringInfo' function failed");
            return isOk;
        }

        lua_pushstring(m_newState, infoId.toStdString().data());
        lua_pushstring(m_newState, value.toStdString().data());

        int ret = lua_resume(m_newState, m_mainState, 2);
        if (ret != 0)
        {
            logger->warn("HJ212Manager::SetEquipmentMonitoringInfo() => lua_resume call 'SetEquipmentMonitoringInfo' function failed, error code:%d", ret);
            return isOk;
        }

        status = lua_isboolean(m_newState ,-1);//取栈顶
        if (status == 1)
        {
            isOk = true;
        }
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::SetEquipmentMonitoringInfo() => %s", e.what());
    }

    return isOk;
}

void HJ212Manager::RtdToHourDatas(QList<RealTimeData> &rtdList, QList<TimeSliceData> &hourDataList)
{
    if (!rtdList.empty())
    {
        foreach(RealTimeData rtd, rtdList)
        {
            TimeSliceData data;
            data.cDataTime = rtd.cDataTime;
            data.polId = rtd.polId;
            data.avg = rtd.value;
            data.cou = 0;
            data.min = rtd.value;
            data.max = rtd.value;
            data.mDateTime = rtd.mDateTime;
            data.flag = rtd.flag;

            hourDataList.append(data);
        }
    }
}

void HJ212Manager::OnMeasureResultAdded(System::String name, ResultData::RecordData result)
{
    //已经做了信号与槽处理
    Q_UNUSED(name)
    Q_UNUSED(result)
    emit OnUpdateMessageSignal(UpdateMessageType::UpdateMeasureResult);
}

void HJ212Manager::OnCalibrateResultAdded(System::String name, ResultData::RecordData result)
{
    Q_UNUSED(name)
    Q_UNUSED(result)
    emit OnUpdateMessageSignal(UpdateMessageType::UpdateParam);
}

void HJ212Manager::OnUpdateWidget(UI::UpdateEvent event, String message)
{
    Q_UNUSED(message);
    switch(event)
    {
    case UI::UpdateEvent::ChangeConfigParam:
    case UI::UpdateEvent::ChangeMeasureParam:
    case UI::UpdateEvent::NewCalibrateCurve:
    case UI::UpdateEvent::ModbusChangeParam:
    case UI::UpdateEvent::ChangeAutoMeasure:
    case UI::UpdateEvent::WqimcChangeParam:
    case UI::UpdateEvent::RecoverInFactoryMode:
        emit OnUpdateMessageSignal(UpdateMessageType::UpdateParam);
        break;
    default:
        break;
    }
}

void HJ212Manager::OnStattusChanged(RunStatus status)
{
    Q_UNUSED(status)
    emit OnUpdateMessageSignal(UpdateMessageType::UpdateEquipmentStatus);
    if (status.name == "Idle" || status.name == "Fault")
    {
        emit OnUpdateLogWithBracketsSignal(false);
    }
    else
    {
        emit OnUpdateLogWithBracketsSignal(true);
    }

}

void HJ212Manager::OnStattusDelay(int time)
{
    Q_UNUSED(time)
    emit OnUpdateMessageSignal(UpdateMessageType::UpdateStatus);
    emit OnUpdateLogWithBracketsSignal(false);
}

void HJ212Manager::OnActionChanged(RunAction action)
{
    Q_UNUSED(action)
    if (action.name != "Idle")
    {
        emit OnUpdateMessageSignal(UpdateMessageType::UpdateLog);
    }
}

void HJ212Manager::OnActionDelay(int time)
{
    Q_UNUSED(time)
}

void HJ212Manager::OnUpdateMessageSlot(int type)
{
    MonitoringInfo info;
    switch(type)
    {
    case (int)UpdateMessageType::UpdateLog:
        UploadEquipmentLog();
        break;
    case (int)UpdateMessageType::UpdateParam:
        UploadAllParam();
        break;
    case (int)UpdateMessageType::UpdateStatus:
        UploadAllStatus();
        break;
    case (int)UpdateMessageType::UpdateEquipmentStatus:
        GetMonitoringInfo((int)MonitoringInfoIndex::iEquipmentStauts, info);
        UploadEquipmentInfoStatus(info.index);
        break;
    case (int)UpdateMessageType::UpdateEquipmentAlarm:
        GetMonitoringInfo((int)MonitoringInfoIndex::iAlarmCode, info);
        UploadEquipmentInfoStatus(info.index);
        break;
    case (int)UpdateMessageType::UpdateMeasureResult:
        UploadMeasureResult();
        break;
    default:
        break;
    }
}

void HJ212Manager::OnUpdateLogWithBrackesSlot(bool isBegin)
{
    EquipmentLog log;
    GetLastestRunLog(log);
    if (isBegin)
    {
        log.content = "<<" + log.content;
    }
    else
    {
        log.content = log.content + ">>";
    }

//    QMap<QString, QList<EquipmentLog>> datas;
//    datas.insert(log.cDataTime, QList<EquipmentLog>() << log);
    m_platform.UploadSystemInfoLog({log});

}

bool HJ212Manager::InitHj212Script()
{
    bool ret = false;

    Script *lua = LuaEngine::Instance()->GetEngine();

    OOLUA::Lua_func_ref initHj212;
    Table _G;

    try
    {
        _G.bind_script(*lua);
        _G.set_table("_G");

        ret = _G.safe_at("InitHj212", initHj212);

        if (ret)
        {
            lua->call(initHj212);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Manager::InitHj212Script => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::InitHj212Script() => %s", e.what());
    }

    return ret;
}

void HJ212Manager::InitPlatformInfo()
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212Type", m_hj212Type);

//        bool status = false;
//        luaEngine->GetLuaValue(state, "config.system.hj212Platform.status", status);
//        m_status = status;

        String strIp;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.address", strIp);
        m_platformInfo.ip = QString::fromStdString(strIp);

        int nPort = 11110;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.port", nPort);
        m_platformInfo.port = nPort;

        String mn;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.mn", mn);
        m_platformInfo.mn = QString::fromStdString(mn);

        String pw;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.pw", pw);
        m_platformInfo.pw = QString::fromStdString(pw);

        String st;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.st", st);
        m_platformInfo.st = QString::fromStdString(st);

        bool respond = false;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.respond", respond);
        m_isRespond = respond;

        int heartPackTime = 0;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.heartPackTime", heartPackTime);
        m_platformInfo.heartPackTime = heartPackTime * 60;

        int overtime = 0;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.overtime", overtime);
        m_platformInfo.overtime = overtime;

        int recount = 0;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.recount", recount);
        m_platformInfo.recount = recount;

        m_platformInfo.protocol = ProtocolVersion::HJ212_2017;
        if (m_hj212Type == (int)HJ212Type::V2005_Wry)
            m_platformInfo.protocol = ProtocolVersion::HJ212_2005;
        else if (m_hj212Type == (int)HJ212Type::V2017_Dbs)
            m_platformInfo.protocol = ProtocolVersion::HJ212_2017D;

        int hj212CommunicationMode = 0;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212CommunicationMode", hj212CommunicationMode);

        m_platformInfo.connectType = hj212CommunicationMode == 2 ? ConnectType::TCP : ConnectType::Serial;
        m_platformInfo.serial = GetSerialPortConfig();

        qDebug("[HJ212] hj212Type:%d hj212CommunicationMode:%d IP:%s Port:%d ST:%s",
               m_hj212Type, hj212CommunicationMode, m_platformInfo.ip.toStdString().c_str(), m_platformInfo.port, m_platformInfo.st.toStdString().c_str());

    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Manager::InitPlatformInfo() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::InitPlatformInfo() => %s", e.what());
    }
}


#ifdef _CS_X86_WINDOWS
    #define COM_232 "COM4"
    #define COM_485 "COM4"
#else
    #define COM_232 "/dev/ttymxc3"
    #define COM_485 "/dev/ttymxc4"
#endif

SerialPortConfig HJ212Manager::GetSerialPortConfig()
{
    int RS232Parity = 0;
    int RS232BaudRate = 9600;
    int RS485Parity = 0;
    int RS485BaudRate = 9600;
    int hj212CommunicationMode = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        String str;
        Table interconnection, options;
        luaEngine->GetLuaValue(state, "config.system.hj212Platform.hj212CommunicationMode", hj212CommunicationMode);
        luaEngine->GetLuaValue(state, "config.interconnection", interconnection);
        luaEngine->GetLuaValue(state, "setting.ui.profile.interconnection[4][1].options", options);

        interconnection.at("RS232Parity", RS232Parity);
        interconnection.at("RS485Parity", RS485Parity);

        int index;
        interconnection.at("RS232BaudRate", index);
        options.at(index + 1, str);
        RS232BaudRate = QString(str.c_str()).toInt();

        interconnection.at("RS485BaudRate", index);
        options.at(index + 1, str);
        RS485BaudRate = QString(str.c_str()).toInt();
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("ModbusManager::GetSerialPortConfig() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("ModbusManager::GetSerialPortConfig() => %s", e.what());
    }

    QMap<int,QString> parityArray = {{0,"N"},{1,"O"},{2,"E"}};

    SerialPortConfig serial;

    //获取串口配置
    bool is232 = hj212CommunicationMode == 0;
    if (is232)
    {
        serial.name = COM_232;
        serial.baud = RS232BaudRate;
        serial.parity = parityArray[RS232Parity];
    }
    else
    {
        serial.name = COM_485;
        serial.baud = RS485BaudRate;
        serial.parity = parityArray[RS485Parity];
    }

    serial.dataBit = 8;
    serial.stopBit = 1;
    return serial;
}


void HJ212Manager::InitMonitoringFactors()
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        String text;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.text", text);
        m_monitoringFactor.text = QString::fromStdString(text);

        String name;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.name", name);
        m_strMeasureRecordName = QString::fromStdString(name);

        String polId;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.polId", polId);
        m_monitoringFactor.polId = QString::fromStdString(polId);

        String oldPolId;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.oldPolId", oldPolId);
        m_monitoringFactor.oldPolId = QString::fromStdString(oldPolId);

        int decimal = 3;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.decimal", decimal);
        m_monitoringFactor.decimal = decimal;

        String unit;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212.monitoringFactor.unit", unit);
        m_monitoringFactor.unit = QString::fromStdString(unit);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Manager::InitMonitoringFactors() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::InitMonitoringFactors() => %s", e.what());
    }
}

void HJ212Manager::InitMonitoringInfos()
{
    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        Script* lua = luaEngine->GetEngine();
        lua_State* state = luaEngine->GetThreadState();

        Table hj212Table;
        luaEngine->GetLuaValue(state, "setting.externalInterface.hj212", hj212Table);

        Table monitoringInfoListTable;
        if (hj212Table.safe_at("monitoringInfo", monitoringInfoListTable))
        {
            oolua_ipairs(monitoringInfoListTable)
            {
                Table itemTable;
                lua->pull(itemTable);

                MonitoringInfo info;

                String infoId = "";
                String text = "";
                String unit = "";

                itemTable.at("index", info.index);
                itemTable.at("infoId", infoId);
                itemTable.at("text", text);
                itemTable.at("unit", unit);
                itemTable.at("infoType", info.infoType);

                info.infoId = QString::fromUtf8(infoId.c_str());
                info.text = QString::fromUtf8(text.c_str());
                info.unit = QString::fromUtf8(unit.c_str());
                info.deviceType = (int)DeviceType::Equipment;

                m_mapMonitorInfo.insert(info.infoId, info);
            }
            oolua_ipairs_end()
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("HJ212Manager::InitMonitoringInfos() => %s", e.what());
        throw e;
    }
    catch (std::exception e)
    {
        logger->warn("HJ212Manager::InitMonitoringInfos() => %s", e.what());
        throw e;
    }
}

PlatformInfo HJ212Manager::GetPlatformInfo()
{
    return m_platformInfo;
}

QList<UploadItem> HJ212Manager::GetUploadItems()
{
    QList<UploadItem> items;

    UploadItem item;
    item.enable = true;
    item.type = UploadDataType::InfoStatus;
    item.interval = m_platformInfo.heartPackTime;
    item.lastTimeSamp = 0;
    items.append(item);

    item.type = UploadDataType::InfoParam;
    items.append(item);

    item.type = UploadDataType::RtdData;
    items.append(item);

    item.type = UploadDataType::HourData;
    items.append(item);
    return items;
}

QList<MonitoringSpace::MonitoringFactor> HJ212Manager::GetMonitoringFactors()
{
    QList<MonitoringSpace::MonitoringFactor> ret;
    ret.append(m_monitoringFactor);
    return ret;
}

QList<MonitoringSpace::MonitoringInfo> HJ212Manager::GetMonitoringInfos()
{
    QList<MonitoringSpace::MonitoringInfo> ret;
    QMap<QString, MonitoringInfo>::iterator iter = m_mapMonitorInfo.begin();
    for (; iter != m_mapMonitorInfo.end(); ++iter)
    {
        ret.append(iter.value());
    }
    return ret;
}

void HJ212Manager::GetAddStandardParam(AddStandardData* param)
{
    QDateTime DataTime;
    QDateTime SampleDataTime;

    int cDataTime;      //采集时间
    float checkValue;       //加标回收值
    int waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    float chroma;           //母液
    float sVolume;          //加标体积
    float dVolume;          //加样杯容值
    String flag;           //数据标识

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetAddStandardData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetAddStandardData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetAddStandardData() => get 'GetAddStandardData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isstring(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-1);//取栈顶
    }

    status = lua_isnumber(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        dVolume = lua_tonumber(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        sVolume = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        chroma = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        waterValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        waterTime = lua_tointeger(m_newState ,-6);//取栈顶
    }

    status = lua_isnumber(m_newState ,-7);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-7);//取栈顶
    }

    status = lua_isinteger(m_newState ,-8);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-8);//取栈顶
    }
    DataTime = QDateTime::fromTime_t(cDataTime);
    SampleDataTime = QDateTime::fromTime_t(waterTime);

    param->waterTime = waterTime;
    param->waterValue = waterValue;
    param->chroma = chroma;
    param->sVolume = sVolume;
    param->dVolume = dVolume;
}

void HJ212Manager::GetSpanCheckParam(SpanCheckData* param)
{
    QDateTime DataTime;

    int cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    String flag;           //数据标识
    float processValue;     //测量信号

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetSpanCheckData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetSpanCheckData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetSpanCheckData() => get 'GetSpanCheckData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        processValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isstring(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        SpanValue = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        standardValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-6);//取栈顶
    }
    param->SpanValue = SpanValue;
    param->standardValue = standardValue;
    param->processValue = processValue;
}

void HJ212Manager::GetZeroCheckParam(ZeroCheckData* param)
{
    int cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    String flag;           //数据标识
    float processValue;     //测量信号

    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetZeroCheckData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetZeroCheckData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetZeroCheckData() => get 'GetZeroCheckData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        processValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isstring(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-2);//取栈顶
    }

    status = lua_isnumber(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        SpanValue = lua_tonumber(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        standardValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isnumber(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-5);//取栈顶
    }

    status = lua_isinteger(m_newState ,-6);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-6);//取栈顶
    }

    param->processValue = processValue;
    param->SpanValue = SpanValue;
    param->standardValue = standardValue;
}

void HJ212Manager::GetParallelParam(ParallelData* param)
{
    int cDataTime;      //采集时间
    float checkValue;       //加标回收值
    int waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    String flag;           //数据标识
    lua_settop(m_newState ,0);//清空栈
    lua_getglobal(m_newState, "Hj212Interface");

    int status = lua_istable(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetParallelData() => get 'Hj212Interface' table failed");
    }

    lua_pushstring(m_newState , "GetParallelData");
    lua_gettable(m_newState ,-2); //取栈顶下一个元素

    status = lua_isfunction(m_newState ,-1);//取栈顶
    if (status != 1)
    {
        logger->warn("HJ212Manager::GetParallelData() => get 'GetParallelData' function failed");
    }
    int flowCode = 0;
    lua_pushinteger(m_newState, flowCode);
    lua_resume(m_newState, m_mainState, 1);

    status = lua_isnumber(m_newState ,-1);//取栈顶
    if (status == 1)
    {
        waterValue = lua_tonumber(m_newState ,-1);//取栈顶
    }

    status = lua_isinteger(m_newState ,-2);//取栈顶
    if (status == 1)
    {
        waterTime = lua_tointeger(m_newState ,-2);//取栈顶
    }

    status = lua_isstring(m_newState ,-3);//取栈顶
    if (status == 1)
    {
        flag = lua_tostring(m_newState ,-3);//取栈顶
    }

    status = lua_isnumber(m_newState ,-4);//取栈顶
    if (status == 1)
    {
        checkValue = lua_tonumber(m_newState ,-4);//取栈顶
    }

    status = lua_isinteger(m_newState ,-5);//取栈顶
    if (status == 1)
    {
        cDataTime = lua_tointeger(m_newState ,-5);//取栈顶
    }

    param->waterTime = waterTime;
    param->waterValue = waterValue;

}

float HJ212Manager::GetStandardCheckConsistency()
{
    float checkConsistency = 0;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

//        lua_settop(state ,0);//清空栈
        lua_getglobal(state, "config");

        int status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::GetStandardCheckConsistency() => get 'config' table failed");
        }

        lua_pushstring(state , "measureParam");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::GetStandardCheckConsistency() => get 'measureParam' table failed");
        }

        lua_pushstring(state , "checkConsistency");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_isnumber(state ,-1);//取栈顶
        if (status == 1)
        {
            checkConsistency = lua_tonumber(state ,-1);
        }
    }
    catch(std::exception e)
    {
        logger->warn("HJ212Manager::GetStandardCheckConsistency() => %s", e.what());
    }

    return checkConsistency;
}

void HJ212Manager::UploadAllParam()
{
    QList<EquipmentParam> params;
    GetEquipmentAllParam(params);

//    QMap<QString, QList<EquipmentParam>> datas;
//    QList<EquipmentParam>::iterator iter = params.begin();
//    for(; iter != params.end(); ++iter)
//    {
//        datas.insert(iter->cDataTime, QList<EquipmentParam>() << (*iter));
//    }

    m_platform.UploadSystemInfoParam({params});
}

void HJ212Manager::UploadAllStatus()
{
    QList<EquipmentStatus> statuses;
    GetEquipmentAllStatus(statuses);

//    QMap<QString, QList<EquipmentStatus>> datas;
//    if (!statuses.isEmpty())
//    {
//        datas.insert(statuses.first().cDataTime, statuses);
//        m_platform.UploadSystemInfoStatus(datas);
//    }
    m_platform.UploadSystemInfoStatus(statuses);
}

bool HJ212Manager::UploadMeasureResult()
{
    bool ret = false;
    MeasureResult data;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

//        lua_settop(state ,0);//清空栈
        lua_getglobal(state, "status");

        int status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::UploadHourData() => get 'status' table failed");
        }

        lua_pushstring(state , "measure");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::UploadHourData() => get 'measure' table failed");
        }

        lua_pushstring(state , "newResult");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::UploadHourData() => get 'report' table failed");
        }

        lua_pushstring(state , "measure");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_istable(state ,-1);//取栈顶
        if (status != 1)
        {
            logger->warn("HJ212Manager::UploadHourData() => get 'measure' table failed");
        }

        lua_pushstring(state , "dateTime");
        lua_gettable(state ,-2); //取栈顶下一个元素

        status = lua_isnumber(state ,-1);//取栈顶
        if (status == 1)
        {
            data.times = lua_tonumber(state ,-1);
        }

        lua_pushstring(state , "consistency");
        lua_gettable(state ,-3); //取栈顶下一个元素

        status = lua_isnumber(state ,-1);//取栈顶
        if (status == 1)
        {
            data.consistency = lua_tonumber(state ,-1);
        }

        lua_pushstring(state , "resultType");
        lua_gettable(state ,-4); //取栈顶下一个元素

        status = lua_isnumber(state ,-1);//取栈顶
        if (status == 1)
        {
            data.resultType = lua_tonumber(state ,-1);
        }

        lua_pushstring(state , "resultInfo");
        lua_gettable(state ,-5); //取栈顶下一个元素

        status = lua_isstring(state ,-1);//取栈顶
        if (status == 1)
        {
            data.flag = lua_tostring(state ,-1);
        }

    }
    catch(std::exception e)
    {
        logger->warn("CCEPInterface::UpReportMessage() => %s", e.what());
        return ret;
    }

    switch (data.resultType)
    {
    case (int)MeasureType::Blank:

        ret = true;
        break;
    case (int)MeasureType::Sample:
        UploadHourData(data);
        ret = true;
        break;
    case (int)MeasureType::Standard:

        ret = true;
        break;
    case (int)MeasureType::Check:
        UploadStandardCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::Addstandard:

        ret = true;
        break;
    case (int)MeasureType::Parallel:

        ret = true;
        break;
    case (int)MeasureType::ZeroCheck:
        UploadZeroCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::RangeCheck:
        UploadSpanCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::ExtAddstandard:
        UploadAddStandardData(data);
        ret = true;
        break;
    case (int)MeasureType::ExtParallel:
        UploadParallelData(data);
        ret = true;
        break;
    default:

        ret = false;
        break;
    }

    return ret;
}

bool HJ212Manager::ManualUploadMeasureResult(int time,float consistency,int resultType,String flag)
{
    bool ret = false;
    MeasureResult data;

    data.times = time;
    data.consistency = consistency;
    data.resultType = resultType;
    data.flag = flag;

    switch (data.resultType)
    {
    case (int)MeasureType::Blank:

        ret = true;
        break;
    case (int)MeasureType::Sample:
        UploadHourData(data);
        ret = true;
        break;
    case (int)MeasureType::Standard:

        ret = true;
        break;
    case (int)MeasureType::Check:
        UploadStandardCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::Addstandard:

        ret = true;
        break;
    case (int)MeasureType::Parallel:

        ret = true;
        break;
    case (int)MeasureType::ZeroCheck:
        UploadZeroCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::RangeCheck:
        UploadSpanCheckData(data);
        ret = true;
        break;
    case (int)MeasureType::ExtAddstandard:
        UploadAddStandardData(data);
        ret = true;
        break;
    case (int)MeasureType::ExtParallel:
        UploadParallelData(data);
        ret = true;
        break;
    default:

        ret = false;
        break;
    }

    return ret;
}

void HJ212Manager::UploadHourData(HJ212Manager::MeasureResult data)
{
    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    TimeSliceData tsData;
    tsData.polId = m_monitoringFactor.polId;          //监测因子编码
    tsData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");    //采集时间
    tsData.avg = data.consistency;                                  //平均值
    tsData.cou = 0;                                                 //累计值
    tsData.min = data.consistency;                                  //最小值
    tsData.max = data.consistency;                                  //最大值
    tsData.mDateTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");    //测量时间;
    tsData.flag = QString::fromStdString(data.flag);                //数据标识

//    QMap<QString, QList<TimeSliceData>> mapDatas;
//    mapDatas.insert(tsData.cDataTime, QList<TimeSliceData>() << tsData);
    m_platform.UploadHourData({tsData});

    return ;
}

void HJ212Manager::UploadStandardCheckData(HJ212Manager::MeasureResult data)
{

    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    StandardCheckData scData;
    scData.polId = m_monitoringFactor.polId;
    scData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");;   //采集时间
    scData.checkValue = data.consistency;                           //核查值
    scData.standardValue = GetStandardCheckConsistency();          //标样浓度
    scData.flag = QString::fromStdString(data.flag);                //数据标识

//    QMap<QString, QList<StandardCheckData>> mapDatas;
//    mapDatas.insert(scData.cDataTime, QList<StandardCheckData>() << scData);
    m_platform.UploadStandardCheckData({scData});

    return ;
}

void HJ212Manager::UploadAddStandardData(HJ212Manager::MeasureResult data)
{
    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    AddStandardData tsData;
    GetAddStandardParam(&tsData);
    tsData.polId = m_monitoringFactor.polId;
    tsData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    tsData.checkValue = data.consistency;
    tsData.flag = QString::fromStdString(data.flag);

    m_platform.UploadAddStandardData({tsData});
    return ;
}

void HJ212Manager::UploadParallelData(HJ212Manager::MeasureResult data)
{
    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    ParallelData tsData;
    GetParallelParam(&tsData);
    tsData.polId = m_monitoringFactor.polId;
    tsData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    tsData.checkValue = data.consistency;
    tsData.flag = QString::fromStdString(data.flag);

    m_platform.UploadParallelData({tsData});
    return ;
}

void HJ212Manager::UploadZeroCheckData(HJ212Manager::MeasureResult data)
{
    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    ZeroCheckData tsData;
    GetZeroCheckParam(&tsData);
    tsData.polId = m_monitoringFactor.polId;
    tsData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    tsData.checkValue = data.consistency;
    tsData.flag = QString::fromStdString(data.flag);

    m_platform.UploadZeroCheckData({tsData});
    return ;
}

void HJ212Manager::UploadSpanCheckData(HJ212Manager::MeasureResult data)
{
    QDateTime dateTime = QDateTime::fromTime_t(data.times);

    SpanCheckData tsData;
    GetSpanCheckParam(&tsData);
    tsData.polId = m_monitoringFactor.polId;
    tsData.cDataTime = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    tsData.checkValue = data.consistency;
    tsData.flag = QString::fromStdString(data.flag);

    m_platform.UploadSpanCheckData({tsData});
    return ;
}

void HJ212Manager::UploadEquipmentInfoStatus(int index)
{
    MonitoringInfo info;
    GetMonitoringInfo(index, info);

    EquipmentStatus status;
    GetEquipmentMonitoringInfo(info.infoId, status);

//    QMap<QString, QList<EquipmentStatus>> datas;
//    datas.insert(status.cDataTime, QList<EquipmentStatus>() << status);
    m_platform.UploadSystemInfoStatus({status});
}

void HJ212Manager::UploadEquipmentLog()
{
    EquipmentLog log;
    if (!GetLastestRunLog(log))
        return;

//    QMap<QString, QList<EquipmentLog>> datas;
//    datas.insert(log.cDataTime, QList<EquipmentLog>() << log);
    m_platform.UploadSystemInfoLog({log});
}

/***********************************************************
 *
 * HJ212插件回调接口实现
 *
 * ********************************************************/

//保存平台配置
static bool SavePlatformInfo(PlatformInfo info)
{
    return true;
}

//保存平台配置
static bool SaveUploadItem(UploadItem item)
{
    return true;
}

//获取PolId
static QString GetPolId()
{
    return HJ212Manager::Instance()->GetMonitoringFactor().polId;
}

//根据数据类型判断是否需要回包
static bool IsNeedRespond(UploadDataType type)
{
    Q_UNUSED(type)
    return HJ212Manager::Instance()->IsRespond();
}

//设置系统时间
static bool SetSystemTimeStatic(QDateTime time)
{
    QString dataTime = time.toString("yyyy-MM-dd hh:mm:ss");
    QByteArray ba = dataTime.toLatin1();
    char* data = ba.data();
    logger->debug("设置系统时间 %s", data);
    System::SetSystemTime::SetTime(time);
    return true;
}

//启动仪器流程
static bool StartEquipmentsFlow(FlowType flowType)
{
    int flowCode = (int)flowType;
    bool ret = false;
    ret = HJ212Manager::Instance()->StartEquipmentFlow(flowCode);
    return ret;
}

//获取系统、仪器日志
static QList<EquipmentLog> GetSysEquipmentLogList(QDateTime startTime, QDateTime endTime)
{
    QList<EquipmentLog> ret;
    QString pattern("(.*)( .*: )(.*)");
    QStringList lstLog = HJ212Manager::Instance()->GetRunLogList(startTime, endTime);
    for(int i = 0; i < lstLog.size(); ++i)
    {
        EquipmentLog log;
        MonitoringInfo info;
        HJ212Manager::Instance()->GetMonitoringInfo((int)MonitoringInfoIndex::iSysLogCode, info);

        log.infoId = info.infoId;
        log.polId = HJ212Manager::Instance()->GetMonitoringFactor().polId;
        log.cDataTime = GetDateTimeFromLog(lstLog[i], pattern).toString("yyyy-MM-dd hh:mm:ss");
        log.content = GetContentFromLog(lstLog[i], pattern);

        ret.append(log);
    }
    return ret;
}

//获取系统、仪器状态
static bool GetSysEquipmentStatus(QString infoId, EquipmentStatus& status)
{
    bool ret = false;
    ret = HJ212Manager::Instance()->GetEquipmentMonitoringInfo(infoId, status);
    return ret;
}

//获取仪器参数（系统没有参数）
static bool GetEquipmentParam(QString infoId, EquipmentParam& params)
{
    bool ret = false;
    ret = HJ212Manager::Instance()->GetEquipmentMonitoringInfo(infoId, params);
    return ret;
}

//设置仪器参数（系统没有参数）
static bool SetEquipmentParam(QString infoId, QString value)
{
    bool ret = false;
    ret = HJ212Manager::Instance()->SetEquipmentMonitoringInfo(infoId, value);
    return ret;
}

//获取最新实时数据
static RealTimeData GetRealTimeDataStatic()
{
    RealTimeData Data =  HJ212Manager::Instance()->GetRealTimeData();
    return Data;
}

//获取最新的小时数据
static QList<TimeSliceData> GetNewestHourDataListStatic()
{
    //TODO
    RealTimeData Data =  HJ212Manager::Instance()->GetRealTimeData();
    QList<RealTimeData> realTimeDataList;
    QList<TimeSliceData> hourDataList;
    realTimeDataList.append(Data);
    HJ212Manager::Instance()->RtdToHourDatas(realTimeDataList, hourDataList);
    return hourDataList;
}

//获取实时数据
static QList<RealTimeData> GetRealTimeDataList(QDateTime startTime, QDateTime endTime)
{
    RealTimeData Data =  HJ212Manager::Instance()->GetRealTimeData();
    return {Data};
}

//获取小时数据
static QList<TimeSliceData> GetHourDataList(QDateTime startTime, QDateTime endTime)
{
    QList<TimeSliceData> lstStdData =  HJ212Manager::Instance()->GetHourData(startTime, endTime);
    return lstStdData;
}

//获取核查数据
static QList<StandardCheckData> GetStandardCheckDataList(QDateTime startTime, QDateTime endTime)
{
    QList<StandardCheckData> lstStdData =  HJ212Manager::Instance()->GetStandardCheckData(startTime, endTime);
    return lstStdData;
}

//获取平行样数据
static QList<ParallelData> GetParallelDataList(QDateTime startTime, QDateTime endTime)
{
    QList<ParallelData> lstStdData =  HJ212Manager::Instance()->GetParallelData(startTime, endTime);
    return lstStdData;
}

//获取零点核查
static QList<ZeroCheckData>GetZeroCheckDataList(QDateTime startTime, QDateTime endTime)
{
    QList<ZeroCheckData> lstStdData =  HJ212Manager::Instance()->GetZeroCheckData(startTime, endTime);
    return lstStdData;
}

//获取加标回收
static QList<AddStandardData> GetAddStandardDataList(QDateTime startTime, QDateTime endTime)
{
    QList<AddStandardData> lstStdData =  HJ212Manager::Instance()->GetAddStandardData(startTime, endTime);
    return lstStdData;
}

//获取跨度核查
static QList<SpanCheckData> GetSpanCheckDataList(QDateTime startTime, QDateTime endTime)
{
    QList<SpanCheckData> lstStdData =  HJ212Manager::Instance()->GetSpanCheckData(startTime, endTime);
    return lstStdData;
}

void HJ212Manager::Test()
{

}

HJ212CallBack HJ212Manager::GetHJ212CallBack()
{
    HJ212CallBack ret;
    ret.SavePlatformInfo = SavePlatformInfo;
    ret.SaveUploadItem = SaveUploadItem;
    ret.GetPolId = GetPolId;
    ret.SetSystemTime = SetSystemTimeStatic;
    ret.IsNeedRespond = IsNeedRespond;
    ret.StartEquipmentsFlow = StartEquipmentsFlow;
    ret.GetSysEquipmentLogList = GetSysEquipmentLogList;
    ret.GetSysEquipmentStatus = GetSysEquipmentStatus;
    ret.GetEquipmentParam = GetEquipmentParam;
    ret.SetEquipmentParam = SetEquipmentParam;

    ret.GetRealTimeData = GetRealTimeDataStatic;
    ret.GetNewestHourDataList = GetNewestHourDataListStatic;

    ret.GetRealTimeDataList = GetRealTimeDataList;
    ret.GetHourDataList = GetHourDataList;
    ret.GetStandardCheckDataList = GetStandardCheckDataList;
    ret.GetParallelDataList = GetParallelDataList;
    ret.GetZeroCheckDataList = GetZeroCheckDataList;
    ret.GetAddStandardDataList = GetAddStandardDataList;
    ret.GetSpanCheckDataList = GetSpanCheckDataList;
    return ret;
}

}
