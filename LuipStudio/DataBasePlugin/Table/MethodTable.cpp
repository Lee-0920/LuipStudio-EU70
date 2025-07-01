#include "MethodTable.h"
#include "Treasure/System/Logger.h"


namespace DataBaseSpace
{

MethodTable::MethodTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{
    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("methodName", "VARCHAR(20) NOT NULL UNIQUE"));
    m_fieldList.append(TbField("createTime", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("meaType", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("turboMode", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("ICRMode", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("TOCMode", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("ECMode", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("autoReagent", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("reagent1Vol", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("reagent2Vol", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("normalRefreshTime", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("measureTimes", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("rejectTimes", "INTEGER NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck(); 
}

void MethodTable::InsertData(const QList<MethodRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty())
    {
        for (QList<MethodRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++)
        {
            MethodRecord data = *it;

            QString sql = QString("INSERT  OR REPLACE INTO %1 (methodName,createTime,meaType,turboMode,ICRMode,TOCMode,ECMode,"
                                  "autoReagent,reagent1Vol,reagent2Vol,normalRefreshTime,measureTimes,rejectTimes) "
                                  "VALUES ('%2',%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14)")
                    .arg(m_tableName)
                    .arg(data.methodName)
                    .arg(data.createTime)
                    .arg(data.meaType)
                    .arg(data.turboMode)
                    .arg(data.ICRMode)
                    .arg(data.TOCMode)
                    .arg(data.ECMode)
                    .arg(data.autoReagent)
                    .arg(data.reagent1Vol)
                    .arg(data.reagent2Vol)
                    .arg(data.normalRefreshTime)
                    .arg(data.measureTimes)
                    .arg(data.rejectTimes);
            qDebug() << sql;
            trLogger->debug("insert: %s", sql.toStdString().c_str());
            sqlList.append(sql);
        }

        Insert(sqlList);
   }
}

void MethodTable::GetData(QList<MethodRecord> &map)
{
//    qDebug()<<m_tableName;
    QMutexLocker locker(&m_queryMutex);
    if (!m_tableName.isEmpty())
    {
        QString sql = QString("SELECT * " \
                              "FROM %1 ")
                            .arg(m_tableName);
        QSqlQuery query = this->Select(sql);

        map = ResolveRecord(query);
    }
}

void MethodTable::DeleteMethod(QDateTime datetime)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;    

    QString sql = QString("DELETE FROM %1 WHERE createTime = %2")
                    .arg(m_tableName)
                    .arg(datetime.toTime_t());

    this->Select(sql);
}

bool MethodTable::IsMethodNameExist(QString name)
{
    QMutexLocker locker(&m_queryMutex);
    QList<MethodRecord> list;

    if (!m_tableName.isEmpty())
    {
        QString sql = QString("SELECT * FROM %1 WHERE methodName = '%2'")
                        .arg(m_tableName)
                        .arg(name);
        QSqlQuery query = this->Select(sql);
        list.clear();
        list = ResolveRecord(query);

        if(!list.empty())
        {
            return true;
        }
    }
    return false;
}


QList<MethodRecord> MethodTable::ResolveRecord(QSqlQuery &query)
{
    QList<MethodRecord> list;

    while(query.next())
    {
        MethodRecord data;

        if (!query.value("methodName").isNull())
        {
            data.id = query.value("id").toInt();
            data.methodName = query.value("methodName").toString();
            data.createTime = query.value("createTime").toInt();
            data.meaType = query.value("meaType").toInt();
            data.turboMode = query.value("turboMode").toInt();
            data.ICRMode = query.value("ICRMode").toInt();
            data.TOCMode = query.value("TOCMode").toInt();
            data.ECMode = query.value("ECMode").toInt();
            data.autoReagent = query.value("autoReagent").toInt();
            data.reagent1Vol = query.value("reagent1Vol").toFloat();
            data.reagent2Vol = query.value("reagent2Vol").toFloat();
            data.normalRefreshTime = query.value("normalRefreshTime").toInt();
            data.measureTimes = query.value("measureTimes").toInt();
            data.rejectTimes = query.value("rejectTimes").toInt();

            if (!query.value("measureTimes").isNull())
            {
                data.measureTimes = query.value("measureTimes").toInt();
            }

            if (!query.value("rejectTimes").isNull())
            {
                data.rejectTimes = query.value("rejectTimes").toInt();
            }
//            qDebug()<<data.createTime;
            list.append(data);
        }
    }

    return list;
}

bool MethodTable::ExportTableToCsv(const QString &csvPath)
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
    cvsTitleName.append("方法名称");
    cvsTitleName.append("创建时间");
    cvsTitleName.append("测量类型");
    cvsTitleName.append("Turbo模式");
    cvsTitleName.append("ICR模式");
    cvsTitleName.append("TOC模式");
    cvsTitleName.append("电导率(NaCl)");
    cvsTitleName.append("自动加试剂");
    cvsTitleName.append("酸剂(uL/分)");
    cvsTitleName.append("氧化剂(uL/分)");
    cvsTitleName.append("冲洗时间(秒)");
    cvsTitleName.append("测量次数(离线)");
    cvsTitleName.append("舍弃次数(离线)");


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
            if(query.value(i) == query.value("createTime").toInt())
            {
                out << QDateTime::fromTime_t(query.value(i).toInt()).toString("yyyy-MM-dd HH:mm:ss");
            }
            else if( i == 3)//测量类型
            {
                out.setCodec("UTF-8");
                if(!query.value(i).toInt())
                {
                    out << QString::fromUtf8("在线");
                }
                else
                {
                    out << QString::fromUtf8("离线");
                }
            }
            else if( i >= 4 && i <= 8)//测量模式
            {
                out.setCodec("UTF-8");
                if(!query.value(i).toInt())
                {
                    out << QString::fromUtf8("否");
                }
                else
                {
                    out << QString::fromUtf8("是");
                }
            }
            else
            {
                out << query.value(i).toString();
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
