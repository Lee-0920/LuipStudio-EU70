#ifndef LOG_H
#define LOG_H

#include <memory>
#include "log4cpp/Category.hh"
#include "oolua.h"

extern log4cpp::Category* logger;

class Log
{

public:
    static Log* Instance();
    static void Init();
    log4cpp::Category *GetLogger();
    static void Proxy(OOLUA::Script * lua);

private:
    static std::unique_ptr<Log> m_instance;
};

#endif //LOG_H
