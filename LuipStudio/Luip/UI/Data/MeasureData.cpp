#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "UI/Frame/MessageDialog.h"
#include <QMetaType>
#include "Log.h"
#include "MeasureData.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Lua/App.h"
#include "LuaException.h"
#include "Setting/SettingManager.h"
#include "System/Translate.h"
#include "PrinterManager/PrinterManager.h"
#include <QDebug>
#include "HJ212Manager/HJ212Manager.h"
#include "MeasureDataDetailWidget.h"

#define RESULT_LINE     8

using namespace Result;
using namespace std;
using namespace Lua;
using namespace OOLUA;
using namespace ResultData;
using namespace ResultDetail;
using namespace PrinterSpace;
using namespace Configuration;
using namespace HJ212;
namespace UI
{

MeasureData::MeasureData(System::String resultDataname, QWidget *parent) :
    QWidget(parent),m_mode("mode"),m_datetime("dateTime"),m_resultType("resultType"),m_resultDataname(resultDataname)
{
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    int hj212Type = 0;
    lua_State * ccepState = LuaEngine::Instance()->GetThreadState();
    LuaEngine::Instance()->GetLuaValue(ccepState, "config.system.hj212Platform.hj212Type", hj212Type);

    MaintainUserChange::Instance()->Register(this);
    ResultManager::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);
    QFont font;                           //字体
    font.setPointSize(14);                //字体大小

    QFont boxfont;                           //字体
    boxfont.setPointSize(12);                //字体大小

    m_nextFlag = 0;
    m_backFlag = 0;

    this->resize(650,400);
    pageIndex = 0;

    m_resultFiles = ResultManager::Instance()->GetMeasureRecordFile(m_resultDataname);
    m_resultDetailFiles = ResultManager::Instance()->GetResultDetailUseMeasureRecordFile(m_resultDataname);

    m_resultDetailWidget = ResultDetailManager::Instance()->GetMeasureResultDetailWidget();

    if(PrinterManager::Instance()->GetMeasureDataPrinter() != nullptr)
    {
        m_measureDataPrintWidget = new MeasureDataPrintWidget(m_resultDataname, this);
    }
    else
    {
        m_measureDataPrintWidget = nullptr;
    }

    m_lua = LuaEngine::Instance()->GetEngine();
    Table table;

    LuaEngine* luaEngine = LuaEngine::Instance();
    m_lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table fieldTable;
    luaEngine->GetLuaValue(state, "setting.ui.measureData", fieldTable);

    //检索用的开始日期
    minTime = new QMyEdit();
    minTime->setFixedHeight(32);
    minTime->setFixedWidth(96);
    minTime->setText(GetMinTime());
    minTime->setFont(boxfont);

    QHBoxLayout *minTimeLayout = new QHBoxLayout();
    minTimeLayout->addWidget(minTime);
    minTimeLayout->setContentsMargins(0, 0, 0, 0);

    toTime = new QLabel();
    toTime->setText("-");
    toTime->setFixedSize(5,30);
    QHBoxLayout *toTimeLayout = new QHBoxLayout();
    toTimeLayout->addWidget(toTime);
    toTimeLayout->setContentsMargins(0, 0, 0, 0);

    //检索用的结束日期
    maxTime = new QMyEdit();
    maxTime->setFixedHeight(32);
    maxTime->setFixedWidth(96);
    maxTime->setText(GetMaxTime());
    maxTime->setFont(boxfont);

    QHBoxLayout *maxTimeLayout = new QHBoxLayout();
    maxTimeLayout->addWidget(maxTime);
    maxTimeLayout->addStretch();
    maxTimeLayout->setSpacing(50);
    maxTimeLayout->setContentsMargins(0, 0, 0, 0);

    dateModel = new QLabel();
    dateModel->setFixedSize(36,36);
    dateModel->setText(tr("标识"));
    dateModel->setFont(boxfont);

    QHBoxLayout *dateModelLayout = new QHBoxLayout();
    dateModelLayout->addWidget(dateModel);
    dateModelLayout->setContentsMargins(10, 0, 0, 0);

    dateType = new QLabel();
    dateType->setFixedSize(36,36);
    dateType->setText(tr("类型"));
    dateType->setFont(boxfont);

    QHBoxLayout *dateTypeLayout = new QHBoxLayout();
    dateTypeLayout->addWidget(dateType);
    dateTypeLayout->setContentsMargins(0, 0, 0, 0);

    //检索类型下拉框
    dateTypeCombobox = new QComboBox();
    dateTypeCombobox->setFixedSize(83,36); //73
    dateTypeCombobox->setFont(boxfont);
    QStringList dateTypeList;
    fieldTable.at("resultTypeList", table);
    oolua_ipairs(table)
    {
        String text;
        m_lua->pull(text);
        dateTypeList << QObject::tr(text.c_str());
    }
    oolua_ipairs_end()
    if(dateTypeList.empty())
    {
        dateTypeList << tr("全部");
    }
    dateTypeCombobox->insertItems(0, dateTypeList);

    QHBoxLayout *dateTypeComLayout = new QHBoxLayout();
    dateTypeComLayout->addWidget(dateTypeCombobox);
    dateTypeComLayout->addStretch();
    dateTypeComLayout->setSpacing(0);
    dateTypeComLayout->setContentsMargins(0, 0, 0, 0);

