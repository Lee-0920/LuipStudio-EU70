/*
 * MasterUpdate.cpp
 *
 *  Created on: 2016年9月13日
 *      Author: liangliwen
 */
#include "MasterUpdate.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include "StatusManager/StatusManager.h"
#include <QApplication>
#include <QThread>
#include <QProcess>
#include "Log.h"
#include "System/Uncompress.h"
#include "Setting/Environment.h"
#include "Interface/Modbus/ModbusManager.h"

using namespace Measure;
using namespace Interface;
using namespace Modbus;

namespace Updater
{
// 升级文件配置项
#define  CONFIG_DELIMITER   '='          // 配置项：分隔符
#define  CONFIG_TYPE        "type"       // 配置项：板卡类型
#define  CONFIG_VERSION     "version"    // 配置项：版本号

MasterUpdate::MasterUpdate(String fileName, bool isDeleteZip)
{
    m_fileName =  fileName;
    m_version = new String;
    m_type = new String;
    m_isDeleteZip = isDeleteZip;
    m_isRunTool = false;
    m_destDirectory = "";
}

MasterUpdate::MasterUpdate(String toolFileName)
{
    m_fileName = toolFileName;
    m_version = new String;
    m_type = new String;
    m_isDeleteZip = false;
    m_isRunTool = true;
    m_destDirectory = "";
}

MasterUpdate::~MasterUpdate()
{
    if (m_version)
        delete m_version;
    if (m_type)
        delete m_type;
}

bool MasterUpdate::Update(void)
{
    bool ret = false;
    QString UpdaterShPath;
    QStringList stringList;

    StatusManager *statusManager = StatusManager::Instance();
    RunStatus status("MASTER", "主控板升级", (int)0);
    StatusManager::Instance()->SetStatus(status);

    RunAction action;
    action.time = 0;

    if (false == m_isRunTool)
    {
        const char *tmp = m_fileName.c_str();
        bool illegalCharacter = false;  //是否有非法字符
        while(*tmp++ != '\0')
        {
            if(*tmp == ' ')
            {
                if(false == illegalCharacter)
                {
                    logger->info("升级包有非法字符---(空格)");
                    logger->info("请删除名称中的空格或与客服确认");
                    illegalCharacter = true;
                }
                logger->info("(空格)在 %s前",tmp+1);
            }
        }

        if(illegalCharacter)
        {
            return false;
        }

        action.name = "Uncompress";
        action.content = "解压";
        statusManager->SetAction(action);

        QFileInfo original(m_fileName.c_str());
        QString destName = original.completeBaseName();
        QString destDirectory = Configuration::Environment::Instance()->GetUnzipPath().c_str();
        destDirectory += "/" + destName;
        m_destDirectory = destDirectory.toStdString();
        m_destDirectory += "/";
        QStringList listPic;
        if (false == Uncompress::UnZipToDirectory(m_fileName, m_destDirectory, listPic, "LuipAdmin@2017"))
        {
            logger->debug("主控板升级包: %s 解压失败", m_fileName.c_str());
            if (m_isDeleteZip)
            {
                QFile::remove(m_fileName.c_str());
            }
            return false;
        }
        if (m_isDeleteZip)
        {
            QFile::remove(m_fileName.c_str());
        }

        QStringList result = listPic.filter("Updater.sh");


        if (result.size() > 0)
        {
            UpdaterShPath = result.at(0);
        }
        stringList << UpdaterShPath;
        stringList << destDirectory;
    }
    else
    {
        UpdaterShPath = m_fileName.c_str();
        stringList << UpdaterShPath;
    }

    action.name = "Updater";
    action.content = "升级";
    statusManager->SetAction(action);

    QFile file(UpdaterShPath);
    if (file.exists())
    {
        ModbusManager::Instance()->GetTCPModbus()->Stop();  //关闭modbus tcp socket连接
        QThread::sleep(3);
        if(QProcess::startDetached("sh", stringList))
        {
            while(1)//启动脚本后，此应用程序会被重启
            {
                QThread::sleep(1);
            }
        }
        else
        {
            logger->warn("升级脚本无法启动");
            ModbusManager::Instance()->GetTCPModbus()->Start();
        }
    }
    else
    {
        logger->warn("升级脚本不存在");
    }
    return ret;
}

String MasterUpdate::GetUnzipDestDirectory(void)
{
    return m_destDirectory;
}

void MasterUpdate::Trim(QString* str)
{
    QString temp = str->trimmed();
    *str = temp;
}

bool MasterUpdate::ParseHeader(String fileName)
{
    bool ret = TRUE;
    QString buffer;
    QFile file(fileName.c_str());
    QString::size_type len;
    QString result;

    if (file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
         while (!in.atEnd())
        {
            // 读取一行文件
            buffer = in.readLine();
            len = buffer.size();
            if (len > 0)
            {
                result = buffer.section(CONFIG_DELIMITER,0,0);//字符串分割，取第0段
                if (result != NULL)
                {
                    Trim(&result);
                    if (CONFIG_TYPE == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);//字符串分割，取第1段
                        Trim(&result);//删除字符串前后的空白符(空格，回车，TAB等)
                        *m_type = result.toStdString();
                    }
                    else if (CONFIG_VERSION == result)
                    {
                        result = buffer.section(CONFIG_DELIMITER,1,1);
                        Trim(&result);
                        *m_version = result.toStdString();
                    }
                }
            }
        }
        file.close();
    }
    else
    {
//        qDebug() << "\n Check Can not find or open file " << fileName << endl;
        ret = FALSE;
    }

    return ret;
}

String MasterUpdate::GetType(void)
{
//    qDebug() << "\n Type is:" << *m_type << endl;
    return *m_type;
}

String MasterUpdate::GetVersion(void)
{
//    qDebug() << "\n Version is:" << *m_version << endl;
    return *m_version;
}

}
