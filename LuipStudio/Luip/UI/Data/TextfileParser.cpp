#include "TextfileParser.h"
#include <QDebug>

using namespace std;
using namespace System;

namespace UI
{

TextfileParser::TextfileParser(QString fileName)
{    
    m_fileName = fileName;
    QFile file(m_fileName);
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        file.close();
    }
    if (file.open(QFile::WriteOnly | QFile::Append))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        file.close();
    }
    memset(m_buffer,0,sizeof(m_buffer));
    m_pos = 0;
}

long long TextfileParser::TotalSize()
{
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        long long totalSize = file.size();
        file.close();
        return totalSize;
    }
    return 0;
}

char* TextfileParser::GetCurrentRecord()
{
    return m_buffer;
}

bool TextfileParser::HasNext()
{
    bool status = false;

    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        if (m_pos < file.size())
        {
            status = true;
        }
        file.close();
    }

    return status;
}

bool TextfileParser::HasPrevious()
{
    if (m_pos > 0)
    {
        return true;
    }

    return false;
}

bool TextfileParser::MoveNext()
{
    bool status = false;

    QFile file(m_fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        file.seek(m_pos);         // 恢复操作前文件指针位置
        file.read(m_buffer, 20480);
        file.close();
        m_pos += 20480;
        status = true;
    }

    return status;
}

bool TextfileParser::MovePrevious()
{
    bool status = false;

    QFile file(m_fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        m_pos -= 20480;
        file.seek(m_pos);

        file.read(m_buffer, 20480);

        file.close();
        status = true;
    }
    return status;
}

bool TextfileParser::MoveToLast()
{
    bool status = false;

    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_pos = file.size();
        file.close();
        status = true;
    }

    return status;
}

void TextfileParser::MoveToFirst()
{
    m_pos = 0;
}

long long TextfileParser::GetPosition()
{
    return m_pos;
}

int  TextfileParser::TotalBlock()
{
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        long long totalBlock = 0;
        if(file.size()%20480)
        {
            totalBlock = file.size()/20480 + 1;
        }
        else
        {
            totalBlock = file.size()/20480;
        }
        file.close();
        return totalBlock;
    }
    return 0;
}

char *TextfileParser::SeekBlock(int currentBlock)
{
    QFile file(m_fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        if(TotalSize() - (currentBlock+1) *20480 > 0)
        {
            file.seek(TotalSize() - (currentBlock+1) *20480);
            file.read(m_buffer, 20480);
            file.readLine(m_buffer + 20480, 256);
        }
        else
        {
            memset(m_buffer,0,sizeof(m_buffer));
            file.seek(0);
            file.read(m_buffer, TotalSize() - currentBlock *20480);
			file.readLine(m_buffer + TotalSize() - currentBlock * 20480, 256);
        }
        file.close();
    }
    return m_buffer;
}

Uint32 TextfileParser::GetRemainNum()
{
    Uint64 remain;
    QFile file(m_fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        remain = file.size() - m_pos;

        file.close();
    }
    return remain;  //距尾部剩余多少条记录
}

Uint32 TextfileParser::GetHeadNum()
{
    return m_pos;  //距头部剩余多少条记录
}

bool TextfileParser::ClearRecord()
{
    bool status = false;

    m_mutex.lock();
    QFile file(m_fileName);
    if (file.exists())
    {
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        file.close();
        m_pos = 0;
        status = true;
    }
    m_mutex.unlock();
    return status;
}
}
