#include "AccessLevelTable.h"
#include "Treasure/System/Logger.h"

namespace DataBaseSpace
{

AccessLevelTable::AccessLevelTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{

    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("dataTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("lastEditTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("levelName", "VARCHAR(30) NOT NULL UNIQUE"));
    m_fieldList.append(TbField("limitsOfAuthority", "INTEGER  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void AccessLevelTable::InsertData(const QList<AccessLevelRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty()) {
        for (QList<AccessLevelRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++) {
            AccessLevelRecord data = *it;

            QString sql = QString("INSERT OR REPLACE INTO %1 (dataTime,lastEditTime,levelName,limitsOfAuthority) "
                                  "VALUES (%2,%3,'%4',%5)")
                          .arg(m_tableName)
                          .arg(data.dataTime)
                          .arg(data.lastEditTime)
                          .arg(data.levelName)
                          .arg(data.limitsOfAuthority);
            sqlList.append(sql);
        }

        Insert(sqlList);
    }
}

void AccessLevelTable::InsertOrIgnoreData(const AccessLevelRecord& data)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;

    QString sql = QString("INSERT OR IGNORE INTO %1 (dataTime,lastEditTime,levelName,limitsOfAuthority) "
                          "VALUES (%2,%3,'%4',%5)")
                  .arg(m_tableName)
                  .arg(data.dataTime)
                  .arg(data.lastEditTime)
                  .arg(data.levelName)
                  .arg(data.limitsOfAuthority);
    sqlList.append(sql);

    Insert(sqlList);
}

QList<AccessLevelRecord> AccessLevelTable::SelectData()
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT * FROM %1 ORDER BY dataTime DESC").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    auto dataList = ResolveRecord(query);
    return dataList;
}

AccessLevelRecord AccessLevelTable::SelectDataFromName(const QString& levelName)
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT * FROM %1 WHERE levelName = '%2' LIMIT 1").arg(m_tableName).arg(levelName);

    QSqlQuery query = this->Select(sql);
    auto dataList = ResolveRecord(query);
    if(dataList.empty()) {
        return {};
    } else {
        return dataList.first();
    }
}

void AccessLevelTable::DeleteDataFromName(const QString& levelName)
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("DELETE FROM %1 WHERE levelName = '%2'").arg(m_tableName).arg(levelName);
    QSqlQuery query = this->Select(sql);
}

int AccessLevelTable::GetDataCount()
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT COUNT(*) "
                          "FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    if(query.next()) {
        if (!query.value("COUNT(*)").isNull()) {
            return query.value("COUNT(*)").toInt();
        }
    }
    return 0;
}

qint64 AccessLevelTable::GetNewestDataTime()
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT MAX(dataTime) "
                          "FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    if(query.next()) {
        return query.value(0).toLongLong();
    }
    return 0;
}

int  AccessLevelTable::GetDataCount(qint64 minTime, qint64 maxTime)
{

    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT COUNT(*) "
                          "FROM %1 WHERE dataTime BETWEEN %2 AND %3")
                  .arg(m_tableName).arg(minTime).arg(maxTime);

    QSqlQuery query = this->Select(sql);
    if(query.next()) {
        if (!query.value("COUNT(*)").isNull()) {
            return query.value("COUNT(*)").toInt();
        }
    }
    return 0;
}

QStringList AccessLevelTable::ConvertLimitsOfAuthority(qint64 limitsOfAuthority)
{
    QStringList strList;
    for (int i = 0; i < LimitsOfAuthority.size(); ++i) {
        if (limitsOfAuthority & (1 << i)) {
            strList.append(LimitsOfAuthority[i]);
        }
    }
    return strList;
}

qint64 AccessLevelTable::ConvertLevelListToMap(const QStringList &list)
{
    qint64 level = 0;
    for (int i = 0; i < LimitsOfAuthority.size(); ++i)
    {
        if (list.contains(LimitsOfAuthority.at(i)))
        {
            level |= 1 << i;
        }
    }
    return level;
}

QMap<Authority, bool> AccessLevelTable::ConvertLimitsOfAuthorityToMap(qint64 limitsOfAuthority)
{
    QMap<Authority, bool> result;
    for (int i = 0; i < LimitsOfAuthority.size(); ++i) {
        if (limitsOfAuthority & (1 << i)) {
            result[(Authority)i] = true;
        }
    }
    return result;
}

QVector<QString> AccessLevelTable::GetLevelNameList()
{
    QVector<QString> nameList;
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT levelName "
                          "FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    while(query.next()) {
        QString levelName = query.value(0).toString();
        nameList.push_back(levelName);
    }
    return nameList;
}

QList<AccessLevelRecord> AccessLevelTable::ResolveRecord(QSqlQuery &query)
{
    QList<AccessLevelRecord> list;

    while(query.next()) {
        AccessLevelRecord data;
        {
            data.id = query.value("id").toInt();
            data.dataTime = query.value("dataTime").toInt();
            data.lastEditTime = query.value("lastEditTime").toInt();
            data.levelName = query.value("levelName").toString();
            data.limitsOfAuthority = query.value("limitsOfAuthority").toInt();
            list.append(data);
        }
    }
    return list;
}


}
