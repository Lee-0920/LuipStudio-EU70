#ifndef __TREASURE_SYSTEM_PROPERTY__
#define __TREASURE_SYSTEM_PROPERTY__

#include <memory>
#include <QMutex>

namespace Treasure
{
    template<class KEY_TYPE, class VALUE_TYPE>
    class Property
    {
    public:
        Property(KEY_TYPE key, VALUE_TYPE value)
        {
            this->key = key;
            this->value = value;
        }

        KEY_TYPE Key()
        {
            return this->key;
        }

        VALUE_TYPE Value()
        {
            return this->value;
        }

    private:
        KEY_TYPE key;
        VALUE_TYPE value;
    };
}

#endif
