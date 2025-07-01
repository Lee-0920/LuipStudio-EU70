/**
 * @file
 * @brief 屏幕蜂鸣器提醒。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/11/15
 */

#ifndef SYSTEM_LOCKSCREEN_BUZZER_H_
#define SYSTEM_LOCKSCREEN_BUZZER_H_

#include <QWidget>
#include <memory>
#include <iostream>
#include <QProcess>

namespace System
{
namespace Screen
{
class Buzzer:public QWidget
{
    Q_OBJECT
public:
    static Buzzer *Instance();
    Buzzer(QWidget *parent = 0);
    ~Buzzer();
    void Beep(void);
    QProcess *m_QProcess;
private:
    static std::unique_ptr<Buzzer> m_instance;
};

}
}

#endif /* SYSTEM_LOCKSCREEN_BUZZER_H_ */
