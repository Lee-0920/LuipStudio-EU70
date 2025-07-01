#include "DynamicPassword.h"
#include "Log.h"
#include "oolua.h"
#include "LuaEngine/LuaEngine.h"
#include "System/Hash/md5.h"
#include <QDateTime>
#include <QDebug>
#include <QByteArray>


using namespace OOLUA;
using namespace Lua;

namespace System
{

QString DynamicPassword::GeneratePassword(QDateTime time)
{
    QString pwdStr;
    pwdStr = CreatePassword(luipKeyWord, time);
    return pwdStr;
}

QString DynamicPassword::GenerateReagentPassword(QDateTime time)
{
    QString pwdStr;
    pwdStr = CreatePassword(reagentKeyWord, time);
    return pwdStr;
}

QStringList DynamicPassword::GeneratePasswordList()
{
    QStringList pwdList;

    QDateTime currentTime = QDateTime::currentDateTime();

    for(int i = 0; i < 21; i++)
    {
        QDateTime time = currentTime.addSecs((i-10)*120);
        QString pwdStr = DynamicPassword::GeneratePassword(time);
        pwdList.append(pwdStr);
    }

    return pwdList;
}

QStringList DynamicPassword::GenerateReagentPasswordList()
{
    QStringList pwdList;

    QDateTime currentTime = QDateTime::currentDateTime();

    for(int i = 0; i < 21; i++)
    {
        QDateTime time = currentTime.addSecs((i-10)*120);
        QString pwdStr = DynamicPassword::GenerateReagentPassword(time);
        pwdList.append(pwdStr);
    }

    return pwdList;
}

QString DynamicPassword::CreatePassword(QString keyWord, QDateTime time)
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    std::string sn;
    luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);

    QString timeStr = QString::number(time.toTime_t()/120);

    QString srcStr = keyWord + QString::fromStdString(sn) + timeStr;

    char* srcData = const_cast<char *>(srcStr.toStdString().c_str());

    unsigned char dstData[16];
    memset(&dstData[0],0,sizeof(dstData));

    MDString(srcData, dstData);

    unsigned char data[3][6] = {{0},{0},{0}};
    memcpy(&data[0][0], &dstData[0], 6);
    memcpy(&data[1][0], &dstData[6], 6);
    memcpy(&data[2][0], &dstData[12], 4);

    QString pwdStr;
    unsigned char pwd[6] = {0};
    for(int t = 0; t < 6; t++)
    {
        pwd[t] = (data[0][t]^data[1][t]^data[2][t])%10;
        pwdStr += QString::number(pwd[t]);
    }
    return pwdStr;
}

}
