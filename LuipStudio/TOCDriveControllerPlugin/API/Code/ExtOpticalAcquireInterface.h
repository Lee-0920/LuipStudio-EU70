/**
 * @page page_ExtOpticalAcquireInterface 扩展光学信号采集接口
 *  扩展光学信号采集接口提供了信号采集的相关操作。对原光学信号采集接口进行了扩展，
 *  支持多个测量LED灯的控制。
 *
 *  具体命令见： @ref module_ExtOpticalAcquireInterface
 *
 * @section sec_EOAI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2016.4.28)
 *
 */

/**
 * @addtogroup module_ExtOpticalAcquireInterface 扩展光学信号采集接口
 * @{
 */

/**
 * @file
 * @brief 扩展光学信号采集接口。
 * @details 定义了扩的展光学信号采集相关的操作。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2020.04.02
 */

#ifndef SRC_LUIPAPI_EXTOPTICALACQUIREINTERFACE_H_
#define SRC_LUIPAPI_EXTOPTICALACQUIREINTERFACE_H_

#include "OpticalAcquireInterface.h"

#define DSCP_EOAI_CBASE                  0x0000 + 0x0D00     ///< 命令基值
#define DSCP_EOAI_EBASE                  0x8000 + 0x0D00     ///< 事件基值
#define DSCP_EOAI_SBASE                  0x0000 + 0x0D00     ///< 状态基值


// *******************************************************************
// 命令和回应

/**
 * @brief 打开测量模块LED灯，控制LED的DAC电压为默认值。
 * @param index Uint8，测量模块LED索引。
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_TURN_ON_LED                    (DSCP_EOAI_CBASE + 0x01)

/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_EOAI_SIGNAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。而且通信失败时不上报。
 */
#define DSCP_CMD_EOAI_SET_SIGNAL_AD_NOTIFY_PERIOD    (DSCP_EOAI_CBASE + 0x02)

/**
 * @brief 启动采集过程。
 * @details 采集到的信号数据将以事件 @ref DSCP_EVENT_OAI_AD_ACQUIRED
 *  的形式上传给上位机。
 * @param acquireTime Float32，采样时间，单位为秒，0表示只采1个样本。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_START_ACQUIRER                 (DSCP_EOAI_CBASE + 0x03)

/**
 * @brief 停止采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_STOP_ACQUIRER                  (DSCP_EOAI_CBASE + 0x04)

/**
 * @brief 启动测量模块LED光强自适应控制器,并打开测量模块LED灯。
 * @param index Uint8，测量模块LED索引。
 * @details LED控制器启动后，系统将根据设定的LED控制器参数进行自动控制，尝试达到指定参考端AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_START_LEDCONTROLLER                  (DSCP_EOAI_CBASE + 0x05)

/**
 * @brief 停止测量模块LED光强自适应控制器和关闭测量模块LED。
 * @param index Uint8，测量模块LED索引。
 * @details 如果LED控制器没有打开则只关闭LED灯
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_STOP_LEDCONTROLLER                  (DSCP_EOAI_CBASE + 0x06)

/**
 * @brief 查询测量模块LED光强自适应控制器设定的目标值。
 * @param index Uint8，测量模块LED索引。
 * @return target Uint32 ，目标值。
 * @see DSCP_CMD_EOAI_SET_LEDCONTROLLER_TARGET
 */
#define DSCP_CMD_EOAI_GET_LEDCONTROLLER_TARGET             (DSCP_EOAI_CBASE + 0x07)

/**
 * @brief 设置测量模块LED光强自适应控制器设定的目标值。该参数永久保存在FLASH。
 * @param index Uint8，测量模块LED索引。
 * @param target Uint32 ，目标值。目标为参考端的AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_SET_LEDCONTROLLER_TARGET             (DSCP_EOAI_CBASE + 0x08)

/**
 * @brief 查询测量模块LED光强自适应控制器参数。
 * @param index Uint8，测量模块LED索引。
 * @return LED控制器参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_EOAI_SET_LEDCONTROLLER_PARAM
 */
#define DSCP_CMD_EOAI_GET_LEDCONTROLLER_PARAM              (DSCP_EOAI_CBASE + 0x09)

/**
 * @brief 设置测量模块LED光强自适应控制器参数。
 * @param index Uint8，测量模块LED索引。
 * @details LED控制器将根据设置的参数进行PID调节。该参数永久保存在FLASH。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_SET_LEDCONTROLLER_PARAM              (DSCP_EOAI_CBASE + 0x0A)

/**
 * @brief 打开测量模块LED灯,启动AD单次定向调节至目标值操作。
 * @param index Uint8，测量模块LED索引。
 * @details LED控制器启动后，系统将根据设定的LED控制器参数进行控制，尝试达到指定参考端AD值。
 * @param targetAD Uint32，目标AD。
 * @param tolerance Uint32，容差AD。
 * @param timeout Uint32，超时时间，单位ms。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_START_LEDADJUST                  (DSCP_EOAI_CBASE + 0x0B)

/**
 * @brief 停止测量模块LED单次定向调节和关闭LED。
 * @param index Uint8，测量模块LED索引。
 * @details 如果LED控制器没有打开则只关闭LED灯
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_STOP_LEDADJUST                    (DSCP_EOAI_CBASE + 0x0C)

/**
 * @brief 启动采集PD电位器调节静态AD至目标值操作。
 * @details 调整信号采集PD电位器值，调整采集到的AD。
 * @param index Uint8，目标PD索引号。
 * @param targetAD Uint32，目标AD。
 * @param useExtLED Uint8，是否针对附加的LED进行设置(只针对测量模块)
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_START_STATIC_AD_CONTROL                  (DSCP_EOAI_CBASE + 0x0D)

/**
 * @brief 停止采集PD电位器调节静态AD并关闭对应LED。
 * @details 停止静态AD调节过程
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_STOP_STATIC_AD_CONTROL                    (DSCP_EOAI_CBASE + 0x0E)

/**
 * @brief 查询采集PD电位器默认静态AD控制参数。
 * @param index Uint8，目标PD索引号。
 * @param useExtLED Uint8，是否针对附加的LED进行设置(只针对测量模块)
 * @return value Uint16 ，静态AD调节参数。
 * @see DSCP_CMD_EOAI_SET_STATIC_AD_CONTROL_PARAM
 */
