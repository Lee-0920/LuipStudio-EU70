#ifndef DB_DATATABLE_H
#define DB_DATATABLE_H

#include "DBTable.h"


template<typename T> using MapIterator = typename QMap<QString, QList<T> >::const_iterator;

namespace DataBaseSpace
{

enum class DataTableType
{
    Undefine = 0,   //未定义
    Real,           //实时数据
    Minute,         //分钟数据
    Hour,           //小时数据
    Day,            //日数据
    Month,          //月数据
    Quarter,        //季数据
    Year,           //年数据
    StandardCheck,  //标样核查
    ZeroCheck,      //零点核查
    SpanCheck,      //跨度核查
    AddStandard,    //加标回收
    Parallel,       //平行核查

    RunLog,         //运行日志
    WarningLog,     //告警日志
    SpecimenRecord, //样品记录
};

struct TimeSliceData
{
    int id;
    QString polId;
    QString cDataTime;  //采集时间
    float avg;          //平均值
    double cou;          //累计值
    float min;          //最小值
    float max;          //最大值
    QString mDateTime;  //测量时间;
    QString flag;       //数据标识
    int pSended;        //平台上报标志
};

class LUIP_SHARE DataTable: public DBTable
{
public:
    DataTable(const QString name, DBConnectionPoolPtr connectionpool);
    virtual ~DataTable();
    virtual void SetDataSendFlag(QList<int> recordIds, int platFormId);

protected:
    QString TransformPolIdListToString(const QList<QString> polIdList);
    template<typename T>void RecordGroupByTime(QMap<QString, QList<T> > &map, QList<T> &list, int group = 0)
    {
        while(1)
        {
            if (!list.empty())
            {
                QString createTime;

                QList<T> itemList;

                createTime = list.first().createTime;

                for (int i = 0; i < list.count();)
                {
                    T item = list.at(i);
                    if (createTime == item.createTime)
                    {
                        itemList.append(item);
                        list.removeAt(i);
                    }
                    else
                    {
                        i++;
                    }
                }
                map.insert(createTime, itemList);

                //达到限定组数
                if (group > 0 && map.count() >= group)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    template<typename T>void RecordGroupByPolId(QMap<QString, QList<T> > &map, QList<T> &list, int group = 0)
    {
        while(1)
        {
            if (!list.empty())
            {
                QString polId;

                QList<T> itemList;

                polId = list.first().polId;

                for (int i = 0; i < list.count();)
                {
                    T item = list.at(i);
                    if (polId == item.polId)
                    {
                        itemList.append(item);
                        list.removeAt(i);
                    }
                    else
                    {
                        i++;
                    }
                }
                map.insert(polId, itemList);

                //达到限定组数
                if (group > 0 && map.count() >= group)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    template<typename T>void RecordGroupByTimeBak(QMap<QString, QList<T> > &map, QList<T> &list, int groupLimit = 0, int listLimit = 0)
    {
        if (!list.empty())
        {
            foreach (T item, list)
            {
                typename QMap<QString, QList<T> >::iterator it = map.find(item.cDataTime);
                if(it != map.end())
                {
                    if(listLimit > 0 && it.value().count() >= listLimit)
                    {
                        continue;
                    }
                    else
                    {
                        it.value().push_back(item);
                    }
                }
                else
                {
                    QString curDataTime = item.cDataTime;
                    QList<T> curItemList;
                    curItemList.push_back(item);

                    //达到限定组数
                    if (groupLimit > 0 && map.count() >= groupLimit)
                    {
                        continue;
                    }
                    else
                    {
                        map.insert(curDataTime, curItemList);
                    }
                }
            }
        }
    }

    template<typename T>void RecordGroupByPolIdBak(QMap<QString, QList<T> > &map, QList<T> &list, int groupLimit = 0, int listLimit = 0)
    {
        if (!list.empty())
        {
            foreach (T item, list)
            {
                typename QMap<QString, QList<T> >::iterator it = map.find(item.polId);
                if(it != map.end())
                {
                    if(listLimit > 0 && it.value().count() >= listLimit)
                    {
                        continue;
                    }
                    else
                    {
                        it.value().push_back(item);
                    }
                }
                else
                {
                    QString curPolId = item.polId;
                    QList<T> curItemList;
                    curItemList.push_back(item);

                    //达到限定组数
                    if (groupLimit > 0 && map.count() >= groupLimit)
                    {
                        continue;
                    }
                    else
                    {
                        map.insert(curPolId, curItemList);
                    }
                }
            }
        }
    }

protected:
    int m_reissueDay;
    int m_unuploadlimit;
};
typedef std::shared_ptr<DataTable> DataTablePtr;
}

#endif // DB_DATATABLE_H
