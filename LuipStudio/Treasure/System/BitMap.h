#ifndef __TREASURE_SYSTEM_BITMAP__
#define __TREASURE_SYSTEM_BITMAP__

#include <QList>
#include <QString>


#define Bit64Map Treasure::BitMap<quint64>          //64位
#define OffBit64Map Treasure::OffBitMap<quint64>    //64位

#define Bit16Map Treasure::BitMap<quint16>          //16位
#define OffBit16Map Treasure::OffBitMap<quint16>    //16位

namespace Treasure
{

template <typename TYPE>
class BitMap
{
public:
    BitMap(TYPE data = 0)
    {
        this->data = data;
    }

    BitMap(QList<int> indexs)
    {
        this->data = 0;
        SetOn(indexs);
    }

    void SetData(TYPE data)
    {
        this->data = data;
    }

    void SetData(void* data)
    {
        this->data = *((TYPE*)data);
    }

    TYPE GetData() const
    {
        return this->data;
    }

    void SetOn(QList<int> indexs)
    {
        foreach(auto item, indexs)
            SetOn(item);
    }

    void SetOn(int index)
    {
        TYPE mark = 1;
        mark = mark << index;
        this->data |= mark;
    }

    void SetOff(int index)
    {
        TYPE mark = 1;
        mark = mark << index;
        this->data &= ~mark;
    }

    bool IsOn(int index)
    {
        TYPE mark = 1;
        mark = mark << index;
        return (this->data & mark);
    }

    void clear()
    {
        this->data = 0;
    }

private:
    TYPE data;
};


template <typename TYPE>
class OffBitMap
{
public:
    OffBitMap(){}
    OffBitMap(QList<int> indexs) : map(indexs)
    {
    }
    OffBitMap(TYPE data = 0) : map(data)
    {
    }

    TYPE GetData()
    {
        return map.GetData();
    }

    void SetOff(int index)
    {
        map.SetOn(index);
    }

private:
    BitMap<TYPE> map;
};



}



#endif // BITMAP_H
