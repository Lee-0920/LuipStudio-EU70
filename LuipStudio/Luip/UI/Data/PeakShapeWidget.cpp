
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDateTime>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QFile>
#include "UI/UserChangeManager/UserChangeManager.h"
#include "UI/Frame/LoginDialog.h"
#include "PeakShapeWidget.h"
#include "LuaEngine/LuaEngine.h"
#include "UI/Frame/UpdateWidgetManager.h"
#include "Lua/App.h"
#include "WaveData.h"

using namespace Result;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;

int SortData(QPointF data1, QPointF data2);

namespace UI
{

PeakShape::PeakShape(QWidget *parent)
    :QWidget(parent),m_scanDataLen(0),m_range(200),m_newDataIndex(0),m_yTopValve(5),m_yBottomValve(0),m_yDisplayPeakHigh(1)
{
    m_lua = LuaEngine::Instance()->GetEngine();
    InitCanvas(); 
}

PeakShape::~PeakShape()
{

}

void PeakShape::InitCanvas()
{
    memset(m_TscanBuf,0,sizeof(m_TscanBuf));
    for (int i=0;i<2048;i++)
    {
        m_TscanBuf[0][i]=i;
    }

    resize(700, 400);
    move(0, 0);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);                        //设置为无标题栏窗体

    plot = new QwtPlot(); //地址容器，里面存放其他对象  // 不加QString转换会提示无法通过编译，无法进行类型转换
    plot->setFixedSize(640, 350);

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

    QComboBox *comboBox = new QComboBox();
    QFont comboBoxFont = comboBox->font();
    comboBoxFont.setPointSize(14);
    comboBox->setFont(comboBoxFont);
    comboBox->setFixedSize(100,40);
    comboBox->addItem(tr("0~200"));
    comboBox->addItem(tr("0~1000"));

    QLabel *comboBoxText = new QLabel();
    QFont TextFont = comboBoxText->font();
    TextFont.setPointSize(14);
    comboBoxText->setText("图形区间:");

    QComboBox *comboBoxPeakHigh = new QComboBox();
    QFont comboBoxPeakHighFont = comboBoxPeakHigh->font();
    comboBoxFont.setPointSize(14);
    comboBoxPeakHigh->setFont(comboBoxFont);
    comboBoxPeakHigh->setFixedSize(100,40);
    comboBoxPeakHigh->addItem(tr("0.005"));
    comboBoxPeakHigh->addItem(tr("0.010"));
    comboBoxPeakHigh->addItem(tr("0.05"));
    comboBoxPeakHigh->addItem(tr("0.1"));
    comboBoxPeakHigh->addItem(tr("0.5"));

    QLabel *comboBoxTextPeakHigh = new QLabel();
    QFont PeakHighTextFont = comboBoxTextPeakHigh->font();
    TextFont.setPointSize(14);
    comboBoxTextPeakHigh->setText("缩放比例:");


    QFont font;
    font.setPointSize(40);
    font.setWeight(QFont::Black);

    QPushButton *addYTopButton = new QPushButton();
    addYTopButton->setFont(font);
    addYTopButton->setFixedSize(40,40);
    addYTopButton->setObjectName("brownButton");
    addYTopButton->setText("+");

    QPushButton *subtractYTopButton = new QPushButton();
    subtractYTopButton->setFont(font);
    subtractYTopButton->setFixedSize(40,40);
    subtractYTopButton->setObjectName("brownButton");
    subtractYTopButton->setText("-");


    QPushButton *addYBottomButton = new QPushButton();
    addYBottomButton->setFont(font);
    addYBottomButton->setFixedSize(40,40);
    addYBottomButton->setObjectName("brownButton");
    addYBottomButton->setText("+");

    QPushButton *subtractYBottomButton = new QPushButton();
    subtractYBottomButton->setFont(font);
    subtractYBottomButton->setFixedSize(40,40);
    subtractYBottomButton->setObjectName("brownButton");
    subtractYBottomButton->setText("-");

