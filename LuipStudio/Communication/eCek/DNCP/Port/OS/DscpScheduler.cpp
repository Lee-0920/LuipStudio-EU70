/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief DSCP 命令处理调度器。
 * @details
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2016-6-28
 */

#include <mutex>
#include <thread>
#include <condition_variable>
#include "DNCP/App/DscpDevice.h"
#include "DscpScheduler.h"

// 线程状态控制
bool s_isSchedulerThreadRunning;
std::mutex s_mutexScheduler;
std::condition_variable s_cvScheduler;

static DscpDevice* s_dscpDevice;

static void DscpScheduler_TaskHandle();

/**
 * @brief 初始化DSCP 命令处理调度器。
 * @details 将开启一个线程用于处理上位机下发的命令。
 * @param dscp DscpDevice 协议实体。
 */
void DscpScheduler_Init(DscpDevice* dscp)
{
    s_dscpDevice = dscp;

    s_isSchedulerThreadRunning = true;
    std::thread threadScheduler(DscpScheduler_TaskHandle);
    threadScheduler.detach();
}

/**
 * @brief 结束化DSCP 命令处理调度器。
 * @details 结束后，相关纯种将被释放。
 */
void DscpScheduler_Uninit()
{
    int sleepTime = 10;
    int timeOut = 2000 / sleepTime;

    // 安全地终止通信线程
    s_isSchedulerThreadRunning = false;
    s_cvScheduler.notify_one();

    while (timeOut--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // 线程退出后，会设置这个标志位
        if (s_isSchedulerThreadRunning)
        {
            break;
        }
    }
}

void DscpScheduler_TaskHandle()
{
    std::unique_lock <std::mutex> lock(s_mutexScheduler);
    while (s_isSchedulerThreadRunning)
    {
        s_cvScheduler.wait(lock);
        DscpDevice_Handle(s_dscpDevice);
    }

    s_isSchedulerThreadRunning = true;
}

void DscpScheduler_Active(void)
{
    //std::unique_lock <std::mutex> lock(s_mutexScheduler);
    s_cvScheduler.notify_one();
}


/** @} */
