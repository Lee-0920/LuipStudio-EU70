#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QFile>
#include <QDateTime>
#include "CalibrateCurve.h"
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include <algorithm>
#include "UI/Frame/MessageDialog.h"
#include "Log.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "RecordDataPlugin/RecordDataProxy.h"
#include "Common.h"
#include "Lua/App.h"
#include "LuaException.h"
#include "Setting/SettingManager.h"
#include "System/Translate.h"
#include "PrinterManager/PrinterManager.h"

#define RESULT_LINE                         7
#define DATA_MAX_POINT_SIZE     14

using namespace Result;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;
using namespace ResultDetail;
using namespace Configuration;
using namespace PrinterSpace;

namespace UI
{

CalibrateCurve::CalibrateCurve(System::String resultDataname, System::String profileTableName, System::String profileFileName, String curveParamName, QWidget *parent) :
        QWidget(parent),m_dateTimeStr("dateTime"),m_curveBStr("curveB"), m_curveKStr("curveK"),m_resultDataname(resultDataname),m_measureRangeIndex(0),
        m_profileTableName(profileTableName),m_profileFileName(profileFileName),m_curveParamName(curveParamName),m_isDisplayHiddenInfo(false), m_isXYChangeing(false)
{
    MaintainUserChange::Instance()->Register(this);
    ResultManager::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);

    nextFlag = 0;
    backFlag = 0;

    this->resize(650,400);                            //字体
    font.setPointSize(DATA_MAX_POINT_SIZE);

    if(PrinterManager::Instance()->GetMeasureDataPrinter() != nullptr)
    {
        m_calPrintWidget = new CalibrateDataPrintWidget(m_resultDataname, this);
    }
    else
    {
        m_calPrintWidget = nullptr;
    }

    QFont boxfont;                           //字体
    boxfont.setPointSize(12);                //字体大小

    m_resultDetailFiles = ResultManager::Instance()->GetResultDetailUseCalibrateRecordFile(m_resultDataname);

    m_resultFiles = ResultManager::Instance()->GetCalibrateRecordFile(m_resultDataname);
    m_resultFields = m_resultFiles->GetRecordFields();

    m_resultDetailWidget = ResultDetailManager::Instance()->GetCalibrateResultDetailWidget();

    RecordField recordField;

    m_resultFields->GetField(m_dateTimeStr, recordField);
    m_dateTimeStart = recordField.start;

    m_resultFields->GetField(m_curveKStr, recordField);
    m_curveKStart = recordField.start;

    m_resultFields->GetField(m_curveBStr, recordField);
    m_curveBStart = recordField.start;

    LuaEngine* luaEngine = LuaEngine::Instance();
    m_lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();

    Table fieldTable;
    luaEngine->GetLuaValue(state, "setting.ui.calibrateCurve", fieldTable);

    m_isUnitChange = fieldTable.safe_at("unitChange", m_unitChangeFunc);

    m_isCurveParamCurveXYChange = fieldTable.safe_at("curveParamCurveXYChange", m_curveParamCurveXYChangeFunc);

    fieldTable.at("defaultRestore", m_defaultRestore);
    fieldTable.at("saveFile", m_saveFile);
    fieldTable.at("changeCurve", m_changeCurve);
    fieldTable.safe_at("checkCurveExist", m_checkCurveExist);
    fieldTable.at("getCurrentCurve", m_getCurrentCurve);
//    fieldTable.at("checkHistoryCurve", m_checkHistoryCurve);
    m_isDisplayExtraInfo = fieldTable.safe_at("getExtraInfo", m_getExtraInfo);
    m_isExtraToMap = fieldTable.safe_at("extraToMap", m_extraToMap);
    fieldTable.at("toInput", m_toInput);

    string strType;
    string strCurveX = "C";
    fieldTable.at("curveType", strType);

    m_curveType = QString(strType.c_str());
    m_curveX = QString(strCurveX.c_str());

    minTime = new QMyEdit();            //时间检索最小值
    minTime->setFixedHeight(32);
    minTime->setFixedWidth(115);
    minTime->setText(GetMinTime());
    minTime->setFont(boxfont);

    toTime = new QLabel();               //时间检索"-"
    toTime->setText("-");
    toTime->setFixedSize(5,30);
    toTime->setFont(boxfont);

    maxTime = new QMyEdit();          //时间检索最大值
    maxTime->setFixedHeight(32);
    maxTime->setFixedWidth(115);
    maxTime->setText(GetMaxTime());
    maxTime->setFont(boxfont);

    toMapButton = new QPushButton();           //设为当前布局
    toMapButton->setObjectName("brownButton");
    toMapButton->setText(tr("设为当前"));
    toMapButton->setFont(font);
    toMapButton->setFixedSize(90,40);

    inputButton = new QPushButton();
    inputButton->setObjectName("brownButton");
    inputButton->setText(tr("输入"));
    inputButton->setFont(font);
    inputButton->setFixedSize(90,40);

    searchButton = new QPushButton();
    searchButton->setObjectName("brownButton");
    searchButton->setText(tr("检索"));
    searchButton->setFont(font);
    searchButton->setFixedSize(90,40);

    QHBoxLayout *leftBottomLayout = new QHBoxLayout();
    leftBottomLayout->addWidget(minTime);
    leftBottomLayout->addWidget(toTime);
    leftBottomLayout->addWidget(maxTime);
    leftBottomLayout->addSpacing(20);
    leftBottomLayout->addWidget(searchButton);
    leftBottomLayout->addSpacing(20);
    leftBottomLayout->addWidget(toMapButton);
    leftBottomLayout->addSpacing(20);
    leftBottomLayout->addWidget(inputButton);
    leftBottomLayout->addStretch();
    leftBottomLayout->setContentsMargins(0, 5, 0, 0);

