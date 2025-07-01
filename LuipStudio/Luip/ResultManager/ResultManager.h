#if !defined(RESULTMANAGER_RESULTMANAGER_H)
#define RESULTMANAGER_RESULTMANAGER_H

#include "System/Types.h"
#include "RecordFields.h"
#include <memory>
#include <list>
#include "IResultNotifiable.h"
#include "UI/Frame/LoginDialog.h"
#include "MeasureRecordFile.h"
#include "CalibrateRecordFile.h"
#include "OperateRecordData.h"
#include <QObject>
#include <QMetaType>
#include <QTimer>

using namespace UI;

Q_DECLARE_METATYPE(ResultData::RecordData)

namespace Result
{

/**
 * @brief 测量类型。
 * @details
 */
enum class MeasureType
{
    Blank = 0,           // 空白水
    Sample = 1,          // 水样
    Standard = 2,        // 标液
    Check = 3,           // 核查样
    Addstandard = 4,     // 加标样
    Parallel = 5,        // 平行样
    ZeroCheck = 6,       // 零点核查
    RangeCheck = 7,      // 量程核查
    ExtAddstandard = 8,  // 外部加标
    ExtParallel = 9,     // 外部平行
    QualityHandle = 10,     //质控

    None = 255,         // 空字符
};
/**
 * @brief 结果管理器
 */
class ResultManager : public QObject
{
    Q_OBJECT
public:
    ResultManager();
    ~ResultManager();
    static ResultManager* Instance();
	void Register(IResultNotifiable *handle);
    void Init(void);

    void AddCalibrateRecord(System::String name, ResultData::RecordData result);
    void AddMeasureRecord(System::String name, ResultData::RecordData result, bool isUpload);
    void AddAuditTrailRecord(System::String info);

    bool IsExistMeasureRecord(System::String name, ResultData::RecordData record);
    bool IsExistCalibrateRecord(System::String name, ResultData::RecordData record);

    void DisplayResultConfigInit();
    QString DisplayUsefulResult(float result);
    QString DisplayResult(float result, QString unitStr);

    MeasureRecordFile* GetMeasureRecordFile(System::String name);
    MeasureRecordFile* GetWaveUseMeasureRecordFile(System::String name);
    MeasureRecordFile* GetResultDetailUseMeasureRecordFile(System::String name);

    CalibrateRecordFile* GetCalibrateRecordFile(System::String name);
    CalibrateRecordFile* GetResultDetailUseCalibrateRecordFile(System::String name);

    System::Uint16 GetMeasureRecordDataSize(System::String name);
    System::Uint16 GetCalibrateRecordDataSize(System::String name);

    OperateRecordData *GetMeasureOperateRecordData(System::String name);
    OperateRecordData *GetCalibraOperateRecordData(System::String name);

    void ClearBackupMeasureRecordFile(System::String dstName);
    void ClearBackupCalibrateRecordFile(System::String dstName);

    void RemoveBackupMeasureRecordFile();
    void RemoveBackupCalibrateRecordFile();
public slots:
    void AddCalibrateRecordSlots(System::String name, ResultData::RecordData result);
    void BackupFile();

private:
    std::list<IResultNotifiable*> m_notifise;
    static std::unique_ptr<ResultManager> m_instance;

    std::map<System::String, MeasureRecordFile*> m_measureFiles;
    std::map<System::String, MeasureRecordFile*> m_waveUseMeasureFiles;
    std::map<System::String, MeasureRecordFile*> m_resultDetailUseMeasureFiles;

    std::map<System::String, CalibrateRecordFile*> m_calibraFiles;
    std::map<System::String, CalibrateRecordFile*> m_resultDetailUsecalibraFiles;
    std::map<System::String, OperateRecordData*> m_measureOperateRecordData;
    std::map<System::String, OperateRecordData*> m_calibraOperateRecordData;

    QTimer *m_timer;

    bool BackupMeasureRecordFile(System::String sourcePath, String exportFileName, String formatTableName);
    bool BackupCalibrateRecordFile(System::String sourcePath, String exportFileName, String formatTableName);
    bool CheckMeasureRecordBackupFile(String path, String exportFileName, String formatTableName);
    bool CheckCalibrateRecordBackupFile(String path, String exportFileName, String formatTableName);
    MeasureRecordFile* RestoreMeasureRecordBackup(String path, MeasureRecordFile* measureRecordFile, String exportFileName, String formatTableName);
    CalibrateRecordFile*  RestoreCalibrateRecordBackup(String path, CalibrateRecordFile* calibrateRecordFile, String exportFileName, String formatTableName);

    bool AddMeasureRecordToSqlite(ResultData::RecordData& result);

    bool AddCalibrateRecordToSqlite(ResultData::RecordData& result);

    bool AddAuditTrailRecordToSqlite(System::String info);

    unsigned int m_decimalNum;
    unsigned int m_settingUsenum;

signals:
    void AddMeasureRecordSignals(System::String name, ResultData::RecordData result, bool isUpload);
    void AddCalibrateRecordSignals(System::String name, ResultData::RecordData result);
    void AddAuditTrailRecordSignals(System::String info);
private slots:
    void AddMeasureRecordSlots(System::String name, ResultData::RecordData result, bool isUpload);
    void AddAuditTrailRecordSlots(System::String info);
};

}
#endif // RESULTMANAGER_RESULTMANAGER_H
