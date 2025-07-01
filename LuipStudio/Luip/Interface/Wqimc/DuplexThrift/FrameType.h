/**
 * @file
 * @brief 调用缓冲分帧传输器。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015-5-18
 */

#ifndef ERCHASHU_THRIFT_FRAME_TYPE_H_
#define ERCHASHU_THRIFT_FRAME_TYPE_H_

namespace erchashu {
namespace thrift {

enum class FrameType
{
    ThriftCall = 0x01,      ///< Thrift 协议调用帧，即由客户端主动发出的帧，帧格式应符合 Thrift 规范；
    ThriftReply = 0x02,     ///< Thrift 协议回应帧，即由服务器被动响应发出的帧，帧格式应符合 Thrift 规范；
    Echo = 0x03,            ///< 回声帧，用于心跳机制，附带一个4字节无符号整数的心跳周期，单位为秒；
    None = 0xFFFF,
};

}
}

#endif  // ERCHASHU_THRIFT_FRAME_TYPE_H_
