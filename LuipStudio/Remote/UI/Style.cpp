#include "Style.h"

#define BASE_SVG_PATH "://UI/svg/"

MyListWidgetItem::MyListWidgetItem(UdpSpace::DeviceInfo info, QListWidget *parent): QListWidgetItem(parent)
{
    m_info = info;
}


QPushButton* Style::CreateTitleButton(QString text, QString iconPath)
{
    return CreateButton(text, iconPath, "Title");
}

QPushButton* Style::CreateExitButton(QString text, QString iconPath)
{
    return CreateButton(text, iconPath, "Exit");
}

QPushButton* Style::CreateIconButton(QString iconPath)
{
    auto ctrl = new QPushButton();
    ctrl->setObjectName("IconButton");
    ctrl->setIcon(QIcon(BASE_SVG_PATH + iconPath));
    ctrl->setIconSize(QSize(32, 32));
    return ctrl;
}

void Style::SetButtonIcon(QPushButton* btn, QString iconPath)
{
    btn->setIcon(QIcon(BASE_SVG_PATH + iconPath));
}

QPushButton* Style::CreateToolButton(QString text, QString iconPath)
{
    return CreateButton(text, iconPath, "Tool");
}

QPushButton* Style::CreateButton(QString text, QString iconPath, QString type)
{
    auto ctrl = new QPushButton();
    ctrl->setObjectName(type + "Button");
    ctrl->setText(text);

    if (!iconPath.isEmpty())
        ctrl->setIcon(QIcon(BASE_SVG_PATH + iconPath));
    return ctrl;
}

QLabel* Style::CreateTitleLabel(QString text)
{
    auto ctrl = new QLabel();
    ctrl->setObjectName("TitleLabel");
    ctrl->setText(text);
    return ctrl;
}

QLabel* Style::CreateHeaderLabel(QString text)
{
    auto ctrl = new QLabel();
    ctrl->setObjectName("HeaderLabel");
    ctrl->setText(text);
    return ctrl;
}

QLabel* Style::CreateToolLabel(QString text)
{
    auto ctrl = new QLabel();
    ctrl->setObjectName("ToolLabel");
    ctrl->setText(text);
    return ctrl;
}

QLabel* Style::CreateLabel(QString text)
{
    auto ctrl = new QLabel();
    ctrl->setText(text);
    return ctrl;
}

QListWidget* Style::CreateClientListWidget()
{
    auto ctrl = new QListWidget();
    ctrl->setObjectName("ClientListWidget");
    return ctrl;
}

MyListWidgetItem* Style::CreateClientItem(UdpSpace::DeviceInfo info)
{
    auto ctrl = new MyListWidgetItem(info);
    ctrl->setText(info.name);
    ctrl->setFont(QFont("Microsoft YaHei", FONT_SIZE - 1));

#ifdef _WIN32
    ctrl->setIcon(QIcon(BASE_SVG_PATH + QString("client.svg")));
#endif
    return ctrl;
}

QLineEdit* Style::CreateLineEdit()
{
    auto ctrl = new QLineEdit();
    return ctrl;
}

QTableWidget* Style::CreateTableWidget(QList<QString> columnNames)
{
    auto ctrl = new QTableWidget();
    ctrl->verticalHeader()->setVisible(false);
    ctrl->setColumnCount(columnNames.size());
    ctrl->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ctrl->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ctrl->setSelectionBehavior(QAbstractItemView::SelectRows);
    ctrl->setSelectionMode(QAbstractItemView::SingleSelection);
    //ctrl->setAlternatingRowColors(true);
    foreach (auto item, columnNames)
    {
        auto column = new QTableWidgetItem(item);
        ctrl->setHorizontalHeaderItem(columnNames.indexOf(item), column);
    }
    return ctrl;
}

void Style::InsertTableWidgetItem(QTableWidget* table, int row, int column, QString text, QString icon)
{
    auto item = table->item(row, column);
    if (item == NULL)
    {
        item = new QTableWidgetItem();
        table->setItem(row, column, item);
    }
    item->setText(text);
    item->setIcon(QIcon(BASE_SVG_PATH + icon));
}
