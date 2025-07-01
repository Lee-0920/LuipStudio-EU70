/**
 * @brief  HJ212定义
 * @author huangyue
 * @date   2020/09/29 
 * @note   主要用于存放协议定义的一些通讯包固定内容，如包头包尾、字段长度、命令之类
 *         通讯包结构(固定)： 包头##(2) + 数据段长度(4左补零) + 数据段(0~1024) + CRC16(4) + 包尾\r\n(2)
 *         
 *         数据段DATA结构(根据协议不同，字段可能有增减)：
 *                  QN: 毫秒时间戳 YYYYMMDDhhmmsszzz(17)
 *                  ST: 系统编码，现在用到有1、地表水监测21 2、污染源32  3、系统交互91
 *                  CN: 命令编码，不同协议有不同的命令，不过大多数都是一致的
 *                  PW: 密码，现在只用默认值123456
 *                  MN: 监测站点的唯一码，由外部设置
 *                  FLAG: 标志(8) V5|V4|V3|V2|V1|V0|D|A
 *                               V5-V0: 000000 污染源2005、000001 污染源2017、000010 地表水2017
 *                                   D: 1分包 0不分包
 *                                   A: 1应答 0不应答    
 *                  PUMN: 总包数(如果没有分包，这个字段不加)
 *                  PNO:  包号(如果没有分包，这个字段不加)
 *                  CP:   指令参数，左右用 && 括起来，用来放一些具体数据, 比如下位机的系统时间、日志、监测参数等
 *         例子:
 *              ##0078QN=20201008154743635;ST=91;CN=9014;PW=123456;MN=SD000001;Flag=9;CP=&&QnRtn=1&&71c0\r\n
 */

#ifndef INTERFACE_HJ212_HJ212DEF_H
#define INTERFACE_HJ212_HJ212DEF_H

#include <string>

namespace HJ212
{

#define HJ212_FINAL_PACKET_HEADER   "##"   //通讯包包头
#define HJ212_FINAL_PACKET_TAIL     "\r\n" //通讯包包尾

//一些字段和包的长度
#define HJ212_FINAL_PACKET_MAXSIZE  1036   //通讯包最大长度 = 包头2+数据段长度4+数据段(0~1024)+CRC4
#define HJ212_DATA_MAXSIZE          1024   //数据段最大长度
#define HJ212_CP_MAXSIZE            950    //CP字段的最大长度
#define HJ212_QN_LENGTH             17     //QN字段的固定长度 YYYYMMDDhhmmsszzz
#define HJ212_CN_LENGTH             4      //CN字段的固定长度
#define HJ212_UPLOAD_CMD_LIMIT      10

//ST系统编码
#define HJ212_ST_SURFACEWATER     21  //地表水监测
#define HJ212_ST_POLLUTIONWATER   32  //污染源
#define HJ212_ST_REPLY            91  //系统交互

//不同协议之间固定的CN命令编码，
#define HJ212_CN_REPLY_REQUEST   "9011"  //请求应答
#define HJ212_CN_REPLY_RESULT    "9012"  //执行结果应答
#define HJ212_CN_REPLY_NOTICE    "9013"  //通知应答
#define HJ212_CN_REPLY_DATA      "9014"  //数据应答


#define UPLOAD_DATA_TIME_INTERVAL 500  //一次性上传多条数据需要等待的时间间隔(毫秒)


/**
 * @brief 命令包版本
 */
enum class ProtocolVersion
{
    HJ212_2017D = 0,     ///< 地表水2017
    HJ212_2017 = 1,      ///< 污染源2017
    HJ212_2005 = 2       ///< 污染源2005
};

//Flag字段 - 回包与分包
enum class FlagRespSubpack
{
    None = 0x0,
    Resp = 0x1,
    SubPackets = 0x2,
    RespAndSubpack = 0x3,
};

//Flag字段 - 协议
enum class FlagProtocol
{
    HJ2005 = 0x0,
    HJ2017 = 0x4,
    HJ2017D = 0x8
};

//执行结果编码 (CP字段的参数)
enum class ExeRtn
{
    Success = 1,              ///< 执行成功
    Failed = 2,               ///< 执行失败(未知原因)
    WrongCondition = 3,       ///< 命令请求条件错误
    CommunicationTimeout = 4, ///< 通讯超时
    SystemBusy = 5,           ///< 系统繁忙不能执行
    SystemFault = 6,          ///< 系统故障
    NoData = 100              ///< 没有数据
};

//应答编码 (CP字段的参数)
enum class QnRtn
{
    Accept = 1,    ///< 准备执行请求
    Refuse = 2,    ///< 请求被拒绝
    ErrorPW = 3,   ///< PW错误
    ErrorMN = 4,   ///< MN误
    ErrorST = 5,   ///< ST错误
    ErrorFlag = 6, ///< Flag错误
    ErrotQN = 7,   ///< QN错误
    ErrorCN = 8,   ///< CN错误

