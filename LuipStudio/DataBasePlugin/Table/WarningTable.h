#ifndef DB_WarningTable_H
#define DB_WarningTable_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{

struct WarningRecord
{
    int     id = 0;
    qint64  dataTime = 0;
    QString level;
    QString type;
    QString name;
    QString cause;
    qint64  clearedTime = 0;
    
};

class LUIP_SHARE WarningTable: public DataTable
{

public:
    WarningTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<WarningRecord>& dataList);                 //插入数据

private:
    QMutex m_queryMutex;
};

typedef std::shared_ptr<WarningTable> WarningTablePtr;
}
#endif
