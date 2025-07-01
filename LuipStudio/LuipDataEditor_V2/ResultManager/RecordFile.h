#if !defined(RESULTMANAGER_RECORDFILE_H)
#define RESULTMANAGER_RECORDFILE_H

#include "System/IO/LuipFile.h"
#include <QMutex>
#include "RecordDataPlugin/RecordData.h"
#include "RecordFields.h"
//#include "UI/Frame/LoginDialog.h"

namespace Result
{
struct RecordFileHeader
{
    char recordType[32];  //当前记录文件的类型
    Byte recordVersion[16];//当前文件的版本号
    char target[24];       //当前仪器的类型
    char reserved[184];//保留信息
};

class RecordFile: public System::LuipFile
{
public:
    RecordFile(String path, String exportFileName, String formatTableName);
    RecordFile(const RecordFile& other);
    virtual ~RecordFile();

    bool Load();
    bool Save();

    bool OpenFile();
    bool AddRecord(ResultData::RecordData result);
    bool ClearRecord(void);
    ResultData::RecordData GetCurrentRecord(void);
    bool IsExistRecord(ResultData::RecordData record);

    bool HasNext(void);
    bool HasPrevious(void);
    bool MoveNext(void);
    bool MovePrevious(void);
    bool MovePrevious(System::Uint32 pieceNumber);
    bool MoveToLast(void);
    void MoveToFirst(void);
    bool SetReadPos(System::Int64 pos);
    System::Int64 GetReadPos(void);
    System::Uint32 GetRemainNum(void);
    System::Uint32 GetHeadNum(void);
    bool RecordCompare(ResultData::RecordData sRecord, ResultData::RecordData dRecord);

    RecordFields* GetRecordFields(void);
    bool IsMigrateData(void);
    RecordFile& MigrateData(RecordFile& originalFile);
    bool ExportFiles(RoleType roletype);
    /**
     * @brief 获取当前记录数据条的名为name字段的数值
     * @param name[in] 字段名字，value[out] 数值
     * @return 是否获取成功
     */
    template<typename T, typename T1>bool GetFieldCurrentRecordValue(T const& name, T1& value)
    {
        bool ret = false;

        RecordField recordField;
        if (m_recordFields->GetField(name, recordField))
        {
            //根据当前字段名获取原始同一名字的字段在当前记录的数值，获取到则把新数据并写到新的单条数据里
            switch((FieldType)recordField.type)
            {
             case FieldType::Bool:
                 {
                     bool oldValue;
                     ret = m_currentRecord.GetBool(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Byte:case FieldType::Enum:
                 {
                     Byte oldValue;
                     ret = m_currentRecord.GetByte(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Int:case FieldType::Time:
                 {
                     int oldValue;
                     ret = m_currentRecord.GetInt(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Float:
                 {
                     float oldValue;
                     ret = m_currentRecord.GetFloat(recordField.start, oldValue);
                     value = oldValue;
                 break;
                 }
             case FieldType::Double:
                 {
                     double oldValue;
                     ret = m_currentRecord.GetDouble(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             default:
                 ret = m_currentRecord.GetFieldData(recordField.start, value);
                 break;
            }
        }

        return ret;
    }
    bool GetFieldCurrentRecordEnumString(const String& name, String& text);
    String GetFormatTableName();
    String GetExportFileName();

protected:
    virtual bool LoadFrom();
    virtual bool SaveTo();

    String GetRecordType() const;
    bool SetRecordType(String type);
    System::Version GetRecordVersion() const;
    bool SetRecordVersion(System::Version ver);
    String GetTarget() const;
    bool SetTarget(String target);


protected:
    RecordFileHeader m_recordFileHeader;
    System::Uint32 m_fileHeaderSize;
    System::Int64 m_readPos;
    System::Int64 m_writePos;
    QMutex m_mutex;//写文件的锁
    String m_formatTableName;
    String m_exportFileName;
    ResultData::RecordData m_currentRecord;//读取文件的一条当前数据记录
    RecordFields *m_recordFields;
    const System::String m_defaultType;
};

}
#endif // RESULTMANAGER_RECORDFILE_H
