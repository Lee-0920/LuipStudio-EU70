#include "ResultManager.h"
#include "Setting/Environment.h"
#include "System/CopyFile.h"
#include "LuaEngine/LuaEngine.h"
#include <QTextStream>
#include <QTextCodec>
#include <QDir>
#include "Log.h"
#include <QDateTime>
#include "System/IO/Exception.h"
#include <QThread>
#include <QDebug>

using namespace Configuration;
using namespace System;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;

namespace Result
{

unique_ptr<ResultManager> ResultManager::m_instance(nullptr);

ResultManager::ResultManager(): m_decimalNum(4), m_settingUsenum(4)
{
    qRegisterMetaType<ResultData::RecordData>("ResultData::RecordData");
    QObject::connect(this, SIGNAL(AddMeasureRecordSignals(System::String, ResultData::RecordData, bool)), this,
            SLOT(AddMeasureRecordSlots(System::String, ResultData::RecordData, bool)), Qt::BlockingQueuedConnection);
    QObject::connect(this, SIGNAL(AddCalibrateRecordSignals(System::String, ResultData::RecordData)), this,
            SLOT(AddCalibrateRecordSlots(System::String, ResultData::RecordData)), Qt::BlockingQueuedConnection);
}

ResultManager::~ResultManager()
{
    if (!m_measureFiles.empty())
    {
        for (std::map<System::String, MeasureRecordFile*>::iterator iter = m_measureFiles.begin(); iter != m_measureFiles.end(); ++iter)
        {
            MeasureRecordFile* measureRecordFile = iter->second;
            if (measureRecordFile != nullptr)
            {
                delete measureRecordFile;
            }
        }
    }
    if (!m_waveUseMeasureFiles.empty())
    {
        for (std::map<System::String, MeasureRecordFile*>::iterator iter = m_waveUseMeasureFiles.begin(); iter != m_waveUseMeasureFiles.end(); ++iter)
        {
            MeasureRecordFile* measureRecordFile = iter->second;
            if (measureRecordFile != nullptr)
            {
                delete measureRecordFile;
            }
        }
    }
    if (!m_resultDetailUseMeasureFiles.empty())
    {
        for (std::map<System::String, MeasureRecordFile*>::iterator iter = m_resultDetailUseMeasureFiles.begin(); iter != m_resultDetailUseMeasureFiles.end(); ++iter)
        {
            MeasureRecordFile* measureRecordFile = iter->second;
            if (measureRecordFile != nullptr)
            {
                delete measureRecordFile;
            }
        }
    }
    if (!m_calibraFiles.empty())
    {
        for (std::map<System::String, CalibrateRecordFile*>::iterator iter = m_calibraFiles.begin(); iter != m_calibraFiles.end(); ++iter)
        {
            CalibrateRecordFile* calibrateRecordFile = iter->second;
            if (calibrateRecordFile != nullptr)
            {
                delete calibrateRecordFile;
            }
        }
    }
    if (!m_resultDetailUsecalibraFiles.empty())
    {
        for (std::map<System::String, CalibrateRecordFile*>::iterator iter = m_resultDetailUsecalibraFiles.begin(); iter != m_resultDetailUsecalibraFiles.end(); ++iter)
        {
            CalibrateRecordFile* calibrateRecordFile = iter->second;
            if (calibrateRecordFile != nullptr)
            {
                delete calibrateRecordFile;
            }
        }
    }
    if (!m_measureOperateRecordData.empty())
    {
        for (std::map<System::String, OperateRecordData*>::iterator iter = m_measureOperateRecordData.begin(); iter != m_measureOperateRecordData.end(); ++iter)
        {
            OperateRecordData* operateRecordData = iter->second;
            if (operateRecordData != nullptr)
            {
                delete operateRecordData;
            }
        }
    }
    if (!m_calibraOperateRecordData.empty())
    {
        for (std::map<System::String, OperateRecordData*>::iterator iter = m_calibraOperateRecordData.begin(); iter != m_calibraOperateRecordData.end(); ++iter)
        {
            OperateRecordData* operateRecordData = iter->second;
            if (operateRecordData != nullptr)
            {
                delete operateRecordData;
            }
        }
    }
}

ResultManager* ResultManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new ResultManager);
    }

    return m_instance.get();
}
/**
 * @brief 注册IResultNotifiable 对象。
 */
