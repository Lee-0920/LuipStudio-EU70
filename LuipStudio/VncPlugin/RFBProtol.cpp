#include "RFBProtol.h"
#include "KeysymDef.h"
#include "DES.h"
#include "Treasure/SystemDef.h"

#define WAIT_TIME_OUT 2000

using namespace VncSpace;

quint16 RFBProtol::qMakeU16(quint8 l, quint8 h)
{
    quint16 result;
    quint8 *result_arr = (quint8*)&result;
    result_arr[0] = h;
    result_arr[1] = l;
    return result;
}

quint32 RFBProtol::qMakeU32(quint16 l, quint16 h)
{
    quint32 result;
    quint16 *result_arr = (quint16*)&result;
    result_arr[0] = h;
    result_arr[1] = l;
    return result;
}

quint32 RFBProtol::qMakeU32(quint8 lowest, quint8 low, quint8 high, quint8 highest)
{
    quint32 result;
    quint8 *result_arr = (quint8*)&result;
    result_arr[0] = highest;
    result_arr[1] = high;
    result_arr[2] = low;
    result_arr[3] = lowest;
    return result;
}

/**
 * @brief 转换按键
 */
quint32 RFBProtol::translateRfbKey(int key, bool modifier)
{
    quint32 k = 5000;

    switch (key)
    {

        case Qt::Key_Backspace: k = XK_BackSpace; break;
        case Qt::Key_Tab: k = XK_Tab;break;
        case Qt::Key_Clear: k = XK_Clear; break;
        case Qt::Key_Return: k = XK_Return; break;
        case Qt::Key_Pause: k = XK_Pause; break;
        case Qt::Key_Escape: k = XK_Escape; break;
        case Qt::Key_Space: k = XK_space; break;
        case Qt::Key_Delete: k = XK_Delete; break;
        case Qt::Key_Period: k = XK_period; break;

        //special keyboard char
        case Qt::Key_Exclam: k = XK_exclam;break; //!
        case Qt::Key_QuoteDbl: k = XK_quotedbl;break; //?
        case Qt::Key_NumberSign: k = XK_numbersign;break; //#
        case Qt::Key_Percent: k = XK_percent;break; //%
        case Qt::Key_Dollar: k = XK_dollar;break;   //$
        case Qt::Key_Ampersand: k = XK_ampersand;break; //&
        case Qt::Key_Apostrophe: k = XK_apostrophe;break;//!
        case Qt::Key_ParenLeft: k = XK_parenleft;break;
        case Qt::Key_ParenRight: k = XK_parenright;break;

        case Qt::Key_Slash: k = XK_slash; break;    ///
        case Qt::Key_Asterisk: k = XK_asterisk; break;  //*
        case Qt::Key_Minus: k = XK_minus; break;    //-
        case Qt::Key_Plus: k = XK_plus; break;  //+
        case Qt::Key_Enter: k = XK_Return; break;   //
        case Qt::Key_Equal: k = XK_equal; break;    //=

        case Qt::Key_Colon: k = XK_colon;break;
        case Qt::Key_Semicolon: k = XK_semicolon; break;
        case Qt::Key_Greater: k = XK_greater; break;
        case Qt::Key_Question: k = XK_question; break;
        case Qt::Key_At: k = XK_at; break;

        case Qt::Key_BracketLeft: k = XK_bracketleft; break;
        case Qt::Key_Backslash: k = XK_backslash;break;
        case Qt::Key_BracketRight: k = XK_bracketright;break;
        case Qt::Key_AsciiCircum: k = XK_asciicircum;break;
        case Qt::Key_Underscore: k = XK_underscore;break;
        case Qt::Key_QuoteLeft: k = XK_quoteleft;break;
        case Qt::Key_BraceLeft: k = XK_braceleft;break;
        case Qt::Key_Bar: k = XK_bar; break;
        case Qt::Key_BraceRight: k = XK_braceright;break;
        case Qt::Key_AsciiTilde: k = XK_asciitilde;break;
        case Qt::Key_nobreakspace: k = XK_nobreakspace;break;
        case Qt::Key_exclamdown: k = XK_exclamdown;break;
        case Qt::Key_cent: k = XK_cent;break;
        case Qt::Key_sterling: k = XK_sterling;break;
        case Qt::Key_currency: k = XK_currency;break;
        case Qt::Key_yen: k = XK_yen;break;
        case Qt::Key_brokenbar: k = XK_brokenbar;break;
        case Qt::Key_section: k = XK_section;break;
        case Qt::Key_diaeresis: k = XK_diaeresis;break;
        case Qt::Key_copyright: k = XK_copyright; break;
        case Qt::Key_ordfeminine: k = XK_ordfeminine; break;
        case Qt::Key_guillemotleft: k = XK_guillemotleft; break;
        case Qt::Key_guillemotright: k = XK_guillemotright; break;
        case Qt::Key_notsign: k = XK_notsign; break;
        case Qt::Key_hyphen: k = XK_hyphen; break;
        case  Qt::Key_registered: k = XK_registered; break;

        case Qt::Key_Up: k = XK_Up; break;
        case Qt::Key_Down: k = XK_Down; break;
        case Qt::Key_Right: k = XK_Right; break;
        case Qt::Key_Left: k = XK_Left; break;
        case Qt::Key_Insert: k = XK_Insert; break;
        case Qt::Key_Home: k = XK_Home; break;
        case Qt::Key_End: k = XK_End; break;
        case Qt::Key_PageUp: k = XK_Page_Up; break;
        case Qt::Key_PageDown: k = XK_Page_Down; break;
        case Qt::Key_F1: k = XK_F1; break;
        case Qt::Key_F2: k = XK_F2; break;
        case Qt::Key_F3: k = XK_F3; break;
        case Qt::Key_F4: k = XK_F4; break;
        case Qt::Key_F5: k = XK_F5; break;
        case Qt::Key_F6: k = XK_F6; break;
        case Qt::Key_F7: k = XK_F7; break;
        case Qt::Key_F8: k = XK_F8; break;
        case Qt::Key_F9: k = XK_F9; break;
        case Qt::Key_F10: k = XK_F10; break;
        case Qt::Key_F11: k = XK_F11; break;
        case Qt::Key_F12: k =  XK_F12; break;
        case Qt::Key_F13: k = XK_F13; break;
        case Qt::Key_F14: k = XK_F14; break;
        case Qt::Key_F15: k = XK_F15; break;
        case Qt::Key_F16: k = XK_F16; break;
        case Qt::Key_F17: k = XK_F17; break;
        case Qt::Key_F18: k = XK_F18; break;
        case Qt::Key_F19: k = XK_F19; break;
        case Qt::Key_F20: k = XK_F20; break;
        case Qt::Key_F21: k = XK_F21; break;
        case Qt::Key_F22: k = XK_F22; break;
        case Qt::Key_F23: k = XK_F23; break;
        case Qt::Key_F24: k = XK_F24; break;
        case Qt::Key_F25: k = XK_F25; break;
        case Qt::Key_F26: k = XK_F26; break;
        case Qt::Key_F27: k = XK_F27; break;
        case Qt::Key_F28: k = XK_F28; break;
        case Qt::Key_F29: k = XK_F29; break;
        case Qt::Key_F30: k = XK_F30; break;
        case Qt::Key_F31: k = XK_F31; break;
        case Qt::Key_F32: k = XK_F32; break;
        case Qt::Key_F33: k = XK_F33; break;
        case Qt::Key_F34: k = XK_F34; break;
        case Qt::Key_F35: k = XK_F35; break;
        case Qt::Key_NumLock: k = XK_Num_Lock; break;
        case Qt::Key_CapsLock: k = XK_Caps_Lock; break;
        case Qt::Key_ScrollLock: k = XK_Scroll_Lock; break;
        case Qt::Key_Shift: k = XK_Shift_R; break; //k = XK_Shift_L; break;
        case Qt::Key_Control: k = XK_Control_R; break;// k = XK_Control_L; break;
        case Qt::Key_Alt: k = XK_Alt_R; break;//k = XK_Alt_L; break;
        case Qt::Key_Meta: k = XK_Meta_R; break;//k = XK_Meta_L; break;*/

        case Qt::Key_Super_L: k = XK_Super_L; break;		/* left "windows" key */
        case Qt::Key_Super_R: k = XK_Super_R; break;		/* right "windows" key */

        case Qt::Key_Mode_switch: k = XK_Mode_switch; break;
        case Qt::Key_Help: k = XK_Help; break;
        case Qt::Key_Print: k = XK_Print; break;
        case Qt::Key_SysReq: k = XK_Sys_Req; break;
        case Qt::Key_0: k = XK_0;break;
        case Qt::Key_1: k = XK_1;break;
        case Qt::Key_2: k = XK_2;break;
        case Qt::Key_3: k = XK_3;break;
        case Qt::Key_4: k = XK_4;break;
        case Qt::Key_5: k = XK_5;break;
        case Qt::Key_6: k = XK_6;break;
        case Qt::Key_7: k = XK_7;break;
        case Qt::Key_8: k = XK_8;break;
        case Qt::Key_9: k = XK_9;break;
    }

    if (k == 5000)
    {

        if (!modifier)
        {
            switch (key)
            {
                case Qt::Key_A: k = XK_a;break;
                case Qt::Key_B: k = XK_b;break;
                case Qt::Key_C: k = XK_c;break;
                case Qt::Key_D: k = XK_d;break;
                case Qt::Key_E: k = XK_e;break;
                case Qt::Key_F: k = XK_f;break;
                case Qt::Key_G: k = XK_g;break;
                case Qt::Key_H: k = XK_h;break;
                case Qt::Key_I: k = XK_i;break;
                case Qt::Key_J: k = XK_j;break;
                case Qt::Key_K: k = XK_k;break;
                case Qt::Key_L: k = XK_l;break;
                case Qt::Key_M: k = XK_m;break;
                case Qt::Key_N: k = XK_n;break;
                case Qt::Key_O: k = XK_o;break;
                case Qt::Key_P: k = XK_p;break;
                case Qt::Key_Q: k = XK_q;break;
                case Qt::Key_R: k = XK_r;break;
                case Qt::Key_S: k = XK_s;break;
                case Qt::Key_T: k = XK_t;break;
                case Qt::Key_U: k = XK_u;break;
                case Qt::Key_V: k = XK_v;break;
                case Qt::Key_W: k = XK_w;break;
                case Qt::Key_X: k = XK_x;break;
                case Qt::Key_Y: k = XK_y;break;
                case Qt::Key_Z: k = XK_z;break;
            }
        }
        else
        {
            switch (key)
            {
                case Qt::Key_A: k = XK_A;break;
                case Qt::Key_B: k = XK_B;break;
                case Qt::Key_C: k = XK_C;break;
                case Qt::Key_D: k = XK_D;break;
                case Qt::Key_E: k = XK_E;break;
                case Qt::Key_F: k = XK_F;break;
                case Qt::Key_G: k = XK_G;break;
                case Qt::Key_H: k = XK_H;break;
                case Qt::Key_I: k = XK_I;break;
                case Qt::Key_J: k = XK_J;break;
                case Qt::Key_K: k = XK_K;break;
                case Qt::Key_L: k = XK_L;break;
                case Qt::Key_M: k = XK_M;break;
                case Qt::Key_N: k = XK_N;break;
                case Qt::Key_O: k = XK_O;break;
                case Qt::Key_P: k = XK_P;break;
                case Qt::Key_Q: k = XK_Q;break;
                case Qt::Key_R: k = XK_R;break;
                case Qt::Key_S: k = XK_S;break;
                case Qt::Key_T: k = XK_T;break;
                case Qt::Key_U: k = XK_U;break;
                case Qt::Key_V: k = XK_V;break;
                case Qt::Key_W: k = XK_W;break;
                case Qt::Key_X: k = XK_X;break;
                case Qt::Key_Y: k = XK_Y;break;
                case Qt::Key_Z: k = XK_Z;break;
            }
        }
    }

    return k;
}