#define DSCP_CMD_EOAI_GET_STATIC_AD_CONTROL_PARAM              (DSCP_EOAI_CBASE + 0x0F)

/**
 * @brief 设置采集PD电位器默认静态AD控制参数。
 * @details 静态AD控制器将根据设置的参数设置器件，并将参数永久保存在FLASH。
 * @details 静态AD调节状态中禁用。
 * @param index Uint8，目标PD索引号。
 * @param value Uint16 ，静态AD调节参数。
 * @param useExtLED Uint8，是否针对附加的LED进行设置(只针对测量模块)
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_SET_STATIC_AD_CONTROL_PARAM              (DSCP_EOAI_CBASE + 0x10)

/**
 * @brief 查询采集PD电位器静态AD控制功能是否有效
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  功能有效，操作成功；
 *  - @ref DSCP_ERROR 功能无效，操作失败；
 */
#define DSCP_CMD_EOAI_IS_STATIC_AD_CONTROL_VALID              (DSCP_EOAI_CBASE + 0x11)

/**
 * @brief 查询测量模块LED默认光强输出电压。
 * @param index Uint8，测量模块LED索引号。
 * @return Float32，电压V：
 */
#define DSCP_CMD_EOAI_GET_LED_DEFAULT_VALUE                                      (DSCP_EOAI_CBASE + 0x12)

/**
 * @brief 设置测量模块LED默认光强输出电压。
 * @param index Uint8，测量模块LED索引号。
 * @param value Float32，电压V。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EOAI_SET_LED_DEFAULT_VALUE                                      (DSCP_EOAI_CBASE + 0x13)
// *******************************************************************
// 事件  //事件号不变 内容在原格式上进行扩展

/**
 * @brief 采集结果。
 */
//typedef enum
//{
//    ACQUIRED_RESULT_FINISHED = 0,     ///< 采集正常完成。
//    ACQUIRED_RESULT_FAILED = 1,       ///< 采集中途出现故障，未能完成。
//    ACQUIRED_RESULT_STOPPED = 2,      ///< 采集被停止。
//}AcquiredResult;

/**
 * @brief 信号AD采集完成事件。
 * @details 启动采集后，采完数据时将产生该事件。
 * @param reference Uint32，参考端AD。
 * @param measure Uint32，测量端AD。
 * @see DSCP_CMD_OAI_START_ACQUIRER
 */
#define DSCP_EVENT_EOAI_AD_ACQUIRED                  DSCP_EVENT_OAI_AD_ACQUIRED

/**
 * @brief 信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_EOAI_SET_SIGNAL_AD_NOTIFY_PERIOD 设定。
 * @param reference Uint32，参考端AD。
 * @param measure Uint32，测量端AD。
 * @param result Uint8，采集结果码（ @ref AcquiredResult ），定义如下：
 *  - @ref ACQUIRED_RESULT_FINISHED  采集正常完成；
 *  - @ref ACQUIRED_RESULT_FAILED  采集中途出现故障，未能完成。
 *  - @ref ACQUIRED_RESULT_STOPPED  采集被停止。
 */
#define DSCP_EVENT_EOAI_SIGNAL_AD_NOTICE            DSCP_EVENT_OAI_SIGNAL_AD_NOTICE

 /**
  * @brief LED光强单次定向调节结果。
  */
// typedef enum
// {
//     ADJUST_RESULT_FINISHED = 0,     ///< LED调节正常完成。
//     ADJUST_RESULT_FAILED = 1,       ///<LED调节中途出现故障，未能完成。
//     ADJUST_RESULT_STOPPED = 2,      ///<LED调节被停止。
//     ADJUST_RESULT_TIMEOUT = 3           ///< LED调节超时，指定时间内仍未达到目标AD。
// }AdjustResult;

/**
 * @brief LED光强单次定向调节完成事件。
 * @details 启动LED光强单次定向调节使AD值到达指定范围后，产生该事件。
 * @see DSCP_CMD_EOAI_START_LEDADJUST
 */
#define DSCP_EVENT_EOAI_AD_ADJUST_RESULT               DSCP_EVENT_OAI_AD_ADJUST_RESULT

 /**
  * @brief 采集PD电位器静态AD调节控制结果。
  */
// typedef enum
// {
//     STATIC_AD_CONTROL_RESULT_UNFINISHED= 0,       ///<静态AD调节未完成。
//     STATIC_AD_CONTROL_RESULT_FINISHED = 1,     ///<  静态AD调节完成。
// }StaticADControlResult;

 /**
  * @brief 采集PD电位器静态AD调节结果事件。
  * @details 通过采集PD电位器调节使信号AD接近至目标AD最优取值时产生该事件。
  * @see DSCP_CMD_EOAI_START_STATIC_AD_CONTROL
  */
 #define DSCP_EVENT_EOAI_STATIC_AD_CONTROL_RESULT              DSCP_EVENT_OAI_STATIC_AD_CONTROL_RESULT

// *******************************************************************
// 状态返回


#endif /* SRC_LUIPAPI_EXTOPTICALACQUIREINTERFACE_H_ */

/** @} */
