#ifndef DB_AccessLevelTable_H
#define DB_AccessLevelTable_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{

const QVector<QString> LimitsOfAuthority =
{
    "基本信号",
    "峰形图",
    "趋势图",
    "测量排期",
    "测量参数",
    "外联接口",
    "系统参数",
    "维护",
    "组合维护",
    "管道测试",
    "硬件测试",
    "试剂管理",
    "硬件校准",
    "组合操作",
    "升级",
    "耗材管理",
    "通信检测",
    "智能诊断",
    "仪器信息",
    "板卡信息",
    "网络设置",
    "系统时间",
    "系统设置",
    "用户管理",
    "权限管理",
};

enum class Authority {
    BasicSignal,
    PeakDiagram,
    TrendDiagram,
    MeasureScheduler,
    MeasureParam,
    Interconnection,
    SystemParam,
    Maintain,
    MaintainCombine,
    LiquidOperator,
    HardwareTest,
    ReagentManager,
    HardwareParam,
    CombineOperator,
    Update,
    UseResource,
    CommunicationCheck,
    SmartDetect,
    InstrumentInformation,
    DeviceInformation,
    NetSet,
    SystemTime,
    FactoryTime,
    UserManagement,
    AccessLevelManagement,
};

struct AccessLevelRecord {
    int     id = 0;
    QString levelName;
    qint64  limitsOfAuthority = 0;
    qint64  dataTime = 0;
    qint64  lastEditTime = 0;
};


class LUIP_SHARE AccessLevelTable: public DataTable
{

public:
    AccessLevelTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<AccessLevelRecord>& dataList);
    void InsertOrIgnoreData(const AccessLevelRecord& data);
    QList<AccessLevelRecord> SelectData();
    AccessLevelRecord SelectDataFromName(const QString& levelName);
    void DeleteDataFromName(const QString& levelName);
    int GetDataCount();
    qint64 GetNewestDataTime();
    int GetDataCount(qint64 minTime, qint64 maxTime);
    QStringList ConvertLimitsOfAuthority(qint64 limitsOfAuthority);
    QMap<Authority, bool> ConvertLimitsOfAuthorityToMap(qint64 limitsOfAuthority);
    qint64 ConvertLevelListToMap(const QStringList &list);
    QVector<QString> GetLevelNameList();
private:
    QList<AccessLevelRecord> ResolveRecord(QSqlQuery &query);
private:
    QMutex m_queryMutex;
};

typedef std::shared_ptr<AccessLevelTable> AccessLevelTablePtr;
}
#endif