    //检索模式下拉框
    dateModelCombobox = new QComboBox();
    dateModelCombobox->setFixedSize(60,36);
    dateModelCombobox->setFont(boxfont);
    QStringList resultMarkList;
    fieldTable.at("resultMarkList", table);
    oolua_ipairs(table)
    {
        String text;
        m_lua->pull(text);
        resultMarkList << QObject::tr(text.c_str());
    }
    oolua_ipairs_end()
    if(resultMarkList.empty())
    {
        resultMarkList << tr("全部");
    }
    dateModelCombobox->insertItems(0, resultMarkList);
    QHBoxLayout *dateModelComLayout = new QHBoxLayout();
    dateModelComLayout->addWidget(dateModelCombobox);
    dateModelComLayout->setContentsMargins(0, 0, 0, 0);

    searchButton = new QPushButton();
    searchButton->setObjectName("brownButton");
    searchButton->setText(tr("检索"));
    searchButton->setFont(font);
    searchButton->setFixedSize(80,40);

    QHBoxLayout *leftBottomLayout = new QHBoxLayout();

    leftBottomLayout->addLayout(minTimeLayout);
    leftBottomLayout->addLayout(toTimeLayout);
    leftBottomLayout->addLayout(maxTimeLayout);
    leftBottomLayout->addStretch();
//    leftBottomLayout->addLayout(dateModelLayout);
//    leftBottomLayout->addLayout(dateModelComLayout);
    leftBottomLayout->addLayout(dateTypeLayout);
    leftBottomLayout->addLayout(dateTypeComLayout);
    leftBottomLayout->addWidget(searchButton);

    detailBox = new QGroupBox(this);
    detailBox->setStyleSheet("QGroupBox{border:0.5px solid lightgray;background-color:rgb(255,255,255);}");

    detailLabel = new QLabel();
    detailLabel->setFixedSize(545,32);
    detailLabel->setText("");
    detailLabel->setFont(boxfont);

    QHBoxLayout *detailBoxLayout = new QHBoxLayout();
    detailBoxLayout->setContentsMargins(0,0,0,0);
    detailBoxLayout->addWidget(detailLabel);
    detailBox->setLayout(detailBoxLayout);
    detailBox->setGeometry(9,345,549,34);
    detailBox->setHidden(true);

    int columnCount = 0;
    fieldTable.at("columnCount", columnCount);
    //数据表格
    measureResultTableWidget = new MQtableWidget();

    measureResultTableWidget->resize(580,344);
    measureResultTableWidget->setColumnCount(columnCount);//列
    measureResultTableWidget->setRowCount(RESULT_LINE);
    measureResultTableWidget->setFixedSize(580,361);

    m_isUnitChange = fieldTable.safe_at("unitChange", m_unitChangeFunc);

    String unit = "mg/L";
    if (m_isUnitChange)
    {
        m_lua->call(m_unitChangeFunc, unit, UnitChange::Read);
        m_lua->pull(unit);
    }

    int tolWidth = 0;
    int col = 0;
    oolua_ipairs(fieldTable)
    {
        ShowField showField;
        String text;
        String format;
        int width;

        m_lua->pull(table);
        table.at("name", showField.name);

        table.at("text", text);
        if(text.c_str()== "结果")
        {
           text.append("(" + unit + ")");
        }
        m_columnName << QObject::tr(text.c_str());

        table.at("width", width);

        if (col == 1)
        {
            width += 15;
        }
        else if (col == 2)
        {
            width += 15;
        }

        measureResultTableWidget->setColumnWidth(col++, width);

        tolWidth = tolWidth + width;

        if (table.safe_at("format", format))
        {
            showField.format = format.c_str();
        }

        m_showFields.push_back(showField);
    }
    oolua_ipairs_end()