    int columnCount = 0;
    fieldTable.at("columnCount", columnCount);

    calibrateTableWidget = new MQtableWidget();
    calibrateTableWidget->setColumnCount(columnCount);
    calibrateTableWidget->setRowCount(RESULT_LINE);
    calibrateTableWidget->setFixedSize(579,321);

    Table table;
    int col = 0;
    oolua_ipairs(fieldTable)
    {
            Table table;
            m_lua->pull(table);

            bool isNeedTarget = false;
            String targetStr;
            isNeedTarget = table.safe_at("targetStr", targetStr);
            QStringList targetList = QString::fromStdString(targetStr).split('+');

            if(!isNeedTarget || (isNeedTarget && FindStringInList(targetList, QString::fromStdString(m_resultDataname))))
            {
                bool isHiddenInfo;
                ShowField showField;
                String text;
                String format;
                int width;

                table.at("name", showField.name);

                if (0 ==  QString::compare("measureRange",QString::fromStdString(showField.name)))
                {
                    m_measureRangeIndex = col;
                }


                table.at("text", text);
                //去除mAbs，量程mAbs更改为标点，测试量程更改为量程
                QString tmpText = QString::fromStdString(text);
                QString filtration = "mAbs";
                tmpText = tmpText.left(tmpText.indexOf(filtration));
                if (tmpText == "量程")
                {
                    tmpText = "标点";
                }
                else if (tmpText == "测试量程")
                {
                    tmpText = "量程";
                }
                m_columnName << tmpText;

                if(table.safe_at("isHiddenInfo", isHiddenInfo))
                {
                    m_items.insert(make_pair(col, isHiddenInfo));
                }
                else
                {
                    m_items.insert(make_pair(col, false));
                }

                table.at("width", width);

                if (col == 1)
                {
                    width += 30;
                }

                calibrateTableWidget->setColumnWidth(col++, width);

                if (table.safe_at("format", format))
                {
                    showField.format = format.c_str();
                }

                m_showFields.push_back(showField);
            }
    }
    oolua_ipairs_end()

    //表头
    calibrateTableWidget->setColumnAndSize(m_columnName,15);
    if(m_showFields.size() != columnCount)
    {
        calibrateTableWidget->setColumnCount(m_showFields.size());
    }
    calibrateTableWidget->horizontalHeader()->setFixedHeight(39); // 设置表头的高度
    calibrateTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:rgb(210,210,210);}");

    QFont dataFont = calibrateTableWidget->font();
    dataFont.setPointSize(10);
    calibrateTableWidget->setFont(dataFont);
    calibrateTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
    calibrateTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    calibrateTableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:18px;}");
    calibrateTableWidget->horizontalScrollBar()->setVisible(false);
    calibrateTableWidget->horizontalScrollBar()->setDisabled(true);
    calibrateTableWidget->verticalScrollBar()->setVisible(false);
    calibrateTableWidget->verticalScrollBar()->setDisabled(true);

    QHBoxLayout *measureResultTableLayout = new QHBoxLayout();
    measureResultTableLayout->addWidget(calibrateTableWidget);

    for(int i =0;i<RESULT_LINE;i++)
    {
        calibrateTableWidget->setRowHeight(i,40);
    }

    calibrateTableWidget->setFont(font);

    currentCurve = new QLabel();
    currentCurve->setFixedHeight(25);

    m_ExtraInfoLabel = new QLabel();
    m_ExtraInfoLabel->setFixedHeight(25);

    QHBoxLayout *leftMiddleLayout = new QHBoxLayout();   //左边中间布局
    leftMiddleLayout->addWidget(currentCurve);
    leftMiddleLayout->addWidget(m_ExtraInfoLabel);
    leftMiddleLayout->setSpacing(30);

    currentCurve->setFont(font);

    double curveK, curveB;
    m_lua->call(m_getCurrentCurve, 0);
    m_lua->pull(curveK);
    m_lua->call(m_getCurrentCurve, 1);
    m_lua->pull(curveB);

//    luaEngine->GetLuaValue(state, String(m_curveParamName + ".curveK"), curveK);
//    luaEngine->GetLuaValue(state, String(m_curveParamName + ".curveB"), curveB);

    this->DisplayCurrentCurve(curveK, curveB);//显示当前标线
    this->DisplayExtraInfo();//显示当前标点浓度

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(measureResultTableLayout);
    leftLayout->addStretch();
    leftLayout->addLayout(leftMiddleLayout);
    leftLayout->addLayout(leftBottomLayout);
    leftLayout->addStretch();
    leftLayout->setContentsMargins(0, 0, 0, 0);

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

    clearButton = new QPushButton();
    clearButton->setObjectName("brownButton");
    clearButton->setText(tr("清空"));
    clearButton->setFont(font);
    clearButton->setFixedSize(80,40);

    curveXYChangeButton = new QPushButton();
    curveXYChangeButton->setObjectName("brownButton");
    QFont fontCurveXYChangeButton = curveXYChangeButton->font();

    fontCurveXYChangeButton.setPointSize(8);

    curveXYChangeButton->setFont(fontCurveXYChangeButton);
    curveXYChangeButton->setFixedSize(80,40);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(toTopButton);
    rightLayout->addWidget(toBackButton);
    rightLayout->addWidget(toNextButton);
    rightLayout->addWidget(toBottomButton);
    rightLayout->addWidget(exportButton);
    rightLayout->addWidget(clearButton);
    if(m_resultDetailWidget != nullptr)
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
    rightLayout->addWidget(curveXYChangeButton);
    if(m_calPrintWidget != nullptr)
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

    connect(toTopButton, SIGNAL(clicked(bool)), this, SLOT(ToTop()));
    connect(toBottomButton, SIGNAL(clicked(bool)), this, SLOT(ToBottom()));
    connect(toNextButton, SIGNAL(clicked(bool)), this, SLOT(ToNext()));
    connect(toBackButton, SIGNAL(clicked(bool)), this, SLOT(ToBack()));
    connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(SearchResult()));
    connect(curveXYChangeButton,SIGNAL(clicked()), this, SLOT(SlotCurveXYChangeButton()));
    connect(exportButton,SIGNAL(clicked()), this, SLOT(SlotExportButton()));
    connect(clearButton,SIGNAL(clicked()), this, SLOT(SlotClearButton()));


    connect(toMapButton, SIGNAL(clicked(bool)), this, SLOT(ToMap()));
    connect(inputButton, SIGNAL(clicked(bool)), this, SLOT(ToInput()));
    connect(minTime, SIGNAL(LineEditClicked()), this, SLOT(ChoseMinDay()));
    connect(maxTime, SIGNAL(LineEditClicked()), this, SLOT(ChoseMaxDay()));
    connect(minDayCaledar, SIGNAL(clicked(const QDate &)), this, SLOT(ChangeMinDay()));
    connect(maxDayCaledar, SIGNAL(clicked(const QDate &)), this, SLOT(ChangeMaxDay()));
    ChangeBottomStatus();
    ReadResult();

    fieldTable.safe_at("getHiddenInfo", m_getHiddenInfo);
    if (m_getHiddenInfo.valid())
    {
        bool ret;
        m_lua->call(m_getHiddenInfo);
        m_lua->pull(ret);
        m_isDisplayHiddenInfo = ret;
        this->DisplayHiddenInfo();
    }

    showCurveXYChangeButton();
}

