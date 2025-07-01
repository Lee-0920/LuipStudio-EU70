#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

namespace Ui {
class FilterDialog;
}

enum class FilterOperator
{
    Unknown,
    GreaterEqual,
    LessEqual
};

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FilterDialog(QWidget *parent = 0);
    ~FilterDialog();

public:
    virtual void accept();

    FilterOperator filterOperator;
    double concentration;

private:
    Ui::FilterDialog *ui;
};

#endif // FILTERDIALOG_H
