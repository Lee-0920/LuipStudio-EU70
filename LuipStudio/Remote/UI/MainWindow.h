#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Style.h"
#include "AddWidget.h"
#include "ExplorerWidget.h"
#include "MessageWidget.h"
#include <QProcess>
#include <QMainWindow>



class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void InitWidget();
    QWidget* InitLeftGrid();
    QWidget* InitRightGrid();
    QLayout* InitExpandGrid();

    void CleanClientList();

signals:
    void SignalAddDevice(DeviceInfo device);

public slots:
    void OnRefresh();
    void OnAddDevice(DeviceInfo device);
    void OnSelectDevice();
    void OnReconnect();
    void OnOpenNFSPath();
    void OnUdhcpdMode();
    void OnLocalAreaMode();

private:
    QListWidget* m_clientListWidget;
    QPushButton* m_logoButton;
    QPushButton* m_addButton;
    QPushButton* m_refreshButton;
    QPushButton* m_exitButton;
    QPushButton* m_reconnectButton;
    QPushButton* m_openNFSPathButton;
    QGridLayout* m_clientLayout;
    QPushButton* m_leftButton;
    QPushButton* m_rightButton;
    QPushButton* m_udhcpdButton;
    QPushButton* m_localAreaButton;
    QPushButton* m_statusButton;

    QWidget* m_leftWidget;
    QWidget* m_rightWidget;

private:
    QList<DeviceInfo> m_devices;
    QWidget*          m_moveWidget;
    AddWidget*        m_addWidget;
    ExplorerWidget*   m_explorerWidget;

    QTimer            m_timer;

};

#endif // MAINWINDOW_H
