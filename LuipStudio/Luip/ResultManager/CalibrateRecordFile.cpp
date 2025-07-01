#include "CalibrateRecordFile.h"
#include "System/IO/Exception.h"
#include "Log.h"

using namespace ResultData;

namespace Result
{
CalibrateRecordFile::CalibrateRecordFile(String path, String exportFileName, String formatTableName):
    RecordFile(path, exportFileName, formatTableName),m_recordType("CalibrateRecordFile")
{

}

bool CalibrateRecordFile::LoadFrom()
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

bool CalibrateRecordFile::SaveTo()
{
    SetRecordType(m_recordType);
    return this->RecordFile::SaveTo();
}

CalibrateRecordFile& CalibrateRecordFile::CleanInvalidData(CalibrateRecordFile& originalFile)
{
    if (this->m_file.isOpen())
    {
        this->m_file.close();
    }

    //改名后文件名，并命名为 this->m_path + "_temp"
    originalFile.m_path = this->m_path + "_temp";
    if (QFile::exists(originalFile.m_path.c_str()))
    {
        QFile::remove(originalFile.m_path.c_str());
    }
    originalFile.m_file.rename(originalFile.m_path.c_str());

    //根据文件头版本号(GetRecordVersion()保存的还是原始文件的信息)获取原始文件的数据模板
    if (originalFile.m_recordFields)
    {
        delete originalFile.m_recordFields;
    }
    originalFile.m_recordFields = new RecordFields(this->m_formatTableName, this->GetRecordVersion());

    //设置原始数据模板的单条数据总长度
    m_mutexReader.lock();
    QMap<int, RecordDataPos*>::iterator iter = originalFile.m_currentRecord.begin();
    for(; iter != originalFile.m_currentRecord.end(); ++iter)
    {
        iter.value()->data.SetSize(originalFile.m_recordFields->GetFieldsSize());
    }
    m_mutexReader.unlock();

    //打开本文件，数据模板当前使用的版本号重新设置到结果文件，清除文件内容计算MD5写入新的文件头
    this->SetRecordVersion(this->m_recordFields->GetVersion());
    this->Load();

    RecordData newRecordData(this->m_recordFields->GetFieldsSize());//要写入新文件的单条数据

    originalFile.OpenFile();
    /**
     * 从头开始循环取出原始文件每条数据
     * 得到一条数据后又根据当前使用的数据模板循环每个字段，从该数据查找是否有同个字段的数据
     */

    originalFile.MoveToFirst(originalFile.GetSelfReaderIndex());
    while(originalFile.GetRemainNum(originalFile.GetSelfReaderIndex()) > 0)
    {
        //取出单条原始数据
        originalFile.MoveNext(originalFile.GetSelfReaderIndex());

        //去除定标时间为0的数据
        RecordField timeField;
        if(this->m_recordFields->GetField("calibrateTime", timeField))
        {
            int ret;
            originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), timeField.name, ret);
            if(ret == 0)
            {
                continue;
            }
        }

        //给新数据每个字段赋值
        RecordField newField;

        while(this->m_recordFields->GetNextField(newField))
        {

             //根据当前字段名获取原始同一名字的字段在当前记录的数值，获取到则把新数据并写到新的单条数据里
            switch((FieldType)newField.type)
            {
             case FieldType::Bool:
                 {
                     bool ret;
                     if (originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Byte:case FieldType::Enum:
                 {
                     Byte ret;
                     if (originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Int:case FieldType::Time:
                 {
                     int ret;
                     if (originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Float:
                 {
                     float ret;
                     if (originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Double:
                 {
                     double ret;
                     if (originalFile.GetFieldCurrentRecordValue(originalFile.GetSelfReaderIndex(), newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::IntArray:
                 {
                     RecordField recordField;
                     originalFile.m_recordFields->GetField(newField.name, recordField);
                     IntArray array(recordField.size / sizeof(int));
                     originalFile.GetCurrentRecord(originalFile.GetSelfReaderIndex()).GetIntArray(recordField.start, array);
                     newRecordData.SetIntArray(newField.start, array);
                 }
                 break;
            }
        }
        this->m_recordFields->ResetFieldPos();
        this->AddRecord(newRecordData);
        newRecordData.ResetPos();
    }
    originalFile.m_file.close();
    QFile::remove(originalFile.m_path.c_str());
    return *this;
}

}
