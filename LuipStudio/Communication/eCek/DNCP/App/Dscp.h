/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief 设备简单控制协议 DSCP 的公共定义。
 * @details 
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2013-2-26
 */

#ifndef DNCP_APP_DSCP_H_
#define DNCP_APP_DSCP_H_

#ifdef __cplusplus
extern "C" {
#endif


//*******************************************************************
// DSCP 包头定义

// DSCP 包类型定义
#define DSCP_TYPE_CMD               0
#define DSCP_TYPE_RESP_INFO         1
#define DSCP_TYPE_RESP_STATUS       2
#define DSCP_TYPE_EVENT             3

/**
 * @brief DSCP 控制字。
 */
typedef struct DscpCtrlWordStruct
{
    unsigned char type       : 2;        ///< DSCP包类型，可以是命令、回应或事件，如： @ref DSCP_TYPE_CMD
    unsigned char needFollow : 1;        ///< 该数据包之后是否还有数据包跟随。
    unsigned char reserved   : 5;        ///< 保留，暂时不用
}
DscpCtrlWord;



//*******************************************************************
// DSCP 应用接口的数据结构定义

typedef unsigned short DscpCode;        ///< 抽象码值类型。
typedef unsigned short CmdCode;         ///< 命令码类型。
typedef unsigned short RespCode;        ///< 回应码类型。
typedef unsigned short StatusCode;      ///< 状态码类型。
typedef unsigned short EventCode;       ///< 事件码类型。

/**
 * @brief DSCP 版本号
 */
typedef struct DscpVersionStruct
{
    unsigned char major;        ///< 主版本号
    unsigned char minor;        ///< 次版本号
    unsigned char revision;     ///< 修订版本号
    unsigned char build;        ///< 编译版本号
}
DscpVersion;


//*******************************************************************
// DSCP 设备端应用命令表接口

/**
 * @brief DSCP 命令处理接口函数。
 * @param[in] dscp DSCP协议体对象。
 * @param[in] data 命令包的参数数据。
 * @param[in] len 命令包的参数数据。
 * @note 协议栈会根据接收的命令自动调用该函数。
 *  <p> 如果需要回应，并且在命令处理函数返回前可以得到执行结果，那么可以直接调用 
 *  SendResp() 发送回应，此时不用处理源地址，因为 SendResp()
 *  会自动处理这些细节。 SendStatus() 类似。
 *  <p> 如果在命令处理函数返回前不能得到结果，应先调用 GetSourceAddr()
 *  获取源地址并缓存，等到执行结果出来后，再调用 SetDestAddr() 设置目的地址，
 *  再调用具体的发送函数。
 * @attention 从 @p data 中取参数时，请注意CPU字对齐问题。本协议栈不能保证
 *  @p data 指向的数据是16位或32位对齐的，为安全起见，请使用循环单字节操作。
 */
typedef void (*IfDscpCmdHandle)(void* dscp, unsigned char* data, unsigned short len);


/**
 * @brief DSCP 命令表结点
 */
typedef struct DscpCmdEntryStruct
{
    CmdCode cmdCode;                ///< 命令码
    IfDscpCmdHandle handle;         ///< 命令处理函数
}
DscpCmdEntry;

typedef DscpCmdEntry* DscpCmdTable;    ///< 命令表类型。

/**
 * @brief 定义一个命令结点，指定命令码对应的处理函数。
 * @param[in] code 命令码。
 * @param[in] handle 处理函数。
 */
#define DSCP_CMD_ENTRY(code, handle)    {code, (IfDscpCmdHandle)handle}

/**
 * @brief 定义一个命令表的结束结点。
 */
#define DSCP_CMD_ENTRY_END              {0, 0}


#ifdef __cplusplus
}
#endif


#endif  // DNCP_APP_DSCP_H_

/** @} */
