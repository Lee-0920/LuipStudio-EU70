#ifndef HJ212DATA_H
#define HJ212DATA_H

#include <QString>
#include <QDateTime>
#include <QMap>
#include "API/HJ212Def.h"

namespace HJ212
{

#define HJ212_NO_POLID "HJ212NOPOLID"

//HJ212串口配置
struct SerialPortConfig
{
    QString name;
    int     baud;
    QString parity;
    int     dataBit;
    int     stopBit;
};

enum ConnectType
{
    TCP = 0,
    Serial = 1,
};

//平台信息
struct PlatformInfo
{
    QString ip;         //服务IP
    int port;           //服务端口

    int connectType;    //连接类型tcp,serial
    SerialPortConfig serial;//串口配置

    int overtime;       //超时时间(单位分钟)
    int recount;        //重传次数
    int heartPackTime;  //心跳时间(单位秒，<=0表示不发)

    QString pw;         //密码
    QString mn;         //站点唯一编码

    QString st;         //st编码

    ProtocolVersion protocol; //HJ212协议类型
};

//HJ212数据上报业务类型
enum class UploadDataType
{
    RtdData = 0,            ///<实时数据
    HourData = 1,           ///<小时数据
    StandardCheckData = 2,  ///<标样核查数据
    ZeroCheckData = 3,      ///<零点核查数据
    SpanCheckData = 4,      ///<跨度核查数据
    AddstandardData = 5,    ///<加标回收数据
    ParallelData = 6,       ///<平行样数据
    InfoLog = 7,            ///<仪器信息日志
    InfoStatus = 8,         ///<仪器信息状态
    InfoParam = 9,          ///<仪器信息参数
    HeartPack = 10,         ///<心跳包
    RunStatus = 11,         ///<运行状态数据
    RunTime = 12,            ///<运行时间日数据
    MinuteData = 13,        ///<分钟数据
    DailyData = 14,         ///<日数据
    MonthData = 15,         ///<月数据
    QuarterData = 16,       ///<季数据
    YearData = 17,          ///<年数据
    PollutionAlarm = 18,    ///<污染物告警数据
};

//实时
struct RealTimeData
{
    QString polId;
    QString cDataTime;  //采集时间
    QString mDateTime;  //测量时间;
    float value;        //监测值
    QString flag;       //数据标识
    bool isException;   //数据是否异常
};

//分钟、小时、日数据
struct TimeSliceData
{
    QString polId;      //监测因子编码
    QString cDataTime;  //采集时间
    float avg;          //平均值
    double cou;         //累计值
    float min;          //最小值
    float max;          //最大值
    QString mDateTime;  //测量时间;
    QString flag;       //数据标识
};

//核查数据
struct StandardCheckData
{
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //核查值
    float standardValue;    //标样浓度
    QString flag;           //数据标识
};

//平行样数据
struct ParallelData
{
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //加标回收值
    QString waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    QString flag;           //数据标识
};

//零点核查数据
struct ZeroCheckData
{
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    QString flag;           //数据标识
    float processValue;     //测量信号
};

//加标回收数据
struct AddStandardData
{
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //加标回收值
    QString waterTime;      //加标前水样测试时间
    float waterValue;       //加标前水样结果
    float chroma;           //母液
    float sVolume;          //加标体积
    float dVolume;          //加样杯容值
    QString flag;           //数据标识
};

//跨度核查数据
struct SpanCheckData
{
    QString polId;
    QString cDataTime;      //采集时间
    float checkValue;       //零点核查值
    float standardValue;    //标准样浓度
    float SpanValue;        //仪器跨度值(量程）
    QString flag;           //数据标识
    float processValue;     //测量信号
};

//HJ212上报业务单项配置
struct UploadItem
{
    UploadDataType type;    //上传类型
    bool enable;            //是否启动
    int interval;           //上传间隔(单位秒，<=0无效)

    int lastTimeSamp;       //最新时间戳(默认设置为0)
};

/*****************************************************
 *
 * 设备流程类型、日志、状态、参数
 *
 ****************************************************/

/**
 * @brief 测量流程类型
 */
enum class FlowType
{
    Calibrate = 0,              ///<定标
    Sample = 1,                 ///<水样
    StandardCheck =  2,         ///<标样核查
    BlankCalibration = 3,       ///<空白校准
    StandardCalibration = 4,    ///<标液校准
    ZeroCheck = 5,              ///<零点核查
    SpanCheck = 6,              ///<跨度核查(量程核查)
    Addstandard = 7,            ///<加标回收
    Parallel = 8,               ///<平行样
    BlankTest = 9,              ///<空白测试

    KeepSample = 10,            ///留样
    SamplerCollect = 11,        ///留样器直接采样

