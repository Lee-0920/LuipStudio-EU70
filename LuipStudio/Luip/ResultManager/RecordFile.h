#if !defined(RESULTMANAGER_RECORDFILE_H)
#define RESULTMANAGER_RECORDFILE_H

#include <QMutex>
#include <QMap>
#include "System/IO/LuipFile.h"
#include "RecordDataPlugin/RecordData.h"
#include "RecordFields.h"
#include "UI/Frame/LoginDialog.h"

namespace Result
{
struct RecordDataPos
{
    System::Int64 pos;
    ResultData::RecordData data;
};

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
    ResultData::RecordData GetCurrentRecord(int index);
    bool IsExistRecord(int index, ResultData::RecordData record);

    bool HasNext(int index);
    bool HasPrevious(int index);
    bool MoveNext(int index);
    bool MovePrevious(int index);
    bool MovePrevious(int index, System::Uint32 pieceNumber);
    bool MoveToLast(int index);
    void MoveToFirst(int index);
    bool SetReadPos(int index, System::Int64 pos);
    bool SetAllReadPos(System::Int64 pos);
    bool SetCurrentRecord(int index, ResultData::RecordData data);
    System::Int64 GetReadPos(int index);
    System::Uint32 GetRemainNum(int index);
    System::Uint32 GetHeadNum(int index);
    bool RecordCompare(ResultData::RecordData sRecord, ResultData::RecordData dRecord);

    RecordFields* GetRecordFields(void);
    bool IsMigrateData(void);
    RecordFile& MigrateData(RecordFile& originalFile);
    bool ExportFiles(UI::RoleType roletype);
    /**
     * @brief 获取当前记录数据条的名为name字段的数值
     * @param index[int] 数据读取者索引, name[in] 字段名字，value[out] 数值
     * @return 是否获取成功
     */
    template<typename T, typename T1>bool GetFieldCurrentRecordValue(int index, T const& name, T1& value)
    {
        bool ret = false;

        RecordField recordField;
        ResultData::RecordData data = GetCurrentRecord(index);
        if (m_recordFields->GetField(name, recordField) && data.GetSize() > 0)
        {
            //根据当前字段名获取原始同一名字的字段在当前记录的数值，获取到则把新数据并写到新的单条数据里
            switch((FieldType)recordField.type)
            {
             case FieldType::Bool:
                 {
                     bool oldValue;
                     ret = data.GetBool(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Byte:case FieldType::Enum:
                 {
                     Byte oldValue;
                     ret = data.GetByte(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Int:case FieldType::Time:
                 {
                     int oldValue;
                     ret = data.GetInt(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             case FieldType::Float:
                 {
                     float oldValue;
                     ret = data.GetFloat(recordField.start, oldValue);
                     value = oldValue;
                 break;
                 }
             case FieldType::Double:
                 {
                     double oldValue;
                     ret = data.GetDouble(recordField.start, oldValue);
                     value = oldValue;
                 }
                 break;
             default:
                 ret = data.GetFieldData(recordField.start, value);
                 break;
            }
        }

        return ret;
    }
    bool GetFieldCurrentRecordEnumString(int index, const String& name, String& text);
    String GetFormatTableName();
    String GetExportFileName();

    //注册数据读取者索引
    int RegisterReader();
    int GetSelfReaderIndex();

protected:
    virtual bool LoadFrom();
    virtual bool SaveTo();

    String GetRecordType() const;
    bool SetRecordType(String type);
    Controller::API::Version GetRecordVersion() const;
    bool SetRecordVersion(Controller::API::Version ver);
    String GetTarget() const;
    bool SetTarget(String target);

protected:
    RecordFileHeader m_recordFileHeader;
    System::Uint32 m_fileHeaderSize;
    System::Int64 m_writePos;
    QMutex m_mutexReader;//读的文件锁
    QMutex m_mutexWriter;//写文件的锁
    String m_formatTableName;
    String m_exportFileName;
//    System::Int64 m_readPos;
//    ResultData::RecordData m_currentRecord;//读取文件的一条当前数据记录
    int m_readerIndex;                                //自身本身读取索引
    QMap<int, RecordDataPos*> m_currentRecord;  //映射<数据读取者索引,当前数据>
    RecordFields *m_recordFields;
    const System::String m_defaultType;
};

}
#endif // RESULTMANAGER_RECORDFILE_H
