#include "CalibrateTable.h"
#include "Treasure/System/Logger.h"

namespace DataBaseSpace
{

CalibrateTable::CalibrateTable(const QString name, DBConnectionPoolPtr connectionpool)
    : DataTable(name, connectionpool)
{
    m_fieldList.append(TbField("id", "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT"));
    m_fieldList.append(TbField("calibrateDateTime", "INTEGER  NOT NULL"));
    m_fieldList.append(TbField("curveK", "DOUBLE NOT NULL"));
    m_fieldList.append(TbField("curveB", "DOUBLE NOT NULL"));
    m_fieldList.append(TbField("zeroShowPeakArea", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroConsistency", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroPeakArea1", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroPeakArea2", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroPeakArea3", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroInitRefrigeratorTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroInitNDIRTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroFinalRefrigeratorTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroFinalNDIRTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroInitThermostatTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroInitEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroInitEnvironmentTempDown", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroFinalThermostatTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroFinalEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("zeroFinalEnvironmentTempDown", "FLOAT  NOT NULL"));
    m_fieldList.append(TbField("showPeakArea", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("consistency", "FLOAT NOT NULL"));

    m_fieldList.append(TbField("standardPeakArea1", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("standardPeakArea2", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("standardPeakArea3", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initRefrigeratorTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initNDIRTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalRefrigeratorTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalNDIRTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initThermostatTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("initEnvironmentTempDown", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalThermostatTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalEnvironmentTemp", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("finalEnvironmentTempDown", "FLOAT NOT NULL"));
    m_fieldList.append(TbField("curveR2", "FLOAT  NOT NULL"));
    m_fieldList.append(TbField("calibrateConsumeTime", "INTEGER NOT NULL"));
    m_fieldList.append(TbField("currentRange", "FLOAT NOT NULL"));

    m_fieldList.append(TbField("flag", "INTEGER  NOT NULL"));

    this->CreateTable();
    this->FieldChangedCheck();
}

void CalibrateTable::InsertData(const QList<CalibrateRecord>& dataList)
{
    QMutexLocker locker(&m_queryMutex);
    QList<QString> sqlList;
    if (!dataList.empty())
    {
        for (QList<CalibrateRecord>::const_iterator it = dataList.begin(); it != dataList.end(); it++)
        {
            CalibrateRecord data = *it;

            QString sql = QString("INSERT INTO %1 (calibrateDateTime,curveK,curveB,zeroShowPeakArea,zeroConsistency,zeroPeakArea1,zeroPeakArea2,zeroPeakArea3,zeroInitRefrigeratorTemp,zeroInitNDIRTemp,zeroFinalRefrigeratorTemp,zeroFinalNDIRTemp,zeroInitThermostatTemp,zeroInitEnvironmentTemp,zeroInitEnvironmentTempDown,zeroFinalThermostatTemp,zeroFinalEnvironmentTemp,zeroFinalEnvironmentTempDown,showPeakArea,consistency,standardPeakArea1,standardPeakArea2,standardPeakArea3,initRefrigeratorTemp,initNDIRTemp,finalRefrigeratorTemp,finalNDIRTemp,initThermostatTemp,initEnvironmentTemp,initEnvironmentTempDown,finalThermostatTemp,finalEnvironmentTemp,finalEnvironmentTempDown,curveR2,calibrateConsumeTime,currentRange,flag) "
                                  "VALUES (%2,%3, %4,%5,%6, %7, %8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25,%26,%27,%28,%29,%30,%31,%32,%33,%34,%35,%36,%37,%38)")
                    .arg(m_tableName)
                    .arg(data.calibrateDateTime)
                    .arg(data.curveK)
                    .arg(data.curveB)
                    .arg(data.zeroShowPeakArea)
                    .arg(data.zeroConsistency)
                    .arg(data.zeroPeakArea1)
                    .arg(data.zeroPeakArea2)
                    .arg(data.zeroPeakArea3)
                    .arg(data.zeroInitRefrigeratorTemp)
                    .arg(data.zeroInitNDIRTemp)
                    .arg(data.zeroFinalRefrigeratorTemp)
                    .arg(data.zeroFinalNDIRTemp)
                    .arg(data.zeroInitThermostatTemp)
                    .arg(data.zeroInitEnvironmentTemp)
                    .arg(data.zeroInitEnvironmentTempDown)
                    .arg(data.zeroFinalThermostatTemp)
                    .arg(data.zeroFinalEnvironmentTemp)
                    .arg(data.zeroFinalEnvironmentTempDown)
                    .arg(data.showPeakArea)
                    .arg(data.consistency)
                    .arg(data.standardPeakArea1)
                    .arg(data.standardPeakArea2)
                    .arg(data.standardPeakArea3)
                    .arg(data.initRefrigeratorTemp)
                    .arg(data.initNDIRTemp)
                    .arg(data.finalRefrigeratorTemp)
                    .arg(data.finalNDIRTemp)
                    .arg(data.initThermostatTemp)
                    .arg(data.initEnvironmentTemp)
                    .arg(data.initEnvironmentTempDown)
                    .arg(data.finalThermostatTemp)
                    .arg(data.finalEnvironmentTemp)
                    .arg(data.finalEnvironmentTempDown)
                    .arg(data.curveR2)
                    .arg(data.calibrateConsumeTime)
                    .arg(data.currentRange)
                    .arg(data.flag);
            trLogger->debug("insert: %s", sql.toStdString().c_str());
            sqlList.append(sql);
        }

        Insert(sqlList);
   }
}


}
