#include "Log.h"

log4cpp::Category* logger = NULL;

log4cpp::Category* dataLogger = NULL;

void InitDataLog()
{
    dataLogger = &log4cpp::Category::getInstance(std::string("DataLog"));
}
