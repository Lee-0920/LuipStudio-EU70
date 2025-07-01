#include <QString>
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/eCek/Common/Utils.h"
#include <QUuid>
#include "ControllerPlugin/API/ManufDate.h"
#include "ControllerPlugin/API/Version.h"
#include <QDateTime>
#include "Setting/SettingManager.h"
#include <time.h>
#include <stdio.h>
#include "InstrumentInfomation.h"
#include "Log.h"
#include "LuaEngine/LuaEngine.h"
#include "System/PlatformInfo.h"

#ifndef  _CS_X86_WINDOWS
#include <sys/time.h>
#endif
using namespace Communication::Dscp;
using namespace Controller::API;
using namespace Configuration;
using namespace std;
using namespace OOLUA;
using namespace Lua;

void InstrumentInfomation_GetType(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String type;
    luaEngine->GetLuaValue(state, "config.info.instrument.type", type);
    qDebug("GetType: %s", type.c_str());
    DscpDevice_SendResp(dscp, (void*)type.c_str(), type.length() + 1);
}

void InstrumentInfomation_SetType(DscpDevice* dscp, Byte* data, Uint16 len)
{
    String type;
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    type = (char*) data;
    if ((len > (type.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", type.length()+1);
    }
    else
    {
        qDebug("SetType: %s", type.c_str());
        luaEngine->SetLuaValue("config.info.instrument.type", type);
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetModel(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String model;
    luaEngine->GetLuaValue(state, "config.info.instrument.model", model);
    qDebug("GetModel: %s", model.c_str());
    DscpDevice_SendResp(dscp, (void*)model.c_str(), model.length() + 1);
}

void InstrumentInfomation_SetModel(DscpDevice* dscp, Byte* data, Uint16 len)
{
    String model;
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    model = (char*) data;
    if ((len > (model.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", model.length()+1);
    }
    else
    {
        qDebug("SetModel: %s", model.c_str());
        luaEngine->SetLuaValue("config.info.instrument.model", model);
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetSn(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String sn;
    luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);
    qDebug("GetSn: %s", sn.c_str());
    DscpDevice_SendResp(dscp, (void*)sn.c_str(), sn.length() + 1);
}

void InstrumentInfomation_SetSn(DscpDevice* dscp, Byte* data, Uint16 len)
{
    String sn;
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    sn = (char*) data;
    if ((len > (sn.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("str = %d len = %d\n", sn.length()+1, len);
    }
    else
    {
        qDebug("SetSn: %s", sn.c_str());
        luaEngine->SetLuaValue("config.info.instrument.sn", sn);
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetUUID(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String str;
    luaEngine->GetLuaValue(state, "config.info.instrument.uuid", str);
    QUuid id = QUuid(str.c_str());

    qDebug("GetUUID: %s ,size %d ", id.toString().toLatin1().data(), id.toRfc4122().size());
    DscpDevice_SendResp(dscp, id.toRfc4122().data(), id.toRfc4122().size());
}

void InstrumentInfomation_SetUUID(DscpDevice* dscp, Byte* data, Uint16 len)
{
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    if (len != 16)
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", len);
    }
    else
    {
        QByteArray array((const char *)data, len);
        QUuid id(QUuid::fromRfc4122(array));
        qDebug("SetUUID: %s", id.toString().toLatin1().data());

        String str(id.toString().toLatin1().data());
        luaEngine->SetLuaValue("config.info.instrument.uuid", str);
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String manufacter;
    luaEngine->GetLuaValue(state, "config.info.instrument.manuFacturer", manufacter);
    qDebug("GetManufacter: %s", manufacter.c_str());
    DscpDevice_SendResp(dscp, (void *)manufacter.c_str(), manufacter.length() + 1);
}

void InstrumentInfomation_SetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len)
{
    Uint16 status = DscpStatus::OK;
    String manufacter;
    LuaEngine* luaEngine = LuaEngine::Instance();


    manufacter = (char*) data;
    if ((len > (manufacter.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("manufacterlength：%d len：%d\n", manufacter.length()+1, len);
    }
    else
    {
        qDebug("SetManuFacturer: %s", manufacter.c_str());
        luaEngine->SetLuaValue("config.info.instrument.manuFacturer", manufacter);
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetManufDate(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    Uint8 buffer[4] = { 0 };
    Uint16 year;
    Uint8 month, day;

    int ret;
    luaEngine->GetLuaValue(state, "config.info.instrument.manuDate.year", ret);
    year = ret;
    luaEngine->GetLuaValue(state, "config.info.instrument.manuDate.month", ret);
    month = ret;
    luaEngine->GetLuaValue(state, "config.info.instrument.manuDate.day", ret);
    day = ret;

    memcpy(buffer, &year, sizeof(Uint16));
    memcpy(buffer + sizeof(Uint16), &month, sizeof(Uint8));
    memcpy(buffer + sizeof(Uint16) + sizeof(Uint8), &day, sizeof(Uint8));

    qDebug("GetManufDate: %d-%d-%d", year, month, day);
    DscpDevice_SendResp(dscp, buffer, 4);
}

void InstrumentInfomation_SetManufDate(DscpDevice* dscp, Byte* data, Uint16 len)
{
    Uint16 status = DscpStatus::OK;
    ManufDate date;
    LuaEngine* luaEngine = LuaEngine::Instance();

    if (len != 4)
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", len);
    }
    else
    {
        date = ManufDate(data);
        qDebug("SetManufDate: %s", date.ToString().c_str());

        luaEngine->SetLuaValue("config.info.instrument.manuDate.year", date.GetYear());
        luaEngine->SetLuaValue("config.info.instrument.manuDate.month", date.GetMonth());
        luaEngine->SetLuaValue("config.info.instrument.manuDate.day", date.GetDay());
        SettingManager::Instance()->InstrumentInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetSystemTime(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;

#ifdef    _CS_ARM_LINUX
    QDateTime dateTime = QDateTime::currentDateTime();
#else
    QDateTime localTime = QDateTime::currentDateTime();
    QDateTime dateTime(localTime.date(), localTime.time(), Qt::UTC);
#endif
    Uint64 utcTime = dateTime.toTime_t();
    qDebug("GetSystemTime %s , %lld", dateTime.toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
           utcTime);
    DscpDevice_SendResp(dscp, &utcTime, sizeof(Uint64));
}

void InstrumentInfomation_SetSystemTime(DscpDevice* dscp, Byte* data, Uint16 len)
{
    Uint16 status = DscpStatus::OK;

    if (len != sizeof(Uint64))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", len);
    }
    else
    {
        Uint64 utcTime = *(Uint64 *) data;
        QDateTime DateTime = QDateTime::fromTime_t(utcTime, Qt::UTC);
        qDebug("SetSystemTime %s , %lld", DateTime.toString("yyyy-MM-dd hh:mm:ss").toLatin1().data(),
               utcTime);

        QTime times = DateTime.time();
        QDate dates = DateTime.date();
#ifdef    _CS_ARM_LINUX
        struct tm *t_tm;
        struct timeval t_timeval;
        time_t t_timet;

        t_timet=time(NULL);
        t_tm=localtime(&t_timet);

        t_tm->tm_hour = times.hour();
        t_tm->tm_min = times.minute();
        t_tm->tm_sec = times.second();
        t_tm->tm_year = dates.year()- 1900;
        t_tm->tm_mon = dates.month() - 1;
        t_tm->tm_mday = dates.day();

        t_timet=mktime(t_tm);

        t_timeval.tv_sec = t_timet;
        t_timeval.tv_usec = 0;

        int rec = settimeofday(&t_timeval,NULL);
        QString year = QString("%1").arg(t_tm->tm_year);
        QString month = QString("%1").arg(t_tm->tm_mon);
        QString day = QString("%1").arg(t_tm->tm_mday);
        QString hour = QString("%1").arg(t_tm->tm_hour);
        QString minute = QString("%1").arg(t_tm->tm_min);
        QString second = QString("%1").arg(t_tm->tm_sec);
        QString str= "date -s " + year + month + day + hour + minute + "." + second;

        //强制写入到CMOS
        system("hwclock -w");
#else
    (void)times;
    (void)dates;
#endif

    }
    DscpDevice_SendStatus(dscp, status);
}

void InstrumentInfomation_GetSoftwareVersion(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    Uint8 buffer[4] = { 0 };
    Uint8 major, minor, revision, build;
    String ret;
    luaEngine->GetLuaValue(state, "setting.version.software", ret);
    QString ver(ret.c_str());

    major = ver.section(".", 0, 0).toInt();
    minor = ver.section(".", 1, 1).toInt();
    revision = ver.section(".", 2, 2).toInt();
    build = ver.section(".", 3, 3).toInt();

    memcpy(buffer, &major, 1);
    memcpy(buffer + 1, &minor, 1);
    memcpy(buffer + 2, &revision, 1);
    memcpy(buffer + 3, &build, 1);

    qDebug("%d %d %d %d", major, minor, revision, build);
    qDebug("GetSoftwareVersion: %s", ver.toLatin1().data());
    DscpDevice_SendResp(dscp, buffer, 4);
}

void InstrumentInfomation_GetSoftwareLable(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    String lable = "STD-[SVN Revision]-[2017.02.15]";
    qDebug("GetSoftwareLable: %s", lable.c_str());
    DscpDevice_SendResp(dscp, (void*)lable.c_str(), lable.length() + 1);
}

void InstrumentInfomation_GetPlatFormVersion(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    Uint8 buffer[4] = { 0 };
    Uint8 major, minor, revision, build;

    QString ver(System::PlatformVersion.c_str());

    major = ver.section(".", 0, 0).toInt();
    minor = ver.section(".", 1, 1).toInt();
    revision = ver.section(".", 2, 2).toInt();
    build = ver.section(".", 3, 3).toInt();

    memcpy(buffer, &major, 1);
    memcpy(buffer + 1, &minor, 1);
    memcpy(buffer + 2, &revision, 1);
    memcpy(buffer + 3, &build, 1);

    qDebug("%d %d %d %d", major, minor, revision, build);
    qDebug("GetPlatFormVersion: %s", ver.toLatin1().data());
    DscpDevice_SendResp(dscp, buffer, 4);
}

void InstrumentInfomation_GetPlatFormLable(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    String lable = "STD-[SVNP Revision]-[2017.02.15]";
    qDebug("GetPlatFormLable: %s", lable.c_str());
    DscpDevice_SendResp(dscp, (void*)lable.c_str(), lable.length() + 1);
}

void InstrumentInfomation_GetInstrumentName(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String name;
    luaEngine->GetLuaValue(state, "setting.instrument.name", name);
    qDebug("GetInstrumentName: %s", name.c_str());
    DscpDevice_SendResp(dscp, (void*)name.c_str(), name.length() + 1);
}

void InstrumentInfomation_GetMeasureParameter(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String parameter;
    Table argus;
    luaEngine->GetLuaValue(state, "setting.instrument.argus", argus);
    oolua_ipairs(argus)
    {
        String ret;
        OOLUA::pull(state, ret);
        parameter += ret + ",";
    }
    oolua_ipairs_end()

    qDebug("GetMeasureParameter: %s", parameter.c_str());
    DscpDevice_SendResp(dscp, (void*)parameter.c_str(), parameter.length() + 1);
}

void InstrumentInfomation_GetMeasureParameterCode(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    int outputType = 0;
    luaEngine->GetLuaValue(state, "config.interconnection.outputType", outputType);

    int code;
    String str = "setting.instrument.deviceTypeNum";
    str += "[" + QString::number(outputType + 1).toStdString() + "]";
    luaEngine->GetLuaValue(state, str, code);

    qDebug("GetMeasureParameter: %d", code);
    DscpDevice_SendResp(dscp, &code, sizeof(code));
}
