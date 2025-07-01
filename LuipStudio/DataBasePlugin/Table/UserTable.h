#ifndef DB_UserTable_H
#define DB_UserTable_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{
const QVector<QString> UserStatusTip = {"启用","停用","未知"};

enum class UserStatus {
    Enable,
    Disable,
    Other,
};

struct UserRecord {
    int     id;
    qint64  dataTime = 0;
    qint64  lastEditTime = 0;
    QString userName;
    QString levelName;
    QString password;
    qint64  lastLoginTime = 0;
    int     status;

};

class LUIP_SHARE UserTable: public DataTable
{

public:
    UserTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<UserRecord>& dataList);
    void InsertOrIgnoreData(const UserRecord data);
    QList<UserRecord> SelectData();
    UserRecord GetUserRecord(const QString& userName);
    int GetDataCount();
    qint64 GetNewestDataTime();
    int GetDataCount(qint64 minTime, qint64 maxTime);
    void UpdateQueryModel(QSqlQueryModel *model, int limit, int offset);
    QVector<QString> GetUserNameList();
    void DeleteDataFromName(const QString& userName);
    QString GetUserStatus(int index);
    void UpdateLevelName(const QString& oldLevelName, const QString& newLevelName);
private:
    QList<UserRecord> ResolveRecord(QSqlQuery &query);
private:
    QMutex m_queryMutex;
};

typedef std::shared_ptr<UserTable> UserTablePtr;
}
#endif