bool CalibrateCurve::FindStringInList(QStringList srcList, QString dstStr)
{
    bool ret = false;
    QStringListIterator it(srcList);
    while(it.hasNext())
    {
        if(dstStr == it.next())
        {
            ret = true;
            break;
        }
    }

    return ret;
}

void CalibrateCurve::SlotDetailButton()
{
    int row = calibrateTableWidget->currentIndex().row();  //获取当前的行
    if(row!=-1)
    {
        Int64 pos = m_resultDetailFiles->GetReadPos(m_resultDetailFiles->GetSelfReaderIndex());
        if (m_resultDetailFiles->MovePrevious(m_resultDetailFiles->GetSelfReaderIndex(), row + 1))
        {
            RecordData recordData = m_resultDetailFiles->GetCurrentRecord(m_resultDetailFiles->GetSelfReaderIndex());

            m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), pos);

            if (m_resultDetailWidget != nullptr)
            {
                m_resultDetailWidget->SetRecordData(recordData);
                m_resultDetailWidget->UpdateRecord();
                m_resultDetailWidget->Show(this);
            }
        }
    }
}

void CalibrateCurve::SlotCurveXYChangeButton()
{
    m_isXYChangeing = !m_isXYChangeing;
    SearchResult();
    UpdateWidget();
}

void CalibrateCurve::ToMap()
{
    try
    {
        int j = calibrateTableWidget->currentIndex().row();  //获取当前的行
        if(j!=-1)
        {
            QString currentCalibrateTime = calibrateTableWidget->item(j ,0)->text();
            QString currentCalibrate = calibrateTableWidget->item(j ,1)->text();
            float rangeValve = calibrateTableWidget->item(j ,m_measureRangeIndex)->text().toFloat();
            if (!currentCalibrate.isEmpty())
            {
                bool isCurveExist = false;
                m_lua->call(m_checkCurveExist, rangeValve);
                m_lua->pull(isCurveExist);
                if (false == isCurveExist)
                {
                    MessageDialog msg(tr("请选择量程一、二、三对应的校准曲线"), this, MsgStyle::ONLYOK);
                    if(msg.exec()== QDialog::Accepted)
                    {
                        return;
                    }
                }

                String strType = m_curveType.toStdString();
                String strCurveX = m_curveX.toStdString();

                if (m_isCurveParamCurveXYChange && m_isXYChangeing)
                {
                    m_lua->call(m_curveParamCurveXYChangeFunc, strCurveX, strType);
                    m_lua->pull(strType);
                    m_lua->pull(strCurveX);
                }

                currentCurve->setText(Translate::ComplexTranslate("当前标线: " + QString(strType.data()) + " = " + currentCalibrate));

                m_currentCurveKValue = m_currentCurveK[j];
                m_currentCurveBValue = m_currentCurveB[j];

//                QString curveKStr = QString::number(m_currentCurveKValue,'f',9);
//                LuaEngine::Instance()->RunChunk(String( + ".curveK = " + curveKStr.toStdString()));

//                QString curveBStr = QString::number(m_currentCurveBValue,'f',9);
//                LuaEngine::Instance()->RunChunk(String( + ".curveB = " + curveBStr.toStdString()));

                m_lua->call(m_saveFile, this->m_profileTableName, this->m_profileFileName, true);
                m_lua->call(m_changeCurve, rangeValve, currentCalibrateTime.toStdString(), currentCalibrate.toStdString());

                UpdateWidgetManager::Instance()->SlotUpdateWidget(UpdateEvent::ChangeMeasureParam, "CalibrateCurve");

                if (m_isExtraToMap)
                {
                    Int64 pos = m_resultDetailFiles->GetReadPos(m_resultDetailFiles->GetSelfReaderIndex());
                    if (m_resultDetailFiles->MovePrevious(m_resultDetailFiles->GetSelfReaderIndex(), j + 1))
                    {
                        RecordData recordData = m_resultDetailFiles->GetCurrentRecord(m_resultDetailFiles->GetSelfReaderIndex());
                        m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), pos);
                        m_lua->call(m_extraToMap, &recordData, this->m_profileTableName, this->m_curveParamName, this->m_profileFileName);
                    }
                }
            }
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CalibrateCurve::ToMap => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CalibrateCurve::ToMap => %s", e.what());
    }
}

