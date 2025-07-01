#ifndef LOG_H
#define LOG_H

#ifdef HAVE_LOG_LIB

#include <list>
#include <memory>
#include "log4cpp/Category.hh"
#include "Treasure/SystemDef.h"

extern log4cpp::Category* logger;

class Log
{
    TREASURE_SINGELTON(Log)
public:
    static void Init();
};

#endif

#endif // LOG_H
