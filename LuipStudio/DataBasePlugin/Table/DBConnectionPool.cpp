#include <QDebug>
#include "DBConnectionPool.h"
#include "Treasure/SystemDef.h"

namespace DataBaseSpace
{

DBConnectionPool::DBConnectionPool(DataBaseInfo db)
{
    this->start();

    // 创建数据库连接的这些信息在实际开发的时都需要通过读取配置文件得到，
    // 这里为了演示方便所以写死在了代码里。

    m_dbInfo = db;

    m_testOnBorrowSql = "SELECT 1";

    m_maxWaitTime  = 5000;
    m_waitInterval = 200;
    m_maxReadConnectionCount  = 10;

   QStringList list = m_dbInfo.databaseName.split(".");//QString字符串分割函数

    m_writeConnectionName = list.at(0) + "_wc";

    m_wdb = this->CreateConnection(m_writeConnectionName);

    if (!m_wdb.isOpen())
    {
        qDebug() << "Write Connection open fail!!!";
    }
}

DBConnectionPool::~DBConnectionPool()
{
    // 请求终止
    requestInterruption();

    // 销毁连接池的时候删除所有的连接
    foreach(QString connectionName, m_usedReadConnectionNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }

    foreach(QString connectionName, m_unusedReadConnectionNames)
    {
        QSqlDatabase::removeDatabase(connectionName);
    }

    QSqlDatabase::removeDatabase(m_writeConnectionName);

    quit();
    wait();
}

void DBConnectionPool::AttachWriteSql(const QList<QString> &queryList)
{
    if (!queryList.empty())
    {
        m_wMutex.lock();

        for (QList<QString>::const_iterator it = queryList.begin(); it != queryList.end(); it++)
        {
            m_wSqlLists.append(*it);
        }

        m_wMutex.unlock();

//      qDebug() << "wake start";
        m_waitWriteConnection.wakeOne();
//      qDebug() << "wake end";
        WaitSqlInsert();
    }
}

void DBConnectionPool::StopWriteThread()
{
   m_waitWriteConnection.wakeOne();

   // 请求终止
   requestInterruption();

   quit();
}

void DBConnectionPool::GetUninitInfo(QMutex* mutex, QMutex* wMutex,QString& writeConnectionName, QQueue<QString>& usedReadConnectionNames, QQueue<QString>& unusedReadConnectionNames)
{
    mutex = &m_mutex;
    wMutex = &m_wMutex;
    usedReadConnectionNames = m_usedReadConnectionNames;
    unusedReadConnectionNames = m_unusedReadConnectionNames;
    writeConnectionName = m_writeConnectionName;
}

void DBConnectionPool::ClearReadConnectionNames() {
    m_usedReadConnectionNames.clear();
    m_unusedReadConnectionNames.clear();
    if(m_wdb.isOpen()){
        m_wdb.close();
    }
}



void DBConnectionPool::WaitSqlInsert()
{
    QMutexLocker locker(&m_waitInsertMutex);
    m_waitInsertFinish.wait(&m_waitInsertMutex, 2000);

    return ;
}

QSqlDatabase DBConnectionPool::OpenConnection()
{
    QString connectionName;

    QSqlDatabase db;

//    long t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QMutexLocker locker(&m_mutex);

//    long t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 已创建连接数
    int connectionCount = this->m_unusedReadConnectionNames.size() + this->m_usedReadConnectionNames.size();

//    logger->debug("connectionCount: %d", connectionCount);

    // 如果连接已经用完，等待 waitInterval 毫秒看看是否有可用连接，最长等待 maxWaitTime 毫秒
    for (int i = 0;
         i < this->m_maxWaitTime && this->m_unusedReadConnectionNames.size() == 0 && connectionCount == this->m_maxReadConnectionCount;
         i += this->m_waitInterval)
    {
        trLogger->Debug("wait free connection");
        m_waitReadConnection.wait(&m_mutex, this->m_waitInterval);
        connectionCount = this->m_unusedReadConnectionNames.size() + this->m_usedReadConnectionNames.size();    // 重新计算已创建连接数
    }

//    long t3 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    if (this->m_unusedReadConnectionNames.size() > 0)
    {
        connectionName = this->m_unusedReadConnectionNames.dequeue();   // 有已经回收的连接，复用它们
//        logger->debug("reUser: %s",  connectionName.toStdString().c_str());
    }
    else if (connectionCount < this->m_maxReadConnectionCount)
    {
          connectionName = m_dbInfo.databaseName + QString("rc%2").arg(connectionCount + 1);   // 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
//        logger->debug("new: %s",  connectionName.toStdString().c_str());
    }
    else
    {
        trLogger->Warn("Cannot create more connections.");
        return QSqlDatabase();                          // 已经达到最大连接数
    }

//    long t4 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 创建连接
    db = this->CreateConnection(connectionName);

//    long t5 = QDateTime::currentDateTime().toMSecsSinceEpoch();

    // 有效的连接才放入 usedConnectionNames
    if (db.isOpen())
    {
        this->m_usedReadConnectionNames.enqueue(connectionName);
    }

//    logger->debug("[OpenConnection] t2 - t1 = %d ms, t3 -t2 = %d ms, t4 -t3 = %d ms, t5 -t4 = %d ms", t2 -t1, t3 -t2, t4 -t3, t5 -t4);

    return db;
}

void DBConnectionPool::SetWriteConnection(const QSqlDatabase& database) {
    m_wdb = database;
}

void DBConnectionPool::CloseConnection(QSqlDatabase connection)
{
    QString connectionName = connection.connectionName();

    // 如果是我们创建的连接，从 used 里删除，放入 unused 里
    if (this->m_usedReadConnectionNames.contains(connectionName))
    {
        QMutexLocker locker(&m_mutex);
        this->m_usedReadConnectionNames.removeOne(connectionName);
        this->m_unusedReadConnectionNames.enqueue(connectionName);
        m_waitReadConnection.wakeOne();
    }
}

QSqlDatabase &DBConnectionPool::GetWriteConnection()
{
    return m_wdb;
}

QSqlDatabase DBConnectionPool::CreateConnection(const QString &connectionName) {
    // 连接已经创建过了，复用它，而不是重新创建
    if (QSqlDatabase::contains(connectionName))
    {
        QSqlDatabase db1 = QSqlDatabase::database(connectionName);

        // 返回连接前访问数据库，如果连接断开，重新建立连接
//            qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
        QSqlQuery query(m_testOnBorrowSql, db1);

        if (query.lastError().type() != QSqlError::NoError && !db1.open())
        {
            qDebug() << "Open datatabase error:" << db1.lastError().text();
            return QSqlDatabase();
        }

        return db1;
    }

    // 创建一个新的连接
    QSqlDatabase db = QSqlDatabase::addDatabase(m_dbInfo.databaseType, connectionName);
    db.setHostName(m_dbInfo.hostName);
    db.setDatabaseName(m_dbInfo.path + "/" + m_dbInfo.databaseName);
    db.setUserName(m_dbInfo.username);
    db.setPassword(m_dbInfo.password);
    db.setConnectOptions(m_dbInfo.connectOptions);

    if (!db.open())
    {
        qDebug() << "Open datatabase error:" << db.lastError().text();
        return QSqlDatabase();
    }

//    qDebug() << "Create new Connecter:" << connectionName;

    return db;
}

void DBConnectionPool::run()
{
    trLogger->Debug("DBConnectionPool thread start!");
    while (!isInterruptionRequested())
    {
        QMutexLocker locker(&m_wMutex);
        m_waitWriteConnection.wait(&m_wMutex, 2000);

        if (!m_wSqlLists.empty())
        {
            m_wdb.transaction();

//            long t1 = QDateTime::currentDateTime().toMSecsSinceEpoch();

            int cnt = m_wSqlLists.size();
            for (int i = 0; i < cnt; i++)
            {
                QString sql = m_wSqlLists.first();

//                logger->debug(sql.toStdString());

                QSqlQuery query(m_wdb);
                if(!query.exec(sql))
                {
                    QString err = query.lastError().text();

                    trLogger->Warn(sql);
                    trLogger->Warn(err);

                    QThread::msleep(10);

                    m_wdb.rollback();
                    trLogger->Debug("Sql rollback!!!!");
                }
                else
                {
                    m_wdb.commit();
                }

                m_wSqlLists.pop_front();

//                QThread::msleep(1);
           }


//          long t2 = QDateTime::currentDateTime().toMSecsSinceEpoch();
//          logger->debug(QString("Write %1 -> %2: %3 ms").arg(cnt).arg(m_wSqlLists.size()).arg(t2 -t1).toStdString());

            m_wSqlLists.clear();
            m_waitInsertFinish.wakeAll();
       }
   }
    trLogger->Debug("DBConnectionPool thread exit!");
}
}