    WryErrorCRC = 9,  ///< 污染源的CRC校验错误
    DbsErrorBusy = 9, ///< 地表水系统繁忙

    DbsErrorCRC = 99, ///< 地表水CRC校验错误
    UnknowError = 100 ///< 未知错误
};

// 命令包类型
enum class CommandType
{
    ControlCommand = 0,         ///< 控制命令CN<9000
    InteractionCommand = 1,     ///< 交互命令CN>=9000
};



/************************************************
 *
 * 不同协议的命令码
 *
 ***********************************************/

//2005污染源命令
enum class WryCNType2005
{
    CmdDisplayPack                 = 0000,   //测试
    
    CmdSetOvetTimeAndReCount       = 1000,   //设置超时时间及重发次数
    CmdSetPollutionAlarmTime       = 1001,   //设置超限报警时间
    CmdGetSystemTime               = 1011,   //提取现场机时间
    CmdSetSystemTime               = 1012,   //设置现场机时间
    CmdGetPollutionAlarmLimitValue = 1021,   //提取污染物报警门限值
    CmdSetPollutionAlarmLimitValue = 1022,   //设置污染物报警门限值
    CmdGetAlarmTargetAddress       = 1031,   //提取上位机地址
    CmdSetAlarmTargetAddress       = 1032,   //设置上位机地址
    CmdGetDataUploadTime           = 1041,   //提取数据上报时间
    CmdSetDataUploadTime           = 1042,   //设置数据上报时间
    CmdGetUploadRtdDataInterval    = 1061,   //提取实时数据间隔
    CmdSetUploadRtdDataInterval    = 1062,   //设置实时数据间隔
    CmdSetPassword                 = 1072,   //设置数采仪密码

    CmdGetRealTimeData             = 2011,   //提取监测指标实时数据
    CmdStopRealTimeDataUpload      = 2012,   //停止察看污染物实时数据[通知]
    CmdGetRealTimeStatus           = 2021,   //取设备运行状态数据
    CmdStopRealTimeStatusUpload    = 2022,   //停止察看设备运行状态[通知]
    CmdGetDayHistoricalData        = 2031,   //取污染物日历史数据
    CmdGetDaySystemRunTime         = 2041,   //取设备运行时间日历史数据
    CmdGetMinuteHistoricalData     = 2051,   //取污染物分钟历史数据
    CmdGetHourHistoricalData       = 2061,   //取污染物小时历史数据
    CmdGetAlarmRecord              = 2071,   //取污染物报警记录

    CmdStartZeroRangeCheck         = 3011,   //零点校准量程校准(定标?)
    CmdStartMeasureSample          = 3012,   //即时采样
    CmdStartClean                  = 3013,   //启动清洗/反吹
    CmdSetMeasureSampleInterval    = 3014,   //设置采样时间周期
//    CmdSetSystemRunMode            = 3042,   //切换系统运行模式
    CmdRebootSystem                = 3043,   //远程重启数采仪
//    CmdSetSystemToStoppedStatus    = 3045,   //远程控制系统紧急停机
//    CmdSetSystemToIdleStatus       = 3046,   //远程控制系统进入待机状态

    UploadResetTime                = 2081,   //上传数采仪开机时间
    UploadRtdData                  = 2011,   //上传监测指标实时数据
    UploadRunStatusData            = 2021,   //上传设备运行状态数据
    UploadDayData                  = 2031,   //上传污染物日数据
    UploadRunTimeData              = 2041,   //上传设备运行时间日数据
    UploadMinuteData               = 2051,   //上传污染物分钟数据
    UploadHourData                 = 2061,   //上传污染物小时数据
    UploadPollutionAlarmData       = 2071,   //上传污染物报警记录
    UploadPollutionAlarmNotice     = 2072,   //上传报警事件

    UploadHeartPack                = 9021,   //上传心跳包
};

//2017污染源命令
enum class WryCNType2017
{
    CmdDisplayPack                  = 0000,   //测试