    //设置表头
    QFont headFont;
    headFont.setPointSize(14);
    measureResultTableWidget->setColumnAndSize(m_columnName,15);
    measureResultTableWidget->horizontalHeader()->setFont(headFont);
    measureResultTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    measureResultTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = measureResultTableWidget->font();
    dataFont.setPointSize(10);
    measureResultTableWidget->setFont(dataFont);
    measureResultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
    measureResultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    measureResultTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:20px;}");
    measureResultTableWidget->horizontalScrollBar()->setVisible(false);
    measureResultTableWidget->horizontalScrollBar()->setDisabled(true);
    measureResultTableWidget->verticalScrollBar()->setVisible(false);
    measureResultTableWidget->verticalScrollBar()->setDisabled(true);


    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(measureResultTableWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout();

    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    leftLayout->addLayout(leftBottomLayout);

    for(int i = 0;i < RESULT_LINE;i++)
    {
        measureResultTableWidget->setRowHeight(i,40);
    }
    measureResultTableWidget->setFont(font);

    SpaceInit();
    readMeasureResultLogFile();

    toTopButton = new QPushButton();
    toTopButton->setObjectName("brownButton");
    toTopButton->setText("首页");
    toTopButton->setFont(font);
    toTopButton->setFixedSize(80,40);

    toBackButton = new QPushButton();
    toBackButton->setObjectName("brownButton");
    toBackButton->setText("上一页");
    toBackButton->setFont(font);
    toBackButton->setFixedSize(80,40);

    toNextButton = new QPushButton();
    toNextButton->setObjectName("brownButton");
    toNextButton->setText("下一页");
    toNextButton->setFont(font);
    toNextButton->setFixedSize(80,40);

    toBottomButton = new QPushButton();
    toBottomButton->setObjectName("brownButton");
    toBottomButton->setText("尾页");
    toBottomButton->setFont(font);
    toBottomButton->setFixedSize(80,40);

    exportButton = new QPushButton();
    exportButton->setObjectName("brownButton");
    exportButton->setText(tr("导出"));
    exportButton->setFont(font);
    exportButton->setFixedSize(80,40);

    clearButton= new QPushButton();
    clearButton->setObjectName("brownButton");
    clearButton->setText(tr("清空"));
    clearButton->setFont(font);
    clearButton->setFixedSize(80,40);

    manualUpButton= new QPushButton();
    manualUpButton->setObjectName("brownButton");
    manualUpButton->setText(tr("上传"));
    manualUpButton->setFont(font);
    manualUpButton->setFixedSize(80,40);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(toTopButton);
    rightLayout->addWidget(toBackButton);
    rightLayout->addWidget(toNextButton);
    rightLayout->addWidget(toBottomButton);
    rightLayout->addWidget(exportButton);
    rightLayout->addWidget(clearButton);
//    if(m_resultDetailWidget != nullptr)
    {
        detailButton = new QPushButton();
        detailButton->setObjectName("brownButton");
        detailButton->setText(tr("详情"));
        detailButton->setFont(font);
        detailButton->setFixedSize(80,40);
        rightLayout->addWidget(detailButton);
        connect(detailButton,SIGNAL(clicked()), this, SLOT(SlotDetailButton()));
    }
    rightLayout->addStretch();
    if(m_measureDataPrintWidget != nullptr)
    {
        printerButton = new QPushButton();
        printerButton->setObjectName("brownButton");
        printerButton->setText(tr("打印"));
        printerButton->setFont(font);
        printerButton->setFixedSize(80,40);
        rightLayout->addWidget(printerButton);
        connect(printerButton,SIGNAL(clicked()), this, SLOT(SlotPrinterButton()));
    }
    else
    {
        printerButton = nullptr;
    }
    if (hj212Type != (int)HJ212::HJ212Type::None)
    {
        rightLayout->addStretch();
        rightLayout->addWidget(manualUpButton);
    }

    rightLayout->setSpacing(10);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(rightLayout);

    this->setLayout(mainLayout);

    minDayCaledar = new QCalendarWidget(this);
    maxDayCaledar = new QCalendarWidget(this);
    if(SettingManager::Instance()->GetAppLanguage() == Language::SimplifiedChinese)
    {
        minDayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::China));
        maxDayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::China));
    }
    else if(SettingManager::Instance()->GetAppLanguage() == Language::TraditionalChinese)
    {
        minDayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::Taiwan));
        maxDayCaledar->setLocale(QLocale(QLocale::Chinese, QLocale::Taiwan));
    }
    else if(SettingManager::Instance()->GetAppLanguage() == Language::English)
    {
        minDayCaledar->setLocale(QLocale::English);
        maxDayCaledar->setLocale(QLocale::English);
    }
    minDayCaledar->hide();
    maxDayCaledar->hide();

    //调整表格大小适应显示
    if(tolWidth > measureResultTableWidget->width())
    {
        this->TableExtraInfoAdjust(true);
    }
    else
    {
        this->TableExtraInfoAdjust(false);
    }

    this->ChangeBottomStatus();

    connect(toTopButton,SIGNAL(clicked()), this, SLOT(ToTop()));
    connect(toBottomButton,SIGNAL(clicked()), this, SLOT(ToBottom()));
    connect(toBackButton,SIGNAL(clicked()), this, SLOT(ToBack()));
    connect(toNextButton,SIGNAL(clicked()), this, SLOT(ToNext()));
    connect(searchButton, SIGNAL(clicked()), this, SLOT(SearchResult()));
    connect(exportButton,SIGNAL(clicked()), this, SLOT(SlotExportButton()));
    connect(clearButton,SIGNAL(clicked()), this, SLOT(SlotClearButton()));
    connect(manualUpButton,SIGNAL(clicked()), this, SLOT(SlotManualUpButton()));

    connect(minTime, SIGNAL(LineEditClicked()), this, SLOT(ChoseMinDay()));
    connect(maxTime, SIGNAL(LineEditClicked()), this, SLOT(ChoseMaxDay()));
    connect(minDayCaledar, SIGNAL(clicked(const QDate &)), this, SLOT(ChangeMinDay()));
    connect(maxDayCaledar, SIGNAL(clicked(const QDate &)), this, SLOT(ChangeMaxDay()));

    connect(measureResultTableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(SlotShowExtraInformation()));
}

//void MeasureData::SlotDetailButton()
//{
//    int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
//    {
//        if (resultReadPos[row] != 0)
//        {
//            m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(),resultReadPos[row]);
//            if (m_resultDetailFiles->MovePrevious(m_resultDetailFiles->GetSelfReaderIndex(), 1))
//            {
//                RecordData recordData = m_resultDetailFiles->GetCurrentRecord(m_resultDetailFiles->GetSelfReaderIndex());

//                if (m_resultDetailWidget != nullptr)
//                {
//                    m_resultDetailWidget->SetRecordData(recordData);
//                    m_resultDetailWidget->UpdateRecord();
//                    m_resultDetailWidget->Show(this, (int)LoginDialog::userType);
//                }
//            }
//        }
//    }
//}

void MeasureData::SlotPrinterButton()
{
    if(m_measureDataPrintWidget != nullptr)
    {
        int iDesktopWidth = QApplication::desktop()->width();
        int iDesktopHeight = QApplication::desktop()->height();

        m_measureDataPrintWidget->Show(this, (int)LoginDialog::userType);
//        m_measureDataPrintWidget->move((iDesktopWidth-m_measureDataPrintWidget->width())/2,(iDesktopHeight-m_measureDataPrintWidget->height())/2);
    }
}

