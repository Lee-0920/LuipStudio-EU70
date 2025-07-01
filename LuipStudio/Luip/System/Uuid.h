#ifndef SYSTEM_UUID_H_
#define SYSTEM_UUID_H_


namespace System
{

/**
 * @brief 128位的通用唯一标识符。
 */
struct Uuid
{
    unsigned long data1;
    unsigned short data2;
    unsigned short data3;
    unsigned char data4[8];

    bool operator< (const Uuid& id) const
    {
        // Uuid 作为大小比较意义不大，这里只是绕过编译器的检测（作map容器的key时）
        return (this->data1 < id.data1);
    }

    /**
     * @brief 判断该UUID是否有效。前半部全为零则无效。
     */
    bool IsValid() const
    {
        return (data1 != 0 || data2 != 0 || data3 != 0);
    }
};


inline bool operator== (const Uuid& id1, const Uuid& id2)
{
    return (0 == memcmp(&id1, &id2, sizeof(Uuid)));
}

inline bool operator!= (const Uuid& id1, const Uuid& id2)
{
    return !(id1 == id2);
}


} // namespace System

#endif  // SYSTEM_UUID_H_