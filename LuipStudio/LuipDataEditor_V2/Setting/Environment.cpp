#include "Environment.h"
#include <QApplication>
#include <QDir>
#include <iostream>
#include <QDebug>

namespace Configuration
{
std::unique_ptr<Environment> Environment::m_instance(nullptr);

Environment* Environment::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new Environment);
    }

    return m_instance.get();
}

void Environment::Init()
{
#ifdef _CS_APP_ROOT
    m_appPath = _CS_APP_ROOT;
    QDir dir(m_appPath.c_str());
    if (!dir.exists())
    dir.mkpath(m_appPath.c_str());
#else
    m_appPath = QApplication::applicationDirPath().toStdString();
#endif
    qDebug()<<"AppPath:"<<QString::fromStdString(m_appPath);
}

void Environment::CreateFolder()
{
    QDir dir(GetDeviceDataPath().c_str());
    if (!dir.exists())
    {
        dir.mkpath(GetDeviceDataPath().c_str());
    }

    QDir datadir(GetAppDataPath().c_str());
    if (!datadir.exists())
    {
        datadir.mkpath(GetAppDataPath().c_str());
    }
}

String Environment::GetAppPath()
{
    return m_appPath;
}

String Environment::GetUpdatePath()
{
#ifdef    _CS_ARM_LINUX
    return String("/mnt/udisk/Updater");
#endif
    return m_appPath + "/Updater";
}

String Environment::GetDefaultConfigPath()
{
    return m_appPath + "/SP/Config/Default";
}

String Environment::GetDeviceDataPath()
{
#ifdef    _CS_ARM_LINUX
    return String("/mnt/datadisk/Data");
#endif
    return m_appPath + "/Data";
}

String Environment::GetUnzipPath()
{
#ifdef    _CS_ARM_LINUX
    return String("/opt/Unzip");
#endif
    return m_appPath + "/Unzip";
}

String Environment::GetAppDataPath()
{
    return m_appPath + "/../LuipData";
}

}
