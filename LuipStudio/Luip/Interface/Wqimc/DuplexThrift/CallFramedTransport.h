/**
 * @file
 * @brief 调用缓冲分帧传输器。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015-5-18
 */

#ifndef ERCHASHU_THRIFT_CALL_FRAMED_TRANSPORT_H_
#define ERCHASHU_THRIFT_CALL_FRAMED_TRANSPORT_H_

#include <thrift/concurrency/Monitor.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TBufferTransports.h>
#include "FrameType.h"

namespace erchashu {
namespace thrift {

/**
 * @brief 帐发送器接口。
 */
class FrameSender
{
public:
    FrameSender() {}
    virtual ~FrameSender() {}

    /**
     * @brief 请求发送数据帧。
     * @param type 要发送的帧类型，见 @ref FrameType 定义。
     * @param buffer 帧数据缓冲。
     * @param len 帧长度。
     * @throws TTransportException
     */
    virtual void sendFrame(FrameType type, uint8_t* buffer, int len) = 0;

};


/**
 * @brief 调用缓冲分帧传输器。
 */
class CallFramedTransport : public apache::thrift::transport::TFramedTransport
{
public:
    CallFramedTransport()
    {
        frameSender_ = NULL;
        timeout_ = 0;
        allocBuffers();
    }

    /**
     * @brief 构造调用缓冲分帧传输器。
     * @param timeout 读等待超时时间。
     * @param bufferSize 读写缓冲的初始大小。
     * @param bufReclaimThresh 缓冲回收限值，当缓冲增长到该限值时，将重新分配为初始大小。
     */
    CallFramedTransport(uint32_t timeout,
                        uint32_t bufferSize = DEFAULT_BUFFER_SIZE,
                        uint32_t bufReclaimThresh = DEFAULT_BUFFER_SIZE*1024)
        : TFramedTransport(NULL, bufferSize, bufReclaimThresh)
    {
        frameSender_ = NULL;
        timeout_ = timeout;
        allocBuffers();
    }

    ~CallFramedTransport()
    {
        if (readFrame_)
        {
            delete[] readFrame_;
        }
    }

    void setFrameSender(FrameSender* sender)
    {
        frameSender_ = sender;
    }

public:
    void putFrame(uint8_t* buffer, int len);
    virtual void flush();
    using TBufferBase::readAll;

protected:
    virtual bool readFrame();
    void allocBuffers();

protected:
    FrameSender* frameSender_;
    uint32_t timeout_;  // read timeout, milliseconds
    bool readFrameReady_;
    uint8_t* readFrame_;
    uint32_t readFrameCapacity_;
    uint32_t readFrameSize_;
    apache::thrift::concurrency::Monitor readMonitor_;
};


/**
 * Wraps a transport into a framed one.
 *
 */
class CallFramedTransportFactory : public apache::thrift::transport::TTransportFactory
{
public:
    CallFramedTransportFactory(uint32_t timeout = 5000,
                        uint32_t bufferSize = apache::thrift::transport::TFramedTransport::DEFAULT_BUFFER_SIZE,
                        uint32_t bufReclaimThresh = 1024*apache::thrift::transport::TFramedTransport::DEFAULT_BUFFER_SIZE)
    {
        timeout_ = timeout;
        bufferSize_ = bufferSize;
        bufReclaimThresh_ = bufReclaimThresh;
    }

    virtual ~CallFramedTransportFactory() {}

    /**
    * Wraps the transport into a framed one.
    */
    virtual boost::shared_ptr<apache::thrift::transport::TTransport> getTransport(
            boost::shared_ptr<apache::thrift::transport::TTransport> trans)
    {
        (void)trans;
        return boost::shared_ptr<apache::thrift::transport::TTransport>(
                    new CallFramedTransport(timeout_, bufferSize_, bufReclaimThresh_));
    }

protected:
    uint32_t timeout_;
    uint32_t bufReclaimThresh_;
    uint32_t bufferSize_;
};


}
}

#endif  // ERCHASHU_THRIFT_CALL_FRAMED_TRANSPORT_H_
