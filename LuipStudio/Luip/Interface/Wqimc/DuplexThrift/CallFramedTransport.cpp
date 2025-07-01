/**
 * @file
 * @brief 调用缓冲分帧传输器。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015-5-18
 */

#include "CallFramedTransport.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::transport;

namespace erchashu {
namespace thrift {


/**
 * @brief 追加 一个已经读到的 Buffer 并唤醒阻塞读线程。
 * @param buffer 帧数据缓冲。
 * @param len 帧长度。
 */
void CallFramedTransport::putFrame(uint8_t* buffer, int len)
{
    Synchronized sync(readMonitor_);
    if (len > maxFrameSize_)
    {
        delete[] readFrame_;
        readFrame_ = new uint8_t[len];
        readFrameCapacity_ = len;
    }
    readFrameSize_ = len;
    std::memcpy(readFrame_, buffer, len);
    readFrameReady_ = true;
    readMonitor_.notifyAll();
}


void CallFramedTransport::flush()
{
    int32_t sz_hbo, sz_nbo;
    assert(wBufSize_ > sizeof(sz_nbo));

    // 帧头已经预留
    sz_hbo = static_cast<uint32_t>(wBase_ - (wBuf_.get() + sizeof(sz_nbo)));
    sz_nbo = (int32_t)htonl((uint32_t)(sz_hbo));
    memcpy(wBuf_.get(), (uint8_t*)&sz_nbo, sizeof(sz_nbo));

    if (sz_hbo > 0)
    {
        // 写指针向前滑动4字节，以预留帧头
        wBase_ = wBuf_.get() + sizeof(sz_nbo);

        // 请求传输整帧数据
        frameSender_->sendFrame(FrameType::ThriftCall, wBuf_.get(), static_cast<uint32_t>(sizeof(sz_nbo)) + sz_hbo);
    }

    // 回收写缓冲，以防止缓冲过大，减小内存开销
    if (wBufSize_ > bufReclaimThresh_)
    {
        wBufSize_ = DEFAULT_BUFFER_SIZE;
        wBuf_.reset(new uint8_t[wBufSize_]);
        setWriteBuffer(wBuf_.get(), wBufSize_);

        // 写指针向前滑动4字节，以预留帧头
        wBase_ = wBuf_.get() + sizeof(sz_nbo);
    }
}


void CallFramedTransport::allocBuffers()
{
    readFrameCapacity_ = DEFAULT_BUFFER_SIZE;
    readFrame_ = new uint8_t[readFrameCapacity_];
    readFrameReady_ = false;
}


bool CallFramedTransport::readFrame()
{
    bool ret = false;
    Synchronized sync(readMonitor_);

    // 未准备好则等待
    if (!readFrameReady_)
    {
        readMonitor_.waitForTimeRelative(timeout_);
    }

    if (readFrameReady_)
    {
        readFrameSize_ -= sizeof(int32_t);  // 忽略帧头
        if (readFrameSize_ < 0)
        {
            throw TTransportException("Frame size has negative value");
        }
        else if ((readFrameSize_ > static_cast<int32_t>(maxFrameSize_)))
        {
            throw TTransportException(TTransportException::CORRUPTED_DATA, "Received an oversized frame");
        }

        // Read the frame payload, and reset markers.
        if (readFrameSize_ > static_cast<int32_t>(rBufSize_))
        {
            rBuf_.reset(new uint8_t[readFrameSize_]);
            rBufSize_ = readFrameSize_;
        }
        std::memcpy(rBuf_.get(), readFrame_ + sizeof(int32_t), readFrameSize_);
        setReadBuffer(rBuf_.get(), readFrameSize_);
        readFrameReady_ = false;
        ret = true;

        // 回收缓冲，以防止缓冲过大，减小内存开销
        if (readFrameCapacity_ > bufReclaimThresh_)
        {
            delete[] readFrame_;
            allocBuffers();
        }
    }

    return ret;
}

}
}
