/**
 * @page page_InstrumentInfoInterface 仪器信息接口
 *  仪器信息接口提供了仪器基本信息查询的一序列操作，同时提供了生产信息的写入操作。
 *
 *  具体命令见： @ref module_InstrumentInfoInterface
 *
 * @section sec_III_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2017.2.10)
 *
 */

/**
 * @addtogroup module_InstrumentInfoInterface 仪器信息接口
 * @{
 */

/**
 * @file
 * @brief 仪器信息接口定义。
 * @details 定义了一序列用于查询或者设置仪器信息的操作。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2017-2-10
 */

#ifndef DSCP_INSTRUMENT_INFO_INTERFACE_H_
#define DSCP_INSTRUMENT_INFO_INTERFACE_H_

#define DSCP_III_CBASE                  0x0000 + 0x2000     ///< 命令基值
#define DSCP_III_EBASE                  0x8000 + 0x2000     ///< 事件基值
#define DSCP_III_SBASE                  0x0000 + 0x2000     ///< 状态基值


// *******************************************************************
// 命令和回应


/**
 * @brief 查询仪器的类型名称。
 * @details 仪器类型是区分不同类别的产品称号，上位机（云平台）将根据这个字段
 *  识别不同的仪器。各类型的字条串列举如下：
 *  - PT62比色法产品：PT62S
 *  - 伏安溶出法产品：
 *  - 实验室产品：
 *  - 电极法产品：
 * @return 仪器类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @note 各仪器在实现该命令时，必须把本字段固化在程序中，即在烧录软件时就设置了正确
 *  的仪器类型字串。一般情况下，本字段不允许任何修改，即使生产工装也不会修改。
 */
#define DSCP_CMD_III_GET_TYPE                       (DSCP_III_CBASE + 0x00)

/**
 * @brief 设置仪器的类型名称。
 * @param name 仪器类型，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 通常情况下，本命令不对外公开，上位机（包括生产工具）不能随便更改仪器类型。
 * @see DSCP_CMD_III_GET_TYPE
 */
#define DSCP_CMD_III_SET_TYPE                       (DSCP_III_CBASE + 0x01)

/**
 * @brief 查询仪器的型号。
 * @details 仪器型号包含了仪器的类型名称和具体的型别，如 PhotoTek 6000 COD。
 * @return 仪器型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 */
#define DSCP_CMD_III_GET_MODEL                      (DSCP_III_CBASE + 0x02)

/**
 * @brief 设置仪器的型号。
 * @details 仪器型号包含了仪器的名称和具体的型别，如PT62单通道型号为：PT62S01。
 * @param model 仪器型号，String，以0结尾的ASCII字符串，长度限制为24字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MODEL
 */
#define DSCP_CMD_III_SET_MODEL                      (DSCP_III_CBASE + 0x03)

/**
 * @brief 查询仪器的序列号。
 * @details 仪器序列号唯一标识了个体仪器的整个生命周期，供产品追溯时使用。
 * @return 仪器序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @note 仪器序列号的具体格式请参考公司产品相关文档。
 */
#define DSCP_CMD_III_GET_SN                         (DSCP_III_CBASE + 0x04)

/**
 * @brief 设置仪器的序列号。
 * @details 仪器序列号唯一标识了个体仪器的整个生命周期，供产品追溯时使用。
 * @param sn 仪器序列号，String，以0结尾的ASCII字符串，长度限制为32字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 仪器序列号的具体格式请参考公司产品相关文档。
 * @see DSCP_CMD_III_GET_SN
 */
#define DSCP_CMD_III_SET_SN                         (DSCP_III_CBASE + 0x05)

/**
 * @brief 查询仪器的UUID。
 * @details 仪器UUID唯一标识了个体仪器，生产时写入。
 * @return 仪器的UUID，Byte[16]，格式见UUID相关规范。
 */
#define DSCP_CMD_III_GET_UUID                       (DSCP_III_CBASE + 0x06)

/**
 * @brief 设置仪器的UUID。
 * @details 仪器UUID唯一标识了个体仪器，生产时写入。
 * @param uuid 要写入的仪器UUID，Byte[16]，格式见UUID相关规范。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_UUID
 */
#define DSCP_CMD_III_SET_UUID                       (DSCP_III_CBASE + 0x07)

