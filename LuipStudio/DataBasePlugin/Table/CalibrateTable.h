#ifndef DB_CalibrateTable_H
#define DB_CalibrateTable_H

#include <QList>
#include "DataTable.h"
namespace DataBaseSpace
{

struct CalibrateRecord
{
    int     id;
    int     calibrateDateTime; //时间
    double   curveK;
    double   curveB;
    float   zeroShowPeakArea;
    float   zeroConsistency;

    float   zeroPeakArea1;
    float   zeroPeakArea2;
    float   zeroPeakArea3;
    float   zeroInitRefrigeratorTemp;
    float   zeroInitNDIRTemp;
    float   zeroFinalRefrigeratorTemp;
    float   zeroFinalNDIRTemp;
    float   zeroInitThermostatTemp;
    float   zeroInitEnvironmentTemp;
    float   zeroInitEnvironmentTempDown;
    float   zeroFinalThermostatTemp;
    float   zeroFinalEnvironmentTemp;
    float   zeroFinalEnvironmentTempDown;
    float   showPeakArea;
    float   consistency;
    float   standardPeakArea1;
    float   standardPeakArea2;
    float   standardPeakArea3;
    float   initRefrigeratorTemp;
    float   initNDIRTemp;
    float   finalRefrigeratorTemp;
    float   finalNDIRTemp;
    float   initThermostatTemp;
    float   initEnvironmentTemp;
    float   initEnvironmentTempDown;
    float   finalThermostatTemp;
    float   finalEnvironmentTemp;
    float   finalEnvironmentTempDown;
    float   curveR2;
    int     calibrateConsumeTime;
    float   currentRange;

    int     flag;
};

class LUIP_SHARE CalibrateTable: public DataTable
{

public:
    CalibrateTable(const QString name, DBConnectionPoolPtr connectionpool);

    void InsertData(const QList<CalibrateRecord>& dataList);                 //插入数据

private:
    QMutex m_queryMutex;
};

typedef std::shared_ptr<CalibrateTable> CalibrateTablePtr;
}
#endif
