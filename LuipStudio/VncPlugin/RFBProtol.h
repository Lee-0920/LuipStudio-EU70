#ifndef RFBPROTOLDEF_H
#define RFBPROTOLDEF_H

#include <memory>
#include <QtCore>
#include <QtWidgets>
#include <QtNetwork>

namespace VncSpace
{
    //客户端到服务的消息
    enum class Client2ServerMessages{
        SetPixelFormat				=	0,  //设置像素格式
        SetEncodings				=	2,  //设置编码方式
        FramebufferUpdateRequest	=	3,  //请求更新
        KeyEvent					=	4,  //按键事件
        PointerEvent				=	5,  //鼠标事件
        ClientCutText				=	6   //剪切文本
    };

    //服务到客户端的消息
    enum class Server2ClientMessages{
        FramebufferUpdate	=	0,  //更新
        SetColourMapEntries	=	1,  //更新colorMap
        Bell				=	2,  //响铃
        ServerCutText		=	3,  //剪切文本
    };

    //图像的传输编码
    enum class Encodings{
        Raw					=	0,  //原始编码,数据就由N个像素值组成
        CopyRect			=	1,  //复制矩形编码,对于像素值完全相同的一组矩形,只发送第一个矩形全部数据,随后的矩形则只需要发送左上角X、Y坐标

        RRE					=	2,  //二维行程编码,把矩形编码成可以被客户机的图形引擎翻译的格式
        CoRRE				=	4,  //RRE编码的变体
        Hextile				=	5,  //RRE编码的变种
        ZRLE				=	16, //结合了zlib 压缩，片技术、调色板和运行长度编码,矩形以4 字节长度区域开始，紧接着是zlib 压缩的数据，一个单一的zlib“流”对象被用在RFB协议的连接上

        DesktopSizePseudo	= -223, //桌面大小改变？
        CursorSizePseudo	= -239
    };

    //像素格式
    struct PixelFormat {
        quint8 bitsPerPixel;    //每个像素值需要的位数 >= depth
        quint8 depth;           //像素值中有用的位数 8\16\32
        quint8 bigEndianFlag;   //非0表示大端，对8位没影响
        quint8 trueColorFlag;
        quint16 redMax;
        quint16 greenMax;
        quint16 blueMax;
        quint8 redShift;
        quint8 greenShift;
        quint8 blueShift;
        quint8 padding[3];
    };

    //像素框的头
    struct RectHeader {
        quint16 xPosition;
        quint16 yPosition;
        quint16 width;
        quint16 height;
        qint32  encodingType;
    };

    class RFBProtol
    {
    protected:
        RFBProtol();

    protected://连接后的初始化处理
        bool handleVersionProtocol();
        bool handleSecurityProtocol();
        bool handleAuthentication();
        bool handleServerInfo();

    public://发送消息
        bool setPixelFormat();
        bool setEncodings();
        bool requestFrameBufferUpdate(bool incremental = true);
        bool sendKeyEvent(int qEventKey, bool isPress, bool isModifier);
        bool sendMouseEvent(int mouseStatus, int posX, int posY);
        bool sendWheelEvent(const QPoint& angleDelta,int mouseStatus, int posX, int posY);

    protected://接收服务消息
        int  recvServerMessageType();
        int  recvFramebufferUpdateRectNum();
        bool recvRectHeader(RectHeader& header);
        QByteArray recvEncodingRawData(const RectHeader& header);
        QByteArray recvEncodingCursorSizePseudoData(const RectHeader& header);


    public:
        static quint16 qMakeU16(quint8 l, quint8 h);
        static quint32 qMakeU32(quint16 l, quint16 h);
        static quint32 qMakeU32(quint8 lowest, quint8 low, quint8 high, quint8 highest);

    public:
        static quint32 translateRfbKey(int key, bool modifier);

    private:
        QByteArray buildPasswordData(QByteArray challenge);

    protected:
        QString password;        //密码
        QString ip;
        int     port;
        QTcpSocket socket;       //套接字

        bool needAuth;           //是否需要认证
        bool isShared;           //是否允许服务多客户端

        char serverMajorVersion; //服务主版本
        char serverMinorVersion; //服务次版本
        char clientMajorVersion; //客户主版本
        char clientMinorVersion; //客户次版本

        int frameBufferWidth;    //服务界面宽度
        int frameBufferHeight;   //服务界面高度
        PixelFormat pixelFormat; //像素格式

        QList<qint32> encodings; //编码

    };
}

#endif // RFBPROTOLDEF_H
