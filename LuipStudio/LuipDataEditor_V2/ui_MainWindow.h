/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionNew;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionInsert;
    QAction *actionCopy;
    QAction *actionDelete;
    QAction *actionFilter;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTableView *recordView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 500);
        MainWindow->setMinimumSize(QSize(800, 500));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/LuipEditor"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/png/about"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon1);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/png/new"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon2);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/png/open"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon3);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/png/save"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon4);
        actionSaveAs = new QAction(MainWindow);
        actionSaveAs->setObjectName(QStringLiteral("actionSaveAs"));
        actionInsert = new QAction(MainWindow);
        actionInsert->setObjectName(QStringLiteral("actionInsert"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/png/insert"), QSize(), QIcon::Normal, QIcon::Off);
        actionInsert->setIcon(icon5);
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QStringLiteral("actionCopy"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/png/copy"), QSize(), QIcon::Normal, QIcon::Off);
        actionCopy->setIcon(icon6);
        actionDelete = new QAction(MainWindow);
        actionDelete->setObjectName(QStringLiteral("actionDelete"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/png/delete"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete->setIcon(icon7);
        actionFilter = new QAction(MainWindow);
        actionFilter->setObjectName(QStringLiteral("actionFilter"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/png/filter"), QSize(), QIcon::Normal, QIcon::Off);
        actionFilter->setIcon(icon8);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        recordView = new QTableView(centralWidget);
        recordView->setObjectName(QStringLiteral("recordView"));
        recordView->horizontalHeader()->setHighlightSections(false);

        verticalLayout->addWidget(recordView);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 800, 23));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setMinimumSize(QSize(0, 30));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionSave);
        menuEdit->addAction(actionInsert);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionDelete);
        menuEdit->addSeparator();
        menuHelp->addAction(actionAbout);
        toolBar->addAction(actionSave);
        toolBar->addSeparator();
        toolBar->addAction(actionInsert);
        toolBar->addAction(actionCopy);
        toolBar->addAction(actionDelete);
        toolBar->addSeparator();
        toolBar->addSeparator();
        toolBar->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(actionDelete, SIGNAL(triggered()), MainWindow, SLOT(onDelete()));
        QObject::connect(actionSave, SIGNAL(triggered()), MainWindow, SLOT(onSave()));
        QObject::connect(recordView, SIGNAL(clicked(QModelIndex)), MainWindow, SLOT(onCurrentChange(QModelIndex)));
        QObject::connect(actionCopy, SIGNAL(triggered()), MainWindow, SLOT(onCopy()));
        QObject::connect(actionInsert, SIGNAL(triggered()), MainWindow, SLOT(onInsert()));
        QObject::connect(actionAbout, SIGNAL(triggered()), MainWindow, SLOT(onAbout()));

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Luip \346\226\207\344\273\266\347\274\226\350\276\221\345\231\250", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216", 0));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("MainWindow", "\350\275\257\344\273\266\344\273\213\347\273\215\345\217\212\344\275\277\347\224\250\350\257\264\346\230\216", 0));
#endif // QT_NO_TOOLTIP
        actionNew->setText(QApplication::translate("MainWindow", "\346\226\260\345\273\272(&N)", 0));
#ifndef QT_NO_TOOLTIP
        actionNew->setToolTip(QApplication::translate("MainWindow", "\346\226\260\345\273\272\344\270\200\344\270\252\347\251\272\347\231\275\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        actionNew->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0));
        actionOpen->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200(&O)", 0));
#ifndef QT_NO_TOOLTIP
        actionOpen->setToolTip(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", 0));
        actionSave->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230(&S)", 0));
#ifndef QT_NO_TOOLTIP
        actionSave->setToolTip(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\210\260\345\275\223\345\211\215\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0));
        actionSaveAs->setText(QApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272(&A)...", 0));
#ifndef QT_NO_TOOLTIP
        actionSaveAs->setToolTip(QApplication::translate("MainWindow", "\345\255\230\345\202\250\344\270\272\345\217\246\344\270\200\344\270\252\346\226\207\344\273\266", 0));
#endif // QT_NO_TOOLTIP
        actionInsert->setText(QApplication::translate("MainWindow", "\346\217\222\345\205\245(&I)", 0));
#ifndef QT_NO_TOOLTIP
        actionInsert->setToolTip(QApplication::translate("MainWindow", "\345\234\250\345\211\215\351\235\242\346\217\222\345\205\245\344\270\200\350\241\214\347\251\272\347\231\275\350\256\260\345\275\225", 0));
#endif // QT_NO_TOOLTIP
        actionInsert->setShortcut(QApplication::translate("MainWindow", "Ins", 0));
        actionCopy->setText(QApplication::translate("MainWindow", "\345\244\215\345\210\266(&C)", 0));
#ifndef QT_NO_TOOLTIP
        actionCopy->setToolTip(QApplication::translate("MainWindow", "\344\273\245\345\275\223\345\211\215\350\256\260\345\275\225\344\270\272\346\250\241\346\235\277\345\244\215\345\210\266\344\270\200\350\241\214", 0));
#endif // QT_NO_TOOLTIP
        actionDelete->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244(&D)", 0));
#ifndef QT_NO_TOOLTIP
        actionDelete->setToolTip(QApplication::translate("MainWindow", "\345\210\240\351\231\244\351\200\211\344\270\255\347\232\204\350\241\214", 0));
#endif // QT_NO_TOOLTIP
        actionDelete->setShortcut(QApplication::translate("MainWindow", "Del", 0));
        actionFilter->setText(QApplication::translate("MainWindow", "\350\277\207\346\273\244(&F)", 0));
#ifndef QT_NO_TOOLTIP
        actionFilter->setToolTip(QApplication::translate("MainWindow", "\345\216\273\351\231\244\346\214\207\345\256\232\346\235\241\344\273\266\347\232\204\350\256\260\345\275\225", 0));
#endif // QT_NO_TOOLTIP
        menuFile->setTitle(QApplication::translate("MainWindow", "\346\226\207\344\273\266(&F)", 0));
        menuEdit->setTitle(QApplication::translate("MainWindow", "\347\274\226\350\276\221(&E)", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "\345\270\256\345\212\251(&H)", 0));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
