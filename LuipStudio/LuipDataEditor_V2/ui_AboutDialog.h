/********************************************************************************
** Form generated from reading UI file 'AboutDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QPushButton *btnExit;
    QLabel *label;
    QLabel *label_3;
    QTextBrowser *textBrowser;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QStringLiteral("AboutDialog"));
        AboutDialog->resize(507, 255);
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/LuipEditor"), QSize(), QIcon::Normal, QIcon::Off);
        AboutDialog->setWindowIcon(icon);
        btnExit = new QPushButton(AboutDialog);
        btnExit->setObjectName(QStringLiteral("btnExit"));
        btnExit->setGeometry(QRect(420, 200, 75, 33));
        btnExit->setMinimumSize(QSize(0, 33));
        label = new QLabel(AboutDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 131, 131));
        label->setPixmap(QPixmap(QString::fromUtf8(":/icon/LuipEditor")));
        label_3 = new QLabel(AboutDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(40, 160, 101, 16));
        textBrowser = new QTextBrowser(AboutDialog);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(180, 20, 311, 151));

        retranslateUi(AboutDialog);
        QObject::connect(btnExit, SIGNAL(clicked()), AboutDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "\345\205\263\344\272\216", 0));
        btnExit->setText(QApplication::translate("AboutDialog", "\351\200\200\345\207\272", 0));
        label->setText(QString());
        label_3->setText(QApplication::translate("AboutDialog", "LuipEditor 2.0.0", 0));
        textBrowser->setHtml(QApplication::translate("AboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#000000;\">\346\234\254\347\274\226\350\276\221\345\231\250\346\224\257\346\214\201PT63\347\232\204\345\244\247\345\244\232\346\225\260\344\272\247\345\223\201\347\232\204\346\225\260\346\215\256\347\274\226\350\276\221</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\344\275\277\347\224\250\350\257\264"
                        "\346\230\216\357\274\232</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">    *\350\257\267\345\260\206\347\274\226\350\276\221\345\267\245\345\205\267\347\275\256\344\272\216Luip\350\277\220\350\241\214\347\216\257\345\242\203\344\270\213\357\274\214\344\270\224\344\277\235\346\214\201Luip\346\225\260\346\215\256\346\226\207\344\273\266\347\233\270\345\257\271\344\275\215\347\275\256\344\270\215\345\217\230\343\200\202\345\257\271\344\272\216\346\225\260\346\215\256\351\207\217\350\276\203\345\244\247\347\232\204\346\226\207\344\273\266\357\274\214\347\274\226\350\276\221\346\227\266\351\227\264\350\276\203\351\225\277\357\274\214\350\257\267\350\200\220\345\277\203\347\255\211\345\200\231\343\200\202</p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
