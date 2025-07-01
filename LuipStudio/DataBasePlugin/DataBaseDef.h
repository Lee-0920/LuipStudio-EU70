#ifndef DB_DATABASE_DEF_H
#define DB_DATABASE_DEF_H

#include <QList>

namespace DataBaseSpace
{

//标样
struct AddStandardData
{
    int id;
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //加标回收值
    QString waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    float chroma;           //母液
    float sVolume;          //加标体积
    float dVolume;          //加样杯容值
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

//平行样
struct ParallelData
{
    int id;
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //加标回收值
    QString waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

struct ZeroCheckData
{
    int id;
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

struct SpanCheckData
{
    int id;
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

struct StandardCheckData
{
    int id;
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //核查值
    float standardValue;    //标样浓度
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

struct RealTimeData
{
    int id;
    QString polId;
    QString cDataTime;  //采集时间
    QString mDateTime;  //测量时间;
    float value;        //监测值
    QString flag;       //数据标识
    int pSended;        //平台上报标志
    bool isException;   //数据是否异常
};

struct CumulativeData
{
    QString startDateTime;
    QString endDateTime;
    QString polId;
    double cou;
    float min;
    float avg;
    float max;
    QString flag;
    int count;
    bool isValid;
};

}
#endif
