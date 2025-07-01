#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(this->windowTitle() + " - LuipDataEditor PT63");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
