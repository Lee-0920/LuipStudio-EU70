#include "AuditTrailTable.h"
#include "Treasure/System/Logger.h"

namespace DataBaseSpace
{

AuditTrailTable::AuditTrailTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{

    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("dataTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("userName", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("userLevel", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("event", "VARCHAR NOT NULL"));
    m_fieldList.append(TbField("oldSetting", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("newSetting", "VARCHAR(30) NOT NULL"));
    m_fieldList.append(TbField("details", "VARCHAR  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void AuditTrailTable::InsertData(const QList<AuditTrailRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty())
    {
        for (QList<AuditTrailRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++)
        {
            AuditTrailRecord data = *it;

            QString sql = QString("INSERT INTO %1 (dataTime,userName,userLevel,event,oldSetting,newSetting,details) "
                                  "VALUES (%2,'%3','%4','%5','%6','%7','%8')")
                    .arg(m_tableName)
                    .arg(data.dataTime)
                    .arg(data.userName)
                    .arg(data.userLevel)
                    .arg(data.event)
                    .arg(data.oldSetting)
                    .arg(data.newSetting)
                    .arg(data.details);;
            sqlList.append(sql);
        }

        Insert(sqlList);
   }
}


QList<AuditTrailRecord> AuditTrailTable::SelectData()
{
    QMutexLocker locker(&m_queryMutex);
    QString sql = QString("SELECT * FROM %1").arg(m_tableName);

    QSqlQuery query = this->Select(sql);
    auto dataList = ResolveRecord(query);
    return dataList;
}

int AuditTrailTable::GetDataCount()
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

qint64 AuditTrailTable::GetNewestDataTime()
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

int  AuditTrailTable::GetDataCount(qint64 minTime, qint64 maxTime)
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

void AuditTrailTable::UpdateQueryModel(QSqlQueryModel *model, int limit, int offset, qint64 minTime, qint64 maxTime)
{

    QMutexLocker locker(&m_queryMutex);
    QString queryStr = QString("SELECT dataTime, userName, event, oldSetting, newSetting, details FROM %1 "
                               "WHERE dataTime BETWEEN %4 AND %5"
                               " ORDER BY dataTime DESC LIMIT %2 OFFSET %3").arg(m_tableName).arg(limit)
                       .arg(offset).arg(minTime).arg(maxTime);

    this->SetQuery(model, queryStr);
}

void AuditTrailTable::InsertAuditTrail(const QString& userName, const QString& levelName, const QString& event, const QString& oldSetting, const QString& newSetting, const QString& detail)
{
    if(oldSetting != "--" && oldSetting == newSetting) {
        return;
    }
    AuditTrailRecord data;
    data.dataTime = QDateTime::currentDateTime().toTime_t();
    data.userName = userName;
    data.userLevel = levelName;
    data.event = event;
    data.newSetting = newSetting;
    data.oldSetting = oldSetting;
    data.details = detail;

    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    QString sql = QString("INSERT INTO %1 (dataTime,userName,userLevel,event,oldSetting,newSetting,details) "
                          "VALUES (%2,'%3','%4','%5','%6','%7','%8')")
                  .arg(m_tableName)
                  .arg(data.dataTime)
                  .arg(data.userName)
                  .arg(data.userLevel)
                  .arg(data.event)
                  .arg(data.oldSetting)
                  .arg(data.newSetting)
                  .arg(data.details);
    sqlList.append(sql);

    Insert(sqlList);
}

QList<AuditTrailRecord> AuditTrailTable::ResolveRecord(QSqlQuery &query)
{
    QList<AuditTrailRecord> list;

    while(query.next()) {
        AuditTrailRecord data;
        {
            data.id = query.value("id").toInt();
            data.dataTime = query.value("dataTime").toInt();
            data.userName = query.value("userName").toString();
            data.userLevel = query.value("userLevel").toString();
            data.event = query.value("event").toString();
            data.oldSetting = query.value("oldSetting").toString();
            data.newSetting = query.value("newSetting").toString();
            data.details = query.value("details").toString();
            list.append(data);
        }
    }
    return list;
}

bool AuditTrailTable::ExportTableToCsv(const QString &csvPath)
{
    QMutexLocker locker(&m_queryMutex);

    QString sql = QString("SELECT * " \
                          "FROM %1")
                        .arg(m_tableName);
    QSqlQuery query = this->Select(sql);


    // 打开 CSV 文件
    QFile file(csvPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "无法创建 CSV 文件:" << file.errorString();
//        db.close();
        return false;
    }

    QTextStream out(&file);
    QStringList cvsTitleName;
    cvsTitleName.append("ID");
    cvsTitleName.append("创建时间");
    cvsTitleName.append("用户名称");
    cvsTitleName.append("权限等级");
    cvsTitleName.append("操作内容");
    cvsTitleName.append("旧值");
    cvsTitleName.append("新值");
    cvsTitleName.append("详情(注释)");

    // 写入表头（列名）
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i)
    {
        out << cvsTitleName.at(i);
        if (i < record.count() - 1)
        {
            out << ",";
        }
    }
    out << "\n";

    // 写入数据
    while (query.next())
    {
        for (int i = 0; i < record.count(); ++i)
        {
            if(query.value(i) == query.value("dataTime").toInt())
            {
                out << QDateTime::fromTime_t(query.value(i).toInt()).toString("yyyy-MM-dd HH:mm:ss");
            }
            else
            {
                out << query.value(i).toString();
//                out.setCodec("UTF-8");
//                out << QString::fromUtf8(query.value(i).toString());
            }

            if (i < record.count() - 1)
            {
                out << ",";
            }
        }
        out << "\n";
    }

    // 关闭文件和数据库
    file.close();
//    db.close();

    qDebug() << "Export Done:" << csvPath;
    return true;
}

}
