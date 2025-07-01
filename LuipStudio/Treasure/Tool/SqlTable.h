///**
// * @brief 数据库表格
// * @author huangyue
// * @date 2021-02-06
// * @note
// */

//#ifndef __TREASURE_TOOL_DBTABLE__
//#define __TREASURE_TOOL_DBTABLE__

//#include <QVector>
//#include <QVariant>
//#include <QSqlQuery>

//namespace Treasure
//{

//class SqlTable
//{
//public:

//    //字段
//    struct Field
//    {
//        QString name;
//        QString type;

//        Field(QString name = "", QString type = "") :name(name),type(type) {}
//    };


//    SqlTable(const QString tableName)
//    {
//        m_tableName = tableName;
//    }


//public:
//    /**
//     * @brief 新建表格
//     * @param fields 字段数组
//     * @return
//     */
//    void CreatTable(const QVector<Field>& fields)
//    {
//        QStringList array;
//        foreach(Field item, fields)
//        {
//            array.append(item.name + ' ' + item.type);
//        }
//        m_fields = fields;

//        QString sql = QString("CREATE TABLE IF NOT EXISTS %1(%2)").arg(m_tableName).arg(array.join(","));

//        //QList<QString> sqlList = {sql};
//        //m_connectionpool->AttachWriteSql(sqlList);
//    }


//    /**
//     * @brief 查询
//     * @param sql 命令
//     */
//    QSqlQuery Select(const QString& sql)
//    {
//        return QSqlQuery();
//    }


//    /**
//     * @brief 表格是否存在
//     */
//    bool IsExist()
//    {
//        QString sql = QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(m_tableName);
//        QSqlQuery query = Select(sql);

//        if(query.next())
//        {
//            int count = query.value(0).toInt();
//            if (count >= 0)
//                return true;
//        }
//        return false;
//    }


//    /**
//     * @brief 清空
//     */
//    void Clean()
//    {
//        QList<QString> sqlList = {"DELETE From " + m_tableName,
//                                  QString("UPDATE sqlite_sequence SET seq = 0 WHERE name = '%1'").arg(m_tableName)};
//    }


//    /**
//     * @brief 删除表格
//     */
//    void Delete()
//    {
//        QList<QString> sqlList = {"DROP TABLE " + m_tableName,
//                                  QString("UPDATE sqlite_sequence SET seq = 0 WHERE name = '%1'").arg(m_tableName)};
//    }


//    /**
//     * @brief 获取表格行数
//     */
//    int Count()
//    {
//        QString sql = QString("SELECT count(*) FROM %1").arg(m_tableName);
//        QSqlQuery query = Select(sql);

//        if (query.next())
//            return query.value(0).toInt();
//        return 0;
//    }



//private:
//    QString m_tableName;
//    QVector<Field> m_fields;


////    DBTable(const QString name, DBConnectionPoolPtr connectionpool);
////    void AttachField(const QString name, const QString type);
////    void Insert(const QList<QString> &sqlList);
////    void Update(const QList<QString> &sqlList);


////protected:
////    DBConnectionPoolPtr m_connectionpool;
////    TbIndex m_index;
////    QMutex m_tableMutex;
////    void FieldChangedCheck();

//};

//}

//#endif
