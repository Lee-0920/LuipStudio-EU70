/**
 * @page page_SolenoidValveInterface 电磁阀控制接口
 *  电磁阀控制接口提供了控制电磁阀开闭的相关操作。
 *
 *  具体命令见： @ref module_SolenoidValveInterface
 *
 * @section sec_IOI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2016.4.26)
 *
 */

/**
 * @addtogroup module_SolenoidValveInterface 电磁阀控制接口
 * @{
 */

/**
 * @file
 * @brief 电磁阀控制接口。
 * @details 定义了一序列电磁阀控制相关的操作。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2016.4.26
 */

#ifndef DSCP_IO_CONTROL_INTERFACE_H_
#define DSCP_IO_CONTROL_INTERFACE_H_

#define DSCP_IOI_CBASE                  0x0000 + 0x0D00     ///< 命令基值
#define DSCP_IOI_EBASE                  0x8000 + 0x0D00     ///< 事件基值
#define DSCP_IOI_SBASE                  0x0000 + 0x0D00     ///< 状态基值


// *******************************************************************
// 命令和回应

/**
 * @brief 设置输出电流。
 * @param Uint8 index:4-20通道（0-2三个通道）
 *        float current:输出电流值
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_IOI_SET_CURRENT               (DSCP_IOI_CBASE + 0x00)

/**
 * @brief 查询当前输出电流
 * @param index:4-20通道（0-2三个通道）
 * @return 设备输出电流值，float
 */
#define DSCP_CMD_IOI_GET_CURRENT                  (DSCP_IOI_CBASE + 0x01)

/**
 * @brief 打开继电器。
 * @param index:打开的继电器通道（0-3）
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_IOI_RELAY_TURN_ON                  (DSCP_IOI_CBASE + 0x02)

/**
 * @brief 关闭继电器。
 * @param index:关闭的继电器通道（0-3）
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_IOI_RELAY_TURN_OFF                  (DSCP_IOI_CBASE + 0x03)

// *******************************************************************
// 事件
/**
 * @brief 干接点触发上报事件。
 * @details 上报周期可通过命令 @ref DSCP_CMD_OAI_SET_REPORT_NOTIFY_PERIOD 设定。
 * @see
 */
#define DSCP_EVENT_IOI_TRIGGER                 	    (DSCP_IOI_EBASE + 0x00)

// *******************************************************************
// 状态返回




#endif // DSCP_SOLENOID_VALVE_INTERFACE_H_

/** @} */
