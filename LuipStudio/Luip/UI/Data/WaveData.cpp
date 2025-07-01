
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDateTime>
#include <QLineEdit>
#include <QLabel>
#include <QFile>
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "WaveData.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Lua/App.h"
#include "Log.h"

using namespace Result;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;

#define AXIS_DEFAULT_VALUE_MAX  1
#define AXIS_DEFAULT_VALUE_MIN   0

int SortData(QPointF data1, QPointF data2);

namespace UI
{

WaveData::WaveData(System::String resultDataname, System::String profileTableName, System::String rangeTableName, QWidget *parent)
    :QWidget(parent),m_consistency("consistency"), m_datetime("dateTime"), m_resultType("resultType"),m_modelType("modelType"),m_curMaxConsistency(0),m_curMinConsistency(0),
      m_profileTableName(profileTableName), m_rangeTableName(rangeTableName),m_resultDataname(resultDataname)
{
    MaintainUserChange::Instance()->Register(this);
    UpdateWidgetManager::Instance()->Register(this);
    ResultManager::Instance()->Register(this);

    m_resultFiles = ResultManager::Instance()->GetWaveUseMeasureRecordFile(m_resultDataname);

    m_lua = LuaEngine::Instance()->GetEngine();

    InitCanvas();
}

WaveData::~WaveData()
{

}

void WaveData::InitCanvas()
{
    m_ObjBackDateTime = GetBackDateTime(6);
    m_ObjNextDateTime = GetBackDateTime(0).addDays(1);

    resize(700, 400);
    move(0, 0);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);                        //设置为无标题栏窗体

    plot = new QwtPlot(); //地址容器，里面存放其他对象  // 不加QString转换会提示无法通过编译，无法进行类型转换
    plot->setFixedSize(650, 400);

    QwtLegend *tempLegend = new QwtLegend;//标注
    plot->insertLegend(tempLegend, QwtPlot::RightLegend);//插入位置，可以分别是上下左右

    // 设置画布
    QwtPlotCanvas *tempCanvas = new QwtPlotCanvas();
    tempCanvas->setBorderRadius(2);//画布四周边框角半径
    plot->setCanvas(tempCanvas);//设置绘图的画布
    plot->plotLayout()->setAlignCanvasToScales(true);//设置对齐画布、坐标轴、刻度
    plot->setAutoReplot(true); // 设置自动重画，相当于更新

    // 设置网格线
    QwtPlotGrid *tempGrid = new QwtPlotGrid;
    tempGrid->enableX(false);
    tempGrid->enableY(true);
    tempGrid->setMajorPen(Qt::lightGray, 0, Qt::DashLine);
    tempGrid->attach(plot);

    backButton = new QPushButton(tr("上一页"));
    backButton->setObjectName("brownButton");
    backButton->setFixedSize(80,40);
    backButton->setFocusPolicy(Qt::NoFocus);

    QFont backFont = backButton->font();
    backFont.setPointSize(14);
    backButton->setFont(backFont);

    QHBoxLayout *backLayout = new QHBoxLayout();
    backLayout->addWidget(backButton);
    backLayout->setSpacing(0);
    backLayout->setContentsMargins(0, 0, 0, 0);

    nextButton = new QPushButton(tr("下一页"));
    nextButton->setObjectName("brownButton");
    nextButton->setFixedSize(80,40);
    nextButton->setFocusPolicy(Qt::NoFocus);

    QFont nextFont = nextButton->font();
    nextFont.setPointSize(14);
    nextButton->setFont(nextFont);

    QHBoxLayout *nextLayout = new QHBoxLayout();
    nextLayout->addWidget(nextButton);
    nextLayout->setSpacing(0);
    nextLayout->setContentsMargins(15, 0, 0, 0);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addLayout(backLayout);
    bottomLayout->addLayout(nextLayout);
    bottomLayout->addSpacing(15);
    bottomLayout->setMargin(0);
    bottomLayout->setAlignment(Qt::AlignRight);

    connect(backButton, SIGNAL(clicked()), this, SLOT(BackItem()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(NextItem()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    WaveDataTableWidget = new QTableWidget();
    QHBoxLayout *dataTableLayOut = new QHBoxLayout();

    mainLayout->addWidget(plot);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(dataTableLayOut);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(5);
    mainLayout->setSpacing(0);

    SetDisplayDays();
    ReadMeasureResultLogFile();
    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        std::sort(category.buffer.begin(), category.buffer.end(), SortData);
        category.curve->attach(plot);
        category.curve->setSamples(category.buffer);
    }
}

void WaveData::SetDisplayDays()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("wareData", itermsTable);

    m_isUnitChange = itermsTable.safe_at("unitChange", m_unitChangeFunc);

    plot->setAxisScaleDraw(QwtPlot::yLeft,new VScaleDraw());//设置y轴的刻度绘画
    this->SetMaxAxisScale(m_curMinConsistency , m_curMaxConsistency);
    plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50); //设置x轴坐标刻度大小  0-350（每天50个点）
    plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));//设置x轴的刻度绘画
    plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);//标签旋转10度

    oolua_ipairs(itermsTable)
    {
        Table itermTable;
        lua->pull(itermTable);
        String str;
        QString text;
        Qt::GlobalColor color;
        itermTable.at("text", str);
        text = QObject::tr(str.c_str());

        itermTable.at("color", color);

        WaveDataCategory category;
        category.curve = new QwtPlotCurve(text);//曲线类
        category.curve->setPen(color, 2);                                    // 设置曲线颜色 粗细
        category.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
        category.curve->setLegendAttribute(category.curve->LegendShowLine);
        m_waveDataCategory.insert(make_pair(text, category));
    }
    oolua_ipairs_end()
}