RFBProtol::RFBProtol()
{
    ip = "";
    port = 0;
    password = "";
    needAuth = false;
    isShared = true;

    //设置客户端默认支持的编码格式
    encodings.append((qint32)Encodings::Raw);
//    encodings.append((qint32)Encodings::CursorSizePseudo);  //Qt5.9.6的VNC插件如果没带,客户端退出时服务会崩溃
//    encodings.append((qint32)Encodings::DesktopSizePseudo);
//    encodings.append((qint32)Encodings::ZRLE);
//    encodings.append((qint32)Encodings::CopyRect);
//    encodings.append((qint32)Encodings::Hextile);
//    encodings.append((qint32)Encodings::RRE);
}

/**
 * @brief 连接后接收服务版本
 */
bool RFBProtol::handleVersionProtocol()
{
    //等待读取
    if (!socket.waitForReadyRead(WAIT_TIME_OUT))
    {
        trLogger->debug("RFBProtol::handleVersionProtoco[%s] RFB-Server Not ready read:%s",
                        ip.toStdString().c_str(),
                        socket.errorString().toStdString().c_str());
        return false;
    }

    QByteArray response = socket.readAll();
    if(response.isEmpty())
    {
        trLogger->debug("RFBProtol::handleVersionProtoco[%s] RFB-Server answer is empty:%s",
                        ip.toStdString().c_str(),
                        socket.errorString().toStdString().c_str());
        return false;
    }

    qDebug() << "RFB-Server Version : " << response;
    serverMajorVersion = response.at(6);
    serverMinorVersion = response.at(10);

    //客户端的版本号与服务一致,并告知服务器（客户端的版本号只能小于等于服务版本）
    clientMajorVersion = serverMajorVersion;
    clientMinorVersion = serverMinorVersion;

    response.clear();
    switch (clientMinorVersion)
    {
        case '3':  response.append("RFB 003.003\n");break;
        case '7':  response.append("RFB 003.007\n");break;
        case '8':  response.append("RFB 003.008\n");break;
    default:
        trLogger->debug("RFBProtol::handleVersionProtoco[%s] RFB unsupport server version:%c",
                        ip.toStdString().c_str(),
                        serverMinorVersion);
        return false;
    }
    return socket.write(response) > 0;
}

