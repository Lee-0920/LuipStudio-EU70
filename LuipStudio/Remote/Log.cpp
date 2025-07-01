#ifdef HAVE_LOG_LIB
#include "Log.h"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/Category.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/PropertyConfigurator.hh"
#include <QApplication>

using namespace std;
using namespace log4cpp;

log4cpp::Category* logger = NULL;

void Log::Init()
{
    // 使用指定目录的配置文件初始化日志系统
    QString strFile = QApplication::applicationDirPath() + "/log4cpp.conf";
    if (!QFile::exists(strFile))
        strFile = QApplication::applicationDirPath() + "/log4cppToARM.conf";

    log4cpp::PropertyConfigurator::configure(strFile.toStdString());

    log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
    layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S} [%p]: %m%n");

    QString name = QString("RemoteLog");
#ifdef _WIN32
    QString fileName = QString("../Data/remotelog.txt");
#else
    QString fileName = QString("/mnt/userdata2/Data/remotelog.txt");
#endif
    log4cpp::RollingFileAppender* fileAppender = new log4cpp::RollingFileAppender(name.toStdString(), fileName.toStdString());
    fileAppender->setLayout(layout);
    fileAppender->setMaxBackupIndex(10);
    fileAppender->setMaximumFileSize(10485760);

    logger = &log4cpp::Category::getInstance(name.toStdString());
    logger->setAdditivity(false);
    logger->setAppender(fileAppender);

    //绑定插件的接口
    trLogger->f_debug = [](int key, std::string content){Q_UNUSED(key); logger->debug(content); };
    trLogger->f_warn  = [](int key, std::string content){Q_UNUSED(key); logger->warn(content); };
    trLogger->f_info  = [](int key, std::string content){Q_UNUSED(key); logger->info(content); };
    trLogger->f_error = [](int key, std::string content){Q_UNUSED(key); logger->error(content); };
}
#endif