void ResultManager::Register(IResultNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void ResultManager::Init(void)
{
    Table calibrateRecordFileTable, measureRecordFileTable, resultFileInfoTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.resultFileInfo", resultFileInfoTable);

    resultFileInfoTable.at("measureRecordFile", measureRecordFileTable);
    resultFileInfoTable.at("calibrateRecordFile", calibrateRecordFileTable);

    oolua_ipairs(measureRecordFileTable)
    {
        String name;
        String fileName;
        String exportFileName;
        String formatTableName;
        String path;

        Table table;
        lua->pull(table);

        table.at("name", name);
        table.at("fileName", fileName);
        table.at("exportFileName", exportFileName);
        table.at("formatTableName", formatTableName);

        path = Environment::Instance()->GetAppDataPath() + "/" + fileName;
        qDebug()<<"ResultFilePath:"<<QString::fromStdString(path);
        MeasureRecordFile* measureRecordFile = new MeasureRecordFile(path, exportFileName, formatTableName);
        try
        {
            measureRecordFile->Load();
        }
        catch(System::IO::LuipFileException e)
        {
            (void)e;
            logger->warn("测量数据文件 %s 加载失败", path.c_str());

            QFileInfo fileInfo(QString::fromStdString(path));
            QString newName = fileInfo.baseName() + "_" + QDateTime::currentDateTime().toString("yyyyhhMMddmmss") + "." + fileInfo.suffix();
            QFile::copy(QString::fromStdString(path), newName);
            logger->warn("错误文件已备份 ==> %s", newName.toStdString().c_str());

            if (CheckMeasureRecordBackupFile(path, exportFileName,formatTableName))
            {
                measureRecordFile = RestoreMeasureRecordBackup(path, measureRecordFile, exportFileName, formatTableName);
            }
            else
            {
                logger->warn("%s 无有效备份，清空文件内容", path.c_str());
                measureRecordFile->ClearRecord();
            }
        }

        if (measureRecordFile->IsMigrateData())
        {
            MeasureRecordFile* originalFile = new MeasureRecordFile(*measureRecordFile);
            measureRecordFile->MigrateData(*originalFile);
            if (originalFile != nullptr)
            {
                delete originalFile;
            }
        }
        BackupMeasureRecordFile(path, exportFileName, formatTableName);
        m_measureFiles.insert(make_pair(name, measureRecordFile));

        MeasureRecordFile* waveUseMeasureFiles = new MeasureRecordFile(*measureRecordFile);
        waveUseMeasureFiles->OpenFile();
        m_waveUseMeasureFiles.insert(make_pair(name, waveUseMeasureFiles));

        MeasureRecordFile* resultDetailUseMeasureFiles = new MeasureRecordFile(*measureRecordFile);
        resultDetailUseMeasureFiles->OpenFile();
        m_resultDetailUseMeasureFiles.insert(make_pair(name, resultDetailUseMeasureFiles));

        OperateRecordData* operateRecordData = new OperateRecordData(*(measureRecordFile->GetRecordFields()));
        m_measureOperateRecordData.insert(make_pair(name, operateRecordData));
    }
    oolua_ipairs_end()


    oolua_ipairs(calibrateRecordFileTable)
    {
        String name;
        String fileName;
        String exportFileName;
        String formatTableName;
        String path;

        Table table;
        lua->pull(table);

        table.at("name", name);
        table.at("fileName", fileName);
        table.at("exportFileName", exportFileName);
        table.at("formatTableName", formatTableName);

        path = Environment::Instance()->GetAppDataPath() + "/" + fileName;
        CalibrateRecordFile* calibrateRecordFile = new CalibrateRecordFile(path, exportFileName, formatTableName);
        try
        {
            calibrateRecordFile->Load();
        }
        catch(System::IO::LuipFileException e)
        {
            (void)e;
            logger->warn("定标数据文件 %s 加载失败", path.c_str());
            if (CheckCalibrateRecordBackupFile(path, exportFileName,formatTableName))
            {
                calibrateRecordFile = RestoreCalibrateRecordBackup(path, calibrateRecordFile, exportFileName, formatTableName);
            }
            else
            {
                logger->warn("%s 无有效备份，清空文件内容", path.c_str());
                calibrateRecordFile->ClearRecord();
            }
        }
        if (calibrateRecordFile->IsMigrateData())
        {
            CalibrateRecordFile* originalFile = new CalibrateRecordFile(*calibrateRecordFile);
            calibrateRecordFile->MigrateData(*originalFile);
            if (originalFile != nullptr)
            {
                delete originalFile;
            }
        }

//        CalibrateRecordFile* usefulFile = new CalibrateRecordFile(*calibrateRecordFile);
//        calibrateRecordFile->CleanInvalidData(*usefulFile);
//        if (usefulFile != nullptr)
//        {
//            delete usefulFile;
//        }

        BackupCalibrateRecordFile(path, exportFileName, formatTableName);
        m_calibraFiles.insert(make_pair(name, calibrateRecordFile));

        CalibrateRecordFile* resultDetailUseCalibraFiles= new CalibrateRecordFile(*calibrateRecordFile);
        resultDetailUseCalibraFiles->OpenFile();
        m_resultDetailUsecalibraFiles.insert(make_pair(name, resultDetailUseCalibraFiles));

        OperateRecordData* operateRecordData = new OperateRecordData(*(calibrateRecordFile->GetRecordFields()));
        m_calibraOperateRecordData.insert(make_pair(name, operateRecordData));
    }
    oolua_ipairs_end()

//    m_timer = new QTimer();
//    connect(m_timer, SIGNAL(timeout()), this, SLOT(BackupFile()));
//    m_timer->start(86400000);

    DisplayResultConfigInit();
}

void ResultManager::BackupFile()
{
//    if (!m_measureFiles.empty())
//    {
//        for (std::map<System::String, MeasureRecordFile*>::iterator iter = m_measureFiles.begin(); iter != m_measureFiles.end(); ++iter)
//        {
//            MeasureRecordFile* measureRecordFile = iter->second;
//            if (measureRecordFile != nullptr)
//            {
//                BackupMeasureRecordFile(measureRecordFile->GetPath(), measureRecordFile->GetExportFileName(), measureRecordFile->GetFormatTableName());
//            }
//        }
//    }
//    if (!m_calibraFiles.empty())
//    {
//        for (std::map<System::String, CalibrateRecordFile*>::iterator iter = m_calibraFiles.begin(); iter != m_calibraFiles.end(); ++iter)
//        {
//            CalibrateRecordFile* calibrateRecordFile = iter->second;
//            if (calibrateRecordFile != nullptr)
//            {
//                BackupCalibrateRecordFile(calibrateRecordFile->GetPath(), calibrateRecordFile->GetExportFileName(), calibrateRecordFile->GetFormatTableName());
//            }
//        }
//    }
}

bool ResultManager::IsExistMeasureRecord(System::String name, ResultData::RecordData record)
{
    if (m_measureFiles.count(name))
    {
        MeasureRecordFile* measureRecordFile = m_measureFiles[name];

        return measureRecordFile->IsExistRecord(record);
    }
    else
    {
        return false;
    }
}

bool ResultManager::IsExistCalibrateRecord(System::String name, ResultData::RecordData record)
{
    if (m_calibraFiles.count(name))
    {
        CalibrateRecordFile* calibrateRecordFile= m_calibraFiles[name];

        return calibrateRecordFile->IsExistRecord(record);
    }
    else
    {
        return false;
    }
}

void ResultManager::AddMeasureRecord(String name, RecordData result, bool isUpload)
{
    emit AddMeasureRecordSignals(name, result, isUpload);
}

void ResultManager::AddMeasureRecordSlots(String name, RecordData result, bool isUpload)
{
    if (m_measureFiles.count(name))
    {
        MeasureRecordFile* measureRecordFile= m_measureFiles[name];
        measureRecordFile->AddRecord(result);
    }
    if (isUpload)
    {
        ///Interface::Wqimc::WqimcManager::Instance()->uploadMeasureData(name, result);
    }
    for(std::list<IResultNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnMeasureResultAdded(name, result);
        }
    }
}

