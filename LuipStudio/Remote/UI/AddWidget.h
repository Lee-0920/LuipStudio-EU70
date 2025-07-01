#ifndef ADDWIDGET_H
#define ADDWIDGET_H

#include "Style.h"
#include "UdpPlugin/UdpManager.h"

using UdpSpace::DeviceInfo;

class AddWidget : public QWidget
{
    Q_OBJECT
public:
    AddWidget(QWidget *parent = 0);

    void Show();

signals:
    void SignalAddDevice(DeviceInfo device);

private:
    void InitWidget();
    void OnComfirmClick();

private:
    QLineEdit* m_ipEdit;
    QLineEdit* m_portEdit;
    QLineEdit* m_nameEdit;
    QLabel* m_msgLable;

    QPushButton* m_confirmBtn;
    QPushButton* m_cancelBtn;
};

#endif // ADDWIDGET_H
