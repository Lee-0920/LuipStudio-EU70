/**
 * @brief 全局日志
 * @author huangyue
 * @date 2021-02-03
 * @note 使用说明：
 *          1、在主程序中，需要先设置每个回调接口，如：trLogger->f_warn = []{ xxxxx };
 *          2、在主程序中，使用时只需要trLogger->Debug() 之类的接口即可
 *          3、在动态库中，需要将主程序的日志句柄传递给库，先注册Logger::Instance(日志句柄);
 *
 *       例子：支持warn\debug\info\error级别输出
 *            如debug的调用： trLogger->Debug("this is a debug message");
 *                           trLogger->Debug("this is a debug message: %d", errorCode);
 *
 *                           #define LOG_FILE_HJ212_KEY 3   //日志文件ID，用于支持多日志文件输出
 *                           trLogger->Debug(LOG_FILE_HJ212_KEY, "this is a debug message for hj212")
 *
 */
#ifndef __TREASURE_SYSTEM_LOGGER__
#define __TREASURE_SYSTEM_LOGGER__

#include <iostream>
#include <functional>
#include <QString>
#include <QDebug>
#include "Global.h"

/**
 * @brief 全局调试工具，使用trLogger就可以调用日志的接口
 */
#define trLogger Treasure::Logger::Instance()

/**
 * @brief 内部使用的宏
 */
#define TREASURE_LOGGER_FUNC(Name, Func)              \
    void Name(const std::string &text)                \
    {                                                 \
        Name(-1, text);                               \
    }                                                 \
    void Name(const QString &text)                    \
    {                                                 \
        Name(text.toStdString());                     \
    }                                                 \
    void Name(const char *stringFormat, ...)          \
    {                                                 \
        char buffer[1024];                            \
        va_list arg;                                  \
        va_start(arg, stringFormat);                  \
        vsprintf(buffer, stringFormat, arg);          \
        Name(std::string(buffer));                    \
    }                                                 \
    void Name(int key, const std::string &text)       \
    {                                                 \
        if (Func)                                     \
            Func(key, text);                          \
        if (this->showQDebug)                         \
            qDebug() << text.c_str();                 \
    }                                                 \
    void Name(int key, const QString &text)           \
    {                                                 \
        Name(key, text.toStdString());                \
    }                                                 \
    void Name(int key, const char *stringFormat, ...) \
    {                                                 \
        char buffer[1024];                            \
        va_list arg;                                  \
        va_start(arg, stringFormat);                  \
        vsprintf(buffer, stringFormat, arg);          \
        Name(key, std::string(buffer));               \
    }

namespace Treasure
{
    class Logger
    {
    public:
        //回调函数
        std::function<void(int, std::string)> f_warn;
        std::function<void(int, std::string)> f_debug;
        std::function<void(int, std::string)> f_error;
        std::function<void(int, std::string)> f_info;

    public:
        Logger()
        {
            f_warn = NULL;
            f_debug = NULL;
            f_error = NULL;
            f_info = NULL;
            showQDebug = false;

#ifdef QT_DEBUG
            showQDebug = true;
#endif
        }

        void ShowQDebug(bool flag)
        {
            this->showQDebug = flag;
        }

    public:
        TREASURE_LOGGER_FUNC(Warn, f_warn)
        TREASURE_LOGGER_FUNC(warn, f_warn)
        TREASURE_LOGGER_FUNC(Debug, f_debug)
        TREASURE_LOGGER_FUNC(debug, f_debug)
        TREASURE_LOGGER_FUNC(Error, f_error)
        TREASURE_LOGGER_FUNC(error, f_error)
        TREASURE_LOGGER_FUNC(Info, f_info)
        TREASURE_LOGGER_FUNC(info, f_info)

    public:
        static Logger *Instance(Logger *registerObj = NULL)
        {
            static Logger *obj = NULL;
            if (obj != NULL)
                return obj;

            if (registerObj != NULL)
                obj = registerObj;
            if (obj == NULL)
                obj = Global<Logger>::Instance();
            return obj;
        }

    private:
        bool showQDebug;
    };

}

#endif
