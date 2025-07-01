
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
//#include "WaveData.h"

using namespace Result;
using namespace OOLUA;
using namespace std;
using namespace Lua;
using namespace ResultData;

int SortData(QPointF data1, QPointF data2);

namespace UI
{

PeakShape::PeakShape(QWidget *parent)
    :QWidget(parent),m_scanDataLen(0),m_range(1000),m_newDataIndex(0),m_yTopValve(4),m_yBottomValve(3),m_yDisplayPeakHigh(0.1),
      m_Adaptive(true)
{
    MaintainUserChange::Instance()->Register(this);
    Table measureResultTable;
    m_lua = LuaEngine::Instance()->GetEngine();
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    m_toc = (ReactController*)ControllerManager::Instance()->GetController("ReactControllerPlugin");
    luaEngine->GetLuaValue(state, "setting.measureResult", measureResultTable);
    measureResultTable.safe_at("yTopValue",m_yTopValve);
    measureResultTable.safe_at("yBottomValue",m_yBottomValve);
    InitCanvas();
}

PeakShape::~PeakShape()
{

}

void PeakShape::InitCanvas()
{
    memset(m_realData,0,sizeof(m_realData));
    memset(m_TscanBuf,0,sizeof(m_TscanBuf));
    for (int i=0;i<2048;i++)
    {
        m_TscanBuf[0][i]=i;
    }

    resize(700, 400);
    move(0, 0);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);                        //设置为无标题栏窗体

    plot = new QwtPlot(); //地址容器，里面存放其他对象  // 不加QString转换会提示无法通过编译，无法进行类型转换
    plot->setFixedSize(620, 350);

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

    m_adaptButton = new QPushButton();
    m_adaptButton->setObjectName("brownButton");
    m_adaptButton->setText("关闭");
    QFont dataFont = m_adaptButton->font();
    dataFont.setPointSize(14);
    m_adaptButton->setFont(dataFont);
    m_adaptButton->setFixedSize(100,40);

    QLabel *comboBoxText = new QLabel();
    QFont TextFont = comboBoxText->font();
    TextFont.setPointSize(14);
    comboBoxText->setText("自适应:");

    QComboBox *comboBoxPeakHigh = new QComboBox();
    QFont comboBoxPeakHighFont = comboBoxPeakHigh->font();
    comboBoxPeakHighFont.setPointSize(14);
    comboBoxPeakHigh->setFont(comboBoxPeakHighFont);
    comboBoxPeakHigh->setFixedSize(100,40);

    Table itermsTable, table;
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();
    luaEngine->GetLuaValue(state, "setting.measureResult", itermsTable);
    QStringList displayPeakHighList;
    itermsTable.at("displayPeakHighList", table);
    oolua_ipairs(table)
    {
        double value;
        lua->pull(value);
        displayPeakHighList << QString::number(value, 'f', 4);
    }
    oolua_ipairs_end()
    comboBoxPeakHigh->insertItems(0, displayPeakHighList);
    comboBoxPeakHigh->setCurrentIndex(1);
    comboBoxPeakHigh->setCurrentText(displayPeakHighList.at(1));

    QLabel *comboBoxTextPeakHigh = new QLabel();
    QFont PeakHighTextFont = comboBoxTextPeakHigh->font();
    TextFont.setPointSize(14);
    comboBoxTextPeakHigh->setText("缩放比例:");

    QLabel *comboBoxSelectText = new QLabel();
    TextFont = comboBoxSelectText->font();
    TextFont.setPointSize(14);
    comboBoxSelectText->setText("显示:");

    m_comboBoxSelect = new QComboBox();
    QFont comboBoxSelectFont = m_comboBoxSelect->font();
    comboBoxSelectFont.setPointSize(14);
    m_comboBoxSelect->setFont(comboBoxSelectFont);
    m_comboBoxSelect->setFixedSize(100,40);
    m_comboBoxSelect->insertItem(0, "全部");
    m_comboBoxSelect->insertItem(1, "测量");
    m_comboBoxSelect->insertItem(2, "参考");


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
    topLayout->addWidget(comboBoxSelectText);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_comboBoxSelect);
    topLayout->addSpacing(10);
    topLayout->addWidget(comboBoxText);
    topLayout->addSpacing(10);
    topLayout->addWidget(m_adaptButton);
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

    connect(m_adaptButton, SIGNAL(clicked()), this, SLOT(SlotRangeChange()));
    connect(comboBoxPeakHigh,SIGNAL(currentTextChanged(QString)),this,SLOT(SlotPeakhighChange(QString)));
    connect(addYTopButton,SIGNAL(clicked()),this,SLOT(SlotAddTopY()));
    connect(subtractYTopButton,SIGNAL(clicked()),this,SLOT(SlotSubstractTopY()));
    connect(addYBottomButton,SIGNAL(clicked()),this,SLOT(SlotAddBottomY()));
    connect(subtractYBottomButton,SIGNAL(clicked()),this,SLOT(SlotSubtractBottomY()));
    SetDisplayDays();

    float updaterPeriod;
    luaEngine->GetLuaValue(state, "setting.measureResult.updaterPeriod", updaterPeriod);
