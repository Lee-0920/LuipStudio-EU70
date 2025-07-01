#include "RecordFile.h"
#include "System/IO/Exception.h"
#include "System/CopyFile.h"
#include "LuaEngine/LuaEngine.h"
#include "oolua.h"
#include <QTextStream>
#include <QTextCodec>
#include <QDateTime>
#include "Log.h"

using namespace OOLUA;
using namespace Lua;
using namespace ResultData;
using System::Version;

namespace Result
{
RecordFile::RecordFile(String path, String exportFileName, String formatTableName):
    LuipFile(path, true), m_formatTableName(formatTableName), m_exportFileName(exportFileName),
    m_defaultType("RecordFile")
{
    memset(&m_recordFileHeader, 0, sizeof(m_recordFileHeader));
    m_fileHeaderSize = sizeof(m_luipFileHeader) + sizeof(m_recordFileHeader);
    m_readPos = m_fileHeaderSize;
    m_writePos = m_readPos;
    m_recordFields = new RecordFields(m_formatTableName);
    m_currentRecord.SetSize(m_recordFields->GetFieldsSize());
}

RecordFile::RecordFile(const RecordFile& other):
    LuipFile(other),m_currentRecord(other.m_currentRecord.GetSize()),m_defaultType("RecordFile")
{
    memcpy(&m_recordFileHeader, &other.m_recordFileHeader, sizeof(this->m_recordFileHeader));
    this->m_fileHeaderSize = other.m_fileHeaderSize;
    this->m_readPos = other.m_readPos;
    this->m_writePos = other.m_writePos;
    this->m_formatTableName = other.m_formatTableName;
    this->m_exportFileName = other.m_exportFileName;
    this->m_recordFields = new RecordFields(*(other.m_recordFields));
}

RecordFile::~RecordFile()
{
    if (m_recordFields != nullptr)
    {
        delete m_recordFields;
    }
    if (m_file.isOpen())
    {
        m_file.close();
    }
}

bool RecordFile::Load()
{
    bool ret = false;
    if (!m_file.exists())
    {
        ret = Save();
        m_writePos = m_file.size();
        m_readPos = m_fileHeaderSize;
    }
    else
    {
        if (m_file.open(QIODevice::ReadWrite))
        {
            ret = LoadFrom();
            m_writePos = m_file.size();
            m_readPos = m_fileHeaderSize;
        }
    }
    return ret;
}

bool RecordFile::Save()
{
    bool ret = false;
    if(m_file.isOpen())
    {
        ret = SaveTo();
        m_file.flush();
    }
    else if (m_file.open(QIODevice::ReadWrite))
    {
         ret = SaveTo();
         m_file.flush();
    }
    return ret;
}

bool RecordFile::LoadFrom()
{
    this->LuipFile::LoadFrom();

    m_file.seek(sizeof(m_luipFileHeader));
    m_file.read((char*) &m_recordFileHeader, sizeof(m_recordFileHeader));

    if (GetType() != m_defaultType)
    {
        logger->warn("%s 文件类型不匹配, 系统支持类型：%s, 文件所属类型：%s", this->m_path.c_str(),
                     m_defaultType.c_str(), GetType().c_str());
        throw System::IO::UnexpectedFormatException();
    }

//    LuaEngine* luaEngine = LuaEngine::Instance();
//    lua_State* state = luaEngine->GetThreadState();

//    String model;
//    luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

//    if (model != GetTarget())
//    {
//        logger->warn("%s 仪器的类型不匹配, 仪器类型：%s, 文件所属类型：%s", this->m_path.c_str(),
//                     model.c_str(), GetTarget().c_str());
//        throw System::IO::LuipFileException();
//    }
    return true;
}

bool RecordFile::SaveTo()
{
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    ///String model = "PhotoTek 6000 TAs";
    String model;
    luaEngine->GetLuaValue(state, "config.info.instrument.model", model);

    SetType(m_defaultType);
    SetRecordVersion(m_recordFields->GetVersion());
    SetTarget(model);

    m_file.seek(sizeof(m_luipFileHeader));
    m_file.write((char*) &m_recordFileHeader, sizeof(m_recordFileHeader));
    m_file.flush();
    return this->LuipFile::SaveTo();
}

String RecordFile::GetRecordType() const
{
    return String(m_recordFileHeader.recordType);
}
bool RecordFile::SetRecordType(String type)
{
    memcpy(m_recordFileHeader.recordType, type.c_str(), sizeof(m_recordFileHeader.recordType));
    return true;
}

Version RecordFile::GetRecordVersion() const
{
    return Version(m_recordFileHeader.recordVersion, 16);
}

bool RecordFile::SetRecordVersion(System::Version ver)
{
    ver.GetData(m_recordFileHeader.recordVersion, 16);
    return true;
}

String RecordFile::GetTarget() const
{
    return String(m_recordFileHeader.target);
}

bool RecordFile::SetTarget(String target)
{
    memcpy(m_recordFileHeader.target, target.c_str(), sizeof(m_recordFileHeader.target));
    return true;
}

bool RecordFile::OpenFile()
{
    if (m_file.open(QIODevice::ReadWrite))
    {
        m_writePos = m_file.size();
        m_readPos = m_fileHeaderSize;
        return true;
    }
    return false;
}

bool RecordFile::IsExistRecord(ResultData::RecordData record)
{
    bool ret = false;
    System::Int64 bak_readPos = m_readPos;
    ResultData::RecordData bak_currentRecord = m_currentRecord;

    this->MoveToFirst();
    while(this->HasNext())
    {
        this->MoveNext();
        RecordData curRecord  = this->GetCurrentRecord();

        if(RecordCompare(curRecord, record))
        {
            ret = true;
            break;
        }
    }

    m_readPos = bak_readPos;
    m_currentRecord = bak_currentRecord;

    return ret;
}

bool RecordFile::RecordCompare(ResultData::RecordData sRecord, ResultData::RecordData dRecord)
{
    bool result = true;

    RecordField newField;
    m_recordFields->ResetFieldPos();
    while(m_recordFields->GetNextField(newField))
    {
        switch((FieldType)newField.type)
        {
         case FieldType::Bool:
             {
                 bool sRet,dRet;
                 sRecord.GetBool(newField.start, sRet);
                 dRecord.GetBool(newField.start, dRet);
                 if(sRet != dRet)
                 {
                     result = false;
                 }
             }
             break;
         case FieldType::Byte:case FieldType::Enum:
             {
                 Byte sRet,dRet;
                 sRecord.GetByte(newField.start, sRet);
                 dRecord.GetByte(newField.start, dRet);
                 if(sRet != dRet)
                 {
                     result = false;
                 }
             }
             break;
         case FieldType::Int:case FieldType::Time:
             {
                 int sRet,dRet;
                 sRecord.GetInt(newField.start, sRet);
                 dRecord.GetInt(newField.start, dRet);
                 if(sRet != dRet)
                 {
                     result = false;
                 }
             }
             break;
         case FieldType::Float:
             {
                 float sRet,dRet;
                 QString sStr,dStr;
                 sRecord.GetFloat(newField.start, sRet);
                 dRecord.GetFloat(newField.start, dRet);
                 sStr = QString::number(sRet,'g',6);
                 dStr = QString::number(dRet,'g',6);
                 if(sStr != dStr)
                 {
                     result = false;
                 }
             }
             break;
         case FieldType::Double:
             {
                 double sRet,dRet;
                 QString sStr,dStr;
                 sRecord.GetDouble(newField.start, sRet);
                 dRecord.GetDouble(newField.start, dRet);
                 sStr = QString::number(sRet,'g',9);
                 dStr = QString::number(dRet,'g',9);
                 if(sStr != dStr)
                 {
                     result = false;
                 }
             }
             break;
         case FieldType::IntArray:
             {
                 IntArray sRet,dRet;
                 int* sData;
                 int* dData;
                 sRecord.GetIntArray(newField.start, sRet);
                 dRecord.GetIntArray(newField.start, dRet);
                 sData = sRet.GetData();
                 dData = dRet.GetData();
                 if((sRet.GetSize() != dRet.GetSize())||(memcmp(sData, dData, sRet.GetSize()) != 0))
                 {
                     result = false;
                 }
             }
             break;
        }
        if(!result)
        {
            break;
        }
    }

    return result;
}

bool RecordFile::AddRecord(RecordData result)
{
     bool status = false;
     m_mutex.lock();
     if (m_file.isOpen())
     {
         m_file.seek(m_writePos);
         m_writePos += m_file.write((const char *)result.GetData(), result.GetSize());
         m_file.flush();
         status = MakeHash();
         m_file.flush();
     }
     m_mutex.unlock();
     return status;
}

bool RecordFile::ClearRecord(void)
{
    bool status = false;
    if (m_file.isOpen())
    {
        m_file.close();
    }
    if (m_file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        status = SaveTo();
        m_file.flush();
        m_writePos = m_file.size();
        m_readPos = m_fileHeaderSize;
    }
    return status;
}

RecordData RecordFile::GetCurrentRecord(void)
{
   return m_currentRecord;
}

bool RecordFile::HasNext(void)
{
    bool status = false;
    if (m_file.isOpen())
    {
        if (m_readPos < m_file.size())
        {
            status = true;
        }
    }
    return status;
}

bool RecordFile::HasPrevious(void)
{
    if (m_readPos > m_fileHeaderSize)
    {
        return true;
    }

    return false;
}

bool RecordFile::MoveNext(void)
{
    bool status = false;

    if (m_file.isOpen())
    {
        m_file.seek(m_readPos);         // 恢复操作前文件指针位置

        m_file.read((char *)m_currentRecord.GetData(), m_currentRecord.GetSize());

        m_readPos += m_currentRecord.GetSize();

        status = true;
    }

    return status;
}

bool RecordFile::MovePrevious(void)
{
    bool status = false;

    if (m_file.isOpen())
    {
        m_readPos -= m_currentRecord.GetSize();
        m_file.seek(m_readPos);

        m_file.read((char *)m_currentRecord.GetData(), m_currentRecord.GetSize());

        status = true;
    }

    return status;
}

bool RecordFile::MovePrevious(System::Uint32 pieceNumber)
{
    bool status = false;

    Int64 pos = m_readPos - m_currentRecord.GetSize() * pieceNumber;
    if (m_file.isOpen() && pieceNumber > 0 && pos >= m_fileHeaderSize)
    {
        m_readPos = pos;
        m_file.seek(m_readPos);

        m_file.read((char *)m_currentRecord.GetData(), m_currentRecord.GetSize());

        status = true;
    }

    return status;
}

bool RecordFile::MoveToLast(void)
{
    bool status = false;

    if (m_file.isOpen())
    {
        m_readPos = m_file.size();

        status = true;
    }

    return status;
}

bool RecordFile::SetReadPos(Int64 pos)
{
    bool status = false;

    if (m_file.isOpen() && pos >= m_fileHeaderSize && pos <= m_file.size())
    {
        m_readPos = pos;
        status = true;
    }
    return status;
}

Int64 RecordFile::GetReadPos(void)
{
    return m_readPos;
}

void RecordFile::MoveToFirst(void)
{
    m_readPos = m_fileHeaderSize;
}

Uint32 RecordFile::GetRemainNum(void)
{
    Uint64 remain;
    if (m_file.isOpen())
    {
        remain = m_file.size() - m_readPos;
    }

    return (remain / m_currentRecord.GetSize());  //距尾部剩余多少条记录
}

Uint32 RecordFile::GetHeadNum(void)
{
    return ((m_readPos - m_fileHeaderSize) / m_currentRecord.GetSize());  //距头部剩余多少条记录
}

RecordFields* RecordFile::GetRecordFields(void)
{
    return m_recordFields;
}

bool RecordFile::IsMigrateData(void)
{
    //获取文件头版本号与当前使用数据模板的版本号是否一样
    if (GetRecordVersion() != m_recordFields->GetVersion())
    {
        logger->debug("%s 文件版本号与当前数据模板的版本号不一样，进行数据迁移", this->m_path.c_str());
        return true;
    }
    else
    {
        return false;
    }
}

RecordFile& RecordFile::MigrateData(RecordFile& originalFile)
{
    if (this->m_file.isOpen())
    {
        this->m_file.close();
    }

    //改名后文件名，并命名为 this->m_path + "o"
    originalFile.m_path = this->m_path + "o";
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
    originalFile.m_currentRecord.SetSize(originalFile.m_recordFields->GetFieldsSize());

    //打开本文件，数据模板当前使用的版本号重新设置到结果文件，清除文件内容计算MD5写入新的文件头
    this->SetRecordVersion(this->m_recordFields->GetVersion());
    this->Load();

    RecordData newRecordData(this->m_recordFields->GetFieldsSize());//要写入新文件的单条数据

    originalFile.OpenFile();
    /**
     * 从头开始循环取出原始文件每条数据
     * 得到一条数据后又根据当前使用的数据模板循环每个字段，从该数据查找是否有同个字段的数据
     */

    originalFile.MoveToFirst();
    while(originalFile.GetRemainNum() > 0)
    {
        //取出单条原始数据
        originalFile.MoveNext();
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
                     if (originalFile.GetFieldCurrentRecordValue(newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Byte:case FieldType::Enum:
                 {
                     Byte ret;
                     if (originalFile.GetFieldCurrentRecordValue(newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Int:case FieldType::Time:
                 {
                     int ret;
                     if (originalFile.GetFieldCurrentRecordValue(newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Float:
                 {
                     float ret;
                     if (originalFile.GetFieldCurrentRecordValue(newField.name, ret))
                     {
                        newRecordData.SetFieldData(newField.start, ret);
                     }
                 }
                 break;
             case FieldType::Double:
                 {
                     double ret;
                     if (originalFile.GetFieldCurrentRecordValue(newField.name, ret))
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
                     originalFile.m_currentRecord.GetIntArray(recordField.start, array);
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
    return *this;
}

bool RecordFile::ExportFiles(RoleType roletype)
{
    std::vector<String> exportFields;
    RecordField recordField;
    m_recordFields->ResetFieldPos();
    while(this->m_recordFields->GetNextField(recordField))
    {
        if (recordField.privilege <= roletype)
        {
            String name = recordField.name;
            exportFields.push_back(name);
        }
    }
    m_recordFields->ResetFieldPos();

    /**
    * 创建导出文件
    */
    CopyFileAction copyFileAction;
    String strDir = copyFileAction.GetTargetDir();
    QDir dir(strDir.c_str());
    QString dPath = dir.filePath(this->m_exportFileName.c_str());
    QFile file(dPath);
    bool ret = true;
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        QTextCodec *codec = QTextCodec::codecForName("GBK");
        out.setCodec(codec);

        /**
        * 把表头写入导出文件
        */
        for (std::vector<String>::iterator iter = exportFields.begin(); iter != exportFields.end(); ++iter)
        {
            String text;
            this->m_recordFields->GetFieldText(*iter, text);
            out << QObject::tr(text.c_str()) << " ,";//兼容中文
        }
        out << ",\n";

        /**
        * 依次读出每一条结果，然后根据导出结果文件的数据模板把需要数据写入导出文件
        */
        Int64 readPos = this->m_readPos;//记录原本读取位置，避免文件导出时弄乱读取位置

        this->MoveToFirst();
        while(this->GetRemainNum() > 0)
        {
            this->MoveNext();
            RecordData recordData = this->GetCurrentRecord();

            for (std::vector<String>::iterator iter = exportFields.begin(); iter != exportFields.end(); ++iter)
            {
                RecordField resultfield;
                if (this->m_recordFields->GetField(*iter, resultfield))
                {
                    switch((FieldType)resultfield.type)
                    {
                        case FieldType::Bool:
                            {
                                bool ret;
                                recordData.GetFieldData(resultfield.start, ret);//获取结果文件的数据
                                out << ret << ",";
                            }
                            break;
                        case FieldType::Byte:
                            {
                                Byte ret;
                                recordData.GetFieldData(resultfield.start, ret);
                                out << ret << ",";
                            }
                            break;
                        case FieldType::Int:
                            {
                                int ret;
                                recordData.GetFieldData(resultfield.start, ret);
                                out << ret << ",";
                            }
                            break;
                        case FieldType::Float:
                            {
                                float ret;
                                recordData.GetFieldData(resultfield.start, ret);
                                out << ret << ",";
                            }
                            break;
                        case FieldType::Double:
                            {
                                double ret;
                                recordData.GetFieldData(resultfield.start, ret);
                                out << ret << ",";
                            }
                            break;
                        case FieldType::IntArray:
                            {
                                IntArray array(resultfield.size / sizeof(int));
                                recordData.GetIntArray(resultfield.start, array);
                                out << array.GetData() << ",";
                            }
                            break;
                        case FieldType::Time:
                            {
                                int ret;
                                recordData.GetFieldData(resultfield.start, ret);
                                out << QDateTime::fromTime_t(ret).toString("yyyy-MM-dd hh:mm:ss").toLatin1().data() << ",";
                            }
                            break;
                        case FieldType::Enum:
                            {
                                Byte ret;
                                String str;
                                recordData.GetFieldData(resultfield.start, ret);
                                str = resultfield.option.at(ret);
                                out << QObject::tr(str.c_str()) << ",";
                            }
                            break;
                    }
                }
            }
            out << endl;
        }
        if(out.status() != QTextStream::Ok)
        {
            ret = false;
        }
        file.close();
        this->m_readPos = readPos;
    }
#ifdef    _CS_ARM_LINUX
    system("sync");
#endif
    if (file.exists() && ret)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordFile::GetFieldCurrentRecordEnumString(const String& name, String& text)
{
    bool ret = false;

    RecordField recordField;
    if (m_recordFields->GetField(name, recordField))
    {
        Byte value;
        ret = m_currentRecord.GetFieldData(recordField.start, value);
        text = recordField.option.at(value);
    }

    return ret;
}

String RecordFile::GetFormatTableName()
{
    return m_formatTableName;
}

String RecordFile::GetExportFileName()
{
    return m_exportFileName;
}

}
