#include "DropShadowWidget.h"

namespace UI
{

DropShadowWidget::DropShadowWidget(QWidget *parent) :
        QDialog(parent)
{
#ifdef _CS_X86_WINDOWS
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    //初始化为未按下鼠标左键
    m_mousePress = false;
#endif
}

DropShadowWidget::~DropShadowWidget()
{

}

void DropShadowWidget::paintEvent(QPaintEvent *)
{
#ifdef _CS_X86_WINDOWS
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10, 10, this->width() - 20, this->height() - 20);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillPath(path, QBrush(Qt::white));

        QColor color(0, 0, 0, 50);
        for (int i = 0; i < 10; i++)
        {
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            path.addRect(10 - i, 10 - i, this->width() - (10 - i) * 2,
                    this->height() - (10 - i) * 2);
            color.setAlpha(150 - qSqrt(i) * 50);
            painter.setPen(color);
            painter.drawPath(path);
        }
#endif

}

void DropShadowWidget::mousePressEvent(QMouseEvent *event)
{
#ifdef _CS_X86_WINDOWS
    //只能是鼠标左键移动和改变大小
    if (event->button() == Qt::LeftButton)
    {
        m_mousePress = true;
    }

    //窗口移动距离
    m_movePoint = event->globalPos() - pos();
#endif

}

void DropShadowWidget::mouseReleaseEvent(QMouseEvent *)
{
#ifdef _CS_X86_WINDOWS
    m_mousePress = false;
#endif
}

void DropShadowWidget::mouseMoveEvent(QMouseEvent *event)
{
#ifdef _CS_X86_WINDOWS
    //移动窗口
    if (m_mousePress)
    {
        QPoint movePos = event->globalPos();
        move(movePos - m_movePoint);
    }
#endif
}

}