/**
 * @brief 连接后接收安全类型
 */
bool RFBProtol::handleSecurityProtocol()
{
    //等待读取
    if (!socket.waitForReadyRead(WAIT_TIME_OUT))
    {
        trLogger->debug("RFBProtol::handleSecurityProtocol[%s] RFB-Server Not ready read:%s",
                        ip.toStdString().c_str(),
                        socket.errorString().toStdString().c_str());
        return false;
    }

    /********************************************
     *
     * 3.3的版本 : 服务器发送一个无符号的32位整数标识一个安全类型（与认证有关）
     *
     * ******************************************/
    if (clientMinorVersion == '3')
    {
        auto response = socket.read(4);
        if (response.length() != 4)
        {
            trLogger->debug("RFBProtol::handleSecurityProtocol[%s] RFB 3.3 security-type not ok, typelength:%d",
                            ip.toStdString().c_str(),
                            response.length());
            return false;
        }
        auto type = qMakeU32(response.at(0), response.at(1), response.at(2), response.at(3));
        switch(type)
        {
        case 0://无效的
            qDebug() << "RFB 3.3 security-type is 0(Invalid)."; return false;
        case 1://不需要认证
            needAuth = false;return true;
        case 2://VNC认证
            needAuth = true;return true;
        default:
            qDebug("RFB 3.3 security-type is %d(unknow)", type); return false;
        }
    }
    /********************************************
     *
     * 3.7及版本 :
     * 1) 服务器发送一个字节标识安全类型的个数N
     * 2) 服务器发送N个字节标识安全类型数组
     * 3) 客户端发送一个字节的安全类型，表示支持某种安全类型
     * 4) 如果次版本是8及以上，还会发送4字节的安全结果消息，如果是0表示成功
     *
     * ******************************************/
    else if (clientMinorVersion == '7' || clientMinorVersion == '8')
    {
        auto response = socket.read(1);
        if (response.length() != 1)
        {
            qDebug() << "RFB 3.3 security-type-number not ok";
            return false;
        }

        //安全类型数组
        response = socket.read(response.at(0));
        if(response.contains('\x01'))//不需要认证
        {
            socket.write("\x01");
            needAuth = false;
            //安全结果消息检查
            if (clientMinorVersion != '7')
            {
                socket.waitForReadyRead();
                response = socket.readAll();
                if (response.length() != 4 ||
                    qMakeU32(response.at(0), response.at(1), response.at(2), response.at(3)) != 0)
                {
                    qDebug() << "RFB>= 3.8 security-clearence not ok:" << response.toStdString().c_str();
                    return false;
                }
            }
            return true;
        }
        else if(response.contains('\x02'))//VNC认证
        {
            socket.write("\x02");
            needAuth = true;
            return true;
        }
        else
        {
            qDebug() << "RFB 3.7/8 security-types not support:" << response.toStdString().c_str();
            return false;
        }
    }
    return false;
}

