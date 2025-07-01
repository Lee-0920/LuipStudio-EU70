#include "MeasureTable.h"
#include "Treasure/System/Logger.h"


namespace DataBaseSpace
{

MeasureTable::MeasureTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{
    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("measureDateTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("consistency", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("consistencyTC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("consistencyIC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("peakTC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("peakIC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("measureType", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("initCellTempTC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initCellTempIC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalCellTempTC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalCellTempIC", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("measureconsumeDateTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("currentRange", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("flag", "INTEGER  NOT NULL"));        
    m_fieldList.append(TbField("meaType", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("turboMode", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("ICRMode", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("TOCMode", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("ECMode", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("autoReagent", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("reagent1Vol", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("reagent2Vol", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("normalRefreshTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("measureTimes", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("rejectTimes", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("methodName", "VARCHAR(20) NOT NULL"));
    m_fieldList.append(TbField("createTime", "INTEGER  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void MeasureTable::InsertData(const QList<MeasureRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty())
    {
        for (QList<MeasureRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++)
        {
            MeasureRecord data = *it;

            QString sql = QString("INSERT INTO %1 (measureDateTime,consistency,consistencyTC,consistencyIC,peakTC,peakIC,"
                                  "measureType,initCellTempTC,initCellTempIC,finalCellTempTC,finalCellTempIC,initEnvironmentTemp,finalEnvironmentTemp,measureconsumeDateTime,currentRange,flag,"
                                  "meaType,turboMode,ICRMode,TOCMode,ECMode,autoReagent,reagent1Vol,reagent2Vol,normalRefreshTime,measureTimes,rejectTimes,methodName,createTime) "
                                  "VALUES (%2,%3, %4,%5,%6, %7, %8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,'%29','%30')")
                    .arg(m_tableName)
                    .arg(data.measureDateTime)
                    .arg(data.consistency)
                    .arg(data.consistencyTC)
                    .arg(data.consistencyIC)
                    .arg(data.peakTC)
                    .arg(data.peakIC)
                    .arg(data.measureType)
                    .arg(data.initCellTempTC)
                    .arg(data.initCellTempIC)
                    .arg(data.finalCellTempTC)
                    .arg(data.finalCellTempIC)
                    .arg(data.initEnvironmentTemp)
                    .arg(data.finalEnvironmentTemp)
                    .arg(data.measureconsumeDateTime)
                    .arg(data.currentRange)
                    .arg(data.flag)
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
                    .arg(data.rejectTimes)
                    .arg(data.methodName)
                    .arg(data.createTime);
            qDebug() << sql;
//            trLogger->debug("insert: %s", sql.toStdString().c_str());
            sqlList.append(sql);
        }

        Insert(sqlList);
   }
}


void MeasureTable::GetData( QList<MeasureRecord> &map)
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

/*
*数据窗口目前是读Bin文件，删掉数据库并不代表Bin文件内的对应数据被删除
*需要把数据窗口数据读取改成从数据库读取才有效(目前不需要，审计需求不允许单独删除数据)
*/
void MeasureTable::DeleteData(QDateTime datetime)
{
//    QMutexLocker locker(&m_queryMutex);
//    QList<QString> sqlList;
//    QString delTimeStr = datetime.toString("yyyy-MM-dd hh:mm:ss");

//    QString sql = QString("DELETE FROM %1 WHERE createTime = '%2'")
//                    .arg(m_tableName)
//                    .arg(delTimeStr);

//    this->Select(sql);
}

QList<MeasureRecord> MeasureTable::ResolveRecord(QSqlQuery &query)
{
    QList<MeasureRecord> list;

    while(query.next())
    {
        MeasureRecord data;

        if (!query.value("measureDateTime").isNull())
        {
            data.id = query.value("id").toInt();
            data.measureDateTime = query.value("measureDateTime").toInt();
            data.consistency = query.value("consistency").toFloat();
            data.consistencyTC = query.value("consistencyTC").toFloat();
            data.consistencyIC = query.value("consistencyIC").toFloat();
            data.peakTC = query.value("peakTC").toFloat();
            data.peakIC = query.value("peakIC").toFloat();
            data.measureType = query.value("measureType").toInt();
            data.initCellTempTC = query.value("initCellTempTC").toFloat();
            data.initCellTempIC = query.value("initCellTempIC").toFloat();
            data.finalCellTempTC = query.value("finalCellTempTC").toFloat();
            data.finalCellTempIC = query.value("finalCellTempIC").toFloat();
            data.initEnvironmentTemp = query.value("initEnvironmentTemp").toFloat();
            data.finalEnvironmentTemp = query.value("finalEnvironmentTemp").toFloat();
            data.measureconsumeDateTime = query.value("measureconsumeDateTime").toInt();
            data.currentRange = query.value("currentRange").toFloat();
            data.flag = query.value("flag").toInt();
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

//            if (!query.value("reagent1Vol").isNull())
//            {
//                data.reagent1Vol = query.value("reagent1Vol").toFloat();
//            }

//            if (!query.value("reagent2Vol").isNull())
//            {
//                data.reagent2Vol = query.value("reagent2Vol").toFloat();
//            }

            if (!query.value("methodName").isNull())
            {
                data.methodName = query.value("methodName").toString();
            }

            if (!query.value("createTime").isNull())
            {
                data.createTime = query.value("createTime").toInt();
            }
            QDateTime dt = QDateTime::fromTime_t(data.measureDateTime);
            qDebug() << dt.toString("yyyy-MM-dd hh:mm:ss");
            list.append(data);
        }
    }

    return list;
}

QList<MeasureRecord> MeasureTable::GetDataByTime(QDateTime datetime)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    QString delTimeStr = datetime.toString("yyyy-MM-dd hh:mm:ss");

    QString sql = QString("SELECT * " \
                          "FROM %1 WHERE measureDateTime = %2")
                        .arg(m_tableName)
                        .arg(datetime.toTime_t());
    QSqlQuery query = this->Select(sql);

    QList<MeasureRecord> list = ResolveRecord(query);

    return list;
}



}
