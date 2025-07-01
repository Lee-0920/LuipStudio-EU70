#ifndef METHODTABLE_H
#define METHODTABLE_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{
enum class MethodType
{
    Online = 0, //在线
    Grab,       //离线
};

struct MethodRecord
{
    int     id;
    QString methodName; //方法名称
    int     createTime; //创建时间
    int     meaType;    //在线、离线
    int    turboMode;  //Turbo模式(快速测量，5s一次)
    int    ICRMode;    //ICR模式(开启真空泵和阀)
    int    TOCMode;    //TOC模式(暂时无效)
    int    ECMode;     //电导率(暂时无效)
    int    autoReagent;//自动添加试剂(暂时无效)
    float   reagent1Vol;//试剂1流速，单位：ul/min
    float   reagent2Vol;//试剂2流速，单位：ul/min
    int     normalRefreshTime;//冲洗时间
    int measureTimes;   //测量次数
    int rejectTimes;    //舍弃次数
};

class LUIP_SHARE MethodTable: public DataTable
{

public:
    MethodTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<MethodRecord>& dataList);                 //插入数据
    void GetData(QList<MethodRecord> &map);
    void DeleteMethod(QDateTime datetime);
    bool IsMethodNameExist(QString name);
    bool ExportTableToCsv(const QString &csvPath);
private:
    QList<MethodRecord> ResolveRecord(QSqlQuery &query);
    QMutex m_queryMutex;
};

typedef std::shared_ptr<MethodTable> MethodTablePtr;
}
#endif // METHODTABLE_H
