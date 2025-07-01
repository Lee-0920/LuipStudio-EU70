#include "UserTable.h"
#include "Treasure/System/Logger.h"

namespace DataBaseSpace
{

UserTable::UserTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{

    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("dataTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("lastEditTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("userName", "VARCHAR(30) NOT NULL UNIQUE"));
    m_fieldList.append(TbField("levelName", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("password", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("lastLoginTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("status", "INTEGER  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void UserTable::InsertData(const QList<UserRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty()) {
        for (QList<UserRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++) {
            UserRecord data = *it;

            QString sql = QString("INSERT OR REPLACE INTO %1 (dataTime,lastEditTime,userName,levelName,password,lastLoginTime,status) "
                                  "VALUES (%2,%3,'%4','%5','%6',%7,%8)")
                          .arg(m_tableName)
                          .arg(data.dataTime)
                          .arg(data.lastEditTime)
                          .arg(data.userName)
                          .arg(data.levelName)
                          .arg(data.password)
                          .arg(data.lastLoginTime)
                          .arg(data.status);
            sqlList.append(sql);
        }

        Insert(sqlList);        
    }
}

void UserTable::InsertOrIgnoreData(const UserRecord data)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;

    QString sql = QString("INSERT OR IGNORE INTO %1 (dataTime,lastEditTime,userName,levelName,password,lastLoginTime,status) "
                          "VALUES (%2,%3,'%4','%5','%6')")
                  .arg(m_tableName)
                  .arg(data.dataTime)
                  .arg(data.lastEditTime)
                  .arg(data.userName)
                  .arg(data.levelName)
                  .arg(data.password)
                  .arg(data.lastLoginTime)
                  .arg(data.status);
    sqlList.append(sql);


    Insert(sqlList);

}

QList<UserRecord> UserTable::SelectData()
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT * FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    auto dataList = ResolveRecord(query);
    return dataList;
}

UserRecord UserTable::GetUserRecord(const QString& userName)
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT * FROM %1 WHERE userName = '%2' LIMIT 1").arg(m_tableName).arg(userName);

    QSqlQuery query = this->Select(sql);
    if(query.next()) {
        UserRecord data;
        {
            data.id = query.value("id").toInt();
            data.dataTime = query.value("dataTime").toInt();
            data.lastEditTime = query.value("lastEditTime").toInt();
            data.userName = query.value("userName").toString();
            data.levelName = query.value("levelName").toString();
            data.password = query.value("password").toString();
            data.lastLoginTime = query.value("lastLoginTime").toInt();
            data.status = query.value("status").toInt();
        }
        return data;
    }
    return {};
}

int UserTable::GetDataCount()
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

qint64 UserTable::GetNewestDataTime()
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

int  UserTable::GetDataCount(qint64 minTime, qint64 maxTime)
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

void UserTable::UpdateQueryModel(QSqlQueryModel *model, int limit, int offset)
{

    QMutexLocker locker(&m_queryMutex);
    QString queryStr = QString("SELECT userName, dataTime, levelName FROM %1 "
                               " ORDER BY dataTime DESC LIMIT %2 OFFSET %3").arg(m_tableName).arg(limit)
                       .arg(offset);

    this->SetQuery(model, queryStr);
}

QVector<QString> UserTable::GetUserNameList()
{
    QVector<QString> nameList;
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT userName "
                          "FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    while(query.next()) {
        QString levelName = query.value(0).toString();
        nameList.push_back(levelName);
    }
    return nameList;
}

void UserTable::DeleteDataFromName(const QString& userName)
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("DELETE FROM %1 WHERE userName = '%2'").arg(m_tableName).arg(userName);
    QSqlQuery query = this->Select(sql);
}

QString UserTable::GetUserStatus(int index)
{
    QString status = UserStatusTip.at((int)UserStatus::Other);
    if(index < (int)UserStatus::Other)
    {
        status = UserStatusTip.at(index);
    }

    return status;
}

QList<UserRecord> UserTable::ResolveRecord(QSqlQuery &query)
{
    QList<UserRecord> list;

    while(query.next()) {
        UserRecord data;
        {
            data.id = query.value("id").toInt();
            data.dataTime = query.value("dataTime").toInt();
            data.lastEditTime = query.value("lastEditTime").toInt();
            data.userName = query.value("userName").toString();
            data.levelName = query.value("levelName").toString();
            data.password = query.value("password").toString();
            data.lastLoginTime = query.value("lastLoginTime").toInt();
            data.status = query.value("status").toInt();
            list.append(data);
        }
    }
    return list;
}

void UserTable::UpdateLevelName(const QString& oldLevelName, const QString& newLevelName)
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("UPDATE %1 SET levelName = '%2' WHERE levelName = '%3'").arg(m_tableName).arg(newLevelName).arg(oldLevelName);
    QSqlQuery query = this->Select(sql);
}

}
