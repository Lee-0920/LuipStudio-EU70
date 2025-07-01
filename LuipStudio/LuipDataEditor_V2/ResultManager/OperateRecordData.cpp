#include "OperateRecordData.h"

using namespace std;
using namespace System;

namespace Result
{

OperateRecordData::OperateRecordData(RecordFields recordFields):
    m_RecordFields(recordFields)
{

}

std::vector<System::String> OperateRecordData::GetRecordAllFieldName()
{
    vector<String>name;

    RecordField recordField;
    m_RecordFields.ResetFieldPos();
    while(m_RecordFields.GetNextField(recordField))
    {
        name.push_back(recordField.name);
    }
    m_RecordFields.ResetFieldPos();

    return name;
}

FieldType OperateRecordData::GetRecordFieldType(System::String fieldName)
{
    FieldType fieldType;
    m_RecordFields.GetFieldType(fieldName, fieldType);
    return  fieldType;
}

String OperateRecordData::GetRecordFieldUnit(System::String fieldName)
{
    String unit;
    m_RecordFields.GetFieldUnit(fieldName, unit);
    return  unit;
}

System::Uint32 OperateRecordData::GetRecordFieldSize(System::String fieldName)
{
    Uint32 size;
    m_RecordFields.GetFieldSize(fieldName, size);
    return  size;
}

System::String OperateRecordData::GetRecordFieldText(System::String fieldName)
{
    String text;
    m_RecordFields.GetFieldText(fieldName, text);
    return  text;
}

bool OperateRecordData::GetRecordFieldName(System::String fieldText, System::String &fieldName)
{
    bool ret = false;

    if ( m_usedFieldTexts.count(fieldText))
    {
        fieldName = m_usedFieldTexts.at(fieldText);
        ret = true;
    }
    else
    {
        RecordField recordField;
        m_RecordFields.ResetFieldPos();
        while(m_RecordFields.GetNextField(recordField))
        {
            if (recordField.text == fieldText)
            {
                fieldName = recordField.name;
                m_usedFieldTexts.insert(make_pair(fieldText, fieldName));
                ret = true;
                break;
            }
        }
        m_RecordFields.ResetFieldPos();
    }
    return ret;
}

bool OperateRecordData::GetRecordDataFieldEnumString(System::String fieldName, ResultData::RecordData recordData, String& text)
{
    bool ret = false;
    RecordField recordField;
    if (m_RecordFields.GetField(fieldName, recordField))
    {
        Byte value;
        ret = recordData.GetFieldData(recordField.start, value);
        text = recordField.option.at(value);
    }

    return ret;
}

bool OperateRecordData::GetRecordDataFieldEnumIndex(System::String fieldName, String text, Byte &index)
{
    bool ret = false;
    Byte value = 0;
    RecordField recordField;
    if (m_RecordFields.GetField(fieldName, recordField))
    {
        for (std::vector<String>::iterator iter = recordField.option.begin(); iter != recordField.option.end(); iter++)
        {
            if (text == *iter)
            {
                ret = true;
                index = value;
                break;
            }
            value++;
        }
    }
    return ret;
}

template<>
bool OperateRecordData::GetRecordDataFieldValue<ResultData::IntArray>(System::String fieldName, ResultData::RecordData recordData, ResultData::IntArray& value)
{
    bool ret = false;
    RecordField recordField;
    if (m_RecordFields.GetField(fieldName, recordField))
    {
        ret = recordData.GetIntArray(recordField.start, value);
    }

    return ret;
}
}
