#ifndef LOG_H
#define LOG_H

#include <memory>
#include "log4cpp/Category.hh"
#include "oolua.h"
#include <list>

extern log4cpp::Category* logger;

class Log
{

public:
    static Log* Instance();
    static void Init();
    log4cpp::Category *GetLogger();
    static void Proxy(OOLUA::Script * lua);

    static void InitHJ212Logger(int index);
    static log4cpp::Category *GetHJ212Logger(int index);

private:
    static std::unique_ptr<Log> m_instance;
    static std::map<int, log4cpp::Category*> m_hjlogMap;
};

#endif //LOG_H
