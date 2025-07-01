/**
 * @file
 * @brief 双向调用客户端服务。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015-5-18
 */

#ifndef ERCHASHU_THRIFT_DUPLEX_CLIENT_SERVICE_H_
#define ERCHASHU_THRIFT_DUPLEX_CLIENT_SERVICE_H_

#include <mutex>
#include <condition_variable>
#include <thrift/TProcessor.h>
#include <thrift/concurrency/Thread.h>
#include <thrift/concurrency/Monitor.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TSocket.h>
#include "CallFramedTransport.h"
#include "LuaEngine/LuaEngine.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::concurrency;

namespace erchashu {
namespace thrift {


/**
 * 连接通知器。
 */
class ConnectionNotifier
{
public:
    /**
     * 服务器连接通知。
     */


    /**
     * 服务器断开通知。
     */
    virtual void onDisconnected() = 0;

    virtual ~ConnectionNotifier() {}
};


/**
 * @brief 双向调用客户端服务。
 */
class DuplexClientService : public FrameSender, public Runnable
{
public:
    static const int DEFAULT_BUFFER_SIZE = 1024;
    static const int DEFAULT_MAX_FRAME_SIZE = 16 * 1024 * 1024 - 1;
    DuplexClientService(const boost::shared_ptr<CallFramedTransport>& transport);
    virtual ~DuplexClientService();
    lua_State *m_newState;
public:
    /**
     * @brief 设置服务器连接参数。
     * @param host 服务器主机名/IP地址。
     * @param port 服务端口号。
     */
    void setServer(const std::string& host, int port)
    {
        serverHost_ = host;
        serverPort_ = port;
    }

    /**
     * @brief 设置反向 RPC 服务的处理器。
     */
    void setProcessor(const boost::shared_ptr<TProcessor>& processor)
    {
        processor_ = processor;
    }

    /**
     * @brief 设置反向 RPC 服务使用的协议工厂，默认为 TBinaryProtocol 工厂。
     */
    void setProtocolFactory(const boost::shared_ptr<TProtocolFactory>& factory)
    {
        protocolFactory_ = factory;
    }


    /**
     * @brief 设置反向 RPC 服务使用的中间传输器工厂，默认为无。
     */
    void setTransportFactory(const boost::shared_ptr<TTransportFactory>& factory)
    {
        transportFactory_ = factory;
    }

    void setBufferPolity(uint32_t bufferInitialSize = DEFAULT_BUFFER_SIZE,
            uint32_t bufferReclaimThresh = DEFAULT_BUFFER_SIZE*1024)
    {
        bufferInitialSize_ = bufferInitialSize;
        bufferReclaimThresh_ = bufferReclaimThresh;
    }

    /**
     * 设置是否启用心跳机制。如果未设置，默认为不启用。
     */
    void enableHeartbeat(bool enable)
    {
        enableHeartbeat_ = enable;
    }

    /**
     * 设置心跳周期，单位为秒，仅在启用心跳机制后有效。默认为 60 秒.
     * 客户端将按照这个周期发送心跳包。
     */
    void setHeartRate(int seconds) {
        heartRate_ = seconds;
        seconds = (int32_t)htonl((uint32_t)(seconds));
        memcpy(echoFrameData_ + 4, (uint8_t*)&seconds, sizeof(seconds));
    }

    void setConnectionNotifier(ConnectionNotifier* notifier) {
        connectionNotifier_ = notifier;
    }

    void EnableDisconnectNotify(bool enable);


    bool connect();
    bool reconnect();
    void disconnect();
    void start();
    void stop();

public:
    // CallReadyNotifier 接口的实现
    virtual void sendFrame(FrameType type, uint8_t* buffer, int len);
    // 线程
    virtual void run();

protected:
    bool isConnected();
    void closeConnection();
    bool readFrame();

protected:
    // 服务器连接参数
    std::string serverHost_;
    int serverPort_;
    boost::shared_ptr<TSocket> socket_;
    boost::shared_ptr<Thread> communicationThread_;
    uint32_t bufferInitialSize_;    // 缓冲初始大小
    uint32_t bufferReclaimThresh_;  // 缓冲回收线，超过该线将重新分配到初始大小，以减小不必要的内存消耗
    uint32_t maxFrameSize_;
    boost::scoped_array<uint8_t> readBuffer_;
    uint32_t readBufferCapacity_;
    uint32_t readBufferSize_;
    FrameType readBufferFrameType_;

    // 客户端调用链使用的传输器
    boost::shared_ptr<CallFramedTransport> clientTransport_;

    // 客户端提供的 RPC 服务的处理链
    boost::shared_ptr<ThreadFactory> threadFactory_;
    boost::shared_ptr<TProcessor> processor_;
    boost::shared_ptr<TProtocolFactory> protocolFactory_;
    boost::shared_ptr<TTransportFactory> transportFactory_;
    boost::shared_ptr<TTransport> inTrans_;
    boost::shared_ptr<TTransport> outTrans_;
    boost::shared_ptr<TMemoryBuffer> inBuffer_;
    boost::shared_ptr<TMemoryBuffer> outBuffer_;
    boost::shared_ptr<TProtocol> inProt_;
    boost::shared_ptr<TProtocol> outProt_;

    // 内部分发
    bool isServing_;
    apache::thrift::concurrency::Monitor sendMonitor_;

    // 心跳机制
    bool enableHeartbeat_;
    int heartRate_;
    ConnectionNotifier* connectionNotifier_;
    unsigned char echoFrameData_[8];

    bool m_isEnableDisconnectNotify;
    std::mutex m_NotifyMutex;
    std::mutex m_socketMutex;
    std::mutex m_sendMutex;

private:
    void DuplexExceptionLogOutput(std::string path, std::exception &e);
};


}
}

#endif  // ERCHASHU_THRIFT_DUPLEX_CLIENT_SERVICE_H_
