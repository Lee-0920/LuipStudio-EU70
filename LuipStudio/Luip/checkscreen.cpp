#include "checkscreen.h"
#include <QFileInfo>
#include <QDebug>
#include "Log.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/types.h>

namespace System
{

std::unique_ptr<CheckScreen> CheckScreen::m_instance(nullptr);

CheckScreen::CheckScreen(QObject *parent) : QObject(parent)
{
    //默认参数为PT65有屏软件使用
    m_checkVNC = false;
    m_checkFNS = false;
    m_checkADC = true;
    m_checkRelay = true;
}

CheckScreen *CheckScreen::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new CheckScreen);
    }
    return m_instance.get();
}

void CheckScreen::Init()
{
#ifdef _CS_ARM_LINUX
    using namespace std;
    //检测区分文件
    QFileInfo fileInfo("/mnt/datadisk/Data/DeviceParam");
    if(fileInfo.isFile())
    {
        fstream inFile;
        inFile.open("/mnt/datadisk/Data/DeviceParam");
        string str;

        while(getline(inFile, str))
        {
            if (string::npos != str.find("vnc"))    //当前只需要判定是否为远程控制平台
            {
                if((string::npos != str.find("1")) || (string::npos != str.find("true")))
                {
                    m_checkVNC = true;
                }
                else
                {
                    m_checkVNC = false;
                }
            }
            else if(string::npos != str.find("nfs"))
            {
                if((string::npos != str.find("1")) || (string::npos != str.find("true")))
                {
                    m_checkFNS = true;
                }
                else
                {
                    m_checkFNS = false;
                }
            }
            else if(string::npos != str.find("adc"))
            {
                if((string::npos != str.find("1")) || (string::npos != str.find("true")))
                {
                    m_checkADC = true;
                }
                else
                {
                    m_checkADC = false;
                }
            }
            else if(string::npos != str.find("relay"))
            {
                if((string::npos != str.find("1")) || (string::npos != str.find("true")))
                {
                    m_checkRelay = true;
                }
                else
                {
                    m_checkRelay = false;
                }
            }
        }
    }
    qDebug("### remote control  %d",m_checkVNC);
#endif
}

bool CheckScreen::isVNC()
{
    return m_checkVNC;
}

bool CheckScreen::isFNS()
{
    return m_checkFNS;
}

bool CheckScreen::isADC()
{
    return m_checkADC;
}

bool CheckScreen::isRelay()
{
    return m_checkRelay;
}

CheckScreen::~CheckScreen()
{

}



};
