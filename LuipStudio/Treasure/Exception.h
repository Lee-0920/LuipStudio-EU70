#ifndef SYSTEM_EXCEPTION_H_
#define SYSTEM_EXCEPTION_H_

#include "Treasure/Types.h"
#include <stdexcept>

namespace System
{

/**
 * @brief 表示在应用程序执行期间发生的错误。
 * @details 此类是所有异常的基类。当发生错误时，
 *  系统或当前正在执行的应用程序通过引发包含关于该错误的信息的异常来报告错误。
 *  异常发生后，将由该应用程序或默认异常处理程序进行处理。
 */
class Exception : public std::exception
{
public:
    Exception() : m_message("Exception") {}
    Exception(const char * message) : m_message(message) {}
    virtual ~Exception() {}

    /**
     * @brief 查询异常的信息。
     */
    virtual const String& what()
    {
        return m_message;
    }

protected:
    String m_message;
};


/**
 * @brief 为 System 命名空间中的预定义异常定义基类。
 */
class SystemException : public Exception
{
public:
    SystemException() : Exception("SystemException") {}
    SystemException(const char* message) : Exception(message) {}
};

/**
 * @brief 升级异常。
 */
class UpdateException : public SystemException
{
public:
    UpdateException() : SystemException("UpdateException") {}
    UpdateException(const char* message) : SystemException(message) {}
};

/**
 * @brief 在向方法提供的其中一个参数无效时引发的异常。
 */
class ArgumentException : public SystemException
{
public:
    ArgumentException() : SystemException("ArgumentException") {}
    ArgumentException(const char* message) : SystemException(message) {}
};


/**
 * @brief 当将空引用（指针）传递给不接受它作为有效参数的方法时引发的异常。
 */
class ArgumentNullException  : public ArgumentException
{
public:
    ArgumentNullException () : ArgumentException("ArgumentNullException ") {}
    ArgumentNullException (const char* message) : ArgumentException(message) {}
};


/**
 * @brief 当参数值超出调用的方法所定义的允许取值范围时引发的异常。
 */
class ArgumentOutOfRangeException  : public ArgumentException
{
public:
    ArgumentOutOfRangeException () : ArgumentException("ArgumentOutOfRangeException ") {}
    ArgumentOutOfRangeException (const char* message) : ArgumentException(message) {}
};


/**
 * @brief 当方法调用对于对象的当前状态无效时引发的异常。
 */
class InvalidOperationException : public SystemException
{
public:
    InvalidOperationException() : SystemException("InvalidOperationException") {}
    InvalidOperationException(const char* message) : SystemException(message) {}
};


/**
 * @brief 对已释放的对象执行操作时所引发的异常。
 */
class ObjectDisposedException : public InvalidOperationException
{
public:
    ObjectDisposedException() : InvalidOperationException("ObjectDisposedException") {}
    ObjectDisposedException(const char* message) : InvalidOperationException(message) {}
};


/**
 * @brief 当调用的方法不受支持时引发的异常。 
 */
class NotSupportedException : public SystemException
{
public:
    NotSupportedException() : SystemException("NotSupportedException") {}
    NotSupportedException(const char* message) : SystemException(message) {}
};


/**
 * @brief 当功能未在特定平台上运行时所引发的异常。 
 */
class PlatformNotSupportedException : public NotSupportedException
{
public:
    PlatformNotSupportedException() : NotSupportedException("PlatformNotSupportedException") {}
    PlatformNotSupportedException(const char* message) : NotSupportedException(message) {}
};


/**
 * @brief 当操作系统因 I/O 错误或指定类型的安全错误而拒绝访问时所引发的异常。 
 */
class UnauthorizedAccessException : public SystemException
{
public:
    UnauthorizedAccessException() : SystemException("UnauthorizedAccessException") {}
    UnauthorizedAccessException(const char* message) : SystemException(message) {}
};


/**
 * @brief 取消线程正在执行的操作时在线程中引发的异常。 
 */
class OperationCanceledException : public SystemException
{
public:
    OperationCanceledException() : SystemException("OperationCanceledException") {}
    OperationCanceledException(const char* message) : SystemException(message) {}
};


/**
 * @brief 没有足够的内存继续执行程序时引发的异常。 
 */
class OutOfMemoryException : public SystemException
{
public:
    OutOfMemoryException() : SystemException("OutOfMemoryException") {}
    OutOfMemoryException(const char* message) : SystemException(message) {}
};

/**
 * @brief HJ212模块通信异常。
 */
class HJ212CommunicationException : public SystemException
{
public:
    HJ212CommunicationException() : SystemException("HJ212CommunicationException") {}
    HJ212CommunicationException(const char* message) : SystemException(message) {}
    HJ212CommunicationException(const char* message = "HJ212CommunicationException", int id = -1) : SystemException(message)
    {
        this->m_id = id;
    }
public:
    int m_id;
};

/**
 * @brief HJ212命令超时异常。
 */
class HJ212CmdTimeoutException : public HJ212CommunicationException
{
public:
    HJ212CmdTimeoutException(const char* message = "HJ212CmdTimeoutException", int id = -1) : HJ212CommunicationException(message, id){}
};

/**
 * @brief HJ212命令发送失败异常。
 */
class HJ212CmdFailException : public HJ212CommunicationException
{
public:
    HJ212CmdFailException(const char* message = "HJ212CmdFailException", int id = -1) : HJ212CommunicationException(message, id){}
};

/**
 * @brief HJ212单次上传数量超出最大限制异常。
 */
class HJ212UploadNumberOutOfLimitException : public HJ212CommunicationException
{
public:
    HJ212UploadNumberOutOfLimitException(const char* message = "HJ212UploadNumberOutOfLimitException", int id = -1) : HJ212CommunicationException(message, id){}
};

/**
 * @brief PLC模块的异常。
 */
class PlcException : public SystemException
{
public:
    PlcException() : SystemException("PlcException") {}
    PlcException(const char* message) : SystemException(message) {}
};

/**
 * @brief Plc通信连接异常。
 */
class PlcCommunicationException : public PlcException
{
public:
    PlcCommunicationException() : PlcException("PlcCommunicationException") {}
    PlcCommunicationException(const char* message) : PlcException(message) {}
};

/**
 * @brief Plc I/O操作错误异常。
 */
class PlcIOWrongException : public PlcException
{
public:
    PlcIOWrongException() : PlcException("PlcIOWrongException") {}
    PlcIOWrongException(const char* message) : PlcException(message) {}
    PlcIOWrongException(const char* message = "PlcIOWrongException", int errCode = -100) : PlcException(message)
    {
        this->m_errCode = errCode;
    }
public:
    int m_errCode;
};


} // namespace System




#endif  // SYSTEM_EXCEPTION_H_
