#include "DataTable.h"


namespace DataBaseSpace
{

DataTable::DataTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DBTable(name, connectionpool),m_reissueDay(-7),m_unuploadlimit(5)
{

}

DataTable::~DataTable()
{

}

QString DataTable::TransformPolIdListToString(const QList<QString> polIdList)
{
    QString str;

    if (!polIdList.empty())
    {
        int i = 0;
        int cnt = polIdList.count();
        foreach (QString polId, polIdList)
        {
            if (i == cnt - 1)
            {
                str += "'" + polId + "'";
            }
            else
            {
                str += "'" + polId + "'" + ",";
            }

            i++;
        }
    }

    return str;
}

void DataTable::SetDataSendFlag(QList<int> recordIds, int platFormId)
{
//    QMutexLocker lock(&m_tableMutex);

    QList<QString> sqlList;
    QString idsStr;

    if (!recordIds.empty())
    {
        int i = 0;
        int cnt = recordIds.count();
        foreach (int id, recordIds)
        {
            if (i == cnt - 1)
            {
                idsStr += QString::number(id);
            }
            else
            {
                idsStr += QString::number(id) + ",";
            }

            i++;
        }

        QString sql = QString("UPDATE %1 SET pSended = pSended | (1 << %2) WHERE id IN(%3)").arg(m_tableName).arg(platFormId).arg(idsStr);
        sqlList.append(sql);

        this->Update(sqlList);
    }
}

}
