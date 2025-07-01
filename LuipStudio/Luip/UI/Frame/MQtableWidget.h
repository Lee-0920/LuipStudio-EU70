#ifndef UI_FRAME_MQTABLEWIDGET_H
#define UI_FRAME_MQTABLEWIDGET_H
#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>

class MQtableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MQtableWidget(QWidget *parent = 0);
    void setColumn(QStringList columnName);
    void setColumnAndSize(QStringList columnName, int pointSize);
    void setColumnItemSize(int index ,int pointSize);    
    void setRowAndSize(QStringList rowName, int pointSize);
    void setRowItemSize(int index ,int pointSize);
    void setRowFixHigh(QStringList rowName, int high);
};

#endif // UI_FRAME_MQTABLEWIDGET_H