/**
 * @brief 查询仪器的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @return 仪器的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 */
#define DSCP_CMD_III_GET_MANUFACTURER               (DSCP_III_CBASE + 0x08)

/**
 * @brief 设置仪器的生产厂商。
 * @details 生产厂商指明了生产该产品个体的厂商，可用于产品的追溯。
 * @param manufacturer 仪器的生产厂商，String，以0结尾的ASCII字符串，长度限制为20字符以内。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MODEL
 */
#define DSCP_CMD_III_SET_MANUFACTURER               (DSCP_III_CBASE + 0x09)

/**
 * @brief 查询仪器的生产日期。
 * @return 仪器的生产日期，Byte[4]：
 *  - @b year Uint16，年。
 *  - @b month Uint8，月。
 *  - @b day Uint8，日。
 */
#define DSCP_CMD_III_GET_MANUF_DATE                 (DSCP_III_CBASE + 0x0A)

/**
 * @brief 设置仪器的生产日期。
 * @param year Uint16，年。
 * @param month Uint8，月。
 * @param day Uint8，日。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_MANUF_DATE
 */
#define DSCP_CMD_III_SET_MANUF_DATE                 (DSCP_III_CBASE + 0x0B)

/**
 * @brief 查询仪器的系统时间。
 * @return 仪器的系统时间， Uint64 ， 从 1970-01-01T00:00:00 (UTC) 开始计时的毫秒数。
 * @note 实现指示：参考：  QDateTime::currentMSecsSinceEpoch()
 */
#define DSCP_CMD_III_GET_SYSTEM_TIME                (DSCP_III_CBASE + 0x0C)

/**
 * @brief 设置仪器的系统时间。
 * @param utcTime Uint64 UTC时间，从 1970-01-01T00:00:00 (UTC) 开始计时的毫秒数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_III_GET_SYSTEM_TIME
 */
#define DSCP_CMD_III_SET_SYSTEM_TIME                (DSCP_III_CBASE + 0x0D)


/**
 * @brief 查询应用软件版本号。
 * @details 软件版本号通常固化在软件代码中，可作后期运维管理用。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
#define DSCP_CMD_III_GET_SOFTWARE_VERSION           (DSCP_III_CBASE + 0x16)

/**
 * @brief 查询应用软件标识。
 * @details 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
 *  标准版本（非定制）的格式统一为： STD-[SVN Revision]-[YYYY.MM.DD]
 * @return 软件标识，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
#define DSCP_CMD_III_GET_SOFTWARE_LABEL             (DSCP_III_CBASE + 0x17)

/**
 * @brief 查询平台软件版本号。
 * @details 软件版本号通常固化在软件代码中，可作后期运维管理用。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
#define DSCP_CMD_III_GET_PLATFORM_VERSION           (DSCP_III_CBASE + 0x18)

/**
 * @brief 查询平台软件标识。
 * @details 软件标识可作为软件版本的补充，如客户定制版本、分支标号、编译日期等。
 *  标准版本（非定制）的格式统一为： STD-[SVN Revision]-[YYYY.MM.DD]
 * @return 软件标识，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
#define DSCP_CMD_III_GET_PLATFORM_LABEL             (DSCP_III_CBASE + 0x19)

/**
 * @brief 查询仪器的名称。
 * @details 仪器名称表征了仪器的用途，如： COD Online Analyzer。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
#define DSCP_CMD_III_GET_INSTRUMENT_NAME            (DSCP_III_CBASE + 0x20)

/**
 * @brief 查询仪器的测量参数。
 * @details 测量参数指仪器能测量的水体中的某一化学元素，如 COD。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为64字符以内。
 */
#define DSCP_CMD_III_GET_MEASURE_PARAMETER          (DSCP_III_CBASE + 0x21)

/**
 * @brief 查询仪器的测量参数编码。
 * @details 测量参数编码指行业统一定义的水体元素代码，如 COD的编码为011。
 * @return 仪器名称，String，以0结尾的ASCII字符串，长度限制为16字符以内。
 */
#define DSCP_CMD_III_GET_MEASURE_PARAMETER_CODE     (DSCP_III_CBASE + 0x22)


// *******************************************************************
// 事件


// *******************************************************************
// 状态返回


#endif // DSCP_INSTRUMENT_INFO_INTERFACE_H_

/** @} */
