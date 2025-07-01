#ifndef UI_FRAME_TOOlBUTTON_H
#define UI_FRAME_TOOlBUTTON_H


#include <QToolButton>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

namespace UI
{

class ToolButton: public QToolButton
{

public:

    explicit ToolButton(QString pic_name, QWidget *parent = 0);
    ~ToolButton();
    void SetMousePress(bool mousePress);

protected:

    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void painterInfo(int topColor, int middleColor, int bottomColor);

public:

    bool m_mouseOver; //鼠标是否移过
    bool m_mousePress; //鼠标是否按下
};

}

#endif //UI_FRAME_TOOlBUTTON_H