void WaveData::OnMeasureResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
    if (m_resultDataname == name)
    {
        UpdaterData();
    }
}

void WaveData::OnCalibrateResultAdded(String name, RecordData result)
{
    (void)name;
    (void)result;
}

void WaveData::UpdaterData()
{
    QDateTime objDateTime = QDateTime::currentDateTime();
    if (objDateTime > m_ObjNextDateTime)
    {
        while(true)
        {
            m_ObjBackDateTime = m_ObjBackDateTime.addDays(7);
            m_ObjNextDateTime = m_ObjNextDateTime.addDays(7);
            if (objDateTime <= m_ObjNextDateTime && objDateTime >= m_ObjBackDateTime)
            {
                break;
            }
        }
    }
    plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50);
    plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));
    plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        category.buffer.clear();
    }

    ReadMeasureResultLogFile();

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        std::sort(category.buffer.begin(), category.buffer.end(), SortData);
        category.curve->attach(plot);
        category.curve->setSamples(category.buffer);
    }
}

int WaveData::DateTimeToInt(QDateTime dateTime)
{
    if(dateTime.secsTo(m_ObjBackDateTime) <= 0)
    {
        return m_ObjBackDateTime.secsTo(dateTime) * 50  / (60 * 60 *24);
    }
    else
    {
        return 0;
    }
}

void WaveData::CurrentWaveChange()
{
    this->SetMaxAxisScale(m_curMinConsistency,m_curMaxConsistency);
    plot->autoRefresh();
    plot->setFixedWidth(669);
    plot->setFixedWidth(670);
}

void WaveData::ReadMeasureResultLogFile()
{
    QString resultType;
    QString modelType;
    float consistency;
    QDateTime theDate;
    int i = 0;
    m_curMaxConsistency = 0;
    m_curMinConsistency = 0;
    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());

    while(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()) && i < 5000)
    {
        int ret;
        String str;

        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());

        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(),m_datetime, ret);
        theDate = QDateTime::fromTime_t(ret);

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(),m_resultType, str);
        resultType = QObject::tr(str.c_str());

        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(),m_consistency, consistency);

        m_resultFiles->GetFieldCurrentRecordEnumString(m_resultFiles->GetSelfReaderIndex(), m_modelType, str);
        modelType = QObject::tr(str.c_str());

        if(theDate >= m_ObjBackDateTime && theDate <= m_ObjNextDateTime && (modelType == (QString(m_showModel.c_str()))))
        {
            if (m_waveDataCategory.count(resultType))
            {
                AddPoint(theDate, consistency, m_waveDataCategory[resultType].buffer);
                i++;
            }
        }
        else if(theDate < m_ObjBackDateTime)
        {
            break;
        }
    }
    this->SetMaxAxisScale(m_curMinConsistency,m_curMaxConsistency);
}

void WaveData::AddPoint(QDateTime dateTime, double value, QVector<QPointF> &vector)
{
    if(dateTime.daysTo(m_ObjBackDateTime) <= 0)
    {
        if (m_isUnitChange)
        {
            m_lua->call(m_unitChangeFunc, value, UnitChange::Read);
            m_lua->pull(value);
        }

        if (value>m_curMaxConsistency)
        {
            m_curMaxConsistency=value;
        }

        if (!m_curMinConsistency || (value<m_curMinConsistency))
        {
            m_curMinConsistency=value;
        }

        vector.append(QPointF(DateTimeToInt(dateTime), value));
    }
}

void WaveData::BackItem()
{
    m_ObjBackDateTime = m_ObjBackDateTime.addDays(-7);
    m_ObjNextDateTime = m_ObjNextDateTime.addDays(-7);

    plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50);
    plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));
    plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        category.buffer.clear();
    }

    ReadMeasureResultLogFile();

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        std::sort(category.buffer.begin(), category.buffer.end(), SortData);
        category.curve->attach(plot);
        category.curve->setSamples(category.buffer);
    }
}

