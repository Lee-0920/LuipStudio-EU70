#ifndef __TREASURE_DATA_RECORD__
#define __TREASURE_DATA_RECORD__

#include <QString>

namespace DataBaseSpace
{

struct ManualFlowData
{
    int id;
    QString flowName;   //流程名
    QString polId;
    QString cDataTime;  //采集时间
    QString mDateTime;  //测量时间;
    float value;        //监测值
    QString flag;       //数据标识
    bool isException;   //数据是否异常
};

struct RealTimeData
{
    int id;
    char    elementId;  //要素ID
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

struct EquipmentAlarm
{
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    QString infoCode;       //信息编码
    int alarmId;              //告警代码
    QString content;        //告警内容
    int pSended;            //平台上报标志
};

struct EquipmentLog
{
    int id;
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    int infoIndex;       //信息编码
    QString logId;              //日志代码
    QString content;        //日志内容
    int pSended;            //平台上报标志
};

struct EquipmentParam
{
    int id;
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    int infoIndex;          //信息索引
    QString valueStr;        //参数值
    int pSended;            //平台上报标志
};

struct EquipmentStatus
{
    int id;
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    int infoIndex;       //信息编码
    QString valueStr;       //状态值
    QString content;        //状态内容
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

    float processValue;     //测量信号
};

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
    float recoveryRate;     //加标回收率 abs(加标回收值-水样值) / ( (加标体积*母液值)/加标容积 )
    QString flag;           //数据标识
    int pSended;            //平台上报标志
};

struct RetainedSampleLog
{
    int id;
    QString cDataTime;           //留样时间
    QString retainedType;       //留样方式
    QString cause;              //留样原因
    int bottleId;               //留样瓶号
    double volume;              //留样容积
    QString result;             //留样结果
    int pSended;                //平台上报标志
};

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

    float processValue;     //测量信号
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

    float processValue;     //测量信号
};


struct SysRunLog
{
    int id;
    QString cDataTime;      //采集时间
    QString content;
    int pSended;            //平台上报标志
};

struct MonitoringFactor
{
    int id;
    QString name;
    QString text;
    QString polId;
    QString oldPolId;
    int decimal;
    QString unit;
    float overMinValue;
    float overMaxValue;
    float zeroCheckValue;
    float spanCheckValue;
    float standardCheckValue;
    float addStandardMother;
    float addStandardVolume;
    float addStandardTotalVol;
};

}

#endif