void CalibrateCurve::ShowRow(Uint16 row)
{
    Uint16 column = 0;
    for (vector<ShowField>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
    {
        if (column == 1 && iter->name == "curve")
        {
            double curveK, curveB;
            String strType = m_curveType.toStdString();
            String strCurveX = m_curveX.toStdString();
            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_curveKStr, curveK);
            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_curveBStr, curveB);

            m_currentCurveK[row] = curveK;
            m_currentCurveB[row] = curveB;

            if (m_isUnitChange)
            {
                m_lua->call(m_unitChangeFunc, curveK, UnitChange::Read);
                m_lua->pull(curveK);
            }

            if (m_isCurveParamCurveXYChange && m_isXYChangeing)
            {
                m_lua->call(m_curveParamCurveXYChangeFunc, curveK, curveB);
                m_lua->pull(curveB);
                m_lua->pull(curveK);

                m_lua->call(m_curveParamCurveXYChangeFunc, strCurveX, strType);
                m_lua->pull(strType);
                m_lua->pull(strCurveX);
            }

            QString curveKString;
            QString curveBString;

            if (iter->format.empty())
            {
                curveKString = QString::number(curveK,'f',9);
                curveBString = QString::number(fabs(curveB),'f',9);
            }
            else
            {
                curveKString = QString::asprintf(iter->format.c_str(), curveK);
                curveBString = QString::asprintf(iter->format.c_str(), fabs(curveB));
            }

            if(curveB < 0) //负数求绝对值
            {
                QString data = curveKString +" * " + QString(strCurveX.data()) + " - "+ curveBString;
                this->DataSelfAdaption(row, column,data);
                calibrateTableWidget->item(row, column++)->setText(data);
            }
            else
            {
                QString data = curveKString +" * " + QString(strCurveX.data()) + " + "+ curveBString;
                this->DataSelfAdaption(row, column,data);
                calibrateTableWidget->item(row, column++)->setText(data);
            }
        }
        else
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
                        calibrateTableWidget->item(row, column++)->setText(QString::number(ret));
                    }
                    break;
                case FieldType::Byte:
                    {
                        Byte ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->format.empty())
                        {
                            calibrateTableWidget->item(row, column++)->setText(QString::number(ret));
                        }
                        else
                        {
                            calibrateTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                        }
                    }
                    break;
                case FieldType::Int:
                    {
                        int ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->format.empty())
                        {
                            this->DataSelfAdaption(row, column,QString::number(ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::number(ret));
                        }
                        else
                        {
                            this->DataSelfAdaption(row, column,QString::asprintf(iter->format.c_str(), ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                        }
                    }
                    break;
                case FieldType::Float:
                    {
                        float ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (ret == 0)
                        {
                            QString nll = "--";
                            calibrateTableWidget->item(row, column++)->setText(nll);
                        }
                        else if (iter->format.empty())
                        {
                            this->DataSelfAdaption(row, column,QString::number(ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::number(ret));
                        }
                        else
                        {
                            this->DataSelfAdaption(row, column,QString::asprintf(iter->format.c_str(), ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                        }
                    }
                    break;
                case FieldType::Double:
                    {
                        double ret;
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        if (iter->format.empty())
                        {
                            this->DataSelfAdaption(row, column,QString::number(ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::number(ret,'f',9));
                        }
                        else
                        {
                            this->DataSelfAdaption(row, column,QString::asprintf(iter->format.c_str(), ret));
                            calibrateTableWidget->item(row, column++)->setText(QString::asprintf(iter->format.c_str(), ret));
                        }
                    }
                    break;
                case FieldType::IntArray:
                    break;
                case FieldType::Time:
                    {
                        int ret;
                        QFont font = calibrateTableWidget->item(row, column)->font();
                        font.setPointSize(12);
                        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), iter->name, ret);
                        calibrateTableWidget->item(row, column)->setFont(font);
                        calibrateTableWidget->item(row, column++)->setText(QDateTime::fromTime_t(ret).toString(iter->format.c_str()));
                    }
                    break;
                case FieldType::Enum:
                    {
                        String str;
                        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), iter->name, str);
                        calibrateTableWidget->item(row, column++)->setText(QObject::tr(str.c_str()));
                    }
                    break;
            }
        }
    }
}

void CalibrateCurve::ReadResult()
{
    Uint16 row = 0;

    ViewRefresh();
    SpaceInit();

    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(),
                                    m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex()));
    while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
        ShowRow(row++);
    }
}

void CalibrateCurve::SearchResult()
{
    QDateTime theDate;
    QDateTime theMinDateTime  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime theMaxDateTime  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");
    Uint16 row = 0;

    ViewRefresh();
    SpaceInit();

    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex()));
    while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
    {
        int ret;
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
        theDate = QDateTime::fromTime_t(ret);


        if(theDate <= theMaxDateTime && theDate >= theMinDateTime)
        {
            ShowRow(row++);
        }
    }
}

void CalibrateCurve::ToTop()      // 对应按钮|<
{
    nextFlag = 0;
    backFlag = 0;
    SearchResult();
}

void CalibrateCurve::ToBottom()    // 对应按钮|>
{
    nextFlag = 0;
    backFlag = 0;

    QDateTime theDate;
    QDateTime theMinDateTime  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime theMaxDateTime  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");
    Uint16 row = 0;

    ViewRefresh();
    SpaceInit();

    m_resultFiles->MoveToFirst(m_resultFiles->GetSelfReaderIndex());

        while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
        {
            int ret;
            m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());

            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
            theDate = QDateTime::fromTime_t(ret);

            if(theDate <= theMaxDateTime && theDate >= theMinDateTime)
            {
                row++;
                ShowRow(RESULT_LINE - row);
            }
        }
        m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex()));
        if(row < RESULT_LINE)
        {
            ToTop();
        }
}