void ResultManager::AddCalibrateRecord(String name, RecordData result)
{
    emit AddCalibrateRecordSignals(name, result);
}

void ResultManager::AddCalibrateRecordSlots(String name, RecordData result)
{
    if (m_calibraFiles.count(name))
    {
        CalibrateRecordFile* calibrateRecordFile= m_calibraFiles[name];
        calibrateRecordFile->AddRecord(result);
    }
    ///Interface::Wqimc::WqimcManager::Instance()->uploadMeasureCurve(name, result);
    ///UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::NewCalibrateCurve);
	for(std::list<IResultNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnCalibrateResultAdded(name, result);
        }
    }
}

void ResultManager::DisplayResultConfigInit()
{
//    LuaEngine* luaEngine = LuaEngine::Instance();
//    lua_State* state = luaEngine->GetThreadState();

//    luaEngine->GetLuaValue(state, "setting.measureResult.useNum", m_settingUsenum);

//    luaEngine->GetLuaValue(state, "setting.measureResult.decimalNum", m_decimalNum);

//    unsigned int unitIndex;
//    Table sysTable;
//    luaEngine->GetLuaValue(state, "config.system", sysTable);
//    if(sysTable.safe_at("unitIndex", unitIndex))
//    {
//        String getUnitStr = QString("setting.unit["+QString::number(unitIndex + 1)+"]").toStdString();
//        Table unitTable;
//        luaEngine->GetLuaValue(state, getUnitStr, unitTable);

//        float multiple;
//        unsigned int shiftNum;
//        if(unitTable.safe_at("multiple", multiple))
//        {
//            shiftNum = log10(multiple);
//            if (m_decimalNum >= shiftNum)
//            {
//                m_decimalNum = m_decimalNum - shiftNum;
//            }
//            else
//            {
//                m_decimalNum = 0;
//            }
//        }
//    }
}

