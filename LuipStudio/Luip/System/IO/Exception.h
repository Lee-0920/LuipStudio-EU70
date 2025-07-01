#ifndef SYSTEM_IO_EXCEPTION_H_
#define SYSTEM_IO_EXCEPTION_H_

#include "System/Exception.h"

namespace System { namespace IO 
{


/**
 * @brief 发生 I/O 错误时引发的异常。
 */
class IOException : public SystemException
{
public:
    IOException() : SystemException("IOException") {}
    IOException(const char* message) : SystemException(message) {}
};


/**
 * @brief 当找不到文件或目录的一部分时所引发的异常。 
 */
class DirectoryNotFoundException : public IOException
{
public:
    DirectoryNotFoundException() : IOException("DirectoryNotFoundException") {}
    DirectoryNotFoundException(const char* message) : IOException(message) {}
};


/**
 * @brief 当尝试访问的驱动器或共享不可用时引发的异常。 
 */
class DriveNotFoundException : public IOException
{
public:
    DriveNotFoundException() : IOException("DriveNotFoundException") {}
    DriveNotFoundException(const char* message) : IOException(message) {}
};


/**
 * @brief 读操作试图超出流的末尾时引发的异常。
 */
class EndOfStreamException : public IOException
{
public:
    EndOfStreamException() : IOException("EndOfStreamException") {}
    EndOfStreamException(const char* message) : IOException(message) {}
};


/**
 * @brief 试图访问磁盘上不存在的文件失败时引发的异常。
 */
class FileNotFoundException : public IOException
{
public:
    FileNotFoundException() : IOException("FileNotFoundException") {}
    FileNotFoundException(const char* message) : IOException(message) {}
};


/**
 * @brief 当路径名或文件名超过系统定义的最大长度时引发的异常。
 */
class PathTooLongException : public IOException
{
public:
    PathTooLongException() : IOException("PathTooLongException") {}
    PathTooLongException(const char* message) : IOException(message) {}
};

/**
 * @brief LuipFileException操作异常。
 */
class LuipFileException : public IOException
{
public:
    LuipFileException() : IOException("LuipFileException") {}
    LuipFileException(const char* message) : IOException(message) {}
};

/**
 * @brief 文件类型错误异常。
 */
class UnexpectedFormatException : public LuipFileException
{
public:
    UnexpectedFormatException() : LuipFileException("UnexpectedFormatException") {}
    UnexpectedFormatException(const char* message) : LuipFileException(message) {}
};

class UnsupportedVersionException : public LuipFileException
{
public:
    UnsupportedVersionException() : LuipFileException("UnsupportedVersionException") {}
    UnsupportedVersionException(const char* message) : LuipFileException(message) {}
};

/**
 * @brief 文件破损异常。
 */
class IntegrityCheckException : public LuipFileException
{
public:
    IntegrityCheckException() : LuipFileException("IntegrityCheckException") {}
    IntegrityCheckException(const char* message) : LuipFileException(message) {}
};
}} // namespace System::IO 



#endif  // SYSTEM_IO_EXCEPTION_H_