QString MeasureData::ResultInfoAnalysis(RecordData result)
{
    String strInfo;

    OperateRecordData* operateRecordData = ResultManager::Instance()->GetMeasureOperateRecordData(m_resultDataname);

    if(operateRecordData == nullptr)
    {
        logger->warn("MeasureData::ResultInfoAnalysis() => operateRecordData is null");
        return " ";
    }

    DetailData clickeData;  // 双击结果行的数据

    std::vector<DetailData> dataList;

    bool status = this->ExtractDetailData(result, clickeData);

    if (status == true)
    {
        if (clickeData.resultType == MeasureType::Addstandard ||
            clickeData.resultType == MeasureType::Parallel)
        {
            dataList.push_back(clickeData);

            int i = 1;

            while(1)
            {
                if (m_resultDetailFiles->MovePrevious(m_resultDetailFiles->GetSelfReaderIndex(),i++))
                {
                    RecordData recordData = m_resultDetailFiles->GetCurrentRecord(m_resultDetailFiles->GetSelfReaderIndex());
                    DetailData detailData;  // 往下遍历结果行的数据

                    status = this->ExtractDetailData(recordData, detailData);
                    if (status == true)
                    {
                        if (detailData.time == clickeData.time)
                        {
                            dataList.push_back(detailData);

                            if (detailData.resultType != clickeData.resultType)
                            {
                                break;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        logger->warn("MeasureData::ResultInfoAnalysis() => Extract detail data 1 false");
                    }
                }
                else
                {
                    break;
                }
            }

            if(dataList.size() >= 2)
            {
                try
                {
                    Table table;
                    OOLUA::Lua_func_ref addConsistency;
                    OOLUA::Lua_func_ref getInfo;

                    LuaEngine* luaEngine = LuaEngine::Instance();
                    Script* lua = luaEngine->GetEngine();
                    lua_State * state = luaEngine->GetThreadState();

                    if (clickeData.resultType == MeasureType::Addstandard)
                    {
                        luaEngine->GetLuaValue(state, "setting.ui.measureData.addstandard", table);
                    }
                    else if (clickeData.resultType == MeasureType::Parallel)
                    {
                        luaEngine->GetLuaValue(state, "setting.ui.measureData.parallel", table);
                    }

                    table.at("addConsistency", addConsistency);
                    table.at("getInfo", getInfo);

                    for (std::vector<DetailData> ::iterator it=dataList.begin();
                         it!=dataList.end(); it++)
                    {
                        lua->call(addConsistency, (*it).consistency, (*it).resultType,
                                  (*it).addstandardConsistency, (*it).range);
                    }

                    lua->call(getInfo);
                    lua->pull(strInfo);
                }
                catch(OOLUA::Exception e)
                {
                    logger->warn("MeasureData::ResultInfoAnalysis() Addstandard => %s", e.what());
                }
                catch(std::exception e)
                {
                    logger->warn("MeasureData::ResultInfoAnalysis() Addstandard  => %s", e.what());
                }
            }
        }
    }
    else
    {
        logger->warn("MeasureData::ResultInfoAnalysis() => Extract detail data 0 false");
    }

    return QString(strInfo.c_str());
}

bool MeasureData::ExtractDetailData(ResultData::RecordData &result, DetailData &detailData)
{
    OperateRecordData* operateRecordData = ResultManager::Instance()->GetMeasureOperateRecordData(m_resultDataname);

    if(operateRecordData == nullptr)
    {
        logger->warn("MeasureData::ResultInfoAnalysis() => operateRecordData is null");
        return false;
    }

    operateRecordData->GetRecordDataFieldValue("dateTime", result, detailData.time);
    operateRecordData->GetRecordDataFieldValue("consistency", result, detailData.consistency);
    operateRecordData->GetRecordDataFieldValue("addstandardConsistency", result, detailData.addstandardConsistency);
    operateRecordData->GetRecordDataFieldValue("measureRange", result, detailData.range);

    Byte type  = 0;
    operateRecordData->GetRecordDataFieldValue("resultType", result, type);
    detailData.resultType = (MeasureType)type;

    return true;
}

void MeasureData::SlotShowExtraInformation()
{
    int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
    if(row!=-1)
    {
        if (resultReadPos[row] != 0)
        {
            m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), resultReadPos[row]);
            if (m_resultDetailFiles->MovePrevious(m_resultDetailFiles->GetSelfReaderIndex(), 1))
            {
                RecordData recordData = m_resultDetailFiles->GetCurrentRecord(m_resultDetailFiles->GetSelfReaderIndex());

                QString strInfo = this->ResultInfoAnalysis(recordData);

                QString typeStr = measureResultTableWidget->item(row,4)->text();

                if((typeStr == QObject::tr("加标(内)") || typeStr == QObject::tr("平行(内)"))&& (strInfo.size() > 1))
                {
                    this->detailLabel->setText(Translate::ComplexTranslate(strInfo));
                    this->detailBox->setHidden(false);
                }
                else
                {
                    this->detailBox->setHidden(true);
                }

            }
        }
    }
    else
    {
        this->detailBox->setHidden(true);
    }
}

