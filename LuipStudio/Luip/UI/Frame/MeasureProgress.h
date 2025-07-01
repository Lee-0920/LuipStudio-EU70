#ifndef UI_FRAME_MEASUREPROGRESS_H
#define UI_FRAME_MEASUREPROGRESS_H
#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include <QPainter>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>

namespace UI
{
class MeasureProgress: public QProgressBar
{
    Q_OBJECT
public:
    explicit MeasureProgress( int actionflag, QProgressBar *parent = 0);
    ~MeasureProgress();

    QWidget *measureWidget;
    QLabel *measureLabel;
    QTimer *curDateTimer;
    void setupUi(QWidget *Form);
    void retranslateUi(QWidget *Form);
    void OnUpdateInfo(QString title, int time);
    void SetDelayTime(int time);

signals:
    void UpdateInfoSignal(QString title, int time);
    void SetDelayTimeSignal(int time);
    void UpdateProgress();
    void BlockingUpdateProgress();
    void StartTimerSignal();
    void StopTimerSignal();

private:
    MeasureProgress *ui;
    virtual void paintEvent(QPaintEvent*);

    void SetProgressContent(const QString name, const QString time, int rate);
    void GetProgressContent(QString &name, QString &time, int &rate);
    QString SetRemainTime(int remain , int fulltime , int flag);

    int ProgressRate(int remain , int fulltime);

    void StartTimer();
    void StopTimer();

    QString m_progressName;
    QString m_progressTime;

    int m_remaintime;
    int m_fulltime;
    int m_progressflag;
    bool m_startFlag;
    int m_progressValue;

    QMutex m_updateMutex;
    QReadWriteLock m_progressContentLock;
    QString m_threadStr;

public slots:
    void SlotUpdateInfo(QString title, int time);
    void slotUpdataDateTime();
    void SetDelayTimeSlot(int time);
};

}
#endif // UI_FRAME_MEASUREPROGRESS_H
