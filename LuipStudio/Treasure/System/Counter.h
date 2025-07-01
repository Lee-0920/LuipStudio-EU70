#ifndef __TREASURE_SYSTEM_COUNTER__
#define __TREASURE_SYSTEM_COUNTER__

#include <QString>
#include <QThread>
#include <QDebug>
#include <QElapsedTimer>
#include "Global.h"

namespace Treasure
{

class Counter
{
public:
    Counter(int maxNum = 100)
    {
        if (maxNum < 0)
            maxNum = 1;
        for(int i = 0; i < maxNum; i++)
            array.append(0);
    }

    void Add(int index, int num = 1)
    {
        if (index >= array.size())
            return;
        array[index] += num;
    }

    int Get(int type)
    {
        if (type >= array.size())
            return 0;
        return array[type];
    }

private:
    QVector<int> array;
};

}

#endif // COUNTER_H