void MeasureData::ShowRow(Uint16 row)
{
    Uint16 column = 0;
    for (vector<ShowField>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
    {
        FieldType type;
        if (!m_resultFiles->GetRecordFields()->GetFieldType(iter->name, type))
        {
            column++;
            continue;
        }
        switch(type)
        {
            case FieldType::Bool:
                {
                    bool ret;
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    measureResultTableWidget->item(row, column++)->setText(QString::number(ret));
                }
                break;
            case FieldType::Byte:
                {
                    Byte ret;
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    if (iter->format.empty())
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::number(ret));
                    }
                    else
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                    }
                }
                break;
            case FieldType::Int:
                {
                    int ret;
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    if (iter->format.empty())
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::number(ret));
                    }
                    else
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                    }
                }
                break;
            case FieldType::Float:
                {
                    float ret;
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    if (iter->name == "consistency")
                    {
                        if (m_isUnitChange)
                        {
                            m_lua->call(m_unitChangeFunc, ret, UnitChange::Read);
                            m_lua->pull(ret);
                        }
                        QString unitStr = " ppb";
                        if(ret > 1000)
                        {
                            ret = ret / 1000;
                            unitStr = " ppm";
                        }
                        QString strConsistency = ResultManager::Instance()->DisplayResult(ret, unitStr);
                        measureResultTableWidget->item(row, column++)->setText(strConsistency + unitStr);
                    }
                    else if (iter->name == "consistencyTC")
                    {
                        if (m_isUnitChange)
                        {
                            m_lua->call(m_unitChangeFunc, ret, UnitChange::Read);
                            m_lua->pull(ret);
                        }
                        QString unitStr = " ppb";
                        if(ret > 1000)
                        {
                            ret = ret / 1000;
                            unitStr = " ppm";
                        }
                        QString strConsistency = ResultManager::Instance()->DisplayResult(ret, unitStr);
                        measureResultTableWidget->item(row, column++)->setText(strConsistency + unitStr);
                    }
                    else if (iter->name == "consistencyIC")
                    {
                        if (m_isUnitChange)
                        {
                            m_lua->call(m_unitChangeFunc, ret, UnitChange::Read);
                            m_lua->pull(ret);
                        }
                        QString unitStr = " ppb";
                        if(ret > 1000)
                        {
                            ret = ret / 1000;
                            unitStr = " ppm";
                        }
                        QString strConsistency = ResultManager::Instance()->DisplayResult(ret, unitStr);
                        measureResultTableWidget->item(row, column++)->setText(strConsistency + unitStr);
                    }
                    else
                    {
                        if (iter->format.empty())
                        {
                            measureResultTableWidget->item(row, column++)->setText(QString::number(ret));
                        }
                        else
                        {
                            measureResultTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                        }
                    }
                }
                break;
            case FieldType::Double:
                {
                    double ret;
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    if (iter->format.empty())
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::number(ret,'f',9));
                    }
                    else
                    {
                        measureResultTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                    }
                }
                break;
            case FieldType::IntArray:
                break;
            case FieldType::Time:
                {
                    int ret;
                    QFont font = measureResultTableWidget->item(row, column)->font();
                    font.setPointSize(12);
                    m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                    measureResultTableWidget->item(row, column)->setFont(font);
                    measureResultTableWidget->item(row, column++)->setText(QDateTime::fromTime_t(ret).toString(iter->format.c_str()));
                }
                break;
            case FieldType::Enum:
                {
                    //String str;
                    //m_resultFiles->GetFieldCurrentRecordEnumString(iter->name, str);
                    //measureResultTableWidget->item(row, column++)->setText(QObject::tr(str.c_str()));
                    QTableWidgetItem *item = measureResultTableWidget->item(row, column);

                    String str;
                    m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), iter->name, str);
                    QString text = QObject::tr(str.c_str());

                    QFont font = measureResultTableWidget->font();
                    int size = font.pointSize();

                    QFontMetrics fontWidth(font);
                    int textWidth = fontWidth.width(text);

                    int itemWidth = measureResultTableWidget->columnWidth(column)-8;

                    float rate = (float)itemWidth/textWidth;

                    int tmpSize = size;
                    if(textWidth > itemWidth)
                    {
                        tmpSize = size*rate;
                    }

                    if (tmpSize<10)
                    {
                        tmpSize = 10;
                    }
                    font.setPointSize(tmpSize);
                    item->setFont(font);
                    item->setText(text);

                    column++;
                }
                break;
        }
    }
}

void MeasureData::readMeasureResultLogFile()
{
    Uint16 row = 0;
    memset(resultReadPos, 0, sizeof(resultReadPos));
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
    {
        resultReadPos[row] = m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex());
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
        ShowRow(row++);
    }
    SearchResult();
}

void MeasureData::SearchResult()
{
    QString mode;
    QString resultType;
    const QString allType = tr("全部");
    QDateTime min;
    QDateTime max;
    QDateTime theDate;
    min  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    max  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");

    ViewRefresh();
    SpaceInit();
    memset(resultReadPos, 0, sizeof(resultReadPos));
    Uint16 row = 0;

    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
    {
        int ret;
        String str;

        resultReadPos[row] = m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex());
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
        theDate = QDateTime::fromTime_t(ret);

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_mode, str);
        mode = QObject::tr(str.c_str());

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_resultType, str);
        resultType = QObject::tr(str.c_str());

        if ((theDate >= min)
            && (theDate <= max)
            && (dateModelCombobox->currentText() == allType || mode == dateModelCombobox->currentText())
            && (dateTypeCombobox->currentText() == allType || resultType == dateTypeCombobox->currentText()))

        {
            ShowRow(row++);
        }
        else
        {
            resultReadPos[row] = 0;
        }
    }
}

void MeasureData::ToTop()
{
    SearchResult();
    m_nextFlag = 0;
    m_backFlag = 0;
}

void MeasureData::ToBottom()
{
    m_nextFlag = 0;
    m_backFlag = 0;

    QString mode;
    QString resultType;
    const QString allType = QObject::tr("全部");
    QDateTime min ;
    QDateTime max;
    QDateTime theDate;

    min  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    max  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");

    ViewRefresh();
    SpaceInit();
    memset(resultReadPos, 0, sizeof(resultReadPos));
    Uint16 row = 0;

    m_resultFiles->MoveToFirst(m_resultFiles->GetSelfReaderIndex());
    while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
    {
        int ret;
        String str;

        m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());
        resultReadPos[9 - row] = m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex());
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
        theDate = QDateTime::fromTime_t(ret);

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_mode, str);
        mode = QObject::tr(str.c_str());

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_resultType, str);
        resultType = QObject::tr(str.c_str());

        if ((theDate >= min)
            && (theDate <= max)
            && (dateModelCombobox->currentText() == allType || mode == dateModelCombobox->currentText())
            && (dateTypeCombobox->currentText() == allType || resultType == dateTypeCombobox->currentText()))
        {
            row++;
            ShowRow(RESULT_LINE - row);
        }
        else
        {
            resultReadPos[9 - row] = 0;
        }
    }
    if(row < RESULT_LINE)
    {
        ToTop();
    }
}

