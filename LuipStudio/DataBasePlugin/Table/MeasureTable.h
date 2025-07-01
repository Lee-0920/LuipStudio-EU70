#ifndef DB_SPECIMENTABLE_H
#define DB_SPECIMENTABLE_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{

struct MeasureRecord
{
    int     id;
    int     measureDateTime; //时间
    float   consistency;
    float   consistencyTC;
    float   consistencyIC;
    float   peakTC;
    float   peakIC;
    int     measureType;
    float   initCellTempTC;
    float   initCellTempIC;
    float   finalCellTempTC;
    float   finalCellTempIC;
    float   initEnvironmentTemp;
    float   finalEnvironmentTemp;
    int     measureconsumeDateTime;
    float   currentRange;
    int     flag;        
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
    //*QString格式不存入Bin文件中*//
    QString methodName;
    int createTime;
};

class LUIP_SHARE MeasureTable: public DataTable
{

public:
    MeasureTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<MeasureRecord>& dataList);                 //插入数据
    void GetData(QList<MeasureRecord> &map);
    QList<MeasureRecord> GetDataByTime(QDateTime datetime);
    void DeleteData(QDateTime datetime);
private:
   QList<MeasureRecord> ResolveRecord(QSqlQuery &query);
    QMutex m_queryMutex;
};

typedef std::shared_ptr<MeasureTable> MeasureTablePtr;
}
#endif