/**
 * @brief 连接后认证
 */
bool RFBProtol::handleAuthentication()
{
    if (!needAuth)
        return true;

    //等待读取
    if (!socket.waitForReadyRead(WAIT_TIME_OUT))
    {
        qDebug() << "RFB-Server Not ready read " << ip;
        qDebug() << socket.errorString();
        return false;
    }

    //服务器发送一个16字节的随机数
    auto response = socket.read(16);
    if (response.length() != 16)
    {
        qDebug() << "RFB auth not ok, security Challenge need 16, but recv:" << response.length();
        return false;
    }

    //客户端使用DES对验证进行加密，使用用户密码作为密钥，把16字节的回复返回到服务器
    auto data = buildPasswordData(response);
    socket.write(data);
    socket.waitForReadyRead();

    //服务发送4字节整数, 0表示成功
    response = socket.read(4);
    if (response.length() != 4 ||
        qMakeU32(response.at(0), response.at(1), response.at(2), response.at(3)) != 0)
    {
        qDebug() << "RFB auth not ok, maybe password wrong.";
        return false;
    }

    return true;
}

/**
 * @brief 构建密码数据
 */
QByteArray RFBProtol::buildPasswordData(QByteArray challenge)
{
    QByteArray pwdByte(8, 0);
    for (int i = 0; i < 8 && i < password.length(); i++)
    {
        pwdByte[i] = password.at(i).toLatin1();
    }

    QByteArray result(16, 0);
    deskey((unsigned char*)pwdByte.data(), EN0);
    des((unsigned char*)challenge.data(), (unsigned char*)result.data());
    des((unsigned char*)challenge.data()+8, (unsigned char*)result.data()+8);
    return result;
}

