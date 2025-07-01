#ifndef DB_AuditTrailTable_H
#define DB_AuditTrailTable_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{

#define DefaultContent "--"

struct AuditTrailRecord
{
    int     id = 0;
    qint64  dataTime = 0;
    QString userName;
    QString userLevel;
    QString event;
    QString oldSetting;
    QString  newSetting;
    QString  details;
    
};

class LUIP_SHARE AuditTrailTable: public DataTable
{

public:
    AuditTrailTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<AuditTrailRecord>& dataList);                 //插入数据

    QList<AuditTrailRecord> SelectData();
    int GetDataCount();
    qint64 GetNewestDataTime();
    int GetDataCount(qint64 minTime, qint64 maxTime);
    void UpdateQueryModel(QSqlQueryModel *model, int limit, int offset, qint64 minTime, qint64 maxTime);
    void InsertAuditTrail(const QString& userName, const QString& levelName, const QString& event, const QString& oldSetting = DefaultContent, const QString& newSetting = DefaultContent, const QString& detail = DefaultContent);
    bool ExportTableToCsv(const QString &csvPath);
private:
    QList<AuditTrailRecord> ResolveRecord(QSqlQuery &query);
private:
    QMutex m_queryMutex;
};

typedef std::shared_ptr<AuditTrailTable> AuditTrailTablePtr;
}
#endif
