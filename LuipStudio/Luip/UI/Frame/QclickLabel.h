#ifndef UI_FRAME_QCLICKLABEL_H
#define UI_FRAME_QCLICKLABEL_H
#include <QLabel>
namespace UI
{

class ClickableLabel : public QLabel
{
Q_OBJECT
public:
    explicit ClickableLabel( const QString& text="", QWidget* parent=0 );
    ~ClickableLabel();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
};


}
#endif // QCLICKLABEL_H
