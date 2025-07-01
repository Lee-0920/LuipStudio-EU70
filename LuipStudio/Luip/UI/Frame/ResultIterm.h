#ifndef UI_FRAME_RESULTITERM_H
#define UI_FRAME_RESULTITERM_H

#include <QWidget>
#include <QLabel>
#include <QString>

namespace UI
{
class ResultIterm : public QWidget
{
    Q_OBJECT
public:
    explicit ResultIterm(QString strTarget, QString strResult,
                         QString strDateTime, QString strUnit, QWidget *parent = 0);
    ~ResultIterm();

    void SetMeasureTarget(QString strTarget);
    void SetMeasureTargetTC(QString strTarget);
    void SetMeasureTargetIC(QString strTarget);
    void SetMeasureResultTC(QString strResult);
    void SetMeasureResultIC(QString strResult);
    void SetMeasureResult(QString strResult);
    void SetMeasureTime(QString strDateTime);
    void SetMeasureResultFont(int fontSize);

private:

    void DateTimeStringAnalysis(QString &dateTime, QString &date, QString &time);
    int GetPointSize(QString& text, int limitWidth);

    QLabel * m_targetLabel;         // 测量参数类型
    QLabel * m_targetTCLabel;       // 测量参数类型
    QLabel * m_targetICLabel;       // 测量参数类型
    QLabel *m_resultLabel;    //测量结果显示
    QLabel *m_resultTCLabel;    //测量结果显示
    QLabel *m_resultICLabel;    //测量结果显示
    QLabel *m_dateLabel;     //测量结果日期
    QLabel *m_timeLabel;     //测量结果时间
    QLabel *m_unitLabel;     // 单位
    QLabel *m_unitTCLabel;     // 单位
    QLabel *m_unitICLabel;     // 单位
};

}
#endif // UI_FRAME_RESULTITERM_H
