#ifndef RECORDDATAREADER_H
#define RECORDDATAREADER_H

#include "RecordFields.h"
#include "System/Types.h"
#include "RecordDataPlugin/RecordData.h"

namespace Result
{

class OperateRecordData
{
public:
    OperateRecordData(RecordFields recordFields);
    std::vector<System::String> GetRecordAllFieldName();
    FieldType GetRecordFieldType(System::String fieldName);
    String GetRecordFieldUnit(System::String fieldName);
    System::Uint32 GetRecordFieldSize(System::String fieldName);
    System::String GetRecordFieldText(System::String fieldName);
    bool GetRecordFieldName(System::String fieldText, System::String &fieldName);
    template<typename T>bool GetRecordDataFieldValue(System::String fieldName, ResultData::RecordData recordData, T& value)
    {
        bool ret = false;
        RecordField recordField;
        if (m_RecordFields.GetField(fieldName, recordField))
        {
            ret = recordData.GetFieldData(recordField.start, value);
        }
        return ret;
    }
    template<typename T>bool SetRecordDataFieldValue(System::String fieldName, ResultData::RecordData recordData, T& value)
    {
        bool ret = false;
        RecordField recordField;
        if (m_RecordFields.GetField(fieldName, recordField))
        {
            ret = recordData.SetFieldData(recordField.start, value);
        }
        return ret;
    }

    bool GetRecordDataFieldEnumString(System::String fieldName, ResultData::RecordData recordData, String& text);
    bool GetRecordDataFieldEnumIndex(System::String fieldName, String text, Byte &index);
private:
    RecordFields m_RecordFields;
    std::map<System::String, System::String>m_usedFieldTexts;
};

template<>
bool OperateRecordData::GetRecordDataFieldValue<ResultData::IntArray>(System::String fieldName, ResultData::RecordData recordData, ResultData::IntArray& value);
}
#endif // RECORDDATAREADER_H
