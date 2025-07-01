/**
 * @brief 监控
 * @author huangyue
 * @date 2021-07-01
 * @note
 */

#ifndef __TREASURE_TOOL_MONITOR__
#define __TREASURE_TOOL_MONITOR__

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Treasure
{
    class Monitor
    {
    public:
        static int GetCPUUsage()
        {
            int usage = 0;
#ifdef _WIN32
            static FILETIME pre_idleTime;
            static FILETIME pre_kernelTime;
            static FILETIME pre_userTime;

            FILETIME idleTime;  //空闲时间
            FILETIME kernelTime;//核心态时间
            FILETIME userTime;  //用户态时间

            bool res = GetSystemTimes(&idleTime, &kernelTime, &userTime);
            if (!res)
                return 0;

            __int64 idle = CompareFileTime(pre_idleTime, idleTime);
            __int64 kernel = CompareFileTime(pre_kernelTime, kernelTime);
            __int64 user = CompareFileTime(pre_userTime, userTime);

            pre_idleTime = idleTime;
            pre_kernelTime = kernelTime;
            pre_userTime = userTime;

            __int64 sum = kernel + user;
            if (sum == 0)
                return 0;
            usage = abs(static_cast<int>((sum - idle) * 100 / sum));
#endif
            return usage;
        }

        static int GetMemoryUsage()
        {
            MEMORYSTATUS ms;
            ::GlobalMemoryStatus(&ms);
            return ms.dwMemoryLoad;
        }

    private:
#ifdef _WIN32
        static __int64 CompareFileTime(FILETIME time1, FILETIME time2)
        {
            __int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
            __int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
            return b - a;
        }
#endif
    };
}


#endif // MONITOR_H
