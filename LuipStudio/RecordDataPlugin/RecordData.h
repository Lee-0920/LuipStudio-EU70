#if !defined(RESULTMANAGER_RECORDDATA_H)
#define RESULTMANAGER_RECORDDATA_H

#include "System/Types.h"
#include <memory>
#include "LuipShare.h"

using System::Byte;

namespace ResultData
{

class LUIP_SHARE IntArray
{
public:
    IntArray(System::Uint32 size = 0):
        m_size(size)
    {
        if (m_size > 0)
        {
            //若使用shared_ptr管理一个动态数组，则需手动制定一个删除器。
            m_data.reset(new int[size], std::default_delete<int[]>());
            memset(m_data.get(), 0, size);
        }
    }

    int& operator[](System::Uint32 index)
    {
        return m_data.get()[index];
    }

    int* GetData(void)
    {
        return m_data.get();
    }

    System::Uint32 GetSize(void) const
    {
        return m_size;
    }

    System::Uint32 GetLength(void) const
    {
        return m_size * sizeof(int);
    }
    /**
     * @brief SetInt
     * @note 为了让LUA能设置数组的元素
     * @param idxByte
     * @param value
     * @return
     */
    bool SetInt(System::Uint32 idxByte, int value)
    {
        if (idxByte <= m_size)
        {
            m_data.get()[idxByte] = value;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool GetInt(System::Uint32 idxByte, int& value)
    {
        if (idxByte <= m_size)
        {
            value = m_data.get()[idxByte];
            return true;
        }
        else
        {
          return false;
        }
    }

private:
    std::shared_ptr<int> m_data;
    System::Uint32 m_size;
};


/**
 * @brief 记录文件的数据
 */
class LUIP_SHARE RecordData
{
public:
    RecordData(System::Uint32 size = 0);
    System::Byte* GetData(void);
    void SetData(System::Byte* data, System::Uint32 size);
    System::Uint32 GetSize(void) const;
    void SetSize(System::Uint32 size);
    void ResetPos(void);
    bool SetPos(System::Uint32 pos);
    bool PushBool(bool value);
    bool PushInt(int value);
    bool PushByte(System::Byte value);
    bool PushFloat(float value);
    bool PushDouble(double value);
    bool PushIntArray(IntArray array);
    bool PushString(std::string info);
    bool GetBool(System::Uint32 idxByte, bool& value);
    bool GetInt(System::Uint32 idxByte, int& value);
    bool GetByte(System::Uint32 idxByte, System::Byte& value);
    bool GetFloat(System::Uint32 idxByte, float& value);
    bool GetDouble(System::Uint32 idxByte, double& value);
    bool GetIntArray(System::Uint32 idxByte, IntArray& array);
    bool SetIntArray(System::Uint32 idxByte, IntArray array);    
    /**
     *@note T1只支持基本内置类型
     */
    template<typename T, typename T1>bool GetFieldData(T const& idxByte, T1& value)
    {
        if ((idxByte + sizeof(value)) <= m_size)
        {
            memcpy(&value, &m_data.get()[idxByte],  sizeof(value));
            return true;
        }
        else
        {
            return false;
        }
    }
    /**
     *@note T1只支持基本内置类型
     */
    template<typename T, typename T1>bool SetFieldData(T const& idxByte, T1  const& value)
    {
        if ((idxByte + sizeof(value)) <= m_size)
        {
            memcpy(&m_data.get()[idxByte], &value, sizeof(value));
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    std::shared_ptr<Byte> m_data;
    System::Uint32 m_size;
    System::Uint32 m_currentBytePos;//位于Byte数组的位置，使用Push*函数都会使其增加
};

}
#endif // RESULTMANAGER_RECORDDATA_H
