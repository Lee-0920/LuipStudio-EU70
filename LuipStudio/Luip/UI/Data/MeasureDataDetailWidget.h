#ifndef MEASUREDETAILDETAILWIDGET_H
#define MEASUREDETAILDETAILWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "UI/Frame/MQtableWidget.h"
#include "oolua.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "ResultManager/RecordFile.h"
using namespace Result;
using namespace DataBaseSpace;

namespace UI
{

class MeasureDataDetailWidget : public QDialog
{
    Q_OBJECT
public:
    explicit MeasureDataDetailWidget(MeasureRecord record, QWidget *parent);
    ~MeasureDataDetailWidget();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SpaceInit();
    void TableSpaceInit();
    void ViewRefresh();
    void ShowTable();
    void LoadMeasureFileFormatTable();
private slots:
    void SlotQuitButton();
private:
    MQtableWidget *measureResultTableWidget;
    QPushButton *quitButton;
    QStringList m_rowName;
    MeasureRecord m_measureDataRecord;    
signals:


};

}

#endif // AUTOTEMPCALIBRATEDIALOG_H
