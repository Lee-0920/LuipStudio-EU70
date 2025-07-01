#ifndef LOG_H
#define LOG_H

#include <QtCore/qglobal.h>
#include "log4cpp/Category.hh"


extern log4cpp::Category* logger;
extern log4cpp::Category* dataLogger;
extern void InitDataLog();

#endif // LOG_H
