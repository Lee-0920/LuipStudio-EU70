#ifndef REMOTE_STYLE_H
#define REMOTE_STYLE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>

#include "UdpPlugin/UdpManager.h"
#include "Treasure/SystemDef.h"
#include "MessageWidget.h"

class MyListWidgetItem : public QListWidgetItem
{
public:
    MyListWidgetItem(UdpSpace::DeviceInfo info, QListWidget *parent = 0);

public:
    UdpSpace::DeviceInfo m_info;
};

class Style
{
    TREASURE_SINGELTON(Style)

    Style()
    {
        messageBox = new MessageWidget();
    }
    void Uninit()
    {
        delete messageBox;
    }

public:
    static QPushButton* CreateTitleButton(QString text, QString iconPth = "");
    static QPushButton* CreateToolButton(QString text, QString iconPth = "");
    static QPushButton* CreateExitButton(QString text, QString iconPth = "");
    static QPushButton* CreateIconButton(QString iconPth = "");
    static QPushButton* CreateButton(QString text, QString iconPath = "", QString type = "Default");

    static QLabel* CreateTitleLabel(QString text);
    static QLabel* CreateToolLabel(QString text);
    static QLabel* CreateHeaderLabel(QString text);
    static QLabel* CreateLabel(QString text);

    static QListWidget* CreateClientListWidget();
    static MyListWidgetItem* CreateClientItem(UdpSpace::DeviceInfo info);
    static QLineEdit* CreateLineEdit();

    static QTableWidget* CreateTableWidget(QList<QString> columnNames);
    static void InsertTableWidgetItem(QTableWidget* table, int row, int column, QString text, QString icon = "");

    static void SetButtonIcon(QPushButton* btn, QString iconPath);

public:
    MessageWidget* messageBox;

};

#endif // STYLE_H
