#ifndef QVNCCLIENTWIDGET_H
#define QVNCCLIENTWIDGET_H
/**
 * @brief Vnc客户端,用于管理socket\图像控件等
 * @author huangyue
 * @date 2021-12-03
 * @note
 */

#include "DES.h"
#include "RFBProtol.h"
#include "VncScreen.h"
#include "Treasure/LuipShare.h"

#include <memory>
#include <QtWidgets>
#include <QtNetwork>

namespace VncSpace
{

/**
 * @brief 适配模式
 */
enum AdaptMode
{
    Original,       //原始大小
    Zoom,           //缩放拉伸
    AspectRation,   //保持比例缩放
};

/**
 * @brief VNC客户端窗口
 */
class LUIP_SHARE VncClientWidget : public QWidget, public RFBProtol
{
    Q_OBJECT

public:
    VncClientWidget(QWidget *parent = 0);
    ~VncClientWidget();

    void Uninit();

    bool Connect(QString ip, int port = 5900, QString password = "123456");
    bool IsConnected();
    void DisConnect();

    void StartUpdate();                 //开始刷新
    void StopUpdate();                  //停止刷新
    void SetAdaptMode(AdaptMode mode);  //设置适配模式

    QString GetIP() {return ip;}
    int     GetPort() {return port;}

    void SetStatusChangeFunc(std::function<void(bool)> func);

private:
    bool ConverRFBPoint(int& posX, int& posY);

private slots:
    void onServerMessage();

signals:
    void signalFrameBufferUpdated();

protected:
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *event);

    void keyEvent(QKeyEvent *event, bool press);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void mouseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QMutex mutex;
    std::atomic_bool enableUpadte;  //启动刷新
    AdaptMode mode;                 //适配模式
    VncScreen screen;               //视图
    std::atomic_bool isUninit;

    int screenWidth;
    int screenHeight;
    int paintTargetX, paintTargetY; //绘图的起始XY

    std::function<void(bool)> statusChangeFunc;
};

typedef std::shared_ptr<VncClientWidget> VncClientWidgetPtr;

}

#endif // QVNCCLIENTWIDGET_H