void CalibrateCurve::ToBack()    //对应按钮 << ,最新数据
{
    QDateTime theDate;
    QDateTime theMinDateTime  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime theMaxDateTime  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");
    Uint16 row = 0;

    SpaceInit();

    if(m_resultFiles->GetRemainNum(m_resultFiles->GetSelfReaderIndex()) <= RESULT_LINE)
    {
        ToTop();
    }
    else
    {
        if(nextFlag == 1)
        {
            int k = RESULT_LINE;
            while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex())&& k > 0)
            {
                m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());
                k--;
            }
            nextFlag = 0;
            backFlag = 1;
        }
        while(m_resultFiles->HasNext(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
        {
            int ret;
            m_resultFiles->MoveNext(m_resultFiles->GetSelfReaderIndex());

            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
            theDate = QDateTime::fromTime_t(ret);

            if(theDate <= theMaxDateTime && theDate >= theMinDateTime)
            {
                row++;
                ShowRow(RESULT_LINE - row);
            }
        }
        m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex()));
        backFlag = 1;
        if(row < RESULT_LINE)
        {
            ToTop();
        }
    }
}

void CalibrateCurve::ToNext()     //对应按钮 >>
{
    QDateTime theDate;
    QDateTime theMinDateTime  = QDateTime::fromString(minTime->text()+" 00:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime theMaxDateTime  = QDateTime::fromString(maxTime->text()+" 23:59:59","yyyy-MM-dd hh:mm:ss");
    Uint16 row = 0;


    SpaceInit();

    if(m_resultFiles->GetHeadNum(m_resultFiles->GetSelfReaderIndex()) <= RESULT_LINE)
    {
        ToBottom();
    }
    else
    {
        if(backFlag ==1)
        {
            int k = RESULT_LINE;
            while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex())&& k > 0)
            {
               m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());
               k--;
            }
            backFlag = 0;
        }
        m_resultDetailFiles->SetReadPos(m_resultDetailFiles->GetSelfReaderIndex(), m_resultFiles->GetReadPos(m_resultFiles->GetSelfReaderIndex()));
        while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && row < RESULT_LINE)
        {
            int ret;
            m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

            m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
            theDate = QDateTime::fromTime_t(ret);

            if(theDate <= theMaxDateTime && theDate >= theMinDateTime)
            {
                ShowRow(row++);
            }
        }
        nextFlag = 1;
        if(row < RESULT_LINE)
        {
            ToBottom();
        }
    }
}

QString CalibrateCurve::GetMinTime()
{
    QString minTime = QDateTime::currentDateTime().addDays(-7).toString("yyyy-MM-dd");
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    if(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        int ret = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
        minTime = QDateTime::fromTime_t(ret).addDays(-7).toString("yyyy-MM-dd");
    }
    return minTime;
}

QString CalibrateCurve::GetMaxTime()
{
    QString maxTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());
    if(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        int ret = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_dateTimeStr, ret);
        maxTime = QDateTime::fromTime_t(ret).toString("yyyy-MM-dd");
    }
    return maxTime;
}

void CalibrateCurve::DisplayCurrentCurve( double curveK, double curveB)
{
    m_currentCurveKValue = curveK;
    m_currentCurveBValue = curveB;
    String strType = m_curveType.toStdString();
    String strCurveX = m_curveX.toStdString();

    for (vector<ShowField>::iterator iter = m_showFields.begin(); iter != m_showFields.end(); ++iter)
    {
        if (iter->name == "curve")
        {
            QString curveKString;
            QString curveBString;

            if (m_isUnitChange)
            {
                m_lua->call(m_unitChangeFunc, curveK, UnitChange::Read);
                m_lua->pull(curveK);
            }

            if (m_isCurveParamCurveXYChange && m_isXYChangeing)
            {
                m_lua->call(m_curveParamCurveXYChangeFunc, curveK, curveB);
                m_lua->pull(curveB);
                m_lua->pull(curveK);

                m_lua->call(m_curveParamCurveXYChangeFunc, strCurveX, strType);
                m_lua->pull(strType);
                m_lua->pull(strCurveX);
            }

            if (iter->format.empty())
            {
                curveKString = QString::number(curveK,'f',9);
                curveBString = QString::number(fabs(curveB),'f',9);
            }
            else
            {
                curveKString = QString::asprintf(iter->format.c_str(), curveK);
                curveBString = QString::asprintf(iter->format.c_str(), fabs(curveB));
            }

            if(curveB < 0) //负数求绝对值
            {
                currentCurve->setText(Translate::ComplexTranslate("当前标线: " + QString(strType.data()) + " = " + curveKString + " * " + QString(strCurveX.data()) + " - "+ curveBString));
            }
            else
            {
                currentCurve->setText(Translate::ComplexTranslate("当前标线: " + QString(strType.data()) + " = " + curveKString + " * " + QString(strCurveX.data()) + " + "+curveBString));
            }

            break;
        }
    }
}