void WaveData::showEvent(QShowEvent *event)
{
    (void)event;

    plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50);
    plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));
    plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        category.buffer.clear();
    }

    ReadMeasureResultLogFile();

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        std::sort(category.buffer.begin(), category.buffer.end(), SortData);
        category.curve->attach(plot);
        category.curve->setSamples(category.buffer);
    }
}

void WaveData::OnUserChange()
{
}

void WaveData::NextItem()
{
    m_ObjBackDateTime = m_ObjBackDateTime.addDays(7);
    m_ObjNextDateTime = m_ObjNextDateTime.addDays(7);

    plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50);
    plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));
    plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        category.buffer.clear();
    }

    ReadMeasureResultLogFile();

    for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
    {
        WaveDataCategory &category = iter->second;
        std::sort(category.buffer.begin(), category.buffer.end(), SortData);
        category.curve->attach(plot);
        category.curve->setSamples(category.buffer);
    }
}

QDateTime WaveData::GetBackDateTime(int iBackDays)
{
    // 设置开始的时间
    QDateTime objDateTime = QDateTime::currentDateTime();

    m_resultFiles->MoveToLast(m_resultFiles->GetSelfReaderIndex());//文件位置移到最后
    if(m_resultFiles->HasPrevious(m_resultFiles->GetSelfReaderIndex()))//判断一下文件是否有内容
    {
        m_resultFiles->MovePrevious(m_resultFiles->GetSelfReaderIndex());//向前移一个数据
        int ret = 0;
        m_resultFiles->GetFieldCurrentRecordValue(m_resultFiles->GetSelfReaderIndex(), m_datetime, ret);
        objDateTime = QDateTime::fromTime_t(ret);
    }

    objDateTime.setTime(QTime(0, 0, 0));//清零时分秒
    if(iBackDays < 0)
    {
        iBackDays = 0;
    }
    objDateTime = objDateTime.addDays(-1 * iBackDays);//往前减iBackDays天

    return objDateTime;
}

void WaveData::OnUpdateWidget(UpdateEvent event, System::String message)
{
    if (event == UpdateEvent::ChangeConfigParam)//一键恢复出厂设置
    {
        CurrentWaveChange();
    }
    else if (event == UpdateEvent::ChangeMeasureParam && message == "ParameterWidget")//更改测量参数
    {
        CurrentWaveChange();
    }
    else if (event == UpdateEvent::ModbusChangeParam)//modbus更改测量参数
    {
        CurrentWaveChange();
    }
    else if ((event == UpdateEvent::ClearMeasureData && message == m_resultDataname) || (event == UpdateEvent::OneKeyClearData))//清除测量数据
    {
        m_ObjBackDateTime = GetBackDateTime(6);
        m_ObjNextDateTime = GetBackDateTime(0).addDays(1);

        plot->setAxisScale(QwtPlot::xBottom, 0.0, 7 * 50);
        plot->setAxisScaleDraw(QwtPlot::xBottom,new CTimeScaleDraw(m_ObjBackDateTime, 7 , 50));
        plot->setAxisLabelRotation(QwtPlot::xBottom,-20.0);

        for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
        {
            WaveDataCategory &category = iter->second;
            category.buffer.clear();
        }

        ReadMeasureResultLogFile();

        for (map<QString, WaveDataCategory>::iterator iter = m_waveDataCategory.begin(); iter != m_waveDataCategory.end(); ++iter)
        {
            WaveDataCategory &category = iter->second;
            std::sort(category.buffer.begin(), category.buffer.end(), SortData);
            category.curve->attach(plot);
            category.curve->setSamples(category.buffer);
        }
    }
    else if (event == UpdateEvent::WqimcChangeParam)
    {
        CurrentWaveChange();
    }
}

void WaveData::SetMaxAxisScale(float minConsistency,float maxConsistency)
{
//    qDebug("minConsistency = %f , maxConsistency = %f",minConsistency,maxConsistency);
    float axisValueMax = AXIS_DEFAULT_VALUE_MAX;
    float axisValueMin = AXIS_DEFAULT_VALUE_MIN;
    if (maxConsistency > axisValueMax)
    {
        axisValueMax = 5.0*((int)maxConsistency/5 + 2);
    }

    if(minConsistency > axisValueMin && minConsistency>5)
    {
        axisValueMin = 5.0*((int)minConsistency/5 - 1);
        if (axisValueMax <= axisValueMin || axisValueMin<0)
        {
            axisValueMin = AXIS_DEFAULT_VALUE_MIN;
        }
    }

    /*最大最小值差距太大时，进行特殊处理*/
    if ((axisValueMax-axisValueMin) > 1000)
    {
        axisValueMax += ((int)(axisValueMax*0.2));
    }

//    qDebug("SetMin = %f , SetMax = %f",axisValueMin,axisValueMax);
    plot->setAxisScale(QwtPlot::yLeft,axisValueMin, axisValueMax);//设置y轴坐标刻度大小: 0.0 到 最大量程
}


}
int SortData(QPointF data1, QPointF data2)
{
    return data1.x() < data2.x();
}




