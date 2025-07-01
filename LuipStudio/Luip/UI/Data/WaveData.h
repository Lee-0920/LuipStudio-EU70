#ifndef WAVEDATA_H
#define WAVEDATA_H

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

using Result::IResultNotifiable;

namespace UI
{

struct WaveDataCategory
{
    QVector<QPointF> buffer;
    QwtPlotCurve *curve;
};

class WaveData: public QWidget , public IUserChangeNotifiable, public IUpdateWidgetNotifiable , public IResultNotifiable
{
    Q_OBJECT
public:
    explicit WaveData(System::String resultDataname, System::String profileTableName, System::String rangeTableName, QWidget *parent = 0);
    ~WaveData();
    void showEvent(QShowEvent *event);
    void OnUserChange();
    void OnUpdateWidget(UpdateEvent event, System::String message);
    virtual void OnMeasureResultAdded(String name, ResultData::RecordData result);

    virtual void OnCalibrateResultAdded(String name, ResultData::RecordData result);

public slots:
    void BackItem();
    void NextItem();

private:
    QTableWidget *WaveDataTableWidget;
    QwtPlot *plot;

    QDateTime m_ObjBackDateTime;
    QDateTime m_ObjNextDateTime;
    int display;
    QPushButton *backButton;
    QPushButton *nextButton;

    Result::MeasureRecordFile *m_resultFiles;
    const String m_consistency;
    const String m_datetime;
    const String m_resultType;
    const String m_modelType;

    float m_curMaxConsistency;
    float m_curMinConsistency;

    std::map<QString, WaveDataCategory>m_waveDataCategory;

    OOLUA::Script *m_lua;
    System::String m_profileTableName;
    System::String m_rangeTableName;
    System::String m_resultDataname;

    bool m_isUnitChange;
    OOLUA::Lua_func_ref m_unitChangeFunc;

    void AddPoint(QDateTime dateTime, double value, QVector<QPointF> &vector);
    void SetDisplayDays();
    int DateTimeToInt(QDateTime dateTime);
    void InitCanvas();
    void ReadMeasureResultLogFile();
    QDateTime GetBackDateTime(int iBackDays);
    void CurrentWaveChange();
    void UpdaterData();
    void SetMaxAxisScale(float minConsistency,float maxConsistency);
    String m_showModel;
};

class CTimeScaleDraw : public QwtScaleDraw
{
public:
    CTimeScaleDraw(const QDateTime base, const int day, const int pointCount)
        : baseTime(base)
        , displayDay(day)
        , oneDayPoints(pointCount)
    {
    }

    virtual QwtText label(double v) const
    {
        QDateTime dateTime = baseTime.addSecs(static_cast<uint>(v) * 60 *60 * 24 / 50);

        return dateTime.toString("yyyy-MM-dd\nhh:mm");
    }

private:
    QDateTime baseTime;
    int displayDay;
    int oneDayPoints;
};

class VScaleDraw: public QwtScaleDraw        //自画坐标轴
{
public:
    VScaleDraw()
    {

    }
    virtual QwtText label(double v) const  //重绘坐标轴刻度值
    {
       return QwtText( QString::number(v));  //默认的大刻度的时候才会显示
    }
};

}
#endif // WAVEDATA_H
