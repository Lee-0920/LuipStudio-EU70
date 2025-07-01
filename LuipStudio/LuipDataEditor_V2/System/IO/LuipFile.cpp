#include "LuipFile.h"
#include "System/IO/Exception.h"
#include "System/Hash/md5.h"
#include "Log.h"

namespace System
{
LuipFile::LuipFile(String path):
    m_file(path.c_str()), m_path(path),m_needIntegrityCheck(false),
    m_defaultUuid("{a8a108c2-5f9d-4721-b3ae-19726701dc33}"),m_defaultVersion(1,0)
{
    memset(&m_luipFileHeader, 0, sizeof(m_luipFileHeader));
}

LuipFile::LuipFile(String path, bool integrityCheck):
    m_file(path.c_str()), m_path(path),m_needIntegrityCheck(integrityCheck),
    m_defaultUuid("{a8a108c2-5f9d-4721-b3ae-19726701dc33}"),m_defaultVersion(1,0)
{
    memset(&m_luipFileHeader, 0, sizeof(m_luipFileHeader));
}

LuipFile::LuipFile(const LuipFile &other):
        m_file(other.m_path.c_str()),
        m_defaultUuid("{a8a108c2-5f9d-4721-b3ae-19726701dc33}"),m_defaultVersion(1,0)
{
    this->m_path = other.m_path;
    memcpy(&m_luipFileHeader, &other.m_luipFileHeader, sizeof(this->m_luipFileHeader));
    this->m_needIntegrityCheck = other.m_needIntegrityCheck;
}

LuipFile::~LuipFile()
{
}

bool LuipFile::Load()
{
    bool ret = false;
    if (!m_file.exists())
    {
        ret = Save();
    }
    else
    {
        if (m_file.open(QIODevice::ReadOnly))
        {
            ret = LoadFrom();
            m_file.close();
        }
    }
    return ret;
}

bool LuipFile::Save()
{
    bool ret = false;
    if (m_file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
         ret = SaveTo();
         m_file.close();
    }
    return ret;
}

bool LuipFile::LoadFrom()
{
    m_file.seek(0);
    m_file.read((char*) &m_luipFileHeader, sizeof(m_luipFileHeader));

    if (m_defaultUuid != GetLabel())
    {
        String fileUuid = GetLabel().toString().toLatin1().data();
        String defaultUuid = m_defaultUuid.toString().toLatin1().data();
        logger->warn("%s uuid不匹配, 系统uuid： %s, 文件uuid：%s", m_path.c_str(),
                     defaultUuid.c_str(), fileUuid.c_str());
        throw System::IO::LuipFileException();
    }

//    if (NeedIntegrityCheck())
//    {
//        Byte hashCode[16];
//        ComputeFileHash(hashCode);

//        if (memcmp(m_luipFileHeader.hashCode, hashCode, sizeof(hashCode)) != 0)
//        {
//            logger->warn("%s 文件破损", m_path.c_str());
//            throw System::IO::IntegrityCheckException();
//        }
//    }

    if (GetVersion() != m_defaultVersion)
    {
        String fileVersion = GetVersion().ToString();
        String defaultVersion = m_defaultVersion.ToString().c_str();
        logger->warn("%s version error, defaultVersion：%s, fileVersion：%s", m_path.c_str(),
                     defaultVersion.c_str(), fileVersion.c_str());
       throw System::IO::UnsupportedVersionException();
    }

    return true;
}

bool LuipFile::SaveTo()
{
    SetLabel(m_defaultUuid);
    if (NeedIntegrityCheck())
    {
        ComputeFileHash(m_luipFileHeader.hashCode);
    }

    SetVersion(m_defaultVersion);

    m_file.seek(0);
    return m_file.write((char*) &m_luipFileHeader, sizeof(m_luipFileHeader));
}

QUuid LuipFile::GetLabel() const
{
    QByteArray array((const char *)m_luipFileHeader.label, sizeof(m_luipFileHeader.label));
    QUuid uuid = QUuid::fromRfc4122(array);
    return uuid;
}

bool LuipFile::SetLabel(QUuid uuid)
{
    memcpy(m_luipFileHeader.label, uuid.toRfc4122().data(), sizeof(m_luipFileHeader.label));
    return true;
}

String LuipFile::GetType() const
{
    return String(m_luipFileHeader.type);
}

bool LuipFile::SetType(String type)
{
    memcpy(m_luipFileHeader.type, type.c_str(), sizeof(m_luipFileHeader.type));
    return true;
}

Version LuipFile::GetVersion() const
{
    return Version(m_luipFileHeader.version, 8);
}

bool LuipFile::SetVersion(Version ver)
{
    ver.GetData(m_luipFileHeader.version, 8);
    return true;
}

void LuipFile::EnableIntegrityCheck(bool enable)
{
    m_needIntegrityCheck = enable;
}

bool LuipFile::NeedIntegrityCheck()
{
    return m_needIntegrityCheck;
}

bool LuipFile::MakeHash()
{
    if (m_file.isOpen())
    {
        ComputeFileHash(m_luipFileHeader.hashCode);
        m_file.seek(sizeof(m_luipFileHeader.label) + sizeof(m_luipFileHeader.type));
        return m_file.write((char*) &m_luipFileHeader.hashCode, sizeof(m_luipFileHeader.hashCode));
    }
    else
    {
        return false;
    }
}

bool LuipFile::ComputeFileHash(Byte *digest)
{
     Int64 pos = sizeof(m_luipFileHeader);
     if (m_file.isOpen() && pos < m_file.size())
     {
         MD5_CTX md5c;

         MD5Init(&md5c);
         int len = 1024;
         char *buffer = new char[len];
         while(pos < m_file.size())
         {
            m_file.seek(pos);
            int readLen = m_file.read(buffer, len);
            MD5Update(&md5c, (unsigned char*)buffer, readLen);
            pos += len;
         }
         delete []buffer;
         MD5Final(digest, &md5c);
         return true;
     }
     else
     {
         memset(digest, 0, 16);
         return false;
     }
}

String LuipFile::GetPath()
{
    return m_path;
}
}