    QVBoxLayout *YTopLayout = new QVBoxLayout();
    YTopLayout->addSpacing(10);
    YTopLayout->addWidget(addYTopButton);
    YTopLayout->addSpacing(10);
    YTopLayout->addWidget(subtractYTopButton);
    YTopLayout->addStretch();
    YTopLayout->addSpacing(10);
    YTopLayout->addWidget(addYBottomButton);
    YTopLayout->addSpacing(10);
    YTopLayout->addWidget(subtractYBottomButton);
    YTopLayout->addSpacing(40);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(comboBoxText);
    topLayout->addSpacing(10);
    topLayout->addWidget(comboBox);
    topLayout->addSpacing(10);
    topLayout->addWidget(comboBoxTextPeakHigh);
    topLayout->addSpacing(10);
    topLayout->addWidget(comboBoxPeakHigh);
    topLayout->addSpacing(10);
    topLayout->setAlignment(Qt::AlignLeft);

    QHBoxLayout *dataTableLayOut = new QHBoxLayout();
   dataTableLayOut->addLayout(YTopLayout);
   dataTableLayOut->addSpacing(10);
   dataTableLayOut->addWidget(plot);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(5);
    mainLayout->addLayout(dataTableLayOut);
    mainLayout->setMargin(5);
    mainLayout->setSpacing(0);

    connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotRangeChange(int)));
    connect(comboBoxPeakHigh,SIGNAL(currentIndexChanged(int)),this,SLOT(SlotPeakhighChange(int)));
    connect(addYTopButton,SIGNAL(clicked()),this,SLOT(SlotAddTopY()));
    connect(subtractYTopButton,SIGNAL(clicked()),this,SLOT(SlotSubstractTopY()));
    connect(addYBottomButton,SIGNAL(clicked()),this,SLOT(SlotAddBottomY()));
    connect(subtractYBottomButton,SIGNAL(clicked()),this,SLOT(SlotSubtractBottomY()));
    SetDisplayDays();

    //刷新频率必须与上报频率一致
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    float updaterPeriod;
    luaEngine->GetLuaValue(state, "setting.measureResult.updaterPeriod", updaterPeriod);
    luaEngine->GetLuaValue(state, "setting.measureResult.displayPeakHigh", m_yDisplayPeakHigh);
    qDebug("updaterPeriod %f", updaterPeriod);
    qDebug("displayPeakHigh %f", m_yDisplayPeakHigh);
    timer = new QTimer();
    connect( timer, SIGNAL(timeout()), this, SLOT(SlotUpdaterData()));
    timer->start(updaterPeriod * 1000);
}

void PeakShape::SetDisplayDays()
{
    Script *lua = LuaEngine::Instance()->GetEngine();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("peakData", itermsTable);

    plot->setAxisScaleDraw(QwtPlot::yLeft,new VCScaleDraw());//设置y轴的刻度绘画
    plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    plot->setAxisScale(QwtPlot::xBottom, 0, m_range); //设置x轴坐标刻度大小  0-350（每天50个点）


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

        m_PeakCategory.curve = new QwtPlotCurve(text);//曲线类
        m_PeakCategory.curve->setPen(color, 2);                                    // 设置曲线颜色 粗细
        m_PeakCategory.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
//        m_PeakCategory.curve->setLegendAttribute(m_PeakCategory.curve->LegendShowLine);
        double time[m_range] = {0};
        double value[m_range] = {0};
        for(int i = 0; i < m_range; i++)
        {
            value[i] = 200;
            time[i] = i;
        }
        m_PeakCategory.curve->attach(plot);
        m_PeakCategory.curve->setSamples(time, value, m_range);

    }
    oolua_ipairs_end()
}

void PeakShape::SlotRangeChange(int index)
{
    if (0 == index)
    {
        m_range = 200;
    }
    else if (1 == index)
    {
        m_range = 1000;
    }
}

