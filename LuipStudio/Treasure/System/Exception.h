#ifndef __TREASURE_SYSTEM_EXCEPTION__
#define __TREASURE_SYSTEM_EXCEPTION__

#include <QString>

namespace Treasure
{

class Exception : public std::exception
{
public:
    Exception() : message("Exception") {}
    Exception(const QString& message) : message(message) {}
    virtual ~Exception() {}

    virtual const QString& What()
    {
        return message;
    }

protected:
    QString message;
};


}

#endif