void CalibrateCurve::DisplayExtraInfo()
{
    try
    {
        if (m_isDisplayExtraInfo)
        {
            String ret;
            m_lua->call(m_getExtraInfo, this->m_profileTableName);
            m_lua->pull(ret);
            m_ExtraInfoLabel->setText(Translate::ComplexTranslate(ret.c_str()));
            m_ExtraInfoLabel->setFont(font);
        }
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("CalibrateCurve::DisplayExtraInfo() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("CalibrateCurve::DisplayExtraInfo() => %s", e.what());
    }
}

QDate CalibrateCurve::GetCurrentMinDate()
{
    QString dateString = minTime->text();
    QStringList dateList = dateString.split("-");
    QString year = dateList.at(0);
    QString month = dateList.at(1);
    QString day = dateList.at(2);

    QDate minDate = QDate(year.toInt(),month.toInt(),day.toInt());
    return minDate;
}

QDate CalibrateCurve::GetCurrentMaxDate()
{
    QString dateString = maxTime->text();
    QStringList dateList = dateString.split("-");
    QString year = dateList.at(0);
    QString month = dateList.at(1);
    QString day = dateList.at(2);

    QDate maxDate = QDate(year.toInt(),month.toInt(),day.toInt());
    return maxDate;
}

void CalibrateCurve::ChoseMinDay()
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

void CalibrateCurve::ChangeMinDay()
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

    minTime->setText(dateString);
    minDayCaledar->hide();
}

void CalibrateCurve::ChoseMaxDay()
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

void CalibrateCurve::ChangeMaxDay()
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

