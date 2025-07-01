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
#include "UI/Frame/UpdateWidgetManager.h"
#include <QThread>
#include <QDebug>
#include "Interface/Wqimc/WqimcManager.h"
#include "DataBasePlugin/DataBaseManager.h"
using namespace DataBaseSpace;

using namespace Configuration;
using namespace System;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;

const int MeasureResultLength = 86;//字符串类型不存入Bin文件，但写入数据库
const int CalibrateResultLength = 152;


namespace Result
{

unique_ptr<ResultManager> ResultManager::m_instance(nullptr);

ResultManager::ResultManager(): m_decimalNum(3), m_settingUsenum(3)
{
    qRegisterMetaType<ResultData::RecordData>("ResultData::RecordData");
    QObject::connect(this, SIGNAL(AddMeasureRecordSignals(System::String, ResultData::RecordData, bool)), this,
            SLOT(AddMeasureRecordSlots(System::String, ResultData::RecordData, bool)), Qt::BlockingQueuedConnection);
    QObject::connect(this, SIGNAL(AddCalibrateRecordSignals(System::String, ResultData::RecordData)), this,
            SLOT(AddCalibrateRecordSlots(System::String, ResultData::RecordData)), Qt::BlockingQueuedConnection);
    QObject::connect(this, SIGNAL(AddAuditTrailRecordSignals(System::String)), this,
            SLOT(AddAuditTrailRecordSlots(System::String)));
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
            logger->warn("校准数据文件 %s 加载失败", path.c_str());
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

        CalibrateRecordFile* usefulFile = new CalibrateRecordFile(*calibrateRecordFile);
        calibrateRecordFile->CleanInvalidData(*usefulFile);
        if (usefulFile != nullptr)
        {
            delete usefulFile;
        }

        BackupCalibrateRecordFile(path, exportFileName, formatTableName);
        m_calibraFiles.insert(make_pair(name, calibrateRecordFile));

        CalibrateRecordFile* resultDetailUseCalibraFiles= new CalibrateRecordFile(*calibrateRecordFile);
        resultDetailUseCalibraFiles->OpenFile();
        m_resultDetailUsecalibraFiles.insert(make_pair(name, resultDetailUseCalibraFiles));

        OperateRecordData* operateRecordData = new OperateRecordData(*(calibrateRecordFile->GetRecordFields()));
        m_calibraOperateRecordData.insert(make_pair(name, operateRecordData));
    }
    oolua_ipairs_end()

    m_timer = new QTimer();
    connect(m_timer, SIGNAL(timeout()), this, SLOT(BackupFile()));
    m_timer->start(86400000);

    DisplayResultConfigInit();
}

void ResultManager::BackupFile()
{
    if (!m_measureFiles.empty())
    {
        for (std::map<System::String, MeasureRecordFile*>::iterator iter = m_measureFiles.begin(); iter != m_measureFiles.end(); ++iter)
        {
            MeasureRecordFile* measureRecordFile = iter->second;
            if (measureRecordFile != nullptr)
            {
                BackupMeasureRecordFile(measureRecordFile->GetPath(), measureRecordFile->GetExportFileName(), measureRecordFile->GetFormatTableName());
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
                BackupCalibrateRecordFile(calibrateRecordFile->GetPath(), calibrateRecordFile->GetExportFileName(), calibrateRecordFile->GetFormatTableName());
            }
        }
    }
}

bool ResultManager::IsExistMeasureRecord(System::String name, ResultData::RecordData record)
{
    if (m_measureFiles.count(name))
    {
        MeasureRecordFile* measureRecordFile = m_measureFiles[name];

        return measureRecordFile->IsExistRecord(measureRecordFile->GetSelfReaderIndex(), record);
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

        return calibrateRecordFile->IsExistRecord(calibrateRecordFile->GetSelfReaderIndex(), record);
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
        AddMeasureRecordToSqlite(result);
    }
    if (isUpload)
    {
        Interface::Wqimc::WqimcManager::Instance()->uploadMeasureData(name, result);
    }
    for(std::list<IResultNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnMeasureResultAdded(name, result);
        }
    }
}