void PeakShape::SlotPeakhighChange(int index)
{
    if (0 == index)
    {
        m_yDisplayPeakHigh = 0.005;
        qDebug("item %d", index);
    }
    else if (1 == index)
    {
        m_yDisplayPeakHigh = 0.01;
        qDebug("item %d", index);
    }
    else if (2 == index)
    {
        m_yDisplayPeakHigh = 0.05;
        qDebug("item %d", index);
    }
    else if (3 == index)
    {
        m_yDisplayPeakHigh = 0.1;
        qDebug("item %d", index);
    }
    else if (4 == index)
    {
        m_yDisplayPeakHigh = 0.5;
        qDebug("item %d", index);
    }
}

void PeakShape::SlotAddTopY()
{
    if(m_yTopValve<5)
    {
        m_yTopValve += m_yDisplayPeakHigh;
        qDebug("SlotAddTopY %f %f", m_yTopValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }
}
void PeakShape::SlotSubstractTopY()
{
    if(m_yTopValve > m_yBottomValve + 2*m_yDisplayPeakHigh)
    {
        m_yTopValve -= m_yDisplayPeakHigh;
        qDebug("SlotSubstractTopY %f %f", m_yTopValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft, m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }
}

void PeakShape::SlotAddBottomY()
{
    if(m_yBottomValve < m_yTopValve - 2*m_yDisplayPeakHigh)
    {
        m_yBottomValve += m_yDisplayPeakHigh;

        qDebug("SlotAddBottomY %f %f", m_yBottomValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft, m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }

}

void PeakShape::SlotSubtractBottomY()
{
    if(m_yBottomValve > 2*m_yDisplayPeakHigh)
    {
        m_yBottomValve -= m_yDisplayPeakHigh;

        qDebug("SlotSubtractBottomY %f %f", m_yBottomValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }

}

void PeakShape::SlotUpdaterData()
{
    int len;
    double time[m_range] = {0};
    double valve[m_range] = {0};
    m_PeakCategory.buffer.clear();

    Script *lua = LuaEngine::Instance()->GetEngine();
    Table table, itermsTable;
    itermsTable.bind_script(*lua);
    itermsTable.set_table("setting");
    itermsTable.at("ui", table);
    table.at("runStatus", itermsTable);

    Lua_func_ref func;
    itermsTable.at("getOpticalData", func);

    double data;
    lua->call(func);
    lua->pull(data);
    m_TscanBuf[1][m_newDataIndex] = data;
    m_newDataIndex++;
    int max = m_newDataIndex;
    if (max>=m_range)
    {
        len = m_range;
        for(int i = 0; i < len; i++)
        {
            time[i] = i;
            valve[i] = m_TscanBuf[1][max-(m_range-i)];
        }
    }
    else
    {
        len = m_newDataIndex;
        for(int i = 0; i < len; i++)
        {
            time[i] = m_TscanBuf[0][i];
            valve[i] = m_TscanBuf[1][i];
        }
    }
    plot->setAxisScale(QwtPlot::xBottom,0, m_range);//设置x轴的刻度绘画

    if(m_range > 500)
    {
        m_PeakCategory.curve->setPen(Qt::GlobalColor::blue, 2);                                    // 设置曲线颜色 粗细
    }
    else
    {
        m_PeakCategory.curve->setPen(Qt::GlobalColor::blue, 1.5);                                    // 设置曲线颜色 粗细
    }
    m_PeakCategory.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_PeakCategory.curve->setLegendAttribute(m_PeakCategory.curve->LegendShowLine);
    m_PeakCategory.curve->attach(plot);
    m_PeakCategory.curve->setSamples(time, valve, len);

    if ((2000-1) <= m_newDataIndex)
    {
        double tmpData[2][1024] = {0};
        for(int i= 0; i<1000; i++)
        {
            tmpData[1][i]=m_TscanBuf[1][i+1000];
        }
        memset(m_TscanBuf,0,sizeof(m_TscanBuf));
        for(int i= 0; i<1000; i++)
        {
            m_TscanBuf[1][i]=tmpData[1][i];
        }
        m_newDataIndex = 1000-1;
    }
}

}








