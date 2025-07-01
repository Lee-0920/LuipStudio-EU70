#include "NT66MeasureCurveWidget.h"
#include <QVBoxLayout>
#include <QTimer>
#include <qapplication.h>
#include <qeventloop.h>
#include "CurveManager.h"
#include "LuaEngine/LuaEngine.h"
#include "RecordDataPlugin/RecordDataProxy.h"
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QString>
#include <QList>
#include <QFileInfo>

using namespace Lua;
using namespace OOLUA;
using namespace System;
using namespace Controller;

#ifdef _CS_ARM_LINUX
#define DEST_DIR "/mnt/udisk/NT92_DataCurve_"
#else
#define DEST_DIR "E:/NT92_DataCurve_"
#endif

namespace  ResultDetail
{
NT66MeasureCurveWidget::NT66MeasureCurveWidget():
    MeasureResultDetailWidget()
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

    m_plot->setTitle(QString("Curve_S_B_T"));
    m_plot->setAxisTitle(QwtPlot::yLeft, QString("I/mA"));
    m_plot->setAxisTitle(QwtPlot::xBottom, QString("U/mV"));

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

    m_sampleCurve = new QwtPlotCurve("水样(S)");//曲线类
    m_sampleCurve->setPen(Qt::blue, 2);                                    // 设置曲线颜色 粗细
    m_sampleCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_sampleCurve->setLegendAttribute(m_sampleCurve->LegendShowLine);
    m_sampleCurve->attach(m_plot);
    m_sampleCurve->setSamples(m_sampleBuffer);

    m_blankCurve = new QwtPlotCurve("空白水(B)");//曲线类
    m_blankCurve->setPen(Qt::red, 2);                                    // 设置曲线颜色 粗细
    m_blankCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_blankCurve->setLegendAttribute(m_blankCurve->LegendShowLine);
    m_blankCurve->attach(m_plot);
    m_blankCurve->setSamples(m_blankBuffer);

    m_standardCurve = new QwtPlotCurve("标液(T)");//曲线类
    m_standardCurve->setPen(Qt::green, 2);                                    // 设置曲线颜色 粗细
    m_standardCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);    // 线条光滑化,设置渲染效果，抗锯齿
    m_standardCurve->setLegendAttribute(m_standardCurve->LegendShowLine);
    m_standardCurve->attach(m_plot);
    m_standardCurve->setSamples(m_standardBuffer);


    QFont font;
    font.setPointSize(12);

    m_exportButton = new QPushButton(tr("曲线导出"));
    m_exportButton->setObjectName("brownButton");
    m_exportButton->setFixedHeight(40);
    m_exportButton->setFixedWidth(80);
    m_exportButton->setFocusPolicy(Qt::NoFocus);
    m_exportButton->setFont(font);

    m_closeButton = new QPushButton(tr("关闭"));
    m_closeButton->setObjectName("brownButton");
    m_closeButton->setFixedHeight(40);
    m_closeButton->setFixedWidth(80);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    m_closeButton->setFont(font);

    m_infoLabel = new QLabel();
    m_infoLabel->setObjectName(QStringLiteral("whiteLabel"));
    m_infoLabel->setFixedSize(600, 60);
    m_infoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_infoLabel->setText(" ");
    m_infoLabel->setWordWrap(true);
    m_infoLabel->adjustSize();
    //m_infoLabel->setStyleSheet("QLabel{border:0.5px solid blue;}");
    m_infoLabel->setFont(font);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_infoLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_exportButton);
    bottomLayout->addWidget(m_closeButton);
    bottomLayout->addStretch();
    bottomLayout->setAlignment(Qt::AlignCenter);
    //bottomLayout->setContentsMargins(20, 0, 40, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_plot);
    mainLayout->addLayout(bottomLayout);
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(Close()));
    connect(m_exportButton, SIGNAL(clicked()), this, SLOT(SlotExportButton()));

    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();

    luaEngine->GetLuaValue(state, "setting.ui.resultDetail.measure.updateRecord", this->updateRecordFunc);
}