QString ResultManager::DisplayUsefulResult(float result)
{
    QString strConsistency;

    int useNumber;
    if(result >= 100)
    {
        useNumber = m_settingUsenum - 3;
    }
    else if(result >= 10 && result < 100)
    {
        useNumber = m_settingUsenum - 2;
    }
    else if(result >= 1 && result < 10)
    {
        useNumber = m_settingUsenum - 1;
    }
    else
    {
        useNumber = m_settingUsenum;
    }

    QString strDecimal = "%." + QString("%1").arg(useNumber) + "f";
    QByteArray ba = strDecimal.toLatin1();
    const char *tempd = ba.data();
    strConsistency.sprintf(tempd,result);

    return strConsistency;
}

QString ResultManager::DisplayResult(float result)
{
    QString strConsistency;

    QString strDecimal = "%." +
                         QString("%1").arg(m_decimalNum) +
                         "f";
    QByteArray ba = strDecimal.toLatin1();
    const char *tempd = ba.data();
    strConsistency.sprintf(tempd,result);

    return strConsistency;
}

MeasureRecordFile* ResultManager::GetMeasureRecordFile(String name)
{
    if (m_measureFiles.count(name))
    {
        return m_measureFiles[name];
    }
    else
    {
        return nullptr;
    }
}

MeasureRecordFile* ResultManager::GetWaveUseMeasureRecordFile(String name)
{
    if (m_waveUseMeasureFiles.count(name))
    {
        return m_waveUseMeasureFiles[name];
    }
    else
    {
        return nullptr;
    }
}

MeasureRecordFile* ResultManager::GetResultDetailUseMeasureRecordFile(String name)
{
    if (m_resultDetailUseMeasureFiles.count(name))
    {
        return m_resultDetailUseMeasureFiles[name];
    }
    else
    {
        return nullptr;
    }
}

