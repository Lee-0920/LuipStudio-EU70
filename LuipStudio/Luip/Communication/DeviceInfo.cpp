#include "DeviceInfo.h"
#include <QString>
#include "Communication/Dscp/DscpStatus.h"
#include "Communication/eCek/Common/Utils.h"
#include <QUuid>
#include "ControllerPlugin/API/ManufDate.h"
#include "ControllerPlugin/API/Version.h"
#include <QDateTime>
#include "Setting/SettingManager.h"
#include "Log.h"
#include "LuaEngine/LuaEngine.h"

using namespace Communication::Dscp;
using namespace Controller::API;
using namespace Configuration;
using namespace std;
using namespace OOLUA;
using namespace Lua;

void DeviceInfo_GetType(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String type;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.type", type);
    qDebug("GetType: %s", type.c_str());
    DscpDevice_SendResp(dscp, (void*)type.c_str(), type.length() + 1);
}

void DeviceInfo_SetType(DscpDevice* dscp, Byte* data, Uint16 len)
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
        luaEngine->SetLuaValue("config.info.mainBoard.type", type);
        SettingManager::Instance()->MainBoardInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void DeviceInfo_GetModel(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String model;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.model", model);
    qDebug("GetModel: %s", model.c_str());
    DscpDevice_SendResp(dscp, (void*)model.c_str(), model.length() + 1);
}

void DeviceInfo_SetModel(DscpDevice* dscp, Byte* data, Uint16 len)
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
        luaEngine->SetLuaValue("config.info.mainBoard.model", model);
        SettingManager::Instance()->MainBoardInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void DeviceInfo_GetSn(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String sn;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.sn", sn);
    qDebug("GetSn: %s", sn.c_str());
    DscpDevice_SendResp(dscp, (void*)sn.c_str(), sn.length() + 1);
}

void DeviceInfo_SetSn(DscpDevice* dscp, Byte* data, Uint16 len)
{
    String sn;
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    sn = (char*) data;
    qDebug("str = %d len = %d\n", sn.length()+1, len);
    if ((len > (sn.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
    }
    else
    {
        qDebug("SetSn: %s", sn.c_str());
        luaEngine->SetLuaValue("config.info.mainBoard.sn", sn);
        SettingManager::Instance()->MainBoardInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void DeviceInfo_GetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    String manufacter;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.manuFacturer", manufacter);
    qDebug("GetManufacter: %s", manufacter.c_str());
    DscpDevice_SendResp(dscp, (void *)manufacter.c_str(), manufacter.length() + 1);
}

void DeviceInfo_SetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len)
{
    String manufacter;
    Uint16 status = DscpStatus::OK;
    LuaEngine* luaEngine = LuaEngine::Instance();

    manufacter = (char*) data;
    if ((len > (manufacter.length() + 1)) || (data[len - 1] != '\0'))
    {
        status = DscpStatus::Error;
        qDebug("Parame Len Error\n");
        qDebug("%d \n", manufacter.length()+1);
    }
    else
    {
        qDebug("SetManuFacturer: %s", manufacter.c_str());
        luaEngine->SetLuaValue("config.info.mainBoard.manuFacturer", manufacter);
        SettingManager::Instance()->MainBoardInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void DeviceInfo_GetManufDate(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    Uint8 buffer[4] = { 0 };
    Uint16 year;
    Uint8 month, day;

    int ret;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.manuDate.year", ret);
    year = ret;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.manuDate.month", ret);
    month = ret;
    luaEngine->GetLuaValue(state, "config.info.mainBoard.manuDate.day", ret);
    day = ret;

    memcpy(buffer, &year, sizeof(Uint16));
    memcpy(buffer + sizeof(Uint16), &month, sizeof(Uint8));
    memcpy(buffer + sizeof(Uint16) + sizeof(Uint8), &day, sizeof(Uint8));

    qDebug("GetManufDate: %d-%d-%d", year, month, day);
    DscpDevice_SendResp(dscp, buffer, 4);
}

void DeviceInfo_SetManufDate(DscpDevice* dscp, Byte* data, Uint16 len)
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

        luaEngine->SetLuaValue("config.info.mainBoard.manuDate.year", date.GetYear());
        luaEngine->SetLuaValue("config.info.mainBoard.manuDate.month", date.GetMonth());
        luaEngine->SetLuaValue("config.info.mainBoard.manuDate.day", date.GetDay());
        SettingManager::Instance()->MainBoardInfoSave();
    }

    DscpDevice_SendStatus(dscp, status);
}

void DeviceInfo_GetSoftwareVersion(DscpDevice* dscp, Byte* data, Uint16 len)
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

void DeviceInfo_GetSoftwareLable(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;

    String lable = "STD-[SVN Revision]-[2017.02.15]";
    qDebug("GetSoftwareLable: %s", lable.c_str());
    DscpDevice_SendResp(dscp, (void*)lable.c_str(), lable.length() + 1);
}

void DeviceInfo_GetHardwareVersion(DscpDevice* dscp, Byte* data, Uint16 len)
{
    (void)data;
    (void)len;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    Uint8 buffer[4] = { 0 };
    Uint8 major, minor, revision, build;
    String ret;
    luaEngine->GetLuaValue(state, "setting.version.hardware", ret);
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
