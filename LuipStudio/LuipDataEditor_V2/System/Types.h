/**
 * @file
 * @brief 基本数据类型。
 * @details 定义常用的基本数据类型。
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2012-10-3
 */

#ifndef SYSTEM_TYPES_H_
#define SYSTEM_TYPES_H_

#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <QChar>
#include <QStringList>

// 定义常量字符数组的包装宏 _T()
#ifndef _T
#ifdef _UNICODE
#define _T(x) L ## x
#else
#define _T(x) x
#endif // _UNICODE
#endif // #ifndef _T


#ifndef TRUE
#define TRUE            1           ///< 布尔值：真
#endif
#ifndef FALSE
#define FALSE           0           ///< 布尔值：假
#endif

#ifndef NULL
#define NULL            ((void *)0) ///< 空指针
#endif


namespace System
{

typedef unsigned char Bool;         ///< 布尔类型

typedef unsigned char Byte;         ///< 字节基本类型
typedef short Wchar;                ///< 宽字符

typedef char Int8;                  ///< 8 位有符号整数
typedef short Int16;                ///< 16 位有符号整数
typedef long Int32;                 ///< 32 位有符号整数
typedef long long Int64;            ///< 64 位有符号整数
typedef unsigned char Uint8;        ///< 8 位无符号整数
typedef unsigned short Uint16;      ///< 16 位无符号整数
typedef unsigned long Uint32;       ///< 32 位无符号整数
typedef unsigned long long Uint64;  ///< 64 位无符号整数

typedef float Float32;              ///< 32 位 IEEE 754 浮点数
typedef double Float64;             ///< 64 位 IEEE 754 浮点数

// 使用 Qt 的字符串
typedef QChar Char;                 ///< 字符基本类型
typedef std::string String;             ///< 基本字符串类型
typedef QStringList StringList;     ///< 基本字符串列表类型

inline std::string Format(const char *format,...)
{
    va_list argptr;
    va_start(argptr, format);

#ifdef _CS_X86_WINDOWS
    int count = _vsnprintf(NULL,0,format,argptr);
#else
    int count = vsnprintf(NULL,0,format,argptr);
#endif

    va_end(argptr);

    va_start(argptr, format);
    char* buf = (char*)malloc(count*sizeof(char));

#ifdef _CS_X86_WINDOWS
    _vsnprintf(buf,count,format,argptr);
#else
    vsnprintf(buf,count,format,argptr);
#endif

    va_end(argptr);

    std::string str(buf,count);
    free(buf);
    return str;
}

} // namespace System


/**
 * @brief 提供类似C++11标准的强类型枚举定义的支持。
 * @details 使用本宏能限定枚举值的作用域。
 * @par 使用举例：
 *  @code
 *      enum_class(Color) // 相当于新标准：enum class Color
 *      {
 *          Red,
 *          White,
 *          Black
 *      };
 *      // ...
 *      Color color = Color::White;
 *  @endcode
 */
#define enum_class(E)                           \
class E                                         \
{                                               \
public:                                         \
    enum Enum;                                  \
private:                                        \
    Enum _value;                                \
public:                                         \
    E() {}                                      \
    E(Enum value) : _value(value) {}            \
    operator Enum() const                       \
    {                                           \
        return _value;                          \
    }                                           \
};                                              \
enum E::Enum


// 包含其它常用的类型定义
#include "Uuid.h"

#endif  // SYSTEM_TYPES_H_
