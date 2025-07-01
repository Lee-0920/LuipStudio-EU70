#include "PushButton.h"

namespace UI
{
PushButton::PushButton(QWidget *parent)
	:QPushButton(parent)
{	
	status = NORMAL;
    mousePress = false;
}

PushButton::~PushButton()
{

}

void PushButton::setPicName(QString picName)
{
    this->picName = picName;
    setFixedSize(QPixmap(picName).size());
}

void PushButton::enterEvent(QEvent *)
{
	status = ENTER;
	update();
}

void PushButton::mousePressEvent(QMouseEvent *event)
{
	//若点击鼠标左键
	if(event->button() == Qt::LeftButton)
	{
        mousePress = true;
		status = PRESS;
		update();
	}
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
	//若点击鼠标左键
    if(mousePress  && this->rect().contains(event->pos()))
	{
        mousePress = false;
		status = ENTER;
		update();
		emit clicked();
	}
}

void PushButton::leaveEvent(QEvent *)
{
	status = NORMAL;
	update();
}

void PushButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	QPixmap pixmap;
	switch(status)
	{
	case NORMAL:
		{
            pixmap.load(picName);
			break;
		}    
	case ENTER:
		{
            pixmap.load(picName + QString("_hover"));
			break;
		} 
	case PRESS:
		{
            pixmap.load(picName + QString("_pressed"));
			break;
		}   
	case NOSTATUS:
		{
            pixmap.load(picName);
			break;
		}   
	default:
        pixmap.load(picName);
	}
	
	painter.drawPixmap(rect(), pixmap);
}

}
