#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
//#include "Measure/V1.2/MeasureResultFile.h"
//#include "Measure/V1.3/MeasureResultFile.h"
//#include "Measure/V1.2/CalibrateResultFile.h"
//#include "Measure/V1.3/CalibrateResultFile.h"
#include "ResultManager/MeasureRecordFile.h"
#include "RecordDataPlugin/RecordData.h"

using namespace Result;
using namespace ResultData;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onAbout();
    void onSave();
    void onCopy();
    void onInsert();
    void onDelete();
    void onCurrentChange(const QModelIndex &index);
    void onTableItemChanged(QStandardItem *item);

private:
    bool loadLocalFile();
    bool saveLocalFile();

    QList<QStandardItem *> createRecordRow(const RecordData* record);
    void updateRecordView();
    void updateTotalRecordNum();
    void updateCurrentSelected();
    void updateTimeFormat();

private:
    Ui::MainWindow *ui;
    QLabel * labelFilePath;
    QLabel * labelResultsNum;
    QLabel * labelCurrentSelected;
    QLabel * labelRecordTimeFormat;
    QStandardItemModel* recordModel;

    MeasureRecordFile *m_resultFiles;
    RecordFields* m_recordFields;

    QVector<RecordData*> m_resultRecords;
    std::vector<String> m_exportFields;

    int currentSelectedIndex;
    QString strFilePath;
    Qt::TimeSpec resultTimeSpec;    // 记录所使用的时间规范（是否为UTC或本地时区<东8区>）
};

#endif // MAINWINDOW_H