void  NT66MeasureCurveWidget::Show(QWidget *parent, int roleType)
{
    if(roleType >= 4)
    {
        m_exportButton->setVisible(true);
    }
    else
    {
        m_exportButton->setVisible(false);
    }
    this->setParent(parent, Qt::FramelessWindowHint | Qt::Dialog);
    this->show();
}

void NT66MeasureCurveWidget::Close()
{
    this->close();
}

void NT66MeasureCurveWidget::SlotExportButton()
{
    if(m_sampleBuffer.empty() || m_sampleBuffer.length() != m_blankBuffer.length() || m_sampleBuffer.length() != m_standardBuffer.length())
    {
        logger->info("曲线数据异常");
        return;
    }

#ifdef _CS_ARM_LINUX
    QDir dev("/dev");
    QStringList nameFilters;
    nameFilters<<"sd*";
    QStringList sdList = dev.entryList(nameFilters, QDir::System);
    if(sdList.empty())
    {
        logger->info("曲线导出失败");
        return;
    }
#endif

    std::string sn;
    LuaEngine* luaEngine = LuaEngine::Instance();
    lua_State * state = luaEngine->GetThreadState();
    luaEngine->GetLuaValue(state, "config.info.instrument.sn", sn);


    QString dirPath = DEST_DIR + QString::fromStdString(sn);
    QDir dir;
    if(!dir.mkpath(dirPath))
    {
        logger->info("曲线导出失败");
        return;
    }

    QString fileName = QString("Curve_S%1_B%2_T%3.csv")
            .arg(QString::number(m_sampleIndex))
            .arg(QString::number(m_blankIndex))
            .arg(QString::number(m_stdIndex));
    QString fPath = dirPath+"/"+fileName;
    QFile file(fPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        logger->info("曲线导出失败");
        return;
    }

    QTextStream out(&file);
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    out.setCodec(codec);

    out<<"voltage ,"<<"sample ,"<<"blank ,"<<"standard ,\n";
    for(int i = 0; i < m_sampleBuffer.length(); i++)
    {
        out<<m_sampleBuffer.at(i).x()<<" ,"
           <<m_sampleBuffer.at(i).y()<<" ,"
           <<m_blankBuffer.at(i).y()<<" ,"
           <<m_standardBuffer.at(i).y()<<" ,\n";
    }
    out<< endl;

    file.close();

#ifdef    _CS_ARM_LINUX
    system("sync");
#endif
    logger->info("曲线导出成功");
}

bool NT66MeasureCurveWidget::UpdateRecord()
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

void NT66MeasureCurveWidget::SetResultInfo(QString info)
{
    m_infoLabel->setText(info);
}

void NT66MeasureCurveWidget::SetCurves(Uint32 sampleIndex, Uint32 blankIndex, Uint32 stdIndex)
{
    m_sampleIndex = sampleIndex;
    m_blankIndex = blankIndex;
    m_stdIndex = stdIndex;

    QString title = QString("Curve_S%1_B%2_T%3")
            .arg(QString::number(m_sampleIndex))
            .arg(QString::number(m_blankIndex))
            .arg(QString::number(m_stdIndex));
    m_plot->setTitle(title);

    CurveData curve = CurveManager::Instance()->Getcurve(sampleIndex);
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

        m_sampleBuffer.clear();

        float vol = begin;
        for (Uint32 i = 0; i < length; i++)
        {
            float current = curve.GetData(i);
            m_sampleBuffer.append(QPointF(vol, current));
            vol += scanResqlution;
        }
    }
    else
    {
        m_sampleBuffer.clear();
    }
    m_sampleCurve->attach(m_plot);
    m_sampleCurve->setSamples(m_sampleBuffer);

    curve = CurveManager::Instance()->Getcurve(blankIndex);

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