bool ResultManager::AddMeasureRecordToSqlite(ResultData::RecordData& result)
{
    unsigned char* ptr = result.GetData();
    int length = result.GetSize();

    if(length != MeasureResultLength) //从lua获取的数据长度要严格对应
    {
        logger->debug("测量结果的长度有误");
        return false;
    }
    MeasureRecord record;
    record.measureDateTime = *reinterpret_cast<int*>(ptr);
    ptr += sizeof(int);
    std::vector<float> floats1;
    for (int i = 0; i < 5; ++i) {
        float value = *reinterpret_cast<float*>(ptr);
        floats1.push_back(value);
        ptr += sizeof(float);
    }
    record.consistency = floats1[0];
    record.consistencyTC = floats1[1];
    record.consistencyIC = floats1[2];
    record.peakTC = floats1[3];
    record.peakIC = floats1[4];

    std::vector<int> chars;
    char value = *reinterpret_cast<char*>(ptr);

    chars.push_back(static_cast<int>(value));
    ptr += sizeof(char);

    record.measureType = chars[0];

    // 读取6个float
    std::vector<float> floats2;
    for (int i = 0; i < 6; ++i) {
        float value = *reinterpret_cast<float*>(ptr);
        floats2.push_back(value);
        ptr += sizeof(float);
    }
    record.initCellTempTC = floats2[0];
    record.initCellTempIC = floats2[1];
    record.finalCellTempTC = floats2[2];
    record.finalCellTempIC = floats2[3];
    record.initEnvironmentTemp = floats2[4];
    record.finalEnvironmentTemp = floats2[5];

    int measureconsumeDateTime = *reinterpret_cast<int*>(ptr);
    ptr += sizeof(int);
    record.measureconsumeDateTime = measureconsumeDateTime;

    float currentRange = *reinterpret_cast<float*>(ptr);
    ptr += sizeof(float);
    record.currentRange = currentRange;

    int meaType = *reinterpret_cast<int*>(ptr);
    ptr += sizeof(int);
    record.meaType = meaType;

    // 读取5个bool
    std::vector<bool> vBool;
    for (int i = 0; i < 5; ++i) {
        bool value = *reinterpret_cast<bool*>(ptr);
        vBool.push_back(value);
        ptr += sizeof(bool);
    }
    record.turboMode = vBool[0]?1:0;
    record.ICRMode = vBool[1]?1:0;
    record.TOCMode = vBool[2]?1:0;
    record.ECMode = vBool[3]?1:0;
    record.autoReagent =  vBool[4]?1:0;

    // 读取2个float
    std::vector<float> floats4;
    for (int i = 0; i < 2; ++i) {
        float value = *reinterpret_cast<float*>(ptr);
        floats4.push_back(value);
        ptr += sizeof(float);
    }
    record.reagent1Vol = floats4[0];
    record.reagent2Vol = floats4[1];

    std::vector<int> ints1;
    for (int i = 0; i < 3; ++i) {
        int value = *reinterpret_cast<int*>(ptr);
        ints1.push_back(value);
        ptr += sizeof(int);
    }
    record.normalRefreshTime = ints1[0];
    record.measureTimes = ints1[1];
    record.rejectTimes = ints1[2];

    //获取方法名称
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();
    String methodName;
    luaEngine->GetLuaValue(state, "config.measureParam.methodName", methodName);
    record.methodName = methodName.c_str();
    record.createTime = record.measureDateTime;

    record.flag = 0;//todo 发送标志
    DataBaseManager::Instance()->GetMeasureTable()->InsertData({record});
    return true;
}

bool ResultManager::AddCalibrateRecordToSqlite(ResultData::RecordData& result)
{
    unsigned char* ptr = result.GetData();
    int length = result.GetSize();

    if(length != CalibrateResultLength) //从lua获取的数据长度要严格对应
    {
        logger->debug("校准结果的长度有误");
        return false;
    }
    CalibrateRecord record;
    record.calibrateDateTime = *reinterpret_cast<int*>(ptr);
    record.curveK = *reinterpret_cast<double*>(ptr + 4);
    record.curveB = *reinterpret_cast<double*>(ptr + 12);
    record.calibrateConsumeTime = *reinterpret_cast<int*>(ptr + 144);
    record.currentRange = *reinterpret_cast<float*>(ptr + 148);
    float* floats = reinterpret_cast<float*>(ptr + 20);

    record.zeroShowPeakArea = floats[0];
    record.zeroConsistency = floats[1];
    record.zeroPeakArea1 = floats[2];
    record.zeroPeakArea2 = floats[3];
    record.zeroPeakArea3 = floats[4];
    record.zeroInitRefrigeratorTemp = floats[5];
    record.zeroInitNDIRTemp = floats[6];
    record.zeroFinalRefrigeratorTemp = floats[7];
    record.zeroFinalNDIRTemp = floats[8];
    record.zeroInitThermostatTemp = floats[9];
    record.zeroInitEnvironmentTemp = floats[10];
    record.zeroInitEnvironmentTempDown = floats[11];
    record.zeroFinalThermostatTemp = floats[12];
    record.zeroFinalEnvironmentTemp = floats[13];
    record.zeroFinalEnvironmentTempDown = floats[14];
    record.showPeakArea = floats[15];
    record.consistency = floats[16];
    record.standardPeakArea1 = floats[17];
    record.standardPeakArea2 = floats[18];
    record.standardPeakArea3 = floats[19];
    record.initRefrigeratorTemp = floats[20];
    record.initNDIRTemp = floats[21];
    record.finalRefrigeratorTemp = floats[22];
    record.finalNDIRTemp = floats[23];
    record.initThermostatTemp = floats[24];
    record.initEnvironmentTemp = floats[25];
    record.initEnvironmentTempDown = floats[26];
    record.finalThermostatTemp = floats[27];
    record.finalEnvironmentTemp = floats[28];
    record.finalEnvironmentTempDown = floats[29];
    record.curveR2 = floats[30];

    record.flag = 0;//todo 发送标志
    DataBaseManager::Instance()->GetCalibrateTable()->InsertData({record});
    return true;
}

