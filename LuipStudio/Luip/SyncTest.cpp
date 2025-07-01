/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 链路适配接口Windows串口的实现。
 * @details 实现Windows平台的串口通信Lai层实体。
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2013-1-15
 */

#include <stdio.h>
#include <assert.h>
#include <thread>
#include <QDebug>
#include <QDateTime>
#include <mutex>
#include <condition_variable>
#include "ControllerPlugin/API/DeviceInfoInterface.h"
#include "SyncTest.h"
#include "System/Exception.h"
#include "Controller/ControllerManager.h"
#include "Communication/CommunicationException.h"
#include "Log.h"
#include "AlarmManager/AlarmManager.h"

using namespace Controller;
using namespace Communication;
using namespace System;
using namespace Measure;

// 线程模拟发送和接收中断


static void SyncTestThreadRun1();
static void SyncTestThreadRun2();
static void SyncTestThreadRun3();


static bool isSyncTestThreadRunning1 = false;
static bool isSyncTestThreadRunning2 = false;
static bool isSyncTestThreadRunning3 = false;



void SyncTestStart(int index)
{
    // 创建并启动收发线程

    if (index == 1)
    {
        isSyncTestThreadRunning1 = true;
        std::thread SyncTestThread1(SyncTestThreadRun1);
        SyncTestThread1.detach();
    }
    else if (index == 2)
    {
        isSyncTestThreadRunning2 = true;
        std::thread SyncTestThread2(SyncTestThreadRun2);
        SyncTestThread2.detach();

    }
    else if (index == 3)
    {
        isSyncTestThreadRunning3 = true;
        std::thread SyncTestThread3(SyncTestThreadRun3);
        SyncTestThread3.detach();
    }
}

void SyncTestStop()
{
    int sleepTime = 10;
    int timeOut = 1000 / sleepTime;

    // 安全地终止通信线程
    isSyncTestThreadRunning1 = false;
    isSyncTestThreadRunning2 = false;
    isSyncTestThreadRunning3 = false;
    while (timeOut--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // 线程退出后，会设置这个标志位
        if ((isSyncTestThreadRunning1) &&
            (isSyncTestThreadRunning2) &&
            (isSyncTestThreadRunning3))
        {
            break;
        }
    }
}




void SyncTestThreadRun1()
{
    BaseController* controller = ControllerManager::Instance()->GetController("DriveControllerPlugin");

    while(isSyncTestThreadRunning1)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        try
        {

//            logger->debug("线程一：%s", controller->IDeviceInfo->GetSerialNumber().c_str());
            controller->IDeviceInfo->GetSerialNumber();
//            MeterPoints points =LC->IOpticalMeter->GetMeterPoints();
//            Temperatuer temp = TC->ITemperatureControl->GetTemperature();
//            logger->debug("线程一：消解器温度 = %f ℃", temp.thermostatTemp);
        }
        catch (CommandTimeoutException e)       // 系统异常。
        {
            logger->debug("线程一");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (CommunicationException e)       // 系统异常。
        {
            logger->debug("线程一");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            logger->debug("线程一");
            AlarmManager::Instance()->AddExceptionAlarm(e);
        }
    }

    isSyncTestThreadRunning1 = true;
}

void SyncTestThreadRun2()
{
    BaseController* controller = ControllerManager::Instance()->GetController("DriveControllerPlugin");


    while(isSyncTestThreadRunning2)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        try
        {
//            float speed =LC->IOpticalMeter->GetMeteSpeed();
//            logger->debug("线程二：定量速度 = %f", speed);

//            Uint32 tartet = OA->IOpticalAcquire->GetLEDControllerTartet();
//            logger->debug("线程二：LED目标值 = %f ℃", tartet);
//            logger->debug("线程二：%d",controller->IDeviceStatus->GetRunMode());
            controller->IDeviceStatus->GetRunMode();

        }
        catch (CommandTimeoutException e)       // 系统异常。
        {
            logger->debug("线程二");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (CommunicationException e)       // 系统异常。
        {
            logger->debug("线程二");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            logger->debug("线程二");
            AlarmManager::Instance()->AddExceptionAlarm(e);
        }
    }

    isSyncTestThreadRunning2 = true;
}

void SyncTestThreadRun3()
{
    BaseController* controller = ControllerManager::Instance()->GetController("DriveControllerPlugin");

    while(isSyncTestThreadRunning3)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        try
        {
//            ThermostatParam param = TC->ITemperatureControl->GetThermostatParam();
//            OA->IOpticalAcquire->TurnOffLED();

//            logger->debug("线程三：P = %f, I = %f, D = %f",
//                          param.proportion, param.integration, param.differential);
//            logger->debug("线程三：%s",controller->IDeviceUpdate->GetVersion().ToString().c_str());
            controller->IDeviceUpdate->GetVersion();
        }
        catch (CommandTimeoutException e)       // 系统异常。
        {
             logger->debug("线程三");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (CommunicationException e)       // 系统异常。
        {
            logger->debug("线程三");
             AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
        }
        catch (std::exception e)
        {
            logger->debug("线程三");
            AlarmManager::Instance()->AddExceptionAlarm(e);
        }
    }

    isSyncTestThreadRunning3 = true;
}

/** @} */
