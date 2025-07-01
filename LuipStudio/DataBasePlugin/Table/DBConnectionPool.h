#ifndef DB_DBCONNECTIONPOOL_H
#define DB_DBCONNECTIONPOOL_H

#include <memory>
#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QList>

namespace DataBaseSpace
{

struct DataBaseInfo
{
    QString hostName;
    QString databaseName;
    QString username;
    QString password;
    QString databaseType;
    QString path;
    QString connectOptions;
};

class DBConnectionPool : public QThread
{

public:
    DBConnectionPool(DataBaseInfo db);
    DBConnectionPool(const DBConnectionPool &other);
    DBConnectionPool& operator=(const DBConnectionPool &other);
    QSqlDatabase CreateConnection(const QString &connectionName); // 创建数据库连接
    ~DBConnectionPool();

    QSqlDatabase OpenConnection();                 // 获取数据库连接
    void CloseConnection(QSqlDatabase connection); // 释放数据库连接回连接池
    QSqlDatabase &GetWriteConnection();
    void SetWriteConnection(const QSqlDatabase& database);
    void AttachWriteSql(const QList<QString> &queryList);

    void StopWriteThread();

    void GetUninitInfo(QMutex* mutex, QMutex* wMutex, QString& writeConnectionName,QQueue<QString>& m_usedReadConnectionNames, QQueue<QString>& m_unusedReadConnectionNames);
    void ClearReadConnectionNames();

    void WaitSqlInsert();

protected:
    void run();

private:
    QList<QString> m_wSqlLists;

    QQueue<QString> m_usedReadConnectionNames;   // 已使用的数据库读连接名
    QQueue<QString> m_unusedReadConnectionNames; // 未使用的数据库读连接名

    QString m_writeConnectionName;

    // 数据库信息
    DataBaseInfo m_dbInfo;

    QString m_testOnBorrowSql; // 测试访问数据库的 SQL

    int m_maxWaitTime;  // 获取连接最大等待时间
    int m_waitInterval; // 尝试获取连接时等待间隔时间
    int m_maxReadConnectionCount; // 最大读连接数

    QSqlDatabase m_wdb;

    QMutex m_mutex;
    QMutex m_wMutex;
    QWaitCondition m_waitReadConnection;
    QWaitCondition m_waitWriteConnection;
    QMutex m_waitInsertMutex;
    QWaitCondition m_waitInsertFinish;
    DBConnectionPool *m_instance;
};

    typedef std::shared_ptr<DBConnectionPool> DBConnectionPoolPtr;
}

#endif // DBCONNECTIONPOOL_H