/**
 * @brief 连接后获取服务信息
 */
bool RFBProtol::handleServerInfo()
{
    //客户端发送一个字节的初始化消息。
    // 0: 服务先关闭其他客户端
    // 1: 服务可以同时连接多个客户端
    socket.write(isShared ? "\x01" : "\x00");
    socket.waitForReadyRead();

    //服务器发送初始化消息，主要告知客户端服务器的帧缓存（桌面屏幕）的高、宽、象素格式和桌面相关的名称
    //2屏幕宽 2屏幕高
    auto response = socket.read(4);
    frameBufferWidth = RFBProtol::qMakeU16(response.at(0), response.at(1));
    frameBufferHeight = RFBProtol::qMakeU16(response.at(2), response.at(3));
    qDebug() << "Width: " << frameBufferWidth << "Height: " << frameBufferHeight;

    //16 像素格式
    if(socket.read((char *)&pixelFormat, sizeof(pixelFormat)) != sizeof(pixelFormat))
        return false;
    pixelFormat.redMax = qFromBigEndian(pixelFormat.redMax);
    pixelFormat.greenMax = qFromBigEndian(pixelFormat.greenMax);
    pixelFormat.blueMax = qFromBigEndian(pixelFormat.blueMax);

    //4名称长度 N名称
    response = socket.read(4);
    response = socket.readAll();

    qDebug() << "Bits per pixel: " << pixelFormat.bitsPerPixel;
    qDebug() << "Depth: " << pixelFormat.depth;
    qDebug() << "Big Endian: " << pixelFormat.bigEndianFlag;
    qDebug() << "True Color: " << pixelFormat.trueColorFlag;
    qDebug() << "Red Max: " << pixelFormat.redMax;
    qDebug() << "Green Max: " << pixelFormat.greenMax;
    qDebug() << "Blue Max: " << pixelFormat.blueMax;
    qDebug() << "Red Shift: " << pixelFormat.redShift;
    qDebug() << "Green Shift: " << pixelFormat.greenShift;
    qDebug() << "Blue Shift: " << pixelFormat.blueShift;
    qDebug() << "Name : " << response;
    return true;
}

/****************************************
 *
 * 发送消息
 *
 * *************************************/


/**
 * @brief 设置像素格式
 */
