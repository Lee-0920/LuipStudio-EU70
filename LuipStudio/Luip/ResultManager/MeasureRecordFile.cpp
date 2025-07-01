#include "MeasureRecordFile.h"
#include "System/IO/Exception.h"
#include "Log.h"

namespace Result
{
MeasureRecordFile::MeasureRecordFile(String path, String exportFileName, String formatTableName):
    RecordFile(path, exportFileName, formatTableName),m_recordType("MeasureRecordFile")
{

}

bool MeasureRecordFile::LoadFrom()
{
    this->RecordFile::LoadFrom();
    if (GetRecordType() != m_recordType)
    {
        logger->warn("%s 记录文件类型不匹配, 系统支持类型： %s, 文件所属类型：%s", this->m_path.c_str(),
                     m_recordType.c_str(), GetRecordType().c_str());
        throw System::IO::UnexpectedFormatException();
    }
    return true;
}

bool MeasureRecordFile::SaveTo()
{
    SetRecordType(m_recordType);
    return this->RecordFile::SaveTo();
}
}
