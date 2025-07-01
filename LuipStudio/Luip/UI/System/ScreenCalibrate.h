#ifndef UI_SYSTEM_SCREENCALIBRATE_H
#define UI_SYSTEM_SCREENCALIBRATE_H
#include <QWidget>
#include <QTimer>
#include "UI/Frame/MessageDialog.h"

namespace UI
{

class ScreenCalibrate: public QWidget
{
    Q_OBJECT
public:
    ScreenCalibrate(QWidget *parent = 0);
    ~ScreenCalibrate();
private:
    QTimer *m_timer;//倒计时定时器
    int m_timerCnt;//倒计时变量
    MessageDialog *m_msgBox;//倒计时消息框
private slots:
    void StartCalibrate();
};

}
#endif // UI_SYSTEM_SCREENCALIBRATE_H