CalibrateRecordFile* ResultManager::GetCalibrateRecordFile(String name)
{
    if (m_calibraFiles.count(name))
    {
        return m_calibraFiles[name];
    }
    else
    {
        return nullptr;
    }
}

CalibrateRecordFile* ResultManager::GetResultDetailUseCalibrateRecordFile(String name)
{
    if (m_resultDetailUsecalibraFiles.count(name))
    {
        return m_resultDetailUsecalibraFiles[name];
    }
    else
    {
        return nullptr;
    }
}

Uint16 ResultManager::GetMeasureRecordDataSize(String name)
{
    if (m_measureFiles.count(name))
    {
        MeasureRecordFile* measureRecordFile = m_measureFiles[name];
        return measureRecordFile->GetRecordFields()->GetFieldsSize();
    }
    else
    {
        return 0;
    }
}

Uint16 ResultManager::GetCalibrateRecordDataSize(String name)
{
    if (m_calibraFiles.count(name))
    {
        CalibrateRecordFile* calibrateRecordFile = m_calibraFiles[name];
        return calibrateRecordFile->GetRecordFields()->GetFieldsSize();
    }
    else
    {
        return 0;
    }
}

OperateRecordData* ResultManager::GetMeasureOperateRecordData(System::String name)
{
    if (m_measureOperateRecordData.count(name))
    {
        return m_measureOperateRecordData[name];
    }
    else
    {
        return nullptr;
    }
}

OperateRecordData* ResultManager::GetCalibraOperateRecordData(System::String name)
{
    if (m_calibraOperateRecordData.count(name))
    {
        return m_calibraOperateRecordData[name];
    }
    else
    {
        return nullptr;
    }
}

bool ResultManager::BackupMeasureRecordFile(System::String sourcePath, String exportFileName, String formatTableName)
{
    Bool ret = false;
    QFileInfo sourceFileInfo(sourcePath.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString cache(str + "Cache." + sourceFileInfo.suffix());
    QString backup(str + "Backup." + sourceFileInfo.suffix());

    CopyFileAction::CopyFileToPath(sourcePath,  cache.toStdString(), true);
    if (QFile::exists(cache))
    {
        MeasureRecordFile measureRecordFile = MeasureRecordFile(cache.toStdString(), exportFileName, formatTableName);
        try
        {
            ret = measureRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)//抛出异常，可能是不完整，类型错误
        {
            (void)e;
        }
    }
    if (ret)
    {
        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }
        QFile::rename(cache, backup);
    }
    return true;
}

bool ResultManager::BackupCalibrateRecordFile(System::String sourcePath, String exportFileName, String formatTableName)
{
    Bool ret = false;
    QFileInfo sourceFileInfo(sourcePath.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString cache(str + "Cache." + sourceFileInfo.suffix());
    QString backup(str + "Backup." + sourceFileInfo.suffix());

    CopyFileAction::CopyFileToPath(sourcePath,  cache.toStdString(), true);
    if (QFile::exists(cache))
    {
        CalibrateRecordFile calibrateRecordFile = CalibrateRecordFile(cache.toStdString(), exportFileName, formatTableName);
        try
        {
            ret = calibrateRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)//抛出异常，可能是不完整，类型错误
        {
            (void)e;
        }
    }
    if (ret)
    {
        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }
        QFile::rename(cache, backup);
    }
    return true;
}

