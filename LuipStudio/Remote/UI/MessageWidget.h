#ifndef MessageWidget_H
#define MessageWidget_H

#include <QLabel>
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include "Treasure/SystemDef.h"

class MessageWidget : public QDialog
{
public:
    MessageWidget(QWidget *parent = 0);
    ~MessageWidget();
    void Show(QString title, QString msg, bool showCancel = false);
    bool IsConfirm() {return m_isConfirm;}

private:
    void InitWidget();
    void OnComfirmClick();
    void OnCancelClick();

private:
    QLabel* m_titleLable;
    QLabel* m_msgLable;
    QPushButton* m_confirmBtn;
    QPushButton* m_cancelBtn;

    std::atomic_bool m_isConfirm;
};

#endif // MessageWidget_H
