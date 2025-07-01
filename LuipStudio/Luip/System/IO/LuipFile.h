#ifndef SYSTEM_IO_LUIPFILE_H
#define SYSTEM_IO_LUIPFILE_H

#include <QFile>
#include "System/Types.h"
#include "ControllerPlugin/API/Version.h"
#include <QUuid>

namespace System
{

/**
 * @brief 所有LuipFile文件的第一层文件头
 */
struct LuipFileHeader
{
    char label[16];//UUID
    char type[24];//文件类型
    Byte hashCode[16];//MD5完整性检测码
    short version[4];//LuipFileHeader文件头版本号
    Byte reserved[32];//预留区域
};

class LuipFile
{
public:
    LuipFile(String path);
    LuipFile(String path, bool integrityCheck);
    LuipFile(const LuipFile &other);

    virtual ~LuipFile();
    virtual bool Load();
    virtual bool Save();
    String GetPath();

protected:
    virtual bool LoadFrom();
    virtual bool SaveTo();
    QUuid GetLabel() const;
    bool SetLabel(QUuid uuid);
    String GetType() const;
    bool SetType(String type);
    Controller::API::Version GetVersion() const;
    bool SetVersion(Controller::API::Version ver);
    void EnableIntegrityCheck(bool enable);
    bool NeedIntegrityCheck();
    virtual bool MakeHash();

private:
    bool ComputeFileHash(Byte *digest);

protected:
    QFile m_file;
    String m_path;
    LuipFileHeader m_luipFileHeader;
    bool m_needIntegrityCheck;
    const QUuid m_defaultUuid;
    const Controller::API::Version m_defaultVersion;
};
}
#endif // SYSTEM_IO_LUIPFILE_H