bool ResultManager::CheckMeasureRecordBackupFile(String path, String exportFileName, String formatTableName)
{
    bool ret = false;
    QFileInfo sourceFileInfo(path.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString cache(str + "Cache." + sourceFileInfo.suffix());
    QString backup(str + "Backup." + sourceFileInfo.suffix());
    bool isBackupValid = false;
    bool isCacheValid = false;

    if (QFile::exists(backup))
    {
        MeasureRecordFile measureRecordFile = MeasureRecordFile(backup.toStdString(), exportFileName, formatTableName);
        try
        {
            isBackupValid = measureRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)//抛出异常，可能是不完整，类型错误
        {
            (void)e;
        }
    }

    if (QFile::exists(cache))
    {
        MeasureRecordFile measureRecordFile = MeasureRecordFile(cache.toStdString(), exportFileName, formatTableName);
        try
        {
            isCacheValid = measureRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)
        {
            (void)e;
        }
    }

    if (true == isCacheValid)//缓存文件有效，使用缓存文件替代备份文件，因为缓存文件是最新的
    {
        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }
        QFile::rename(cache, backup);
        ret = true;
    }
    else
    {
        if (QFile::exists(cache))//缓存文件无效删除缓存
        {
             QFile::remove(cache);
        }
        if (false == isBackupValid)//备份文件无效删除备份
        {
            if (QFile::exists(backup))
            {
                 QFile::remove(backup);
            }
        }
        else
        {
            ret = true;
        }
    }
    return ret;
}

bool ResultManager::CheckCalibrateRecordBackupFile(String path, String exportFileName, String formatTableName)
{
    bool ret = false;
    QFileInfo sourceFileInfo(path.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString cache(str + "Cache." + sourceFileInfo.suffix());
    QString backup(str + "Backup." + sourceFileInfo.suffix());
    bool isBackupValid = false;
    bool isCacheValid = false;

    if (QFile::exists(backup))
    {
        CalibrateRecordFile calibrateRecordFile = CalibrateRecordFile(backup.toStdString(), exportFileName, formatTableName);
        try
        {
            isBackupValid = calibrateRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)//抛出异常，可能是不完整，类型错误
        {
            (void)e;
        }
    }

    if (QFile::exists(cache))
    {
        CalibrateRecordFile calibrateRecordFile = CalibrateRecordFile(cache.toStdString(), exportFileName, formatTableName);
        try
        {
            isCacheValid = calibrateRecordFile.Load();
        }
        catch(System::IO::LuipFileException e)
        {
            (void)e;
        }
    }

    if (true == isCacheValid)//缓存文件有效，使用缓存文件替代备份文件，因为缓存文件是最新的
    {
        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }
        QFile::rename(cache, backup);
        ret = true;
    }
    else
    {
        if (QFile::exists(cache))//缓存文件无效删除缓存
        {
             QFile::remove(cache);
        }
        if (false == isBackupValid)//备份文件无效删除备份
        {
            if (QFile::exists(backup))
            {
                 QFile::remove(backup);
            }
        }
        else
        {
            ret = true;
        }
    }
    return ret;
}

