#ifndef NT66MEASURECURVEWIDGET_H
#define NT66MEASURECURVEWIDGET_H

#include "ResultDetailPlugin/MeasureResultDetailWidget.h"
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
#include <QDateTime>
#include <QPushButton>
#include <QLabel>
#include "oolua.h"

namespace  ResultDetail
{


class NT66MeasureCurveWidget : public MeasureResultDetailWidget
{
    Q_OBJECT
public:
    NT66MeasureCurveWidget();
    void SetCurves(System::Uint32 sampleIndex, System::Uint32 blankIndex, System::Uint32 stdIndex);
    void Show(QWidget *parent, int roleType = 0);
    bool UpdateRecord();
    void SetResultInfo(QString info);

private:
    QwtPlot *m_plot;
    QwtPlotCurve *m_sampleCurve;
    QwtPlotCurve *m_blankCurve;
    QwtPlotCurve *m_standardCurve;
    QLabel *m_infoLabel;
    QVector<QPointF> m_sampleBuffer;
    QVector<QPointF> m_blankBuffer;
    QVector<QPointF> m_standardBuffer;
    QPushButton *m_closeButton;
    QPushButton *m_exportButton;
    OOLUA::Lua_func_ref updateRecordFunc;

    int m_sampleIndex;
    int m_blankIndex;
    int m_stdIndex;
public slots:
    void Close();
    void SlotExportButton();
};

}
#endif // NT66MEASURECURVEWIDGET_H