void MeasureData::ToBack()       //返回新数据，文本指针后移
{
    QString mode;
    QString resultType;
    const QString allType = tr("全部");
    QDateTime min;
    QDateTime max;
    QDateTime theDate;

    min  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    max  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");
    Uint16 row = 0;

    if(m_resultFiles->GetRemainNum(m_resultFiles->GetSelfReaderIndex()) <= RESULT_LINE)
    {
        ToTop();
    }
    else
    {
        if(m_nextFlag == 1)
        {
            int k = RESULT_LINE;
            while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex())&& k > 0)
            {
                m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());
                k--;
            }
            m_nextFlag = 0;
            m_backFlag = 1;
        }
        memset(resultReadPos, 0, sizeof(resultReadPos));
        while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
        {
            int ret;
            String str;

            m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());
            resultReadPos[9 - row] = m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex());

            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
            theDate = QDateTime::fromTime_t(ret);

            m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_mode, str);
            mode = QObject::tr(str.c_str());

            m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_resultType, str);
            resultType = QObject::tr(str.c_str());

            if ((theDate >= min)
                && (theDate <= max)
                && (dateModelCombobox->currentText() == allType || mode == dateModelCombobox->currentText())
                && (dateTypeCombobox->currentText() == allType || resultType == dateTypeCombobox->currentText()))
            {
                row++;
                ShowRow(RESULT_LINE - row);
            }
            else
            {
                resultReadPos[9 - row] = 0;
            }
        }
        m_backFlag = 1;
        if(row < RESULT_LINE)
        {
            ToTop();
        }
    }

}

void MeasureData::ToNext()        //返回老数据，文件指针前移动
{
    QString mode;
    QString resultType;    
    const QString allType = tr("全部");
    QDateTime min ;
    QDateTime max;
    QDateTime theDate;

    min  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    max  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");

    Uint16 row = 0;

    if(m_resultFiles->GetHeadNum(m_resultFiles->GetSelfReaderIndex()) <= RESULT_LINE)
    {
        ToBottom();
    }

    else
    {
        if(m_backFlag == 1)
        {
            int k = RESULT_LINE;
            while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex())&& k > 0)
            {
                m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
                k--;
            }
            m_backFlag = 0;
        }
        memset(resultReadPos, 0, sizeof(resultReadPos));
        while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
        {
            int ret;
            String str;
            resultReadPos[row] = m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex());
            m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
            theDate = QDateTime::fromTime_t(ret);

            m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_mode, str);
            mode = QObject::tr(str.c_str());

            m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_resultType, str);
            resultType = QObject::tr(str.c_str());

            if ((theDate >= min)
                && (theDate <= max)
                && (dateModelCombobox->currentText() == allType || mode == dateModelCombobox->currentText())
                && (dateTypeCombobox->currentText() == allType || resultType == dateTypeCombobox->currentText()))
            {
                ShowRow(row++);
            }
            else
            {
                resultReadPos[row] = 0;
            }
        }
        m_nextFlag = 1;
        if(row < RESULT_LINE)
        {
            ToBottom();
        }
    }
}

QString MeasureData::GetMinTime()
{
    QString minTime = QDateTime::currentDateTime().addDays(-7).toString("yyyy-MM-dd");
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    if(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
        int ret = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
        minTime = QDateTime::fromTime_t(ret).addDays(-7).toString("yyyy-MM-dd");
    }
    return minTime;
}

QString MeasureData::GetMaxTime()
{
    QString maxTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    if(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
        int ret = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
        maxTime = QDateTime::fromTime_t(ret).toString("yyyy-MM-dd");
    }
    return maxTime;
}

QDate MeasureData::GetCurrentMinDate()
{
    QString dateString = minTime->text();
    QStringList dateList = dateString.split("-");
    QString year = dateList.at(0);
    QString month = dateList.at(1);
    QString day = dateList.at(2);

    QDate minDate = QDate(year.toInt(),month.toInt(),day.toInt());
    return minDate;
}

QDate MeasureData::GetCurrentMaxDate()
{
    QString dateString = maxTime->text();
    QStringList dateList = dateString.split("-");
    QString year = dateList.at(0);
    QString month = dateList.at(1);
    QString day = dateList.at(2);

    QDate maxDate = QDate(year.toInt(),month.toInt(),day.toInt());
    return maxDate;
}

void MeasureData::ChoseMinDay()
{
    minDayCaledar->setFixedSize(300,300);
    minDayCaledar->move(130,120);
    minDayCaledar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    if(minDayCaledar->isHidden())
    {
        minDayCaledar->show();
        maxDayCaledar->hide();
    }
    else
    {
        minDayCaledar->hide();
        maxDayCaledar->hide();
    }
}

