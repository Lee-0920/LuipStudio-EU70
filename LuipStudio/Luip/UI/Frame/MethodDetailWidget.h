#ifndef METHODDETAILWIDGET_H
#define METHODDETAILWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"
#include "DataBasePlugin/DataBaseManager.h"
using namespace DataBaseSpace;

namespace UI
{

class MethodDetailWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MethodDetailWidget(MethodRecord record, QWidget *parent);
    ~MethodDetailWidget();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SpaceInit();
    void TableSpaceInit();
    void ViewRefresh();
    void ShowTable();
private slots:
    void SlotQuitButton();    
private:
    QPushButton* quitButton;
    MQtableWidget *measureResultTableWidget;    
    QStringList m_rowName;
    MethodRecord m_methodRecord;
signals:

};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
