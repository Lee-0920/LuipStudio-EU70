#include "Treasure/SystemDef.h"
#include "VncClientWidget.h"

using namespace VncSpace;

VncClientWidget::VncClientWidget(QWidget *parent) : QWidget(parent), screen(width(), height())
{
    mode = AdaptMode::Original;
    enableUpadte = false;
    statusChangeFunc = NULL;
    isUninit = false;

    //鼠标追踪
    setMouseTracking(true);

    connect(&socket, &QTcpSocket::stateChanged, [&](QAbstractSocket::SocketState state)
    {
        switch (state)
        {
        case QAbstractSocket::UnconnectedState:
            disconnect(&socket, SIGNAL(readyRead()), this, SLOT(onServerMessage()));
            if (!isUninit)
            {
                screen.SetEmpty();
                update();

                if (statusChangeFunc)
                    statusChangeFunc(false);
            }
            break;
        case QAbstractSocket::ConnectedState:
            if (!isUninit)
            {
                if (statusChangeFunc)
                    statusChangeFunc(true);
            }
            break;
        default:
            break;
        }
    });
    connect(this, &VncClientWidget::signalFrameBufferUpdated, this, [this]{requestFrameBufferUpdate();});
}

VncClientWidget::~VncClientWidget()
{
    Uninit();
}

void VncClientWidget::Uninit()
{
    if (!isUninit)
    {
        isUninit = true;
        statusChangeFunc = NULL;

        StopUpdate();
        DisConnect();
        socket.close();
    }
}

/**
 * @brief 是否连接
 */
bool VncClientWidget::IsConnected()
{
    auto state = socket.state();
    if (socket.state() == QTcpSocket::ConnectedState)
        return true;

    if (state != QTcpSocket::UnconnectedState)
        trLogger->debug("VncClientWidget::IsConnected state:%d ip:%s", (int)state, ip.toStdString().c_str());
    return false;
}

/**
 * @brief 断开连接
 */
void VncClientWidget::DisConnect()
{
    QMutexLocker lock(&mutex);
    if (socket.state() == QTcpSocket::ConnectedState)
        socket.disconnectFromHost();
}

/**
 * @brief 连接服务
 */
bool VncClientWidget::Connect(QString ip, int port, QString password)
{
    if (!mutex.tryLock(3000))
        return false;

    this->password = password;
    this->ip = ip;
    this->port = port;

    //连接
    if (socket.state() == QTcpSocket::ConnectedState)
    {
        trLogger->debug("VncClientWidget::Connect[%s] disconnect first.", ip.toStdString().c_str());
        StopUpdate();
        socket.disconnectFromHost();
    }
    socket.connectToHost(QHostAddress(ip), port);

    //检查是否正常
    if (!socket.waitForConnected(1000))
    {
        trLogger->debug("VncClientWidget::Connect[%s] not connected:%s", ip.toStdString().c_str(), socket.errorString().toStdString().c_str());
        mutex.unlock();
        return false;
    }

    //读取服务版本\ 获取安全模式\ 认证\ 获取服务信息
    if (!handleVersionProtocol()||
        !handleSecurityProtocol()||
        !handleAuthentication()||
        !handleServerInfo())
    {
        trLogger->debug("VncClientWidget::Connect[%s] handle failed:%s", ip.toStdString().c_str(), socket.errorString().toStdString().c_str());
        socket.disconnectFromHost();
        mutex.unlock();
        return false;
    }

    //发送客户端的像素格式和编码
    if (!setPixelFormat() || !setEncodings())
    {
        trLogger->debug("VncClientWidget::Connect[%s] set info failed:%s", ip.toStdString().c_str(), socket.errorString().toStdString().c_str());
        socket.disconnectFromHost();
        mutex.unlock();
        return false;
    }

    //视图初始化
    screen.SetSize(frameBufferWidth, frameBufferHeight);

    connect(&socket, SIGNAL(readyRead()), this, SLOT(onServerMessage()));
    trLogger->debug("VncClientWidget::Connect[%s] connect success.", ip.toStdString().c_str());
    mutex.unlock();
    return true;
}

/*
 * @brief 开始更新
 */
void VncClientWidget::StartUpdate()
{
    enableUpadte = true;
    requestFrameBufferUpdate(false);
}

/**
 * @brief 停止更新
 */
void VncClientWidget::StopUpdate()
{
    enableUpadte = false;
}

/**
 * @brief 设置适配模式
 */
void VncClientWidget::SetAdaptMode(AdaptMode mode)
{
    this->mode = mode;
    resizeEvent(NULL);
}

void VncClientWidget::SetStatusChangeFunc(std::function<void(bool)> func)
{
    this->statusChangeFunc = func;
}

/******************************************************
 *
 * 接收响应
 *
 * ***************************************************/