    clean = 20,                 ///<清洗
    None = 255,                 ///< 空字符
};

struct EquipmentLog
{
    QString cDataTime;      //采集时间
    QString polId;          //因子编码 (比如COD仪器 w01018, 系统为 w00000)
    QString infoId;         //信息编码 (比如系统的日志 i11001)
    QString content;        //日志内容
};

struct EquipmentStatus
{
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    QString infoId;         //信息编码
    QString valueStr;       //状态值
};

struct EquipmentParam
{
    QString cDataTime;      //采集时间
    QString polId;          //因子编码
    QString infoId;         //信息编码
    QString valueStr;       //参数值
};


/*****************************************************
 *
 * 监测因子
 *
 ****************************************************/

namespace MonitoringSpace
{

/* 监测信息类型 */
enum class InfoType
{
    None = 0,
    Logger = 1,                 //日志
    Status = 2,                 //状态
    Parameter = 3,              //参数
};

/* 监测信息索引 */
enum class MonitoringInfoIndex
{
    none = 0,
    iEquipmentLogCode = 1,              //分析仪运行日志
    iEquipmentStauts = 2,               //分析仪工作状态
    iEquipmentConnectStatus = 3,        //分析仪与数采仪通讯状态
    iReagentMargin = 4,                 //反应试剂余量
    iAlarmCode = 5,                     //分析仪报警状态
    iRangeValue = 6,                    //测量量程
    iMeaAccuracy = 7,                   //测量精度
    iMeaInterval = 8,                   //测量间隔
    iDigestionTemp = 9,                 //消解温度
    iDigestionTime = 10,                //消解时长
    iBlankCalibrationDateTime = 11,     //空白校准时间
    iCurveB = 12,                       //曲线截距
    iCurveK = 13,                       //曲线斜率
    iDetectionLimit = 14,               //测量检出限
    iMeaProcessValue = 15,              //测量信号值
    iLinear = 16,                       //线性相关系数（R²）
    iBinomialFactor = 17,               //二次多项式系数
    iStdCalibrationDateTime = 18,       //标准样校准时间
    iSysLogCode = 19,                   //数采仪运行日志
    iSysStauts = 20,                    //数采仪工作状态
    iSysUser = 21,                      //数采仪用户状态
    iSysConnectStatus = 22,             //数采仪与上位机通讯状态
    iChunnelConnectStauts = 23,         //数采仪通道通讯状态

    iAlarmInfo = 24,                    //报警详情
    iCorrectionFactor = 25,             //修正系数
    iCorrectionValue = 26,              //修正值
    iSystemTime = 27,                   //系统时间
    iSamplingLastTime = 28,             //最后一次采样时间
    iStdCheckLastTime = 29,             //最近一次标液核查时间
    iStdCheckResult = 30,               //最近一次标液核查结果
    iZeroCheckLastTime = 31,            //最近一次零点核查时间
    iMeaAbsorbanceValue = 32,           //测量吸光度值
    iZeroCheckDensity = 33,             //零点校准浓度
    iZeroCheckOriginalValue = 34,       //零点校准原始值
    iZeroCheckDrift  = 35,              //零点校准漂移

    iRangeCheckTime = 36,                //量程校准时间
    iRangeCheckDensity = 37,             //量程校准浓度
    iRangeCheckOriginalValue = 38,       //量程校准原始值
    iRangeCheckDrift  = 39,              //量程校准漂移

    iMeaOriginalValue = 40,              //测量原始值
    iAutoCalibrationInterval = 41,       //自动校准时间间隔

    iStd1AbsorbanceValue = 42,            //标1吸光度值
    iStd2AbsorbanceValue = 43,            //标2吸光度值
    iZeroAbsorbanceValue = 44,            //零点吸光度值

    iCorrectionK = 45,                    //修正斜率
    iCorrectionB = 46,                    //修正截距
    iStdCheckTime = 47,                   //标样核査时间
    iStdCheckDensity = 48,                //标样核查标液浓度

};

/* 设备类型 */
enum class DeviceType
{
    None = 0,
    Equipment = 1,              //分析仪
    DateCollecter = 2,          //数采仪
    Assistor = 3,               //辅助设备
};

/**
 * @brief 监测因子配置
 */
struct MonitoringFactor
{
    QString text;       //名称
    QString polId;      //因子编码
    QString oldPolId;   //旧的编码
    int decimal;        //小数位
    QString unit;       //单位
};

/**
 * @brief 监测信息
 */
struct MonitoringInfo
{
    int index;          //信息索引
    QString infoId;     //信息编码
    QString text;       //名称
    QString unit;       //单位
    int infoType;       //类型:日志、状态、参数
    int deviceType;     //设备类型
};

}

/*****************************************************
 *
 * 回调接口
 *
 ****************************************************/

struct HJ212CallBack
{
    //保存平台配置|上传配置
    bool (*SavePlatformInfo)(PlatformInfo info);
    bool (*SaveUploadItem)(UploadItem item);

    //获取PolId
    QString (*GetPolId)();

    //设置系统时间
    bool (*SetSystemTime)(QDateTime time);

    //根据数据类型判断是否需要回包
    bool (*IsNeedRespond)(UploadDataType type);

    //启动仪器流程
    bool (*StartEquipmentsFlow)(FlowType flowType);

    //获取仪器日志|状态
    QList<EquipmentLog> (*GetSysEquipmentLogList)(QDateTime startTime, QDateTime endTime);
    bool (*GetSysEquipmentStatus)(QString infoId, EquipmentStatus& status);

    //获取|设置仪器参数
    bool (*GetEquipmentParam)(QString infoId, EquipmentParam& params);
    bool (*SetEquipmentParam)(QString infoId, QString value);

    //获取实时数据 | 小时数据
    RealTimeData (*GetRealTimeData)();
    QList<TimeSliceData> (*GetNewestHourDataList)();

    //获取实时|小时|核查历史数据
    QList<RealTimeData> (*GetRealTimeDataList)(QDateTime startTime, QDateTime endTime);
    QList<TimeSliceData> (*GetHourDataList)(QDateTime startTime, QDateTime endTime);
    QList<StandardCheckData> (*GetStandardCheckDataList)(QDateTime startTime, QDateTime endTime);
    QList<ParallelData> (*GetParallelDataList)(QDateTime startTime, QDateTime endTime);
    QList<ZeroCheckData> (*GetZeroCheckDataList)(QDateTime startTime, QDateTime endTime);
    QList<AddStandardData> (*GetAddStandardDataList)(QDateTime startTime, QDateTime endTime);
    QList<SpanCheckData> (*GetSpanCheckDataList)(QDateTime startTime, QDateTime endTime);




};

}
#endif // HJ212DATA_H