void ResultManager::AddAuditTrailRecord(System::String info)
{
    emit AddAuditTrailRecordSignals(info);
}

void ResultManager::AddAuditTrailRecordSlots(System::String info)
{
     AddAuditTrailRecordToSqlite(info);
}

bool ResultManager::AddAuditTrailRecordToSqlite(System::String info)
{
    if(info.empty())
    {
        return false;
    }
    QString strRd = info.c_str();
    QStringList list = strRd.split("#");
    AuditTrailRecord record;
    record.dataTime = list.at(0).toInt();//创建时间
    record.userName = list.at(1);        //用户名称
    record.userLevel = list.at(2);       //用户等级
    record.event = list.at(3);           //用户操作
    record.oldSetting = list.at(4);      //旧操作值
    record.newSetting = list.at(5);      //新操作值
    record.details = list.at(6);         //详情

    DataBaseManager::Instance()->GetAuditTrailTable()->InsertData({record});
    return true;
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
        AddCalibrateRecordToSqlite(result);
    }
    Interface::Wqimc::WqimcManager::Instance()->uploadMeasureCurve(name, result);
    UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::NewCalibrateCurve);
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
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State* state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.measureResult.useNum", m_settingUsenum);

    luaEngine->GetLuaValue(state, "setting.measureResult.decimalNum", m_decimalNum);

    Table itermsTable;
    luaEngine->GetLuaValue(state, "setting.measureResult", itermsTable);

    OOLUA::Lua_func_ref getDecimalNum;
    if (itermsTable.safe_at("getDecimalNum", getDecimalNum))
    {
        unsigned int decimalNum = 0;
        OOLUA::Lua_function call(state);
        call(getDecimalNum);
        OOLUA::pull(state, decimalNum);
        if(m_decimalNum != decimalNum)
        {
            m_decimalNum = decimalNum;
        }
    }

    unsigned int unitIndex;
    Table sysTable;
    luaEngine->GetLuaValue(state, "config.system", sysTable);
    if(sysTable.safe_at("unitIndex", unitIndex))
    {
        String getUnitStr = QString("setting.unit["+QString::number(unitIndex + 1)+"]").toStdString();
        Table unitTable;
        luaEngine->GetLuaValue(state, getUnitStr, unitTable);

        float multiple;
        unsigned int shiftNum;
        if(unitTable.safe_at("multiple", multiple))
        {
            shiftNum = log10(multiple);
            if (m_decimalNum >= shiftNum)
            {
                m_decimalNum = m_decimalNum - shiftNum;
            }
            else
            {
                m_decimalNum = 0;
            }
        }
    }
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

QString ResultManager::DisplayResult(float result, QString unitStr)
{
    QString strConsistency;

    QString strDecimal = "%." +
                         QString("%1").arg(m_decimalNum) +
                         "f";
    if(unitStr == " ppm") //超过1ppm取小数点后两位显示
    {
        strDecimal = "%.2f";
    }
    if(result > 100)
    {
        strDecimal = "%.1f";
    }

    QByteArray ba = strDecimal.toLatin1();
    const char *tempd = ba.data();
    strConsistency.sprintf(tempd,result);

    //-0.00检查
    bool isZero = false;
    float fNegativeZeroCheck = strConsistency.toFloat();
    if (fNegativeZeroCheck > -0.000001 && fNegativeZeroCheck < 0.000001)
    {
        isZero = true;
    }
    if (isZero && signbit(fNegativeZeroCheck))    //signbit()检查浮点数0的符号位是否为负
    {
        result = qAbs(result);
        strConsistency.sprintf(tempd,result);
    }

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

