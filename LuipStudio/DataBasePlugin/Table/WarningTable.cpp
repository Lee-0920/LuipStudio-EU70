#include "WarningTable.h"
#include "Treasure/System/Logger.h"

namespace DataBaseSpace
{

WarningTable::WarningTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{

    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("dataTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("level", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("type", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("name", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("cause", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("clearedTime", "INTEGER  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void WarningTable::InsertData(const QList<WarningRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty())
    {
        for (QList<WarningRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++)
        {
            WarningRecord data = *it;

            QString sql = QString("INSERT INTO %1 (dataTime,level,type,name,cause,clearedTime) "
                                  "VALUES (%2,'%3','%4','%5','%6',%7)")
                    .arg(m_tableName)
                    .arg(data.dataTime)
                    .arg(data.level)
                    .arg(data.type)
                    .arg(data.name)
                    .arg(data.cause)
                    .arg(data.clearedTime);
            sqlList.append(sql);
        }

        Insert(sqlList);
   }
}


}
