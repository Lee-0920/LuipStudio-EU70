#include "FilterDialog.h"
#include "ui_FilterDialog.h"

FilterDialog::FilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterDialog)
{
    ui->setupUi(this);


    filterOperator = FilterOperator::GreaterEqual;
    concentration = 0;
}

FilterDialog::~FilterDialog()
{
    delete ui;
}

void FilterDialog::accept()
{
    if (ui->rbGreaterEqual->isChecked())
        filterOperator = FilterOperator::GreaterEqual;
    else if (ui->rbLessEqual->isChecked())
        filterOperator = FilterOperator::LessEqual;
    else
        filterOperator = FilterOperator::Unknown;

    concentration = ui->editConcentration->text().toDouble();

    QDialog::accept();
}
