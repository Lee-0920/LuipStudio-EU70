/**
 * @page page_OpticalAcquireInterface 光学信号采集接口
 *  光学信号采集接口提供了控制温度的相关操作。
 *
 *  具体命令见： @ref module_OpticalAcquireInterface
 *
 * @section sec_OAI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2016.4.28)
 *
 */

/**
 * @addtogroup module_OpticalAcquireInterface 光学信号采集接口
 * @{
 */

/**
 * @file
 * @brief 光学信号采集接口。
 * @details 定义了一序列光学信号采集相关的操作。
 * @version 1.1.0
 * @author xiejinqiang
 * @date 2016.09.02
 */

#ifndef DSCP_OPTICAL_ACQUIRE_INTERFACE_H_
#define DSCP_OPTICAL_ACQUIRE_INTERFACE_H_

#define DSCP_OAI_CBASE                  0x0000 + 0x0C00     ///< 命令基值
#define DSCP_OAI_EBASE                  0x8000 + 0x0C00     ///< 事件基值
#define DSCP_OAI_SBASE                  0x0000 + 0x0C00     ///< 状态基值


// *******************************************************************
// 命令和回应

/**
 * @brief 打开LED灯，控制LED的DAC电压为默认值。
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_TURN_ON_LED                    (DSCP_OAI_CBASE + 0x01)

/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_OAI_SIGNAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。
 */
#define DSCP_CMD_OAI_SET_SIGNAL_AD_NOTIFY_PERIOD    (DSCP_OAI_CBASE + 0x02)

/**
 * @brief 启动采集过程。
 * @details 采集到的信号数据将以事件 @ref DSCP_EVENT_OAI_AD_ACQUIRED
 *  的形式上传给上位机。
 * @param acquireTime Float32，采样时间，单位为秒，0表示只采1个样本。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_START_ACQUIRER                 (DSCP_OAI_CBASE + 0x03)

/**
 * @brief 停止采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_STOP_ACQUIRER                  (DSCP_OAI_CBASE + 0x04)

/**
 * @brief 启动LED控制器,并打开LED灯。
 * @details LED控制器启动后，系统将根据设定的LED控制器参数进行自动控制，尝试达到指定参考端AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_START_LEDCONTROLLER                  (DSCP_OAI_CBASE + 0x05)

/**
 * @brief 停止LED控制器和关闭LED。
 * @details 如果LED控制器没有打开则只关闭LED灯
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_STOP_LEDCONTROLLER                  (DSCP_OAI_CBASE + 0x06)

/**
 * @brief 查询LED控制器设定的目标值。
 * @return target Uint32 ，目标值。
 * @see DSCP_CMD_OAI_SET_LEDCONTROLLER_TARGET
 */
#define DSCP_CMD_OAI_GET_LEDCONTROLLER_TARGET             (DSCP_OAI_CBASE + 0x07)

/**
 * @brief 设置LED控制器设定的目标值。该参数永久保存在FLASH。
 * @param target Uint32 ，目标值。目标为参考端的AD值。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_SET_LEDCONTROLLER_TARGET             (DSCP_OAI_CBASE + 0x08)

/**
 * @brief 查询LED控制器参数。
 * @return LED控制器参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_OAI_SET_LEDCONTROLLER_PARAM
 */
#define DSCP_CMD_OAI_GET_LEDCONTROLLER_PARAM              (DSCP_OAI_CBASE + 0x09)

/**
 * @brief 设置LED控制器参数。
 * @details LED控制器将根据设置的参数进行PID调节。该参数永久保存在FLASH。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OAI_SET_LEDCONTROLLER_PARAM              (DSCP_OAI_CBASE + 0x0A)


// *******************************************************************
// 事件

/**
 * @brief 采集结果。
 */
enum AcquiredResult
{
    ACQUIRED_RESULT_FINISHED = 0,     ///< 采集正常完成。
    ACQUIRED_RESULT_FAILED = 1,       ///< 采集中途出现故障，未能完成。
    ACQUIRED_RESULT_STOPPED = 2,      ///< 采集被停止。
};

/**
 * @brief 信号AD采集完成事件。
 * @details 启动采集后，采完数据时将产生该事件。
 * @param reference Uint32，参考端AD。
 * @param measure Uint32，测量端AD。
 * @param result Uint8，采集结果码（ @ref AcquiredResult ），定义如下：
 *  - @ref ACQUIRED_RESULT_FINISHED  采集正常完成；
 *  - @ref ACQUIRED_RESULT_FAILED  采集中途出现故障，未能完成。
 *  - @ref ACQUIRED_RESULT_STOPPED  采集被停止。
 * @see DSCP_CMD_OAI_START_ACQUIRER
 */
#define DSCP_EVENT_OAI_AD_ACQUIRED                  (DSCP_OAI_EBASE + 0x00)

/**
 * @brief 信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_TCI_SET_TEMPERATURE_NOTIFY_PERIOD 设定。
 * @param reference Uint32，参考端AD。
 * @param measure Uint32，测量端AD。
 */
#define DSCP_EVENT_OAI_SIGNAL_AD_NOTICE               (DSCP_OAI_EBASE + 0x01)

// *******************************************************************
// 状态返回



#endif // DSCP_OPTICAL_ACQUIRE_INTERFACE_H_

/** @} */