void VncClientWidget::onServerMessage()
{
    if (!enableUpadte)
        return;

    if (!mutex.tryLock())
    {
        trLogger->debug("VncClientWidget::onServerMessage[%s] get lock failed.", ip.toStdString().c_str());
        return;
    }


    auto msgType = recvServerMessageType();
    if (msgType != (int)Server2ClientMessages::FramebufferUpdate)
    {
        auto data = socket.readAll();

        qDebug() << "RFB server message not support.type:" << msgType << " datalen:" << data.length();
        mutex.unlock();
        return;
    }

    RectHeader rectHeader;
    auto numOfRects = recvFramebufferUpdateRectNum();
    for (int i = 0; i < numOfRects && IsConnected(); i++)
    {
        qApp->processEvents();
        if (!recvRectHeader(rectHeader))
            break;

        if(rectHeader.encodingType == (qint32)Encodings::Raw)
        {
            auto bytes = recvEncodingRawData(rectHeader);
            if (bytes.length() <= 0)
                break;

            QImage image(rectHeader.width, rectHeader.height, QImage::Format_ARGB32);
            memcpy((char*)image.bits(), bytes.data(), bytes.length());

            screen.CoverImage(image, rectHeader.xPosition, rectHeader.yPosition);
        }
        else if (rectHeader.encodingType == (qint32)Encodings::CursorSizePseudo)
        {
            auto bytes = recvEncodingCursorSizePseudoData(rectHeader);
            if (bytes.length() <= 0)
                break;
        }
        else
        {
            qDebug() << "encoding Type:" << rectHeader.encodingType;
            socket.readAll();
            break;
        }
    }

    repaint();

    emit signalFrameBufferUpdated();
    mutex.unlock();
}

/******************************************************
 *
 * 系统函数重构
 *
 * ***************************************************/

void VncClientWidget::resizeEvent(QResizeEvent *e)
{
    paintTargetX = paintTargetY = 0;
    screenWidth  = screen.GetImage().width();
    screenHeight = screen.GetImage().height();

    //缩放下的视图尺寸变化
    if (this->mode == AdaptMode::Zoom || this->mode == AdaptMode::AspectRation)
    {
        auto img = screen.GetImage(width(), height(), this->mode == AdaptMode::AspectRation);
        screenWidth = img.width();
        screenHeight = img.height();
    }

    //比例不变下，绘图的XY变化
    if (this->mode == AdaptMode::Original || this->mode == AdaptMode::AspectRation)
    {
        if(screenWidth < this->geometry().width())
            paintTargetX = (this->geometry().width() - screenWidth)/2;
        if(screenHeight < this->geometry().height())
            paintTargetY = (this->geometry().height() - screenHeight)/2;
    }

    QWidget::resizeEvent(e);
}

void VncClientWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!IsConnected())
        return;

    QPainter painter;
    painter.begin(this);

    switch (this->mode)
    {
    case AdaptMode::Original:
        painter.drawImage(paintTargetX, paintTargetY, screen.GetImage());
        break;
    case AdaptMode::Zoom:
        painter.drawImage(paintTargetX, paintTargetY, screen.GetImage(width(), height(), false));
        break;
    case AdaptMode::AspectRation:
        painter.drawImage(paintTargetX, paintTargetY, screen.GetImage(width(), height(), true));
        break;
    }

    painter.end();
}

void VncClientWidget::keyEvent(QKeyEvent *event, bool press)
{
    if(!IsConnected())
        return;

    sendKeyEvent(event->key(), press, event->modifiers() == Qt::NoModifier ? false : true);
    event->accept();
}

void VncClientWidget::keyPressEvent(QKeyEvent *event)
{
    keyEvent(event, true);
}

void VncClientWidget::keyReleaseEvent(QKeyEvent *event)
{
    keyEvent(event, false);
}

void VncClientWidget::mouseEvent(QMouseEvent *event)
{
    if(!IsConnected())
        return;
    int posX = event->x();
    int posY = event->y();

    if (!ConverRFBPoint(posX, posY))
        return;

    sendMouseEvent(event->buttons(), posX, posY);
    event->accept();
}

void VncClientWidget::mouseMoveEvent(QMouseEvent *event)
{
    mouseEvent(event);
}

void VncClientWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    mouseEvent(event);
}

void VncClientWidget::mouseReleaseEvent(QMouseEvent *event)
{
    mouseEvent(event);
}

void VncClientWidget::wheelEvent(QWheelEvent *event)
{
    if(!IsConnected())
        return;
    int posX = event->x();
    int posY = event->y();

    if (!ConverRFBPoint(posX, posY))
        return;

    sendWheelEvent(event->angleDelta(), event->buttons(), posX, posY);
    event->accept();
}

/**
 * @brief 转为RFB屏幕坐标
 */
bool VncClientWidget::ConverRFBPoint(int& posX, int& posY)
{
    switch (this->mode)
    {
    case AdaptMode::Original:
        posX -= paintTargetX;
        posY -= paintTargetY;
        if(posX > frameBufferWidth)
            posX = -1;
        if(posY > frameBufferHeight)
            posY = -1;
        break;
    case AdaptMode::AspectRation:
        //先减去绘制的起始坐标，再拉伸坐标
        posX -= paintTargetX;
        posY -= paintTargetY;
        if(posX > screenWidth)
            posX = -1;
        if(posY > screenHeight)
            posY = -1;
    case AdaptMode::Zoom:
        if (posX >= 0)
            posX = (double(posX) / double(screenWidth)) * double(frameBufferWidth);
        if (posY >= 0)
            posY = (double(posY) / double(screenHeight)) * double(frameBufferHeight);
        break;
    }

    if (posX < 0 || posY < 0)
        return false;
    return true;
}

