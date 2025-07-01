/********************************************************************************
** Form generated from reading UI file 'FilterDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FILTERDIALOG_H
#define UI_FILTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FilterDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QRadioButton *rbGreaterEqual;
    QRadioButton *rbLessEqual;
    QLineEdit *editConcentration;
    QLabel *label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FilterDialog)
    {
        if (FilterDialog->objectName().isEmpty())
            FilterDialog->setObjectName(QStringLiteral("FilterDialog"));
        FilterDialog->resize(269, 138);
        verticalLayout = new QVBoxLayout(FilterDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(FilterDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        rbGreaterEqual = new QRadioButton(groupBox);
        rbGreaterEqual->setObjectName(QStringLiteral("rbGreaterEqual"));
        rbGreaterEqual->setGeometry(QRect(10, 30, 41, 16));
        rbGreaterEqual->setChecked(true);
        rbLessEqual = new QRadioButton(groupBox);
        rbLessEqual->setObjectName(QStringLiteral("rbLessEqual"));
        rbLessEqual->setGeometry(QRect(10, 50, 41, 16));
        editConcentration = new QLineEdit(groupBox);
        editConcentration->setObjectName(QStringLiteral("editConcentration"));
        editConcentration->setGeometry(QRect(70, 40, 113, 20));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(200, 40, 31, 16));

        verticalLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(FilterDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(FilterDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), FilterDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FilterDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(FilterDialog);
    } // setupUi

    void retranslateUi(QDialog *FilterDialog)
    {
        FilterDialog->setWindowTitle(QApplication::translate("FilterDialog", "\346\225\260\346\215\256\350\277\207\346\273\244", 0));
        groupBox->setTitle(QApplication::translate("FilterDialog", "\346\265\223\345\272\246", 0));
        rbGreaterEqual->setText(QApplication::translate("FilterDialog", "\342\211\245", 0));
        rbLessEqual->setText(QApplication::translate("FilterDialog", "\342\211\244", 0));
        editConcentration->setText(QApplication::translate("FilterDialog", "0", 0));
        label->setText(QApplication::translate("FilterDialog", "mg/L", 0));
    } // retranslateUi

};

namespace Ui {
    class FilterDialog: public Ui_FilterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FILTERDIALOG_H