bool RFBProtol::setPixelFormat()
{
    struct SET_PIXELFORMAT_STRUCT
    {
        quint8 		msgType;
        quint8 		padding[3];
        PixelFormat pixfmt;
    }setpixfmt;

    memset(&setpixfmt, 0, sizeof(SET_PIXELFORMAT_STRUCT));

    //与服务一致即可
    setpixfmt.msgType = (quint8)Client2ServerMessages::SetPixelFormat;
    setpixfmt.pixfmt = pixelFormat;
    setpixfmt.pixfmt.redMax = qToBigEndian(pixelFormat.redMax);
    setpixfmt.pixfmt.greenMax = qToBigEndian(pixelFormat.greenMax);
    setpixfmt.pixfmt.blueMax = qToBigEndian(pixelFormat.blueMax);

    if (socket.write((char *)&setpixfmt, sizeof(setpixfmt)) <= 0)
    {
        qDebug() << "RFB setPixelFormat not ok.";
        return false;
    }

    return true;
}

/**
 * @brief 设置编码格式
 */
bool RFBProtol::setEncodings()
{
    struct SET_ENCODING_STRUCT
    {
        quint8   msgType;
        quint8   padding;
        quint16  numOfEncodings;
        qint32   encoding[10];
    }enc;

    enc.msgType = (quint8)Client2ServerMessages::SetEncodings;
    enc.padding = 0;
    enc.numOfEncodings = qToBigEndian((quint16)encodings.size());
    for (int i = 0; i < encodings.size(); i++)
        enc.encoding[i] = qToBigEndian(encodings[i]);

    auto len = sizeof(SET_ENCODING_STRUCT) - (10 - encodings.size())*4;
    if (socket.write((char *)&enc, len) <= 0)
    {
        qDebug() << "RFB setEncodings not ok.";
        return false;
    }

    return true;
}

/**
 * @brief 请求更新
 * @param incremental 是否增量更新
 */
bool RFBProtol::requestFrameBufferUpdate(bool incremental)
{
    if (socket.state() != QTcpSocket::ConnectedState)
        return false;

    QByteArray frameBufferUpdateRequest(10, 0);
    frameBufferUpdateRequest[0] = (quint8)Client2ServerMessages::FramebufferUpdateRequest;
    frameBufferUpdateRequest[1] = incremental ? 1 : 0;
    frameBufferUpdateRequest[2] = 0;// x position
    frameBufferUpdateRequest[3] = 0;// x position
    frameBufferUpdateRequest[4] = 0;// y position
    frameBufferUpdateRequest[5] = 0;// y position

    frameBufferUpdateRequest[6] = (frameBufferWidth >> 8) & 0xFF; // width
    frameBufferUpdateRequest[7] = (frameBufferWidth >> 0) & 0xFF; // width
    frameBufferUpdateRequest[8] = (frameBufferHeight >> 8) & 0xFF; // height
    frameBufferUpdateRequest[9] = (frameBufferHeight >> 0) & 0xFF; // height

    return socket.write(frameBufferUpdateRequest) > 0;
}

/**
 * @brief 按键事件
 */
bool RFBProtol::sendKeyEvent(int qEventKey, bool isPress, bool isModifier)
{
    if (socket.state() != QTcpSocket::ConnectedState)
        return false;

    QByteArray message(8, 0);
    message[0] = (quint8)Client2ServerMessages::KeyEvent;
    message[1] = isPress ? 1 : 0;
    message[2] = message[3] = 0;  //padding

    quint32 key = RFBProtol::translateRfbKey(qEventKey, isModifier);

    message[4] = (key >> 24) & 0xFF;
    message[5] = (key >> 16) & 0xFF;
    message[6] = (key >> 8) & 0xFF;
    message[7] = (key >> 0) & 0xFF;

    return socket.write(message) > 0;
}

/**
 * @brief 发送鼠标事件
 */
bool RFBProtol::sendMouseEvent(int mouseStatus, int posX, int posY)
{
    if (socket.state() != QTcpSocket::ConnectedState)
        return false;

    //bit:0-2 represent left,middle and right button (after 2 is wheel button, up down left right) 1:down 0:up
    quint8 buttonMask = 0;
    if(mouseStatus&Qt::LeftButton)
        buttonMask |= 0x01;
    if(mouseStatus&Qt::MidButton)
        buttonMask |= 0x02;
    if(mouseStatus&Qt::RightButton)
        buttonMask |= 0x04;

    QByteArray message(6, 0);
    message[0] = (quint8)Client2ServerMessages::PointerEvent;
    message[1] = buttonMask;
    message[2] = (posX >> 8) & 0xFF;
    message[3] = (posX >> 0) & 0xFF;
    message[4] = (posY >> 8) & 0xFF;
    message[5] = (posY >> 0) & 0xFF;

    return socket.write(message) > 0;
}

