#ifndef PEAKSHAPEWIDGET_H
#define PEAKSHAPEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_scale_draw.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_picker_machine.h>
#include<qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_column_symbol.h>
#include <qwt_series_data.h>
#include <qpen.h>
#include <qwt_symbol.h>
#include <qwt_picker_machine.h>
#include <QTableWidget>
#include <QMap>
#include <qwt_scale_draw.h>
#include <QDateTime>
#include "ResultManager/ResultManager.h"
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/IUpdateWidgetNotifiable.h"

namespace UI
{

struct PeakDataCategory
{
    QVector<QPointF> buffer;
    QwtPlotCurve *curve;
};

class PeakShape: public QWidget
{
    Q_OBJECT
public:
    explicit PeakShape(QWidget *parent = 0);
    ~PeakShape();

private slots:
    void SlotUpdaterData();
    void SlotRangeChange(int index);
    void SlotPeakhighChange(int index);
    void SlotAddTopY();
    void SlotSubstractTopY();
    void SlotAddBottomY();
    void SlotSubtractBottomY();

private:
    QwtPlot *plot;

    std::map<QString, PeakDataCategory>m_peakDataCategory;
    OOLUA::Script *m_lua;
    PeakDataCategory m_PeakCategory;

    void SetDisplayDays();
    void InitCanvas();
    double m_scanBuf[4096];
    double m_TscanBuf[2][2048];
    int m_scanDataLen;
    int m_newDataIndex;
    int m_range;
    int m_changeCount;
    float m_yTopValve;
    float m_yBottomValve;
    float m_yDisplayPeakHigh;
    QTimer *timer;
};

class VCScaleDraw: public QwtScaleDraw        //自画坐标轴
{
public:
    VCScaleDraw()
    {

    }
    virtual QwtText label(double v) const  //重绘坐标轴刻度值
    {
       return QwtText( QString::number(v));  //默认的大刻度的时候才会显示
    }
};

}

#endif // PEAKSHAPEWIDGET_H
