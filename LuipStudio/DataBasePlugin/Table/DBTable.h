#ifndef DB_DBTABLE_H
#define DB_DBTABLE_H

#include <QMap>
#include <QSqlQuery>
#include <QVariant>
#include <QMutex>
#include "DBConnectionPool.h"
#include "Treasure/LuipShare.h"

namespace DataBaseSpace
{
    struct TbField
    {
        QString field;
        QString type;

        TbField(){}
        TbField(QString f ,QString t) :field(f),type(t) {}
    };

    struct FieldValue
    {
        QString field;
        QVariant value;

        FieldValue(){}
        FieldValue(QString f ,QVariant v) :field(f),value(v) {}
    };

    struct TbIndex
    {
        QString name;
        QStringList fields;

        TbIndex(){}
        TbIndex(QString n, QStringList s) :name(n),fields(s) {}
    };

class LUIP_SHARE DBTable
{
public:
    DBTable(const QString name, DBConnectionPoolPtr connectionpool);
    QString GetTableName();
    QSqlQuery Select(const QString &sql);
    void SetQuery(QSqlQueryModel *model, const QString& sql);
    void AttachField(const QString name, const QString type);

    void Insert(const QList<QString> &sqlList);
    void Update(const QList<QString> &sqlList);
    void Delete();
    void Clean();
    int GetTotalCount();
    bool IsExist();
    QList<TbField> GetTableFieldList() const;



protected:
    QString m_tableName;
    DBConnectionPoolPtr m_connectionpool;
    QList<TbField> m_fieldList;
    TbIndex m_index;
    QMutex m_tableMutex;

    void FieldChangedCheck();
    void CreateTable();
};
}
#endif // DBTABLE_H
