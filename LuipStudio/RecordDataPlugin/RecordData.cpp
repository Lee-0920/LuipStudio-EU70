#include "RecordData.h"
#include "QDebug"

using namespace System;

namespace ResultData
{
/**
 * @brief 根据size创建一个数组
 * @param size 数组字节数
 */
RecordData::RecordData(Uint32 size):
    m_size(size),m_currentBytePos(0)
{
    if (m_size > 0)
    {
        m_data.reset(new Byte[size], std::default_delete<Byte[]>());
        memset(m_data.get(), 0, size);
    }
}

/**
 * @brief 获取数组数据，当数组销毁时返回的指针所指向的数据出错
 * @note 调用 SetData SetSize 便会销毁以前的数据
 * @return  数组数据的指针
 */
Byte* RecordData::GetData(void)
{
    return m_data.get();
}

/**
 * @brief 销毁以前数据，以size创建一个数组，并拷贝一份data的内容到数组里
 * @param data
 * @param size
 */
void RecordData::SetData(Byte* data, Uint32 size)
{
    if (size > 0)
    {
        m_currentBytePos = 0;
        m_size = size;
        m_data.reset(new Byte[size], std::default_delete<Byte[]>());
        memcpy(m_data.get(), data, size);
    }
}

Uint32 RecordData::GetSize(void) const
{
    return m_size;
}

/**
 * @brief 设置数组大小，比原来小的话，会把多余的数据舍弃。
 * @note 设置大小会把原来地址对应的数据销毁
 * @param size
 */
void RecordData::SetSize(Uint32 size)
{
    if (size > 0)
    {
        Byte *data = new Byte[m_size];//保留原有数据
        memcpy(data, m_data.get(), m_size);
        m_data.reset(new Byte[size], std::default_delete<Byte[]>());
        if (m_size <= size)//如果原来数据的大小比现在小于等于，则把原来的数据全部拷贝
        {
            memcpy(m_data.get(), data, m_size);
        }
        else//如果原来数据的大小比现在大，则按现在的大小拷贝原来的数据，剩余的舍弃
        {
            memcpy(m_data.get(), data, size);
        }
        m_size = size;
        m_currentBytePos = 0;
        delete []data;
    }
}

void RecordData::ResetPos(void)
{
    m_currentBytePos = 0;
}

bool RecordData::SetPos(System::Uint32 pos)
{
    if(pos < m_size)
    {
        m_currentBytePos = pos;
        return true;
    }
    else
    {
        return false;
    }
}


bool RecordData::PushBool(bool value)
{
    if ((m_currentBytePos + sizeof(value)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], &value, sizeof(value));
        m_currentBytePos += sizeof(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushInt(int value)
{
    if ((m_currentBytePos + sizeof(value)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], &value, sizeof(value));
        m_currentBytePos += sizeof(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushByte(Byte value)
{
    if ((m_currentBytePos + sizeof(value)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], &value, sizeof(value));
        m_currentBytePos += sizeof(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushFloat(float value)
{
    if ((m_currentBytePos + sizeof(value)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], &value, sizeof(value));
        m_currentBytePos += sizeof(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushDouble(double value)
{
    if ((m_currentBytePos + sizeof(value)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], &value, sizeof(value));
        m_currentBytePos += sizeof(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushIntArray(IntArray array)
{
    if ((m_currentBytePos + array.GetLength()) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], array.GetData(), array.GetLength());
        m_currentBytePos += array.GetLength();
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::GetBool(Uint32 idxByte, bool& value)
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

bool RecordData::GetInt(Uint32 idxByte, int& value)
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

bool RecordData::GetByte(Uint32 idxByte, Byte& value)
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

bool RecordData::GetFloat(Uint32 idxByte, float& value)
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

bool RecordData::GetDouble(Uint32 idxByte, double& value)
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
 * @brief RecordData::GetIntArray
 * @param index
 * @param array
 * @return
 */
bool RecordData::GetIntArray(Uint32 idxByte, IntArray& array)
{
    if ((array.GetLength() + idxByte) <= m_size)
    {
        memcpy(array.GetData(), &m_data.get()[idxByte], array.GetLength());
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::SetIntArray(Uint32 idxByte, IntArray array)
{
    if ((m_currentBytePos + array.GetLength()) <= m_size)
    {
        memcpy(&m_data.get()[idxByte], array.GetData(), array.GetLength());
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordData::PushString(std::string info)
{
    QString str = QString(info.c_str());
    QByteArray ba = str.toLocal8Bit();
    if ((m_currentBytePos + sizeof(str)) <= m_size)
    {
        memcpy(&m_data.get()[m_currentBytePos], ba.data(), ba.size()+1);
        m_currentBytePos += sizeof(ba.size()+1);
        return true;
    }
    else
    {
        return false;
    }
}


}