//    luaEngine->GetLuaValue(state, "setting.measureResult.displayPeakHigh", m_yDisplayPeakHigh);
    qDebug("updaterPeriod %f", updaterPeriod);
//    qDebug("displayPeakHigh %f", m_yDisplayPeakHigh);
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


//    oolua_ipairs(itermsTable)
//    {
//        Table itermTable;
//        lua->pull(itermTable);
//        String str;
//        QString text;
//        Qt::GlobalColor color;
//        itermTable.at("text", str);
//        text = QObject::tr(str.c_str());

//        itermTable.at("color", color);

        m_PeakCategory.curve = new QwtPlotCurve("");//曲线类
        m_PeakCategory.curve->setPen(Qt::GlobalColor::blue, 2);                                    // 设置曲线颜色 粗细
        m_PeakCategory.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿

        m_PeakCategoryRef.curve = new QwtPlotCurve("");//曲线类
        m_PeakCategoryRef.curve->setPen(Qt::GlobalColor::darkRed, 2);                                    // 设置曲线颜色 粗细
        m_PeakCategoryRef.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
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
        m_PeakCategoryRef.curve->attach(plot);
        m_PeakCategoryRef.curve->setSamples(time, value, m_range);

//    }
//    oolua_ipairs_end()
}

void PeakShape::SlotRangeChange()
{
    if(m_Adaptive)
    {
        m_Adaptive = false;
        m_adaptButton->setText("打开");
    }
    else
    {
        m_Adaptive = true;
        m_adaptButton->setText("关闭");
    }
}

void PeakShape::SlotPeakhighChange(QString arg)
{
    double value = arg.toDouble();
    m_yDisplayPeakHigh = value;
}

