#ifndef DEBUGMODEDIALOG_H
#define DEBUGMODEDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>

namespace UI
{
class DebugModeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DebugModeDialog(QWidget *parent = 0);

signals:

public slots:
    void ConfirmSlot();

private:
    QLineEdit *m_passwadData;
    QLineEdit *m_minData;
    QLineEdit *m_maxData;
    QPushButton *m_confirmBtn;
};

}

#endif // DEBUGMODEDIALOG_H