    CmdSetOvetTimeAndReCount        = 1000,   //设置超时时间及重发次数
    CmdGetSystemTime                = 1011,   //提取现场机时间
    CmdSetSystemTime                = 1012,   //设置现场机时间
    CmdGetUploadRtdDataInterval     = 1061,   //提取实时数据间隔
    CmdSetUploadRtdDataInterval     = 1062,   //设置实时数据间隔
    CmdGetUploadMinuteDataInterval  = 1063,   //提取分钟数据间隔
    CmdSetUploadMinuteDataInterval  = 1064,   //设置分钟数据间隔
    CmdSetPassword                  = 1072,   //设置数采仪密码

    CmdGetRealTimeData              = 2011,   //提取监测指标实时数据
    CmdStopRealTimeDataUpload       = 2012,   //停止察看污染物实时数据[通知]
    CmdGetRealTimeStatus            = 2021,   //取设备运行状态数据
    CmdStopRealTimeStatusUpload     = 2022,   //停止察看设备运行状态[通知]
    CmdGetDayHistoricalData         = 2031,   //取污染物日历史数据
    CmdGetDaySystemRunTime          = 2041,   //取设备运行时间日历史数据
    CmdGetMinuteHistoricalData      = 2051,   //取污染物分钟历史数据
    CmdGetHourHistoricalData        = 2061,   //取污染物小时历史数据

    CmdGetCheckData             = 2062,   //取核查数据
    CmdGetAddstandardData       = 2063,   //取加标回收数据
    CmdGetParallelData          = 2064,   //取平行样数据
    CmdGetZeroCheckData         = 2065,   //取零点核查数据
    CmdGetRangeCheckData        = 2066,   //取跨度核查数据

    CmdStartZeroRangeCheck          = 3011,   //零点校准量程校准
    CmdStartMeasureSample           = 3012,   //即时采样
    CmdStartClean                   = 3013,   //启动清洗/反吹
    CmdStartComparisonMeasureSample = 3014,   //比对采样
    CmdRemoteStoreSample            = 3015,   //超标留样/手动远程留样
    CmdSetMeasureSampleInterval     = 3016,   //设置采样时间周期
    CmdGetMeasureSampleInterval     = 3017,   //获取采样时间周期
    CmdGetMeasureSampleTime         = 3018,   //提取出样时间
    CmdGetSystemManufactureNumber   = 3019,   //提取设备唯一标识
    CmdGetSystemInformation         = 3020,   //提取数采仪/监测仪信息
    CmdSetSystemInformation         = 3021,   //设置数采仪/监测仪信息(参数)

    CmdSamplerRemoteLawSample       = 3023,   //采样器远程执法留样（用于上位机手动启动采样器留样）
    CmdSamplerStartMeasureSample    = 3024,   //采样器采样（用于上位机启动釆样器釆样）
    CmdSetOverValue                 = 3025,   //设置超标值

    CmdSamplerSetMeasureSampleTime  = 3060,   //设置采样器采样时间（设置源水泵从河口取水采样时长（单位为秒））
    CmdSamplerSetInSampleTime       = 3061,   //设置采样器进样时间（从设置沉淀池向采样杯打水时长（单位为秒））

    CmdStartMeasureCheck        = 3080,   //启动单台仪表标液核查
    CmdStartMeasureAddstandard  = 3081,   //启动单台仪表加标回收
    CmdStartMeasureParallel     = 3082,   //启动单台仪表平行样测试
    CmdStartMeasureZeroCheck    = 3083,   //启动单台仪表零点核查
    CmdStartMeasureRangeCheck   = 3084,   //启动单台仪表跨度核查
    CmdStartBlankCalibrate      = 3085,   //启动空白校准(定标?)
    CmdStartStandardCalibrate   = 3086,   //启动标样校准(定标?)
    CmdStopInstrumentFlow       = 3100,   //停止仪器当前动作

//    CmdStopInstrumentFlow           = 3100,   //停止仪器当前动作
//    CmdSetSystemRunMode             = 3042,   //切换系统运行模式
    CmdRebootSystem                 = 3043,   //远程重启数采仪
//    CmdSetSystemToStoppedStatus     = 3045,   //远程控制系统紧急停机
//    CmdSetSystemToIdleStatus        = 3046,   //远程控制系统进入待机状态

    SystemTimeCheck                 = 1013,   //现场机时间校准(向上的通知命令)

    UploadResetTime                 = 2081,   //上传数采仪开机时间
    UploadRtdData                   = 2011,   //上传监测指标实时数据
    UploadRunStatusData             = 2012,   //上传设备运行状态数据
    UploadDayData                   = 2031,   //上传污染物日数据
    UploadRunTimeData               = 2031,   //上传设备运行时间日数据
    UploadMinuteData                = 2031,   //上传污染物分钟数据
    UploadHourData                  = 2031,   //上传污染物小时数据

