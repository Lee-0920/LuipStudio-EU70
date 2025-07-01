#ifndef UI_FRAME_QMYEDIT_H
#define UI_FRAME_QMYEDIT_H
#include <QLineEdit>
#include <QWidget>
#include <QtGui>
class QMyEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QMyEdit(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent *event);

private slots:

signals:
    void LineEditClicked();
};

#endif // UI_FRAME_QMYEDIT_H
