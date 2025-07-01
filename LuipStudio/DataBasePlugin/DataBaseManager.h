#ifndef DB_DATABASEMANAGER_H
#define DB_DATABASEMANAGER_H

#include <memory>
#include <QString>
#include <QSqlQuery>
#include <QWaitCondition>

#include "DataBaseDef.h"
#include "Treasure/SystemDef.h"

#include "Table/MeasureTable.h"
#include "Table/CalibrateTable.h"
#include "Table/DBConnectionPool.h"
#include "Table/MethodTable.h"
#include "Table/WarningTable.h"
#include "Table/AuditTrailTable.h"
#include "Table/UserTable.h"
#include "Table/AccessLevelTable.h"

namespace DataBaseSpace
{

class LUIP_SHARE DataBaseManager
{
public:
    static DataBaseManager *Instance();

    bool DataBaseFileCheck(const QString& dbFilePath);

    void Init(QString appDataPath, Treasure::Logger* logger);
    void Uninit();
    void DataBaseArchived(const QString& path, const QString& bakPath = "");

    DBConnectionPoolPtr    GetResultDBConnectionPool();
    MeasureTablePtr        GetMeasureTable();
    CalibrateTablePtr      GetCalibrateTable();
    MethodTablePtr         GetMethodTable();
    WarningTablePtr        GetWarningTable();
    AuditTrailTablePtr     GetAuditTrailTable();
    UserTablePtr           GetUserTable();
    AccessLevelTablePtr    GetAccessLevelTable();
private:
    bool CheckTable(DataTablePtr table);

private:
    DBConnectionPoolPtr  m_resultDBConnectionPool;
    DBConnectionPoolPtr  m_systemDBConnectionPool;
    MeasureTablePtr m_measureTable;
    CalibrateTablePtr m_calibrateTable;
    MethodTablePtr m_methodTable;
    WarningTablePtr m_warningTable;
    AuditTrailTablePtr m_auditTrailTable;
    UserTablePtr m_userTable;
    AccessLevelTablePtr m_accessLevelTable;
};

}
#endif // DBMANAGER_H
