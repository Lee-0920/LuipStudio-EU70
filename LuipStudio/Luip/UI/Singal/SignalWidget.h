#ifndef SIGNALWIDGET_H
#define SIGNALWIDGET_H
#include <QWidget>
#include "UI/UserChangeManager/IUserChangeNotifiable.h"
#include "UI/Frame/MQtableWidget.h"
#include "System/Types.h"
#include <QTimer>
#include "SignalManager/IUpdateSignalNotifiable.h"

using namespace System;

namespace UI
{
struct Signal
{
    String name;
    String format;
};

class SignalWidget:
        public QWidget,
        public IUpdateSignalNotifiable,
        public IUserChangeNotifiable
{
    Q_OBJECT
public:
    explicit SignalWidget(QWidget *parent = 0);
    ~SignalWidget();

    void OnUserChange();
    void OnUpdateSignal();

private:
    void ConfigInit();
    void SpaceInit();

    MQtableWidget *signalTableWidget;
    std::vector<Signal> m_signals;
};

}
#endif // SIGNALWIDGET_H