MeasureRecordFile* ResultManager::RestoreMeasureRecordBackup(String path, MeasureRecordFile* measureRecordFile, String exportFileName, String formatTableName)
{
    QFileInfo sourceFileInfo(path.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString backup(str + "Backup." + sourceFileInfo.suffix());

    if (measureRecordFile != nullptr)
    {
        delete measureRecordFile;
    }
    if (QFile::exists(path.c_str()))
    {
         QFile::remove(path.c_str());
    }
    CopyFileAction::CopyFileToPath(backup.toStdString(),  path, true);
    measureRecordFile = new MeasureRecordFile(path, exportFileName, formatTableName);
    try
    {
        measureRecordFile->Load();
        logger->warn("%s 备份恢复成功", path.c_str());
    }
    catch(System::IO::LuipFileException e)
    {
        (void)e;
        measureRecordFile->ClearRecord();
    }
    return measureRecordFile;
}

CalibrateRecordFile*  ResultManager::RestoreCalibrateRecordBackup(String path, CalibrateRecordFile* calibrateRecordFile, String exportFileName, String formatTableName)
{
    QFileInfo sourceFileInfo(path.c_str());
    QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
    QString backup(str + "Backup." + sourceFileInfo.suffix());

    if (calibrateRecordFile != nullptr)
    {
        delete calibrateRecordFile;
    }
    if (QFile::exists(path.c_str()))
    {
         QFile::remove(path.c_str());
    }
    CopyFileAction::CopyFileToPath(backup.toStdString(),  path, true);
    calibrateRecordFile = new CalibrateRecordFile(path, exportFileName, formatTableName);
    try
    {
        calibrateRecordFile->Load();
        logger->warn("%s 备份恢复成功", path.c_str());
    }
    catch(System::IO::LuipFileException e)
    {
        (void)e;
        calibrateRecordFile->ClearRecord();
    }
    return calibrateRecordFile;
}

void ResultManager::ClearBackupMeasureRecordFile(System::String dstName)
{
    Table measureRecordFileTable, resultFileInfoTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.resultFileInfo", resultFileInfoTable);

    resultFileInfoTable.at("measureRecordFile", measureRecordFileTable);

    oolua_ipairs(measureRecordFileTable)
    {
        String name;
        String fileName;

        Table table;
        lua->pull(table);

        table.at("name", name);
        table.at("fileName", fileName);

        if(name == dstName)
        {
            String path = Environment::Instance()->GetAppDataPath() + "/" + fileName;

            QFileInfo sourceFileInfo(path.c_str());
            QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
            QString cache(str + "Cache." + sourceFileInfo.suffix());
            QString backup(str + "Backup." + sourceFileInfo.suffix());

            if (QFile::exists(backup))
            {
                 QFile::remove(backup);
            }

            if (QFile::exists(cache))
            {
                 QFile::remove(cache);
            }
        }
    }oolua_ipairs_end()
}

void ResultManager::ClearBackupCalibrateRecordFile(System::String dstName)
{
    Table calibrateRecordFileTable, resultFileInfoTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.resultFileInfo", resultFileInfoTable);

    resultFileInfoTable.at("calibrateRecordFile", calibrateRecordFileTable);

    oolua_ipairs(calibrateRecordFileTable)
    {
        String name;
        String fileName;

        Table table;
        lua->pull(table);

        table.at("name", name);
        table.at("fileName", fileName);

        if(name == dstName)
        {
            String path = Environment::Instance()->GetAppDataPath() + "/" + fileName;

            QFileInfo sourceFileInfo(path.c_str());
            QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
            QString cache(str + "Cache." + sourceFileInfo.suffix());
            QString backup(str + "Backup." + sourceFileInfo.suffix());

            if (QFile::exists(backup))
            {
                 QFile::remove(backup);
            }

            if (QFile::exists(cache))
            {
                 QFile::remove(cache);
            }
        }
    }oolua_ipairs_end()
}

void ResultManager::RemoveBackupMeasureRecordFile()
{
    Table measureRecordFileTable, resultFileInfoTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.resultFileInfo", resultFileInfoTable);

    resultFileInfoTable.at("measureRecordFile", measureRecordFileTable);

    oolua_ipairs(measureRecordFileTable)
    {
        String fileName;

        Table table;
        lua->pull(table);

        table.at("fileName", fileName);

        String path = Environment::Instance()->GetAppDataPath() + "/" + fileName;

        QFileInfo sourceFileInfo(path.c_str());
        QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
        QString cache(str + "Cache." + sourceFileInfo.suffix());
        QString backup(str + "Backup." + sourceFileInfo.suffix());

        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }

        if (QFile::exists(cache))
        {
             QFile::remove(cache);
        }
    }oolua_ipairs_end()
}

void ResultManager::RemoveBackupCalibrateRecordFile()
{
    Table calibrateRecordFileTable, resultFileInfoTable;

    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.resultFileInfo", resultFileInfoTable);

    resultFileInfoTable.at("calibrateRecordFile", calibrateRecordFileTable);

    oolua_ipairs(calibrateRecordFileTable)
    {
        String fileName;

        Table table;
        lua->pull(table);

        table.at("fileName", fileName);

        String path = Environment::Instance()->GetAppDataPath() + "/" + fileName;

        QFileInfo sourceFileInfo(path.c_str());
        QString str = sourceFileInfo.path() + "/" + sourceFileInfo.completeBaseName();
        QString cache(str + "Cache." + sourceFileInfo.suffix());
        QString backup(str + "Backup." + sourceFileInfo.suffix());

        if (QFile::exists(backup))
        {
             QFile::remove(backup);
        }

        if (QFile::exists(cache))
        {
             QFile::remove(cache);
        }
    }oolua_ipairs_end()
}


}