/**
 * @brief 发送鼠标滚轮事件
 */
bool RFBProtol::sendWheelEvent(const QPoint& angleDelta,int mouseStatus, int posX, int posY)
{
    if (socket.state() != QTcpSocket::ConnectedState)
        return false;

    //bit:0-2 represent left,middle and right button (after 2 is wheel button, up down left right) 1:down 0:up
    quint8 buttonMask = 0;
    if(mouseStatus&Qt::LeftButton)
        buttonMask |= 0x01;
    if(mouseStatus&Qt::MidButton)
        buttonMask |= 0x02;
    if(mouseStatus&Qt::RightButton)
        buttonMask |= 0x04;

    quint8 bitmask = 0;
    if(angleDelta.y()>0)
        bitmask = 1<<3;
    else
        bitmask = 1<<4;

    QByteArray message(6, 0);
    message[0] = (quint8)Client2ServerMessages::PointerEvent;
    message[1] = buttonMask | bitmask ;
    message[2] = (posX >> 8) & 0xFF;
    message[3] = (posX >> 0) & 0xFF;
    message[4] = (posY >> 8) & 0xFF;
    message[5] = (posY >> 0) & 0xFF;

    socket.write(message);

    message[1] = buttonMask & (~bitmask);
    QThread::msleep(10);
    return socket.write(message) > 0;
}

/****************************************
 *
 * 接收消息
 *
 * *************************************/

/**
 * @brief 接收服务消息类型
 */
int RFBProtol::recvServerMessageType()
{
    auto response = socket.read(1);
    auto type = (quint8)response.at(0);
    return type;
}

/**
 * @brief 接收更新包的矩形数量
 */
int RFBProtol::recvFramebufferUpdateRectNum()
{
    //1字节padding
    //2字节number of rectangles
    auto response = socket.read(3);
    if (response.length() != 3)
        return 0;

    auto num = RFBProtol::qMakeU16(response.at(1), response.at(2));
    return num;
}

/**
 * @brief 接收像素框的头
 */
bool RFBProtol::recvRectHeader(RectHeader& rectHeader)
{
    while(socket.bytesAvailable() < sizeof(rectHeader))
    {
        qApp->processEvents();
        if (socket.state() != QTcpSocket::ConnectedState)
            return false;
    }

    if(socket.read((char *)&rectHeader, sizeof(rectHeader)) != sizeof(rectHeader))
    {
        qDebug() << "RFB recv rectHeader faile. size ont ok.";
        socket.readAll();
        return false;
    }

    rectHeader.xPosition = qFromBigEndian(rectHeader.xPosition);
    rectHeader.yPosition = qFromBigEndian(rectHeader.yPosition);
    rectHeader.width = qFromBigEndian(rectHeader.width);
    rectHeader.height = qFromBigEndian(rectHeader.height);
    rectHeader.encodingType = qFromBigEndian(rectHeader.encodingType);
    return true;
}

/**
 * @brief 接收RAW编码数据
 */
QByteArray RFBProtol::recvEncodingRawData(const RectHeader& rectHeader)
{
    int numOfBytes = rectHeader.width * rectHeader.height * (pixelFormat.bitsPerPixel / 8);
    if (numOfBytes <= 0)
        return QByteArray();

    while(socket.bytesAvailable() < numOfBytes)
    {
        qApp->processEvents();
        if (socket.state() != QTcpSocket::ConnectedState)
            return QByteArray();
    }

    return socket.read(numOfBytes);
}

/**
 * @brief 接收CursorSizePseudo编码数据
 */
QByteArray RFBProtol::recvEncodingCursorSizePseudoData(const RectHeader& rectHeader)
{
    int numOfBytes = rectHeader.width * rectHeader.height * (pixelFormat.bitsPerPixel / 8);
    if (numOfBytes <= 0)
        return QByteArray();

    int floor = (rectHeader.width + 7)/8 * rectHeader.height;
    numOfBytes += floor;

    while(socket.bytesAvailable() < numOfBytes)
    {
        qApp->processEvents();
        if (socket.state() != QTcpSocket::ConnectedState)
            return QByteArray();
    }
    return socket.read(numOfBytes);
}