void PeakShape::SlotAddTopY()
{
    if(m_yTopValve<MAX_SIGNAL)
    {
        m_yTopValve += m_yDisplayPeakHigh;
//        qDebug("SlotAddTopY %f %f", m_yTopValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }
}
void PeakShape::SlotSubstractTopY()
{
    if(m_yTopValve > m_yBottomValve + 2*m_yDisplayPeakHigh)
    {
        m_yTopValve -= m_yDisplayPeakHigh;
//        qDebug("SlotSubstractTopY %f %f", m_yTopValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft, m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }
}

void PeakShape::SlotAddBottomY()
{
    if(m_yBottomValve < m_yTopValve - 2*m_yDisplayPeakHigh)
    {
        m_yBottomValve += m_yDisplayPeakHigh;

//        qDebug("SlotAddBottomY %f %f", m_yBottomValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft, m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }

}

void PeakShape::SlotSubtractBottomY()
{
    if(m_yBottomValve > 2*m_yDisplayPeakHigh)
    {
        m_yBottomValve -= m_yDisplayPeakHigh;

//        qDebug("SlotSubtractBottomY %f %f", m_yBottomValve, m_yDisplayPeakHigh);
        plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }

}

void PeakShape::SlotUpdaterData()
{
    int len;
    double time[m_range] = {0};
    double valve[m_range] = {0};
    double valveRef[m_range] = {0};
    m_PeakCategory.buffer.clear();
    m_PeakCategoryRef.buffer.clear();

    double data = m_toc->GetData();
    double dataRef = m_toc->GetDataRef();

    if(m_newDataIndex >= 5)
    {
        double validSum = 0;
        double validSumRef = 0;
        int throwNum = 0;
        int throwNumRef = 0;
        if(data > 0.001)
        {
            validSum = data;
        }
        else
        {
            throwNum++;
        }

        for(int i = 1;i<5;i++)
        {
            if(m_realData[i] > 0.001)
            {
                validSum+= m_realData[i-1];
                m_realData[i-1] = m_realData[i];
            }
            else
            {
                throwNum++;
            }
        }
        m_realData[4] = data;
        if(throwNum<5)
        {
            m_TscanBuf[1][m_newDataIndex] = validSum/(5-throwNum);
        }
        else
        {
            m_TscanBuf[1][m_newDataIndex] = data;
        }

        if(dataRef > 0.001)
        {
            validSumRef = dataRef;
        }
        else
        {
            throwNumRef++;
        }

        for(int i = 1;i<5;i++)
        {
            if(m_realDataRef[i] > 0.001)
            {
                validSumRef+= m_realDataRef[i-1];
                m_realDataRef[i-1] = m_realDataRef[i];
            }
            else
            {
                throwNumRef++;
            }
        }
        m_realDataRef[4] = dataRef;
        if(throwNumRef<5)
        {
            m_TscanBufRef[1][m_newDataIndex] = validSumRef/(5-throwNumRef);
        }
        else
        {
            m_TscanBufRef[1][m_newDataIndex] = dataRef;
        }
    }
    else
    {
         m_TscanBuf[1][m_newDataIndex] = data;
         m_realData[m_newDataIndex] = data;
         m_TscanBufRef[1][m_newDataIndex] = dataRef;
         m_realDataRef[m_newDataIndex] = dataRef;
    }
    m_newDataIndex++;
    int max = m_newDataIndex;
    double maxV = 0, minV = MAX_SIGNAL;
    if (max>=m_range)
    {
        len = m_range;
        for(int i = 0; i < len; i++)
        {
            time[i] = i;
            valve[i] = m_TscanBuf[1][max-(m_range-i)];
            valveRef[i] = m_TscanBufRef[1][max-(m_range-i)];
            if(valve[i] > maxV)
            {maxV = valve[i];}
            if((valve[i] < minV)&&(qAbs(valve[i] - 0) > PRECISE))
            {minV = valve[i];}
        }
    }
    else
    {
        len = m_newDataIndex;
        for(int i = 0; i < len; i++)
        {
            time[i] = m_TscanBuf[0][i];
            valve[i] = m_TscanBuf[1][i];
            valveRef[i] = m_TscanBufRef[1][i];
            if(valve[i] > maxV)
            {maxV = valve[i];}
            if((valve[i] < minV)&&(qAbs(valve[i] - 0) > PRECISE))
            {minV = valve[i];}
        }
    }

    //自适应模式下调整Y轴刻度值
    if(m_Adaptive)
    {
        if (maxV+m_yDisplayPeakHigh >= MAX_SIGNAL || qAbs(maxV - 0) < PRECISE)
        {
            m_yTopValve = MAX_SIGNAL;
        }
        else
        {
            m_yTopValve = maxV + m_yDisplayPeakHigh;
        }

        if (minV-m_yDisplayPeakHigh <= 0 || qAbs(minV - 0) < PRECISE)
        {
            m_yBottomValve = 0.1;
        }
        else
        {
            m_yBottomValve = minV - m_yDisplayPeakHigh;
        }
        plot->setAxisScale(QwtPlot::yLeft,m_yBottomValve, m_yTopValve);//设置y轴坐标刻度大小: 0.0 到 最大量程
    }

    plot->setAxisScale(QwtPlot::xBottom,0, m_range);//设置x轴的刻度绘画

    if(m_range > 500)
    {
        m_PeakCategory.curve->setPen(Qt::GlobalColor::blue, 3);                                    // 设置曲线颜色 粗细
    }
    else
    {
        m_PeakCategory.curve->setPen(Qt::GlobalColor::blue, 1.5);                                    // 设置曲线颜色 粗细
    }
    m_PeakCategory.curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_PeakCategory.curve->setLegendAttribute(m_PeakCategory.curve->LegendShowLine);
    m_PeakCategory.curve->attach(plot);
    if(m_comboBoxSelect->currentIndex() == 1)//测量
    {
        m_PeakCategory.curve->setSamples(time, valve, len);
        m_PeakCategoryRef.curve->hide();
    }
    else if(m_comboBoxSelect->currentIndex() == 2)//参考
    {
        m_PeakCategoryRef.curve->setSamples(time, valveRef, len);
        m_PeakCategory.curve->hide();
    }
    else
    {
        m_PeakCategory.curve->setSamples(time, valve, len);
        m_PeakCategoryRef.curve->setSamples(time, valveRef, len);
        m_PeakCategory.curve->show();
        m_PeakCategoryRef.curve->show();
    }

    if ((2000-1) <= m_newDataIndex)
    {
        double tmpData[2][1024] = {0};
        double tmpDataRef[2][1024] = {0};
        for(int i= 0; i<1000; i++)
        {
            tmpData[1][i]=m_TscanBuf[1][i+1000];
            tmpDataRef[1][i]=m_TscanBufRef[1][i+1000];
        }
        memset(m_TscanBuf,0,sizeof(m_TscanBuf));
        memset(m_TscanBufRef,0,sizeof(m_TscanBufRef));
        for(int i= 0; i<1000; i++)
        {
            m_TscanBuf[1][i]=tmpData[1][i];
            m_TscanBufRef[1][i]=tmpDataRef[1][i];
        }
        m_newDataIndex = 1000-1;
    }
}

void PeakShape::OnUserChange()
{
//    if((LoginDialog::userType == Administrator))
//    {
//        confirmButton->setEnabled(true);
//        dayEdit->setEnabled(true);
//        timeEdit->setEnabled(true);
//    }
//    else
//    {
//        confirmButton->setEnabled(false);
//        dayEdit->setEnabled(false);
//        timeEdit->setEnabled(false);
//    }
}
}








