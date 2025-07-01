#include "DataBaseManager.h"



namespace DataBaseSpace
{

DataBaseManager* DataBaseManager::Instance()
{
    return Treasure::Global<DataBaseManager>::Instance();
}

bool DataBaseManager::DataBaseFileCheck(const QString& dbFilePath)
{
    if (!QFile::exists(dbFilePath)) {
        return true;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "temp_connection");
    db.setDatabaseName(dbFilePath);

    if (!db.open()) {
        qDebug() << "无法打开数据库：" << db.lastError().text();
        QSqlDatabase::removeDatabase("temp_connection");
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT name FROM sqlite_master WHERE type='table'")) {
        qDebug() << "无效的数据库文件：" << db.lastError().text();
        db.close();
        QSqlDatabase::removeDatabase("temp_connection");
        return false;
    }

    db.close();
    QSqlDatabase::removeDatabase("temp_connection");
    return true;
}

void DataBaseManager::Init(QString appDataPath, Treasure::Logger* logger)
{
    bool measuringRes = DataBaseFileCheck(appDataPath + "/measuring.ds");
    bool systemRes = DataBaseFileCheck(appDataPath + "/system.ds");
    if(measuringRes == false) {
        auto dbMeasuringPath = appDataPath + "/measuring.ds";
        QString timestamp = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
        QString newFilePath = dbMeasuringPath + "_" + timestamp + ".bak";
        QFile::rename(dbMeasuringPath, newFilePath);

        trLogger->Debug(QString("measuring table is invalid!"));
    }
    if(systemRes == false) {
        auto dbSystemPath = appDataPath + "/system.ds";
        QString timestamp = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");
        QString newFilePath = dbSystemPath + "_" + timestamp + ".bak";
        QFile::rename(dbSystemPath, newFilePath);

        trLogger->Debug(QString("table is invalid!"));
    }

    if(logger != nullptr){
       Treasure::Logger::Instance(logger);
    }

    //连接库
    DataBaseInfo dbInfo;
    dbInfo.hostName        = "127.0.0.1";
    dbInfo.databaseName    = "measuring.ds";
    dbInfo.username        = "root";
    dbInfo.password        = "Tek6000UAdmin@2019";
    dbInfo.databaseType    = "QSQLITE";
    dbInfo.connectOptions  = "QSQLITE_REMOVE_KEY";
    dbInfo.path            = appDataPath;
    m_resultDBConnectionPool = DBConnectionPoolPtr(new DBConnectionPool(dbInfo));

    DataBaseInfo systemDBInfo;
    systemDBInfo.hostName        = "127.0.0.1";
    systemDBInfo.databaseName    = "system.ds";
    systemDBInfo.username        = "root";
    systemDBInfo.password        = "Tek6000UAdmin@2019";
    systemDBInfo.databaseType    = "QSQLITE";
    systemDBInfo.connectOptions  = "QSQLITE_REMOVE_KEY";
    systemDBInfo.path            = appDataPath;
    m_systemDBConnectionPool = DBConnectionPoolPtr(new DBConnectionPool(systemDBInfo));



    //样品记录数据表
    m_measureTable = MeasureTablePtr(new MeasureTable("tb_measureRecord", m_resultDBConnectionPool));
    m_calibrateTable = CalibrateTablePtr(new CalibrateTable("tb_calibrateRecord", m_resultDBConnectionPool));
    m_methodTable = MethodTablePtr(new MethodTable("tb_methodRecord", m_resultDBConnectionPool));
    m_warningTable = WarningTablePtr(new WarningTable("tb_warningRecord", m_resultDBConnectionPool));
    m_auditTrailTable = AuditTrailTablePtr(new AuditTrailTable("tb_auditTrailRecord", m_resultDBConnectionPool));
    m_userTable = UserTablePtr(new UserTable("tb_userRecord", m_systemDBConnectionPool));
    m_accessLevelTable = AccessLevelTablePtr(new AccessLevelTable("tb_levelRecord", m_systemDBConnectionPool));

    //检查表
    this->CheckTable(m_measureTable);
    this->CheckTable(m_calibrateTable);
    this->CheckTable(m_methodTable);
    this->CheckTable(m_warningTable);
    this->CheckTable(m_auditTrailTable);
    this->CheckTable(m_userTable);
    this->CheckTable(m_accessLevelTable);

}

bool DataBaseManager::CheckTable(DataTablePtr table)
{
    int cnt = 30;

    while(cnt--)
    {
        if(table->IsExist())
        {
            return true;
        }
        else
        {
            trLogger->Debug(QString("wait %1 table create!!").arg(table->GetTableName()));
            QThread::sleep(1);
        }
    }

    return false;
}

void DataBaseManager::Uninit()
{
    m_resultDBConnectionPool->StopWriteThread();
    m_systemDBConnectionPool->StopWriteThread();
}

void DataBaseManager::DataBaseArchived(const QString& path, const QString& bakPath)
{
    QMutex resultMutex;
    QMutex resultWMutex;
    QQueue<QString> resultUsedReadConnectionNames;
    QQueue<QString> resultUnusedReadConnectionNames;
    QString resWriteConnectionName;
    m_resultDBConnectionPool->GetUninitInfo(&resultMutex,&resultWMutex,  resWriteConnectionName,resultUsedReadConnectionNames, resultUnusedReadConnectionNames);

    QMutex sysMutex;
    QMutex sysWMutex;
    QQueue<QString> sysUsedReadConnectionNames;
    QQueue<QString> sysUnusedReadConnectionNames;
    QString sysWriteConnectionName;
    m_systemDBConnectionPool->GetUninitInfo(&sysMutex,&sysWMutex,  sysWriteConnectionName,sysUsedReadConnectionNames, sysUnusedReadConnectionNames);

    QMutexLocker resLocker(&resultMutex);
    QMutexLocker sysLocker(&sysMutex);
    QMutexLocker resWLocker(&resultWMutex);
    QMutexLocker sysWLocker(&sysWMutex);
    // 删除所有的连接
    foreach(QString connectionName, resultUsedReadConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    foreach(QString connectionName, resultUnusedReadConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    m_resultDBConnectionPool->ClearReadConnectionNames();
    QSqlDatabase::removeDatabase(resWriteConnectionName);

    foreach(QString connectionName, sysUsedReadConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    foreach(QString connectionName, sysUnusedReadConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    sysUsedReadConnectionNames.clear();
    sysUnusedReadConnectionNames.clear();
    m_systemDBConnectionPool->ClearReadConnectionNames();
    QSqlDatabase::removeDatabase(sysWriteConnectionName);

    QString currentDir = QCoreApplication::applicationDirPath();
    QDir dir(currentDir);
    dir.cdUp();
    QString dataDirPath = dir.absolutePath() + "/LuipData/measuring.ds";
    Treasure::File::Copy(dataDirPath, path);
    QFile file(dataDirPath);
    if (file.exists()) {
        file.remove();
    }

    if(!bakPath.isEmpty()){//启用备份文件
        Treasure::File::Copy(bakPath, dataDirPath);
    }
    QFile bakFile(bakPath);//备份文件存储在电脑，仪器不存储
    if (bakFile.exists()) {
        bakFile.remove();
    }


    auto resWdb = m_resultDBConnectionPool->CreateConnection(resWriteConnectionName);
    if (!resWdb.isOpen()) {
        qDebug() << "Write Connection open fail!!!";
    }
    m_resultDBConnectionPool->SetWriteConnection(resWdb);

    auto sysWdb = m_systemDBConnectionPool->CreateConnection(sysWriteConnectionName);
    if (!sysWdb.isOpen()) {
        qDebug() << "Write Connection open fail!!!";
    }
    m_systemDBConnectionPool->SetWriteConnection(sysWdb);
    m_measureTable.reset(new MeasureTable("tb_measureRecord", m_resultDBConnectionPool));
    m_calibrateTable.reset(new CalibrateTable("tb_calibrateRecord", m_resultDBConnectionPool));
    m_methodTable.reset(new MethodTable("tb_methodRecord", m_resultDBConnectionPool));
    m_warningTable.reset(new WarningTable("tb_warningRecord", m_resultDBConnectionPool));
    m_auditTrailTable.reset(new AuditTrailTable("tb_auditTrailRecord", m_resultDBConnectionPool));
    m_userTable.reset(new UserTable("tb_userRecord", m_systemDBConnectionPool));
    m_accessLevelTable.reset(new AccessLevelTable("tb_levelRecord", m_systemDBConnectionPool));

}

DBConnectionPoolPtr DataBaseManager::GetResultDBConnectionPool()
{
    return this->m_resultDBConnectionPool;
}

MeasureTablePtr DataBaseManager::GetMeasureTable()
{
    return m_measureTable;
}

CalibrateTablePtr DataBaseManager::GetCalibrateTable()
{
    return m_calibrateTable;
}

MethodTablePtr DataBaseManager::GetMethodTable()
{
    return m_methodTable;
}

WarningTablePtr DataBaseManager::GetWarningTable()
{
    return m_warningTable;
}

AuditTrailTablePtr DataBaseManager::GetAuditTrailTable()
{
    return m_auditTrailTable;
}

UserTablePtr DataBaseManager::GetUserTable()
{
    return m_userTable;
}

AccessLevelTablePtr DataBaseManager::GetAccessLevelTable()
{
    return m_accessLevelTable;
}

}
