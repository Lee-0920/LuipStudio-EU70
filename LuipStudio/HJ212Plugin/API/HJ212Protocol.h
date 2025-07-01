/**
 * @brief HJ212协议基类
 * @author huangyue
 * @date 2020/11/11
 */

#ifndef HJ212PROTOCOL_H
#define HJ212PROTOCOL_H

#include <QDateTime>
#include <QList>
#include <QMap>

#include "../HJ212Data.h"
#include "../HJ212TcpConnection.h"
#include "HJ212Packet.h"

namespace HJ212
{

/**
 * @brief 服务请求的监测因子信息
 */
struct CmdPackPolIdInfo
{
    QDateTime beginTime;         //开始时间
    QDateTime endTime;           //结束时间

    QMap<QString, QMap<QString, QString>> polIdInfos;  //<监测因子 - <信息 - 值>> (信息编码 - 信息值,如 “i13004 - 16.8” 消解温度为16.8)
};


class HJ212Platform;
class HJ212Protocol
{
public:
    HJ212Protocol(ProtocolVersion version, FlagProtocol flagProtocol, PlatformInfo* info, HJ212CallBack callback, HJ212Platform* platform);
    virtual ~HJ212Protocol();

    //设置默认参数
    void SetDefaultPara(int st,
                        bool toGB2312 = false,      //将日志转为GB2312
                        bool noFlag = false,        //默认不带Flag字段，2005才需要
                        bool qnInCP = false,        //默认QN号放在CP字段，2005才需要
                        bool isOldPolId = false);   //是否为旧的PolId，2005才需要

    bool IsNeedRespond(UploadDataType type);        //数据是否需要回应


    virtual bool PacketFormatCheck(HJ212PackPtr pack, QnRtn& error);                //检查包

public:
    virtual void OnExecute(HJ212PackPtr pack) = 0;                                              // 命令包响应
    virtual HJ212PackPtr UploadHeartPack() = 0;                                                 // 上传心跳包
    virtual QList<HJ212PackPtr> UploadRtdData(QList<RealTimeData> datas) = 0;                   // 上传实时数据
    virtual QList<HJ212PackPtr> UploadHourData(QList<TimeSliceData> datas) = 0;                 // 上传小时数据
    virtual QList<HJ212PackPtr> UploadStandardCheckData(QList<StandardCheckData> datas) = 0;    // 上传核查数据
    virtual QList<HJ212PackPtr> UploadAddStandardData(QList<AddStandardData> datas) = 0;        // 上传加标回收数据
    virtual QList<HJ212PackPtr> UploadParallelData(QList<ParallelData> datas) = 0;              // 上传平行样数据
    virtual QList<HJ212PackPtr> UploadZeroCheckData(QList<ZeroCheckData> datas) = 0;            // 上传零点核查数据
    virtual QList<HJ212PackPtr> UploadSpanCheckData(QList<SpanCheckData> datas) = 0;            // 上传跨度核查数据

    virtual QList<HJ212PackPtr> UploadSystemInfoLog(QList<EquipmentLog> datas) = 0;             // 上传数采仪信息(日志)
    virtual QList<HJ212PackPtr> UploadSystemInfoStatus(QList<EquipmentStatus> datas) = 0;       // 上传监测仪信息(状态)
    virtual QList<HJ212PackPtr> UploadSystemInfoParam(QList<EquipmentParam> datas) = 0;         // 上传监测仪信息(参数)


public://工具

    //应答相关接口
    HJ212PackPtr ReplyRequest(std::string qn, int code, bool isRespon = false, bool isSubpack = false);
    HJ212PackPtr ReplyExecuteResult(std::string qn, int code);
    HJ212PackPtr ReplyNotice(std::string qn);
    HJ212PackPtr ReplySystemBusy(std::string qn);

    //获取上传数据包 | 获取命令提取的上传包
    QList<HJ212PackPtr> UploadData(QMap<std::string, QList<std::string>> datas, UploadDataType type, std::string cn);
    QList<HJ212PackPtr> CmdUploadData(QMap<std::string, QList<std::string>> datas, UploadDataType type, std::string cn, std::string qn);

    //创建新包
    HJ212PackPtr CreatNewPack(int st, std::string cn);
    HJ212PackPtr CreatNewPack(int st, std::string qn, std::string cn, int flag, bool qnInCP, std::string needRespCN = "-1");

    //PolID新旧转换
    std::string PolId2017To2005(std::string Id2017);
    std::string PolId2005To2017(std::string Id2005);

    //获取当前的QN号
    std::string GetCurrentQN();

    //获取命令请求的时间和PolId数组
    CmdPackPolIdInfo GetCmdPackPolIdInfo(HJ212PackPtr pack, UploadDataType type);

    //按PolId分类
    QMap<QString, QList<EquipmentLog>> ClassifyEquipmentInfo(const QList<EquipmentLog>& list);
    QMap<QString, QList<EquipmentStatus>> ClassifyEquipmentInfo(const QList<EquipmentStatus>& list);
    QMap<QString, QList<EquipmentParam>> ClassifyEquipmentInfo(const QList<EquipmentParam>& list);

    //获取数据小数位
    int  GetDecimal(QString polId);

public:
    FlagProtocol m_flagProtocol;        //Flag字段的协议版本
    ProtocolVersion m_protocolVersion;  //自己定义的协议版本（好像可以直接使用m_flagProtocol）
    PlatformInfo* m_platformInfo;       //平台信息
    HJ212CallBack m_callBack;           //回调接口
    HJ212Platform* m_platform;
    QString m_polId;                    //监测因子


    bool m_defaultNoFlag;               //默认无flag字段（2005的一些通讯包不带flag）
    bool m_defaultQnInCP;               //默认QN字段放在CP字段中（2005的一些通讯包会将QN放在CP中作为一个参数）
    bool m_defaultIsOldPolId;           //默认是否为旧的PolId (协议2005的为旧的，需要转为客户端的2017)
    bool m_defaultToGB2312;             //默认将日志转换为GB2312（地表水2017需要用到）
    int m_defaultST;                    //默认ST

};






}

#endif // HJ212PROTOCOL_H