    UploadStandardCheckData     = 2062,   //上传地表水核查数据
    UploadAddStandardData       = 2063,   //上传地表水加标回收数据
    UploadParallelData          = 2064,   //上传地表水平行样数据
    UploadZeroCheckData         = 2065,   //上传零点核查数据
    UploadSpanCheckData         = 2066,   //上传跨度核查数据

    UploadSystemLog                 = 3020,   //上传数采仪信息(日志)
    UploadInfoLog                   = 3020,   //上传监测仪信息(日志)
    UploadInfoStatus                = 3020,   //上传监测仪信息(状态)
    UploadInfoParam                 = 3020,   //上传监测仪信息(参数)

    UploadHeartPack                 = 9015,   //上传心跳包
};

//2017地表水命令
enum class DbsCNType2017
{
    CmdDisplayPack              = 0000,   //测试

    CmdSetOvetTimeAndReCount    = 1000,   //设置超时时间及重发次数
    CmdGetInstrumentTime        = 1011,   //提取监测仪表时间
    CmdSetInstrumentTime        = 1012,   //设置监测仪表时间
    CmdGetSystemTime            = 1014,   //提取数采仪时间
    CmdSetSystemTime            = 1015,   //设置数采仪时间
    CmdGetUploadRtdDataInterval = 1061,   //提取实时数据间隔
    CmdSetUploadRtdDataInterval = 1062,   //设置实时数据间隔
    CmdSetPassword              = 1072,   //设置数采仪密码

    CmdGetRealTimeData          = 2011,   //提取监测指标实时数据
    CmdGetHourHistoricalData    = 2061,   //取地表水小时历史数据
    CmdGetCheckData             = 2062,   //取地表水核查数据
    CmdGetAddstandardData       = 2063,   //取地表水加标回收数据
    CmdGetParallelData          = 2064,   //取地表水平行样数据
    CmdGetZeroCheckData         = 2065,   //取零点核查数据
    CmdGetRangeCheckData        = 2066,   //取跨度核查数据

    CmdRemoteStoreSample        = 3015,   //手动远程留样
    CmdGetSystemInformation     = 3020,   //提取数采仪/监测仪信息
    CmdSetSystemInformation     = 3021,   //设置数采仪/监测仪信息(参数)

    CmdSetSystemRunMode         = 3042,   //切换系统运行模式
    CmdRebootSystem             = 3043,   //远程重启数采仪
    CmdStartMeasureSample       = 3044,   //启动单次水样测试
    CmdSetSystemToStoppedStatus = 3045,   //远程控制系统紧急停机
    CmdSetSystemToIdleStatus    = 3046,   //远程控制系统进入待机状态
    CmdSetSystemMeasureInterval = 3056,   //设置系统运行测量时间间隔

    CmdStartMeasureCheck        = 3080,   //启动单台仪表标液核查
    CmdStartMeasureAddstandard  = 3081,   //启动单台仪表加标回收
    CmdStartMeasureParallel     = 3082,   //启动单台仪表平行样测试
    CmdStartMeasureZeroCheck    = 3083,   //启动单台仪表零点核查
    CmdStartMeasureRangeCheck   = 3084,   //启动单台仪表跨度核查
    CmdStartBlankCalibrate      = 3085,   //启动空白校准(定标?)
    CmdStartStandardCalibrate   = 3086,   //启动标样校准(定标?)
    CmdStopInstrumentFlow       = 3100,   //停止仪器当前动作

    UploadResetTime             = 2081,   //上传数采仪时间
    UploadRtdData               = 2011,   //上传监测指标实时数据
    UploadHourData              = 2061,   //上传地表水小时历史数据
    UploadStandardCheckData     = 2062,   //上传地表水核查数据
    UploadAddStandardData       = 2063,   //上传地表水加标回收数据
    UploadParallelData          = 2064,   //上传地表水平行样数据
    UploadZeroCheckData         = 2065,   //上传零点核查数据
    UploadSpanCheckData         = 2066,   //上传跨度核查数据
    
    UploadSystemLog             = 3020,   //上传数采仪信息(日志)
    UploadInfoLog               = 3020,   //上传监测仪信息(日志)
    UploadInfoStatus            = 3020,   //上传监测仪信息(状态)
    UploadInfoParam             = 3020,   //上传监测仪信息(参数)

    UploadHeartPack             = 9015,   //上传心跳包
};




}

#endif


