#ifndef EXPLORERWIDGET_H
#define EXPLORERWIDGET_H

#include "Style.h"

class ExplorerWidget : public QWidget
{
    Q_OBJECT
public:
    ExplorerWidget(QWidget *parent = 0);
    ~ExplorerWidget();
    void Show(QString path);

private:
    void InitWidget();
    void OnExportClick();
    void OnTableItemClick(const QModelIndex &index);
    void OnReturnClick();
    void OnDelClick();
    void OnImportClick();
    void SetPath(QString path);

signals:
    void SignalExportEnd(bool result);
    void SignalImportEnd(bool result, QString msg);

private:
    QString m_basePath;
    QString m_curPath;

private:
    QLineEdit* m_pathEdit;
    QTableWidget* m_objTable;
    QPushButton* m_returnBtn;
    QPushButton* m_delBtn;
    QPushButton* m_exportBtn;
    QPushButton* m_importBtn;

private:
    Treasure::ThreadPtr m_thread;
};

#endif // EXPLORERWIDGET_H
