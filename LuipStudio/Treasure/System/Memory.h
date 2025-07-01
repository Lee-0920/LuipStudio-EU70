#ifndef __TREASURE_SYSTEM_MEMORY__
#define __TREASURE_SYSTEM_MEMORY__

#include <QMap>
#include <QVector>
#include <QString>


namespace Treasure
{

class Memory
{
public:
    template<typename T>
    static T* Delete(T* ptr)
    {
        if (ptr)
            delete ptr;
        return nullptr;
    }

};
}

#endif
