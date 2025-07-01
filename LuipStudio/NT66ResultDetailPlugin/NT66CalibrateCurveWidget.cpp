#include "NT66CalibrateCurveWidget.h"
#include <QVBoxLayout>
#include <QTimer>
#include <qapplication.h>
#include <qeventloop.h>
#include "CurveManager.h"
#include "LuaEngine/LuaEngine.h"
#include "RecordDataPlugin/RecordDataProxy.h"

using namespace Lua;
using namespace OOLUA;
using namespace System;
using namespace Controller;
namespace  ResultDetail
{


NT66CalibrateCurveWidget::NT66CalibrateCurveWidget():
    CalibrateResultDetailWidget()
{
    resize(810, 625);
    move(-5, -5);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);                        //设置为无标题栏窗体
    this->setWindowModality(Qt::WindowModal);//模态

    m_plot = new QwtPlot(); //地址容器，里面存放其他对象  // 不加QString转换会提示无法通过编译，无法进行类型转换

    m_plot->setFixedSize(740, 450);

    // 设置画布
    QwtPlotCanvas *tempCanvas = new QwtPlotCanvas();
    tempCanvas->setBorderRadius(2);//画布四周边框角半径
    m_plot->setCanvas(tempCanvas);//设置绘图的画布
    m_plot->plotLayout()->setAlignCanvasToScales(true);//设置对齐画布、坐标轴、刻度
    m_plot->setAutoReplot(true); // 设置自动重画，相当于更新

    //设置光标
    QwtPlotPicker *picker =  new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                                  QwtPlotPicker::CrossRubberBand, QwtPicker::ActiveOnly,
                                        m_plot->canvas());
    picker->setStateMachine(new QwtPickerDragPointMachine());
    picker->setRubberBandPen(QPen(QColor(255,200,150)));
    picker->setTrackerPen(QColor(Qt::black));

    // 设置网格线
    QwtPlotGrid *tempGrid = new QwtPlotGrid;
    tempGrid->enableX(false);
    tempGrid->enableY(true);
    tempGrid->setMajorPen(Qt::lightGray, 0, Qt::DashLine);
    tempGrid->attach(m_plot);

    QwtLegend *tempLegend = new QwtLegend;//标注
    m_plot->insertLegend(tempLegend, QwtPlot::RightLegend);//插入位置，可以分别是上下左右

    m_plot->setAxisAutoScale(QwtPlot::yLeft, true);
//    m_plot->setAxisAutoScale(QwtPlot::xBottom, true);

    m_blankCurve = new QwtPlotCurve("空白水");//曲线类
    m_blankCurve->setPen(Qt::red, 2);                                    // 设置曲线颜色 粗细
    m_blankCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_blankCurve->setLegendAttribute(m_blankCurve->LegendShowLine);
    m_blankCurve->attach(m_plot);
    m_blankCurve->setSamples(m_blankBuffer);

    m_standardCurve = new QwtPlotCurve("标液");//曲线类
    m_standardCurve->setPen(Qt::green, 2);                                    // 设置曲线颜色 粗细
    m_standardCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_standardCurve->setLegendAttribute(m_standardCurve->LegendShowLine);
    m_standardCurve->attach(m_plot);
    m_standardCurve->setSamples(m_standardBuffer);


    m_closeButton = new QPushButton(tr("关闭"));
    m_closeButton->setObjectName("brownButton");
    m_closeButton->setFixedHeight(40);
    m_closeButton->setFixedWidth(80);
    m_closeButton->setFocusPolicy(Qt::NoFocus);

    QFont closeFont = m_closeButton->font();
    closeFont.setPointSize(12);
    m_closeButton->setFont(closeFont);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_closeButton);
    bottomLayout->setAlignment(Qt::AlignRight);
    bottomLayout->setContentsMargins(0, 0, 40, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_plot);
    mainLayout->addLayout(bottomLayout);
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(Close()));

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    luaEngine->GetLuaValue(state, "setting.ui.resultDetail.calibrate.updateRecord", this->updateRecordFunc);
}

void  NT66CalibrateCurveWidget::Show(QWidget *parent, int roleType)
{
    this->setParent(parent, Qt::FramelessWindowHint | Qt::Dialog);
    this->show();
}

void NT66CalibrateCurveWidget::Close()
{
    this->close();
}

bool NT66CalibrateCurveWidget::UpdateRecord()
{
    bool ret = false;

    try
    {
        lua_State * newState = LuaEngine::Instance()->GetThreadState();
        OOLUA::Lua_function call(newState);
        ret = call(this->updateRecordFunc, &m_recordData);
    }
    catch(OOLUA::Exception e)
    {
        logger->warn("NT66CalibrateCurveWidget::UpdateRecord() => %s", e.what());
    }
    catch(std::exception e)
    {
        logger->warn("NT66CalibrateCurveWidget::UpdateRecord() => %s", e.what());
    }

    return ret;
}

void NT66CalibrateCurveWidget::SetCurves(Uint32 blankIndex, Uint32 stdIndex)
{
    CurveData curve = CurveManager::Instance()->Getcurve(blankIndex);
    float xMin = 0, xMax = 0;
    if (curve.GetLength() > 0)
    {
        Uint32 length = curve.GetLength();
        float begin = curve.GetBegin();
        float end = curve.GetEnd();
        float scanResqlution = (end - begin) / length;
        if (begin < xMin)
        {
            xMin = begin;
        }
        if (end > xMax)
        {
            xMax = end;
        }

        m_blankBuffer.clear();


        float vol = begin;
        for (Uint32 i = 0; i < length; i++)
        {
            float current = curve.GetData(i);
            m_blankBuffer.append(QPointF(vol, current));
            vol += scanResqlution;
        }
    }
    else
    {
        m_blankBuffer.clear();
    }
    m_blankCurve->attach(m_plot);
    m_blankCurve->setSamples(m_blankBuffer);


    curve = CurveManager::Instance()->Getcurve(stdIndex);

    if (curve.GetLength() > 0)
    {
        Uint32 length = curve.GetLength();
        float begin = curve.GetBegin();
        float end = curve.GetEnd();
        float scanResqlution = (end - begin) / length;
        if (begin < xMin)
        {
            xMin = begin;
        }
        if (end > xMax)
        {
            xMax = end;
        }

        m_standardBuffer.clear();

        float vol = begin;
        for (Uint32 i = 0; i < length; i++)
        {
            float current = curve.GetData(i);
            m_standardBuffer.append(QPointF(vol, current));
            vol += scanResqlution;
        }
    }
    else
    {
        m_standardBuffer.clear();
    }
    m_standardCurve->attach(m_plot);
    m_standardCurve->setSamples(m_standardBuffer);
    m_plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
}
}
