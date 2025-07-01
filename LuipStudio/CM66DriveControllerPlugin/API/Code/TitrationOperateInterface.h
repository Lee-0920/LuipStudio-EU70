/**
 * @page page_TitrationOperateInterface 滴定控制接口
 *  滴定控制接口提供了控制滴定相关操作。
 *
 *  具体命令见： @ref module_TitrationOperateInterface
 *
 * @section sec_TOI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2017.3.27)
 */

/**
 * @addtogroup module_TitrationOperateInterface 滴定控制接口
 * @{
 */

/**
 * @file
 * @brief 滴定控制接口。
 * @details 定义了一序列滴定控制相关的操作。
 * @version 1.0.0
 * @author weizhe
 * @date 2017.03.21
 */

#ifndef DSCP_TITRATION_OPERATE_INTERFACE_H_
#define DSCP_TITRATION_OPERATE_INTERFACE_H_

#define DSCP_TOI_CBASE                  0x0000 + 0x0C00     ///< 命令基值
#define DSCP_TOI_EBASE                  0x8000 + 0x0C00     ///< 事件基值
#define DSCP_TOI_SBASE                  0x0000 + 0x0C00     ///< 状态基值

// *******************************************************************
// 命令和回应

/**
 * @brief 启动泵进行滴定。
 * @details 滴定过程中电极电位值及已滴定步数将以事件形式上报至主控器 @ref DSCP_EVENT_TOI_TITRATE_AD_ACQUIRED
 * @param speed Float32，滴定速度，单位为 ml/秒。
 * @param samplePeriod Float32，滴定过程中信号值及已滴定步数的采样周期，单位：秒。
 * @param notifyPeriod Float32，滴定过程中数据上报周期，单位：秒。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败，如泵正在工作，无法启动泵，需要先停止；
 * @note 该命令将立即返回，需要调用DSCP_CMD_TOI_STOP_TITRATION终止滴定过程。
 */
#define DSCP_CMD_TOI_START_TITRATION                                (DSCP_TOI_CBASE + 0x00)

/**
 * @brief 停止滴定。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_TOI_STOP_TITRATION                                 (DSCP_TOI_CBASE + 0x01)

/**
 * @brief 查询滴定操作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
#define DSCP_CMD_TOI_GET_TITRATION_STATUS                           (DSCP_TOI_CBASE + 0x02)

/**
 * @brief 设置信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出信号AD上报事件。
 * @param period Float32，信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_TOI_ELECTRODE_SIGNAL_NOTICE。
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。而且通信失败时不上报。
 */
#define DSCP_CMD_TOI_SET_ELECTRODE_SIGNAL_NOTIFY_PERIOD             (DSCP_TOI_CBASE + 0x03)

/**
 * @brief 获取当前电极信号值。
 * @param value Uint32，电极信号AD值
 */
#define DSCP_CMD_TOI_GET_ELECTRODE_SIGNAL                           (DSCP_TOI_CBASE + 0x04)

/**
 * @brief 设置滴定速度
 * @details 该接口主要用于滴定过程中在电极信号值达到设置的切换阈值时，调用该接口改变滴定速度
 * @param titrationSpeed Float32，滴定速度，单位ml/s
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败
 */
#define DSCP_CMD_TOI_SET_TITRATION_SPEED                            (DSCP_TOI_CBASE + 0x05)

 /**
  * @brief 滴定电极输出
  * @param isOpen Bool 滴定电极输出。TRUE 开启，FALSE 关闭
  * @return 状态回应，Uint16，支持的状态有：
  *  - @ref DSCP_OK  操作成功；
  *  - @ref DSCP_ERROR 操作失败；
  */
#define DSCP_CMD_TOI_ELECTRODE_OUTPUT                               (DSCP_TOI_CBASE + 0x06)
// *******************************************************************
// 事件

/**
 * @brief 启动滴定后，采样到的当前信号值及滴定步数。
 * @details 启动滴定后，采集数据时将产生该事件。
 * @param number Uint32，采样到的电极AD值及滴定步数对数目。
 * @param {Uint32 ad1, Uint32 step1}、{Uint32 ad2, Uint32 step2}....。
 * @see DSCP_CMD_EAI_START_ACQUIRER。
 */
#define DSCP_EVENT_TOI_TITRATE_AD_ACQUIRED                          (DSCP_TOI_EBASE + 0x00)

/**
* @brief 电极信号定时上报事件。
* @details 系统将根据设置的上报周期，定时向上汇报电极信号AD值。
*  上报周期可通过命令 @ref DSCP_CMD_TOI_SET_ELECTRODE_SIGNAL_NOTIFY_PERIOD 设定。
* @param electrodeValue Uint32，电极信号值。
*/
#define DSCP_EVENT_TOI_ELECTRODE_SIGNAL_NOTICE                     (DSCP_TOI_EBASE + 0x01)

#endif // DSCP_TITRATION_OPERATE_INTERFACE_H_

/** @} */