void CalibrateCurve::SpaceInit()
{
    for(int i = 0;i < calibrateTableWidget->rowCount();i++)
    {
        for(int j = 0;j < calibrateTableWidget->columnCount();j++)
        {
            calibrateTableWidget->setItem(i, j, new QTableWidgetItem());
            calibrateTableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
}

void CalibrateCurve::ViewRefresh()
{
    calibrateTableWidget->clear();
    calibrateTableWidget->setColumnAndSize(m_columnName,15);
}

void CalibrateCurve::ChangeBottomStatus()
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
        toMapButton->show();
        inputButton->show();
        exportButton->show();
        clearButton->show();
    }
    else if(LoginDialog::userType == RoleType::Administrator)
    {
        toMapButton->show();
        exportButton->show();
        inputButton->hide();
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
        toMapButton->show();
        exportButton->show();
        inputButton->hide();
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
        toMapButton->hide();
        inputButton->hide();
        exportButton->hide();
        clearButton->hide();
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

void CalibrateCurve::DisplayHiddenInfo()
{
    int width = 0;
    for (map<Uint8, bool>::iterator iter = m_items.begin(); iter != m_items.end(); ++iter)
    {
        Uint8 column = iter->first;
        bool isHiddenInfo = iter->second;
        if (!m_isDisplayHiddenInfo && isHiddenInfo)
        {
            calibrateTableWidget->setColumnHidden(column, true);
        }
        else
        {
            calibrateTableWidget->setColumnHidden(column, false);
            width = width + calibrateTableWidget->columnWidth(column);
        }
    }

    //调整表格大小适应显示
    if(width > calibrateTableWidget->width())
    {
        this->TableExtraInfoAdjust(true);
    }
    else
    {
        this->TableExtraInfoAdjust(false);
    }

}

void CalibrateCurve::TableExtraInfoAdjust(bool isEnable)
{
    if(isEnable)
    {
        calibrateTableWidget->horizontalScrollBar()->setVisible(true);
        calibrateTableWidget->horizontalScrollBar()->setDisabled(false);
        for(int i =0;i<RESULT_LINE;i++)
        {
            calibrateTableWidget->setRowHeight(i,37);
        }
        calibrateTableWidget->horizontalHeader()->setFixedHeight(39);
    }
    else
    {
        calibrateTableWidget->horizontalScrollBar()->setVisible(false);
        calibrateTableWidget->horizontalScrollBar()->setDisabled(true);
        for(int i =0;i<RESULT_LINE;i++)
        {
            calibrateTableWidget->setRowHeight(i,40);
        }
        calibrateTableWidget->horizontalHeader()->setFixedHeight(37);
    }
}

void CalibrateCurve::OnUserChange()
{
    this->ChangeBottomStatus();
}

void CalibrateCurve::OnCalibrateResultAdded(String name, RecordData result)
{
    if (m_resultDataname == name)
    {
        double curveK, curveB;
        result.GetFieldData(m_curveKStart, curveK);
        result.GetFieldData(m_curveBStart, curveB);
        UpdaterCalibrate(curveB, curveK);
    }
}

void CalibrateCurve::UpdaterCalibrate(double curveB, double curveK)
{
    this->DisplayCurrentCurve(curveK, curveB);
    this->DisplayExtraInfo();//显示当前标点浓度

    minTime->setText(QDateTime::currentDateTime().addDays(-7).toString("yyyy-MM-dd"));
    maxTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

    ReadResult();
}

//更新标线，浓度
void CalibrateCurve::UpdateWidget()
{
    if(!minDayCaledar->isHidden())
    {
      minDayCaledar->hide();
    }
    if(!maxDayCaledar->isHidden())
    {
      maxDayCaledar->hide();
    }
    this->setFocus();

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    double curveK, curveB;
    m_lua->call(m_getCurrentCurve, 0);
    m_lua->pull(curveK);
    m_lua->call(m_getCurrentCurve, 1);
    m_lua->pull(curveB);

    this->DisplayCurrentCurve(curveK, curveB);//显示当前标线
    DisplayExtraInfo();
    UpdateCurveXYChangeButtonText();
}

void CalibrateCurve::UpdateCurveXYChangeButtonText()
{
    String strType = m_curveType.toStdString();
    String strCurveX = m_curveX.toStdString();

    if (m_isCurveParamCurveXYChange && m_isXYChangeing)
    {
        m_lua->call(m_curveParamCurveXYChangeFunc, strCurveX, strType);
        m_lua->pull(strType);
        m_lua->pull(strCurveX);
    }

    QString strInfo;
    strInfo = QString(strType.data()) + "=K*" + QString(strCurveX.data()) + "+B";
    curveXYChangeButton->setText(strInfo);
}

void CalibrateCurve::OnMeasureResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
}

void CalibrateCurve::ToInput()
{
    QDialog *checkDialog = new QDialog();

    CNumberKeyboard *numberKey = new CNumberKeyboard(checkDialog);
    checkDialog->setFixedSize(450,150);
    checkDialog->move(this->width()/2 -100,this->height()/2);


    String strType = m_curveType.toStdString();
    String strCurveX = m_curveX.toStdString();

    if (m_isCurveParamCurveXYChange && m_isXYChangeing)
    {
        m_lua->call(m_curveParamCurveXYChangeFunc, strCurveX, strType);
        m_lua->pull(strType);
        m_lua->pull(strCurveX);
    }

    QString strInfo("校准曲线模型: ");
    strInfo = strInfo + QString(strType.data()) + " = K * " + QString(strCurveX.data()) + " + B";

    QLabel *info;
    info = new QLabel(strInfo);
    info->setText(strInfo);
    info->setFixedSize(320,40);
    info->setFont(font);

    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(info);
    infoLayout->addStretch();
    infoLayout->setContentsMargins(10, 0, 0, 0);

    QLabel *curveK;
    curveK = new QLabel();
    curveK->setText(tr("K值"));
    curveK->setFixedSize(30,40);
    curveK->setFont(font);

    QLabel *curveB;
    curveB = new QLabel();
    curveB->setText(tr("B值"));
    curveB->setFixedSize(30,40);
    curveB->setFont(font);

    QLineEdit *curveKEdit;
    curveKEdit = new QLineEdit();
    curveKEdit->setFixedHeight(30);
    curveKEdit->setFixedWidth(140);
    curveKEdit->setFont(font);
    curveKEdit->installEventFilter(numberKey);

    QLineEdit *curveBEdit;
    curveBEdit = new QLineEdit();
    curveBEdit->setFixedHeight(30);
    curveBEdit->setFixedWidth(140);
    curveBEdit->setFont(font);
    curveBEdit->installEventFilter(numberKey);

    QPushButton *isConfirm;
    isConfirm = new QPushButton();
    isConfirm->setObjectName("brownButton");
    isConfirm->setText(tr("确定"));
    isConfirm->setFixedSize(80,35);
    isConfirm->setFont(font);

    QPushButton *isConcel;
    isConcel = new QPushButton();
    isConcel->setObjectName("brownButton");
    isConcel->setText(tr("取消"));
    isConcel->setFixedSize(80,35);
    isConcel->setFont(font);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(curveK);
    inputLayout->addWidget(curveKEdit);
    inputLayout->addStretch();
    inputLayout->addWidget(curveB);
    inputLayout->addWidget(curveBEdit);
    inputLayout->addStretch();
    inputLayout->setContentsMargins(10, 0, 0, 0);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(isConfirm);
    buttonLayout->addWidget(isConcel);
    buttonLayout->setSpacing(40);
    buttonLayout->setContentsMargins(0, 0, 100, 0);

    QVBoxLayout *dialogLayout = new QVBoxLayout();
    dialogLayout->addStretch();
    dialogLayout->addLayout(infoLayout);
    dialogLayout->addLayout(inputLayout);
    dialogLayout->addLayout(buttonLayout);
    dialogLayout->addStretch();

    checkDialog->setLayout(dialogLayout);

    connect(isConfirm,SIGNAL(clicked()),checkDialog,SLOT(accept()));
    connect(isConcel, SIGNAL(clicked()),checkDialog, SLOT(close()));
    if(checkDialog->exec() == QDialog::Rejected)
    {
        return;
    }
    else
    {
        QString pattern("^([+-]?)(\\d{1,6})(\\.\\d{1,6})?$");
        QRegExp rx(pattern);

        if(rx.exactMatch(curveKEdit->text())&&rx.exactMatch(curveBEdit->text()))
        {
            double curveK, curveB;

            curveK = curveKEdit->text().toDouble();
            curveB = curveBEdit->text().toDouble();

            if (m_isUnitChange)
            {
                m_lua->call(m_unitChangeFunc, curveK, UnitChange::Write);
                m_lua->pull(curveK);
            }

            if (m_isCurveParamCurveXYChange && m_isXYChangeing)
            {
                m_lua->call(m_curveParamCurveXYChangeFunc, curveK, curveB);
                m_lua->pull(curveB);
                m_lua->pull(curveK);
            }

//            LuaEngine::Instance()->RunChunk(String(m_curveParamName + ".curveK = " + QString::number(curveK,'f',9).toStdString()));
//            LuaEngine::Instance()->RunChunk(String(m_curveParamName + ".curveB = " + QString::number(curveB,'f',9).toStdString()));

            m_lua->call(m_saveFile, this->m_profileTableName, this->m_profileFileName, true);

            RecordData recordData(ResultManager::Instance()->GetCalibrateRecordFile(m_resultDataname)->GetRecordFields()->GetFieldsSize());
            int time = QDateTime::currentDateTime().toTime_t();

            m_lua->call(m_toInput, &recordData, time, curveK, curveB);
            m_lua->push(&recordData);

            //给结果管理添加校准曲线，结果管理通过观察者更新注册的所有界面，会调用 UpdaterCalibrateSlot 函数更新表格和当前曲线
            ResultManager::Instance()->AddCalibrateRecordSlots(m_resultDataname, recordData);
        }

        if (numberKey)
            delete numberKey;
        if (checkDialog)
            delete checkDialog;
    }
}

void CalibrateCurve::SlotClearButton()
{
    MessageDialog msg(tr("是否确认清空标线记录?"), this, MsgStyle::OKANDCANCEL, true);
    if(QDialog::Accepted != msg.exec())
    {
        return;
    }
    else
    {
        ResultManager::Instance()->ClearBackupCalibrateRecordFile(m_resultDataname);
        ResultManager::Instance()->GetCalibrateRecordFile(m_resultDataname)->ClearRecord();
        minTime->setText(GetMinTime());
        maxTime->setText(GetMaxTime());
        ViewRefresh();
        SpaceInit();
        this->DisplayCurrentCurve(1, 0);
        m_lua->call(m_defaultRestore, this->m_profileTableName, this->m_profileFileName, this->m_curveParamName);
    }
}

void CalibrateCurve::SlotExportButton()
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
        MessageDialog msg(tr("创建目录，校准数据导出失败。"), this);
        msg.exec();
        return;
    }

    if  (m_resultFiles->ExportFiles(LoginDialog::userType))
    {
        result = tr("校准数据导出成功");
        logger->info("校准数据导出成功");
    }
    else
    {
        result = tr("校准数据导出失败");
        logger->info("校准数据导出失败");
    }
    MessageDialog msg(result, this);
    msg.exec();
    return;
}