void MeasureData::ChangeMinDay()
{
    QDate date = minDayCaledar->selectedDate();
    QDate curMaxDate = this->GetCurrentMaxDate();
    if(date > curMaxDate)
    {
        minDayCaledar->hide();
        MessageDialog msg(tr("检索起始日期不能晚于结束日期！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    QString dateString;
    QString monthsign;
    QString daysign;
    if(date.month()>=RESULT_LINE)
    {
        monthsign = "-";
    }
    else
    {
        monthsign = "-0";
    }

    if(date.day()>=RESULT_LINE)
    {
        daysign = "-";
    }
    else
    {
        daysign = "-0";
    }

    dateString = QString::number(date.year())+monthsign+QString::number(date.month())+daysign+QString::number(date.day());

    minTime->setText(dateString);
    minDayCaledar->hide();
}

void MeasureData::ChoseMaxDay()
{
    maxDayCaledar->setFixedSize(300,300);
    maxDayCaledar->move(260,120);
    maxDayCaledar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    if(maxDayCaledar->isHidden())
    {
        maxDayCaledar->show();
        minDayCaledar->hide();
    }
    else
    {
        maxDayCaledar->hide();
        minDayCaledar->hide();
    }
}

void MeasureData::ChangeMaxDay()
{
    QDate date = maxDayCaledar->selectedDate();
    QDate curMinDate = this->GetCurrentMinDate();
    if(date < curMinDate)
    {
        maxDayCaledar->hide();
        MessageDialog msg(tr("检索结束日期不能早于起始日期！\n"), this,MsgStyle::ONLYOK);
        if(msg.exec()== QDialog::Accepted)
        {
            return;
        }
    }

    QString dateString;
    QString monthsign;
    QString daysign;
    if(date.month()>=10)
    {
        monthsign = "-";
    }
    else
    {
        monthsign = "-0";
    }

    if(date.day()>=10)
    {
        daysign = "-";
    }
    else
    {
        daysign = "-0";
    }

    dateString = QString::number(date.year())+monthsign+QString::number(date.month())+daysign+QString::number(date.day());
    maxTime->setText(dateString);
    maxDayCaledar->hide();
}

void MeasureData::SpaceInit()
{
    for(int i = 0;i < measureResultTableWidget->rowCount();i++)
    {
        for(int j = 0;j < measureResultTableWidget->columnCount();j++)
        {
            measureResultTableWidget->setItem(i, j, new QTableWidgetItem());
            measureResultTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void MeasureData::ViewRefresh()
{
    measureResultTableWidget->clear();
    measureResultTableWidget->setColumnAndSize(m_columnName,15);
}

void MeasureData::ChangeBottomStatus()
{
    bool solidifyMeaParamFromUI = false;

    try
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

        luaEngine->GetLuaValue(state, "config.system.solidifyMeaParamFromUI", solidifyMeaParamFromUI);
    }
    catch(OOLUA::LuaException e)
    {
        logger->warn("MeasureData::ChangeBottomStatus() => %s", e.What().c_str());
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("MeasureData::ChangeBottomStatus() => %s", e.what());
    }

    if(LoginDialog::userType == RoleType::Super)
    {
        exportButton->show();
        clearButton->show();
    }
    else if(LoginDialog::userType == RoleType::Administrator)
    {
        exportButton->show();
        clearButton->hide();
//        if (solidifyMeaParamFromUI)
//        {
//            clearButton->hide();
//        }
//        else
//        {
//            clearButton->show();
//        }
    }
    else if(LoginDialog::userType == RoleType::Maintain)
    {
        exportButton->show();
        clearButton->hide();
//        if (solidifyMeaParamFromUI)
//        {
//            clearButton->hide();
//        }
//        else
//        {
//            clearButton->show();
//        }
    }
    else if(LoginDialog::userType == RoleType::General)
    {
        exportButton->hide();
        clearButton->hide();
    }

    if(printerButton != nullptr)
    {
        RoleType showPrivilege;
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        luaEngine->GetLuaValue(state, "setting.ui.measureDataPrint.showPrivilege", showPrivilege);

        if(LoginDialog::userType >= showPrivilege)
        {
            printerButton->show();
        }
        else
        {
            printerButton->hide();
        }
    }

    if(m_resultDetailWidget != nullptr)
    {
        RoleType showPrivilege;
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        luaEngine->GetLuaValue(state, "setting.ui.resultDetail.showPrivilege", showPrivilege);

        if(LoginDialog::userType >= showPrivilege)
        {
            detailButton->show();
        }
        else
        {
            detailButton->hide();
        }
    }
}

void MeasureData::OnUserChange()
{
    this->ChangeBottomStatus();
}

void MeasureData::OnMeasureResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
    UpdaterData();
}

void MeasureData::UpdaterData()
{
    minTime->setText(QDateTime::currentDateTime().addDays(-7).toString("yyyy-MM-dd"));
    maxTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    ToTop();
    measureResultTableWidget->repaint();
}

void MeasureData::OnCalibrateResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
}

void MeasureData::SlotClearButton()
{
    MessageDialog msg(tr("是否确认清空数据记录?"), this, MsgStyle::OKANDCANCEL, true);
    if(QDialog::Accepted != msg.exec())
    {
        return;
    }
    else
    {
        ResultManager::Instance()->ClearBackupMeasureRecordFile(m_resultDataname);
        m_resultFiles->ClearRecord();
        minTime->setText(GetMinTime());
        maxTime->setText(GetMaxTime());
        ViewRefresh();
        SpaceInit();
        logger->debug("清空数据记录");
        UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ClearMeasureData, m_resultDataname);
    }
}

void MeasureData::SlotExportButton()
{
    QString result = "";
    CopyFileAction copyFileAction;
    String strDir = copyFileAction.GetTargetDir();
    QDir dir(strDir.c_str());

    QString errmsg;
    if (!copyFileAction.ExMemoryDetect(errmsg)) //U盘检测
    {
        MessageDialog msg(errmsg, this);
        msg.exec();
        return;
    }

    if (!copyFileAction.TargetDirCheck(dir)) //拷贝目录检测
    {
        MessageDialog msg(tr("创建目录，测量数据导出失败"), this);
        msg.exec();
        return;
    }

    if  (m_resultFiles->ExportFiles(LoginDialog::userType))
    {
        result = tr("测量数据导出成功");
        logger->info("测量数据导出成功");
    }
    else
    {
        result = tr("测量数据导出失败");
        logger->info("测量数据导出失败");
    }
    MessageDialog msg(result, this);
    msg.exec();
    return;
}

void MeasureData::TableExtraInfoAdjust(bool isEnable)
{
    if(isEnable)
    {
        measureResultTableWidget->horizontalScrollBar()->setVisible(true);
        measureResultTableWidget->horizontalScrollBar()->setDisabled(false);
        for(int i =0;i<RESULT_LINE;i++)
        {
            measureResultTableWidget->setRowHeight(i,37);
        }
    }
    else
    {
        measureResultTableWidget->horizontalScrollBar()->setVisible(false);
        measureResultTableWidget->horizontalScrollBar()->setDisabled(true);
        for(int i =0;i<RESULT_LINE;i++)
        {
            measureResultTableWidget->setRowHeight(i,40);
        }
    }
}

void MeasureData::SlotManualUpButton()
{
    int row = measureResultTableWidget->currentIndex().row();
    if (row<0 || (measureResultTableWidget->item(row ,0)->text().isEmpty() == true))
    {
        qDebug("This line is Empty");
        MessageDialog msg(tr("请正确选择需要上传的数据行"), this);
        msg.exec();
        return;
    }
    else
    {
        const char *flagName[8] = {"正常","离线","维护","故障","校准","超上限","超上限","质控"};
        const char *flagSign[8] = {"N","B","M","D","C","T","T","K"};
        const char *resultTypeName[11] = {"零点校准液","水样","量程校准液","核查液","加标(内)","平行(内)","零点核查","跨度(标样)核查","加标(外)","平行(外)","质控样"};

        //补传结果类型
        QDateTime timeDate;
        String flag = "N";
        float consistency = 0;
        int resultType = 255;

        QString timeQString;        //获取界面文本
        QString TypeQString;       //获取界面文本
        QString flagQString;         //获取界面文本

        timeQString = measureResultTableWidget->item(row ,0)->text();
        consistency = measureResultTableWidget->item(row ,1)->text().toFloat();
        flagQString = measureResultTableWidget->item(row ,3)->text();
        TypeQString = measureResultTableWidget->item(row ,4)->text();
        QString t1 = "是否上传 ";
        QString t2 = " 的数据";
        QString text = t1+timeQString+t2;
        MessageDialog msg(text, this, MsgStyle::OKANDCANCEL, true);
        if(QDialog::Accepted != msg.exec())
        {
            return;
        }
        else
        {
            if(timeQString.indexOf(":") == timeQString.lastIndexOf(":"))
            {
               timeQString = QString(timeQString + ":00");
            }
            timeDate = QDateTime::fromString(timeQString,"yyyy-MM-dd hh:mm:ss");
            logger->debug("");
            logger->debug("结果补传");
            logger->debug("时间戳为 %d",timeDate.toTime_t());
            logger->debug("浓度为 %f",consistency);
            for(Uint8 index=0; index<sizeof(resultTypeName)/sizeof(resultTypeName[0]); index++)
            {
                if (0 == TypeQString.toStdString().compare(resultTypeName[index]))
                {
                    resultType = index;
                    logger->debug("类型为 %s",resultTypeName[resultType]);
                }
            }
            for(Uint8 index=0; index<sizeof(flagName)/sizeof(flagName[0]); index++)
            {
                 if (string::npos != flagQString.toStdString().find(flagName[index]))
                 {
                     if (flagSign[index]!=nullptr)
                     {
                         flag = flagSign[index];
                         logger->debug("标识为 %s",flag.data());
                     }
                     else
                     {
                        logger->debug("flag is nullptr");
                     }
                 }
            }
            logger->debug("");
            HJ212Manager::Instance()->ManualUploadMeasureResult(timeDate.toTime_t(),consistency,resultType,flag);
        }
    }
}

void MeasureData::showEvent(QShowEvent *event)
{
    (void)event;
    if(!minDayCaledar->isHidden())
    {
      minDayCaledar->hide();
    }
    if(!maxDayCaledar->isHidden())
    {
      maxDayCaledar->hide();
    }
    this->setFocus();
}

void MeasureData::OnUpdateWidget(UpdateEvent event, System::String message)
{
    (void)message;
    if( event == UpdateEvent::OneKeyClearData)
    {
        ResultManager::Instance()->ClearBackupMeasureRecordFile(m_resultDataname);
        m_resultFiles->ClearRecord();
        minTime->setText(GetMinTime());
        maxTime->setText(GetMaxTime());
        ViewRefresh();
        SpaceInit();
    }
}

void MeasureData::SlotDetailButton()
{
    QList<QTableWidgetItem*> items = measureResultTableWidget->selectedItems();
    if(!items.empty())
    {
        int row = measureResultTableWidget->currentIndex().row();  //获取当前的行
        QString timeStr = measureResultTableWidget->item(row,0)->text();
        if(!timeStr.isEmpty())
        {
            //            QList<MeasureRecord> GetDataByTime(QDateTime datetime);
            QDateTime datetime = QDateTime::fromString(timeStr,"yyyy-MM-dd hh:mm:ss");
            MeasureRecord record;
            QList<MeasureRecord> list = DataBaseManager::Instance()->GetMeasureTable()->GetDataByTime(datetime);
            if(!list.empty())
            {
                record = list.at(0);
            }
            MeasureDataDetailWidget *md = new MeasureDataDetailWidget(record, this);
            md->exec();
        }
    }
    else
    {
        MessageDialog msg(tr("请选中一行再操作"), this, MsgStyle::ONLYOK);
        msg.exec();
    }

}

MeasureData::~MeasureData()
{

}

}

