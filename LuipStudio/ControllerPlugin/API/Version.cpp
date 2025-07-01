/**
 * @file
 * @brief 版本号。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */

#include "Version.h"

using namespace System;

namespace Controller
{
namespace API
{
Version::Version()
{
    m_major = 0;
    m_minor = 0;
    m_revision = 0;
    m_build = 0;
}

Version::Version(int major, int minor)
{
    m_major = major;
    m_minor = minor;
    m_revision = 0;
    m_build = 0;
}

Version::Version(int major, int minor, int revision)
{
    m_major = major;
    m_minor = minor;
    m_revision = revision;
    m_build = 0;
}

Version::Version(int major, int minor, int revision, int build)
{
    m_major = major;
    m_minor = minor;
    m_revision = revision;
    m_build = build;
}

Version::Version(String ver)
{ 
    char *str = (char*)ver.c_str();
    char *p = strtok(str,".");
    if (p)
    {
        m_major = atoi(p);
        p = strtok(NULL,".");
        if (p)
        {
            m_minor = atoi(p);
            p = strtok(NULL,".");
            if (p)
            {
                m_revision = atoi(p);
                p = strtok(NULL,".");
                if (p)
                {
                    m_build = atoi(p);
                }
                else
                {
                    m_build = 0;
                }
           }
            else
            {
                m_revision = 0;
                m_build = 0;
            }
        }
        else
        {
            m_minor = 0;
            m_revision = 0;
            m_build = 0;
        }
    }
    else
    {
        m_major = 0;
        m_minor = 0;
        m_revision = 0;
        m_build = 0;
    }
}
 
/**
 * @brief 使用裸数据构建版本号对象。
 * @param raw 裸数据。
 * @param len 裸数据的总长度，支持 4、8、16 个字节。
 * @note @ref SetData() 
 */
Version::Version(const void* raw, int len)
{
    if (len >= 16)
    {
        int* data = (int*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
    else if (len >= 8)
    {
        Uint16* data = (Uint16*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
    else if (len >= 4)
    {
        Uint8* data = (Uint8*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
}

/**
 * @brief 使用裸数据设置版本号对象，必须指定版本号数据的总长度（字节数）。
 * @details 目前支持的总长度有：
 *  - 4字节，major、minor等子版本号占1字节；
 *  - 8字节，major、minor等子版本号占2字节；
 *  - 16字节，major、minor等子版本号占4字节；
 * @param raw 裸数据。
 * @param len 裸数据的总长度，支持 4、8、16 个字节。
 */
void Version::SetData(void* raw, int len)
{
    if (len >= 16)
    {
        int* data = (int*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
    else if (len >= 8)
    {
        Uint16* data = (Uint16*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
    else if (len >= 4)
    {
        Uint8* data = (Uint8*)raw;
        m_major = data[0];
        m_minor = data[1];
        m_revision = data[2];
        m_build = data[3];
    }
}

void Version::GetData(void* raw, int len)
{
    if (len >= 16)
    {
        int* data = (int*)raw;
        data[0] = m_major;
        data[1] = m_minor;
        data[2] = m_revision;
        data[3] = m_build;
    }
    else if (len >= 8)
    {
        Uint16* data = (Uint16*)raw;
        data[0] = m_major;
        data[1] = m_minor;
        data[2] = m_revision;
        data[3] = m_build;
    }
    else if (len >= 4)
    {
        Uint8* data = (Uint8*)raw;
        data[0] = m_major;
        data[1] = m_minor;
        data[2] = m_revision;
        data[3] = m_build;
    }
}

int Version::GetMajor()
{
    return m_major;
}

void Version::SetMajor(int major)
{
    m_major = major;
}

int Version::GetMinor()
{
    return m_minor;
}

void Version::SetMinor(int minor)
{
    m_minor = minor;
}

int Version::GetRevision()
{
    return m_revision;
}

void Version::SetRevision(int revision)
{
    m_revision = revision;
}

int Version::GetBuild()
{
    return m_build;
}

void Version::SetBuild(int build)
{
    m_build = build;
}

bool Version::Parse(String ver)
{
    char *str = (char*)ver.c_str();
    char *p = strtok(str,".");
    if (p)
    {
        m_major = atoi(p);
        p = strtok(NULL,".");
        if (p)
        {
            m_minor = atoi(p);
            p = strtok(NULL,".");
            if (p)
            {
                m_revision = atoi(p);
                p = strtok(NULL,".");
                if (p)
                {
                    m_build = atoi(p);
                }
           }
        }
    }

    return true;
}

String Version::ToString() const
{
    String ver;
    
    if (m_build == 0 && m_revision == 0)
    {
        ver = std::to_string(m_major) + "." + std::to_string(m_minor);
    }
    else if (m_build == 0)
    {
        ver = std::to_string(m_major) + "." +
              std::to_string(m_minor) + "." +
              std::to_string(m_revision);
    }
    else
    {
        ver = std::to_string(m_major) + "." +
              std::to_string(m_minor) + "." +
              std::to_string(m_revision) + "." +
              std::to_string(m_build);
    }

    return ver;
}

String Version::ToCompleteVersionString() const
{
    String ver;
    
    ver = std::to_string(m_major) + "." +
            std::to_string(m_minor) + "." +
            std::to_string(m_revision) + "." +
            std::to_string(m_build);

    return ver;
}

bool Version::operator ==(const Version &other)
{
    if (other.m_major == this->m_major &&
        other.m_minor == this->m_minor &&
        other.m_revision == this->m_revision &&
        other.m_build == this->m_build)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Version::operator !=(const Version &other)
{
    if (*this == other)
    {
       return false;
    }
    else
    {
       return true;
    }
}
}
}