void CalibrateCurve::showEvent(QShowEvent *event)
{
    (void)event;
    showCurveXYChangeButton();

    SearchResult();
    UpdateWidget();
}

void CalibrateCurve::showCurveXYChangeButton()
{
    // 不能进行转换，或开关关闭，隐藏按钮
    if (m_isCurveParamCurveXYChange)
    {
        bool isEnble = false;
        bool ret = false;

        Table sysTable;
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();
        luaEngine->GetLuaValue(state, "config.system", sysTable);
        ret = sysTable.safe_at("curveXYChange", isEnble);

        if (ret && isEnble)
        {
            curveXYChangeButton->show();
            m_isXYChangeing = true;
        }
        else
        {
            curveXYChangeButton->hide();
            m_isXYChangeing = false;
        }
    }
    else
    {
        curveXYChangeButton->hide();
        m_isXYChangeing = false;
    }
}

void CalibrateCurve::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if ((event == UpdateEvent::ChangeMeasureParam && message != "CalibrateCurve") || //更改测量参数
         event == UpdateEvent::ModbusChangeParam ||
          event ==  UpdateEvent::ChangeConfigParam ||
            event ==  UpdateEvent::WqimcChangeParam)
    {
        LuaEngine* luaEngine = LuaEngine::Instance();
        lua_State * state = luaEngine->GetThreadState();

//        double curveK, curveB, currentRangeValve;
//        m_lua->call(m_getCurrentCurve, 0);
//        m_lua->pull(curveK);
//        m_lua->call(m_getCurrentCurve, 1);
//        m_lua->pull(curveB);
//        m_lua->call(m_getCurrentCurve, 2);
//        m_lua->pull(currentRangeValve);

//        bool ret = true;
//        m_lua->call(m_checkHistoryCurve);
//        m_lua->pull(ret);
//        if (ret)
//        {
//            RecordData recordData(ResultManager::Instance()->GetCalibrateRecordFile(m_resultDataname)->GetRecordFields()->GetFieldsSize());
//            int time = QDateTime::currentDateTime().toTime_t();
//            m_lua->call(m_toInput, &recordData, time, curveK, curveB);
//            m_lua->push(&recordData);
//            //给结果管理添加校准曲线，结果管理通过观察者更新注册的所有界面，会调用 UpdaterCalibrateSlot 函数更新表格和当前曲线
//            ResultManager::Instance()->AddCalibrateRecordSlots(m_resultDataname, recordData);
//        }

        //是否显示隐藏信息开关
        if (m_getHiddenInfo.valid())
        {
            bool ret;
            m_lua->call(m_getHiddenInfo);
            m_lua->pull(ret);
            m_isDisplayHiddenInfo = ret;
            this->DisplayHiddenInfo();
        }
    }
    else if(event == UpdateEvent::OneKeyClearData)
    {
        ResultManager::Instance()->ClearBackupCalibrateRecordFile(m_resultDataname);
        ResultManager::Instance()->GetCalibrateRecordFile(m_resultDataname)->ClearRecord();
        minTime->setText(GetMinTime());
        maxTime->setText(GetMaxTime());
        ViewRefresh();
        SpaceInit();
        this->DisplayCurrentCurve(1, 0);
        m_lua->call(m_defaultRestore, this->m_profileTableName, this->m_profileFileName, this->m_curveParamName);
    }
}

void CalibrateCurve::DataSelfAdaption(int row, int column, QString text)
{
    QFont font ;
    font.setPointSize(DATA_MAX_POINT_SIZE);
    int size = font.pointSize();

    int limitWidth = calibrateTableWidget->columnWidth(column) - 5;

    QFontMetrics fontWidth(font);
    int textWidth = fontWidth.width(text);

    while(size > 11 && textWidth > limitWidth)
    {
        size = size - 1;
        font.setPointSize(size);

        QFontMetrics fontWidth(font);
        textWidth = fontWidth.width(text);
    }

//    qDebug("%s %d %d %d",text.toStdString().c_str(),textWidth,limitWidth,size);

    calibrateTableWidget->item(row,column)->setFont(font);
}

void CalibrateCurve::SlotPrinterButton()
{
    if(m_calPrintWidget != nullptr)
    {
        int iDesktopWidth = QApplication::desktop()->width();
        int iDesktopHeight = QApplication::desktop()->height();

        m_calPrintWidget->Show(this, (int)LoginDialog::userType);
//        m_measureDataPrintWidget->move((iDesktopWidth-m_measureDataPrintWidget->width())/2,(iDesktopHeight-m_measureDataPrintWidget->height())/2);
    }
}

CalibrateCurve::~CalibrateCurve()
{

}

}

