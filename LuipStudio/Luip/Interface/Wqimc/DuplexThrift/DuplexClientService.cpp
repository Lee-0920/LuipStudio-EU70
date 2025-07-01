/**
 * @file
 * @brief 双向调用客户端服务。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015-5-18
 */

#include <boost/make_shared.hpp>
#include <thrift/concurrency/Monitor.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/transport/TBufferTransports.h>
#include "FrameType.h"
#include "DuplexClientService.h"
#include <QThread>
#include "Log.h"
#include "LuaEngine/LuaEngine.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::transport;
using namespace Lua;

namespace erchashu {
namespace thrift {

#define LIVE_COEF       3    // 存活次数，即心跳包没响应的次数超过这个值，即认为连接断开

DuplexClientService::DuplexClientService(const boost::shared_ptr<CallFramedTransport>& transport)
    : clientTransport_(transport),
      m_isEnableDisconnectNotify(false)
{
    clientTransport_->setFrameSender(this);

    protocolFactory_ = boost::make_shared<TCompactProtocolFactory>();
    transportFactory_ = boost::make_shared<TTransportFactory>();
    threadFactory_ = boost::make_shared<PlatformThreadFactory>(true);

    bufferInitialSize_ = DEFAULT_BUFFER_SIZE;
    bufferReclaimThresh_ = DEFAULT_BUFFER_SIZE * 1024;
    maxFrameSize_ = DEFAULT_MAX_FRAME_SIZE;

    readBufferSize_ = 0;
    readBufferCapacity_ = bufferInitialSize_;
    readBuffer_.reset(new uint8_t[readBufferCapacity_]);

    enableHeartbeat_ = false;
    heartRate_ = 60;
    connectionNotifier_ = nullptr;
    int echoFrameSize = 4;
    echoFrameSize = (int32_t)htonl((uint32_t)(echoFrameSize));
    memcpy(echoFrameData_, (uint8_t*)&echoFrameSize, sizeof(echoFrameSize));
    echoFrameSize = (int32_t)htonl((uint32_t)(heartRate_));
    memcpy(echoFrameData_ + 4, (uint8_t*)&echoFrameSize, sizeof(echoFrameSize));
    
    m_newState = LuaEngine::Instance()->CreateThread();
}

DuplexClientService::~DuplexClientService()
{

}

void DuplexClientService::EnableDisconnectNotify(bool enable)
{
    unique_lock<mutex> lock(m_NotifyMutex);
    m_isEnableDisconnectNotify = enable;
}

bool DuplexClientService::isConnected()
{
    if (socket_ == nullptr)
        return false;
    return socket_->isOpen();
}

class ThreadRunner : public Runnable
{
public:
    ThreadRunner(Runnable* runnable) : runnable_(runnable) {}
    virtual ~ThreadRunner() {}
    void run()
    {
        runnable_->run();
    }
private:
    Runnable* runnable_;
};

/**
 * @brief 连接到服务器。
 * @throw TTransportException 连接异常。
 */
bool DuplexClientService::connect()
{
    unique_lock<mutex> socketLock(m_socketMutex);

    if (serverHost_.length() == 0)
    {
        throw TTransportException(TTransportException::NOT_OPEN, "Cannot open null host.");
    }
    if (serverPort_ <= 0 || serverPort_ > 65535)
    {
        throw TTransportException(TTransportException::NOT_OPEN, "Invalid port " + serverPort_);
    }

    socket_.reset(new TSocket(serverHost_, serverPort_));
    socket_->setKeepAlive(true);
    // 设置阻塞接收的超时时间，超时未接收到数据将抛出异常
    if (enableHeartbeat_)
    {
        socket_->setRecvTimeout(heartRate_ * 1000);
    }

    try
    {
        // 建立反向服务的 RPC 处理链
        inBuffer_ = boost::make_shared<TMemoryBuffer>((uint8_t*)0, 0);
        inTrans_ = transportFactory_->getTransport(inBuffer_);
        outBuffer_ = boost::make_shared<TMemoryBuffer>();
        outTrans_ = transportFactory_->getTransport(outBuffer_);
        inProt_ = protocolFactory_->getProtocol(inTrans_);
        outProt_ = protocolFactory_->getProtocol(outTrans_);

        // 启动通信线程
        communicationThread_ = threadFactory_->newThread(boost::make_shared<ThreadRunner>(this));
        this->thread(communicationThread_);
        this->start();

        socket_->open();
        this_thread::sleep_for(chrono::milliseconds(20));

        if (socket_->isOpen())
        {
//            logger->debug("DuplexClientService::connect() => TCP socket connected success.");
            GlobalOutput("TCP socket connected success.");

            return true;
        }
        else
        {
//            logger->debug("DuplexClientService::connect() => TCP socket connected fail!!!");
            GlobalOutput("TCP socket connected fail!!!");
        }

    }catch (TTransportException ex)
    {
//        closeConnection();
        throw ex;
    }catch (TProtocolException ex)
    {
        throw ex;
    }catch(TException ex)
    {
//        closeConnection();
       throw ex;
   } catch(exception ex)
   {
//       closeConnection();
       throw ex;
   }

   return false;
}

bool DuplexClientService::reconnect()
{
    unique_lock<mutex> socketLock(m_socketMutex);

    socket_->close();

    this_thread::sleep_for(chrono::seconds(5));
    socket_.reset(new TSocket(serverHost_, serverPort_));
    socket_->setKeepAlive(true);

    // 设置阻塞接收的超时时间，超时未接收到数据将抛出异常
    if (enableHeartbeat_)
    {
        socket_->setRecvTimeout(heartRate_ * 1000);
    }

    try
    {
        socket_->open();

        this_thread::sleep_for(chrono::seconds(1));

        if (socket_->isOpen())
        {
            GlobalOutput("TCP socket reconnected success.");
//            logger->debug("DuplexClientService::reconnect() => TCP socket reconnected success.");
            return true;
        }
        else
        {
//            logger->debug("DuplexClientService::reconnect() => TCP socket reconnected fail.");
            GlobalOutput("TCP socket reconnected fail!!!!!!!");
        }

    }catch (TTransportException ex)
    {
//        closeConnection();
        throw ex;
    }catch (TProtocolException ex)
    {
        throw ex;
    }
    catch(TException ex)
    {
//        closeConnection();
        throw ex;
    } catch(exception ex)
    {
//        closeConnection();
        throw ex;
    }

    return false;
}

void DuplexClientService::closeConnection()
{
    socket_->close();
}

void DuplexClientService::start()
{
    isServing_ = true;
    communicationThread_->start();
}

void DuplexClientService::stop()
{
    isServing_ = false;
    communicationThread_->join();
}

void DuplexClientService::run()
{
    logger->debug("DuplexClientService Thread ID = %d", QThread::currentThreadId());
    LuaEngine::Instance()->AttachCurrentThread(m_newState);

    int heartbeatMissingCount_ = 0;    // 心跳未响应计数器
    int readFailCount_ = 0;            // 读数据失败计数器

    this_thread::sleep_for(chrono::seconds(5));

    while (isServing_)
    {
        bool isSendEcho = false;    // 是否发生心跳包
        bool status = false;        // 从 socket 中读出一帧数据，根据帧类型进行分发

        try
        {
            status = readFrame();

            // 接收到一帧数据
            if (status)
            {
                readFailCount_ = 0;
                heartbeatMissingCount_ = 0;

                uint8_t* buf = readBuffer_.get();
                switch (readBufferFrameType_)
                {
                case FrameType::ThriftCall:
                    inBuffer_->resetBuffer(buf, readBufferSize_);
                    outBuffer_->resetBuffer();
                    processor_->process(inProt_, outProt_, (void*)0);
                    uint32_t size;
                    outBuffer_->getBuffer(&buf, &size);
                    {
                        Synchronized s(sendMonitor_);
                        buf[0] = (uint8_t)FrameType::ThriftReply;


                        unique_lock<mutex> socketLock(m_socketMutex);
                        if (this->isConnected())
                        {
                            socket_->write(buf, size);
                        }
                    }
                    break;

                case FrameType::ThriftReply:
                    clientTransport_->putFrame(buf, readBufferSize_);
                    break;

                case FrameType::Echo:
                    if (enableHeartbeat_)
                    {
                        // 使用服务器建议的心跳时间
                        int hr;
                        memcpy(&hr, buf + 4, 4);
                        hr = ntohl(hr);
                        if (hr != heartRate_ && hr > 0 && hr < 3600)
                        {
                            setHeartRate(hr);

                            unique_lock<mutex> socketLock(m_socketMutex);
                            if (this->isConnected())
                            {
                                socket_->setRecvTimeout(heartRate_ * 1000);
                            }
                        }

 //                       logger->debug("[thrift] DuplexClientService::run() => Echo Reply, hr = %d", hr);
//                        GlobalOutput.printf("Echo Reply, hr = %d", hr);
                    }
                    break;

                default:
                    break;
                }
            }
            else
            {
                readFailCount_++;
//              logger->debug("DuplexClientService::run() => readFailCount_ = %d", readFailCount_);

                if (readFailCount_ >= 70)
                {
//                    logger->debug("[thrift] DuplexClientService::run() => ******* Read frame fail !!!");
                    readFailCount_ = 0;
                    isSendEcho = true;
                }
            }

        } catch(TTransportException ex)
        {
            if (ex.getType() == TTransportException::TIMED_OUT)
            {
                isSendEcho = true;
            }
            else
            {
                this->DuplexExceptionLogOutput("DuplexClientService::run() 1", ex);
            }

        } catch (TProtocolException ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 1", ex);

        }catch (TException ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 1", ex);
        }catch(exception ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 1", ex);
        }

        try
        {
            // 读数据超时，发送心跳帧
            if (isSendEcho && enableHeartbeat_)
            {
                if (heartbeatMissingCount_ >= LIVE_COEF)     // 超过一定次数仍未收到回包，认为通信断开
                {
                    logger->debug("DuplexClientService::run() => *******Heartbeat Missing to reconnet!!!");
                    this->EnableDisconnectNotify(false);
                    heartbeatMissingCount_ = 0;
                    readFailCount_ = 0;
                    connectionNotifier_->onDisconnected();
                }
                else                                        // 超时未收到回包，发送心跳帧
                {
//                    logger->debug("[thrift] DuplexClientService::run() => send Echo .....");
                    heartbeatMissingCount_++;
                    sendFrame(FrameType::Echo, echoFrameData_, sizeof(echoFrameData_));
                }
            }
        }catch(TTransportException ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 2", ex);
        }catch (TProtocolException ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 2", ex);
        }catch (TException ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 2", ex);
        }catch(exception ex)
        {
            this->DuplexExceptionLogOutput("DuplexClientService::run() 2", ex);
        }

        if (status)
        {
            this_thread::sleep_for(chrono::milliseconds(2));
        }
        else
        {
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

   logger->debug("DuplexClientService thread exit!!!");
   this->closeConnection();
}

// 从 socket 中读出一帧数据
bool DuplexClientService::readFrame()
{
    unique_lock<mutex> socketLock(m_socketMutex);
    if (!this->isConnected())
    {
        return false;
    }

    // 读出下一帧的头部
    // 不能调用 readAll(&sz, sizeof(sz)) ，因为这会导致 EOF 情况下抛出异常
    int32_t frameSize = -1;
    uint32_t sizeBytesRead = 0;

    readBufferFrameType_ = FrameType::None;

    while (sizeBytesRead < sizeof(frameSize))
    {
        uint8_t* szp = reinterpret_cast<uint8_t*>(&frameSize) + sizeBytesRead;
        uint32_t bytesRead
                = socket_->read(szp, static_cast<uint32_t>(sizeof(frameSize)) - sizeBytesRead);
        if (bytesRead == 0)
        {
            if (sizeBytesRead == 0)
            {
                // EOF before any data was read.
                return false;
            }
            else
            {
                // EOF after a partial frame header.  Raise an exception.
                throw TTransportException(TTransportException::END_OF_FILE,
                                          "No more data to read after "
                                          "partial frame header.");
            }
        }
        sizeBytesRead += bytesRead;
    }

    readBufferFrameType_ = static_cast<FrameType>(frameSize & 0x000000FF);
    frameSize = ntohl(frameSize & 0xFFFFFF00);

    if (frameSize < 0)
    {
        throw TTransportException("Frame size has negative value");
    }

    // 帧过大，或者帧数据被损坏
    if (frameSize > static_cast<int32_t>(maxFrameSize_))
        throw TTransportException(TTransportException::CORRUPTED_DATA, "Received an oversized frame");

    // 如有必要，自动调整帧缓冲：容量不够则增大缓冲，容量超过回收线则减小缓冲到初始大小
    int size = frameSize + sizeof(frameSize);
    if (size > static_cast<int32_t>(readBufferCapacity_))
    {
        readBuffer_.reset(new uint8_t[size]);
        readBufferCapacity_ = size;
    }
    else if (readBufferCapacity_ > bufferReclaimThresh_)
    {
        if (size <= bufferInitialSize_)
        {
            readBuffer_.reset(new uint8_t[bufferInitialSize_]);
            readBufferCapacity_ = bufferInitialSize_;
        }
    }
    else
    {
        memset(readBuffer_.get(), 0, readBufferCapacity_);
    }

    // 读出帧数据
    socket_->readAll(readBuffer_.get() + sizeof(frameSize), frameSize);
    readBufferSize_ = size;
    frameSize = htonl(frameSize);   // 转回网络字节序
    memcpy(readBuffer_.get(), &frameSize, sizeof(frameSize));


    return true;
}

void DuplexClientService::sendFrame(FrameType type, uint8_t* buffer, int len)
{
    if (isServing_ && this->isConnected())
    {
        Synchronized s(sendMonitor_);
        buffer[0] = (uint8_t)type;

        socket_->write(buffer, len);

    }
}

void DuplexClientService::DuplexExceptionLogOutput(std::string path, std::exception &e)
{
    static int count = 10;

    if (count > 0)
    {
        if (OOLUA::Exception* ex = dynamic_cast<OOLUA::Exception* >(&e))
        {
            logger->warn((std::string("[thrift] ") + path + " => [OOLUA::Exception] %s").c_str(),ex->what());
        }
        else if (TTransportException* ex = dynamic_cast<TTransportException* >(&e))
        {
            logger->warn((std::string("[thrift] ") + path + " => [TTransportException] %s").c_str(), ex->what());
        }
        else if (TProtocolException* ex = dynamic_cast<TProtocolException* >(&e))
        {
            logger->warn((std::string("[thrift] ") + path + " => [TProtocolException] %s").c_str(), ex->what());
        }
        else if (TException* ex = dynamic_cast<TException* >(&e))
        {
            logger->warn((std::string("[thrift] ") + path + " => [TException] %s").c_str(), ex->what());
        }
        else if (std::exception* ex = dynamic_cast<std::exception* >(&e))
        {
            logger->warn((std::string("[thrift] ") + path + " => [std::exception] %s").c_str(), ex->what());
        }
    }

    count--;
}

}
}
