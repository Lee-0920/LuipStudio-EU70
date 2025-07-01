#include "Treasure/SystemDef.h"
#include "DBTable.h"

namespace DataBaseSpace
{

DBTable::DBTable(const QString name, DBConnectionPoolPtr connectionpool):
    m_tableName(name),
    m_connectionpool(connectionpool)
{

}

QString DBTable::GetTableName()
{
    return m_tableName;
}

void DBTable::CreateTable()
{
   QMutexLocker lock(&m_tableMutex);

   QString fieldStr = "(";
   for (QList<TbField>::const_iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++)
   {
        TbField fieldItem = *it;
        fieldStr += fieldItem.field + " " + fieldItem.type;
        if (it != m_fieldList.end() - 1)
        {
            fieldStr += ",";
        }

   }

//   if (!m_index.fields.empty())
//   {
//       fieldStr += ",INDEX " + m_index.name + "(";

//       for (QStringList::const_iterator it = m_index.fields.begin(); it != m_index.fields.end(); it++)
//       {

//            fieldStr += *it;
//            if (it != m_index.fields.end() - 1)
//            {
//                fieldStr += ",";
//            }

//       }

//       fieldStr += ")";
//   }

   fieldStr += ")";

   QString sql = QString("CREATE TABLE IF NOT EXISTS ") +  m_tableName + fieldStr;
//   logger->debug(sql.toStdString());

   QList<QString> sqlList;
   sqlList.append(sql);

   m_connectionpool->AttachWriteSql(sqlList);
}

QSqlQuery DBTable::Select(const QString &sql)
{
    QMutexLocker lock(&m_tableMutex);

    QSqlDatabase db = m_connectionpool->OpenConnection();    // 从数据库连接池里取得连接
    QSqlQuery query(db);

//    logger->debug(sql.toStdString());

//    long t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();
    bool ret = query.exec(sql);
//    long t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if(!ret)
    {       
        QString err = query.lastError().text();

        trLogger->Warn(sql);
        trLogger->Warn(err);
    }

    m_connectionpool->CloseConnection(db);   // 连接使用完后需要释放回数据库连接池

//    logger->debug("[Select] t2 - t1 = %d ms", t2 -t1);

    return query;
}

void DBTable::SetQuery(QSqlQueryModel *model, const QString& sql)
{
    QMutexLocker lock(&m_tableMutex);

    QSqlDatabase db = m_connectionpool->OpenConnection();    // 从数据库连接池里取得连接

    model->setQuery(sql, db);

    m_connectionpool->CloseConnection(db);   // 连接使用完后需要释放回数据库连接池

}

void DBTable::Insert(const QList<QString> &sqlList)
{
    m_connectionpool->AttachWriteSql(sqlList);
}


void DBTable::Update(const QList<QString> &sqlList)
{
    QMutexLocker lock(&m_tableMutex);
    m_connectionpool->AttachWriteSql(sqlList);
}

void DBTable::Delete()
{
    QMutexLocker lock(&m_tableMutex);

    QString sql1 = "DROP TABLE " + m_tableName;

    QString sql2 = "UPDATE sqlite_sequence SET seq = 0 WHERE name = " + m_tableName;

    QList<QString> sqlList;
    sqlList.append(sql1);
    sqlList.append(sql2);
    m_connectionpool->AttachWriteSql(sqlList);
}

void DBTable::FieldChangedCheck()
{
//    QSqlQuery query;

//    QString sql = "PRAGMA table_info(" + m_tableName + ")";
//    QStringList fieldList;
//    query = this->Select(sql);
//    while (query.next())
//    {
//        fieldList.append(query.value(1).toString());
//    }

//    if (!fieldList.empty())
//    {
//        QList<QSqlQuery> queryList;

//        for (QList<TbField>::const_iterator itfl = m_fieldList.begin(); itfl != m_fieldList.end(); itfl++)
//        {
//             TbField fieldItem = *itfl;

//             bool isFinded = false;
//             QList<QString>::Iterator itFL = fieldList.begin(),itend = fieldList.end();
//             for (;itFL != itend; itFL++)
//             {
//                 if (*itFL == fieldItem.field)
//                 {
//                     isFinded = true;
//                     break;
//                 }
//             }

//             if (isFinded == false)
//             {
//                QString sql = QString("ALTER " +  m_tableName + "ADD " + fieldItem.field + fieldItem.type) ;
//                QSqlDatabase db = m_connectionpool->GetWriteConnection();    // 从数据库连接池里取得连接
//                QSqlQuery query(db);
//                query.prepare(sql);
//                queryList.append(query);
//             }

//             m_connectionpool->AttachWriteSql(queryList);
//        }

//            QList<QString>::Iterator it = fieldList.begin(),itend = fieldList.end();
//            for (;it != itend; it++)
//            {
//              QMap<QString, QString>::const_iterator itfm = m_tableFieldMap.find(*it);

//              if (itfm == m_tableFieldMap.end())
//              {
//                    QString sql = QString("ALTER " +  m_tableName + " " + "DROP COLUMN " + *it) ;
//                    m_connectionpool->AttachWriteSql(sql);
//              }
//            }
//    }
}

void DBTable::Clean()
{
    QMutexLocker lock(&m_tableMutex);

    QString sql1 = "DELETE From " + m_tableName;
    QString sql2 = QString("UPDATE sqlite_sequence SET seq = 0 WHERE name = '%1'").arg(m_tableName);

    QList<QString> sqlList;
    sqlList.append(sql1);
    sqlList.append(sql2);
    m_connectionpool->AttachWriteSql(sqlList);
}

int DBTable::GetTotalCount()
{
    int count = 0 ;
    QString sql = QString("SELECT count(*) FROM %1").arg(m_tableName);
    QSqlQuery query = Select(sql);

    if (query.next())
    {
        count = query.value(0).toInt();
    }

    return count;
}

bool DBTable::IsExist()
{
    QString sql = QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(m_tableName);

    QSqlQuery query = this->Select(sql);

    if(query.next())
    {
        int count = query.value(0).toInt();

        if (count >= 0)
        {
            return true;
        }
    }

    return false;
}

QList<TbField> DBTable::GetTableFieldList() const
{
    return m_fieldList;
}



}
