#ifndef OUTAGE_H
#define OUTAGE_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include "Common.h"
#include "UI/Frame/NumberKeyboard.h"
#include <QTimer>
#include <QObject>



//namespace Ui { class Outage; }



class Outage : public QObject
{
    Q_OBJECT
public:
    explicit Outage(QObject *parent = 0);
    //QTimer *timer;

signals:

public slots:
    void show();
    void time();
    void stopFlow();
};







#endif // OUTAGE_H
