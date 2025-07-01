/**
 * @page page_TemperatureControlInterface 温度控制接口
 *  温度控制接口提供了控制温度的相关操作。
 *
 *  具体命令见： @ref module_TemperatureControlInterface
 *
 * @section sec_TCI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2016.4.26)
 *
 */

/**
 * @addtogroup module_TemperatureControlInterface 温度控制接口
 * @{
 */

/**
 * @file
 * @brief 温度控制接口。
 * @details 定义了一序列温度控制相关的操作。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2016.4.26
 */

#ifndef DSCP_TEMPERATURE_CONTROL_INTERFACE_H_
#define DSCP_TEMPERATURE_CONTROL_INTERFACE_H_

#define DSCP_TCI_CBASE                  0x0000 + 0x0A00     ///< 命令基值
#define DSCP_TCI_EBASE                  0x8000 + 0x0A00     ///< 事件基值
#define DSCP_TCI_SBASE                  0x0000 + 0x0A00     ///< 状态基值


// *******************************************************************
// 命令和回应

/**
 * @brief 查询温度传感器的校准系数。
 * @return 负输入分压，Float32。
 * @return 参考电压 ，Float32。
 * @return 校准电压 Float32。
 * @see DSCP_CMD_TCI_SET_CALIBRATE_FACTOR
 */
#define DSCP_CMD_TCI_GET_CALIBRATE_FACTOR           (DSCP_TCI_CBASE + 0x00)

/**
 * @brief 设置温度传感器的校准系数。
 * @details 因为个体温度传感器有差异，出厂或使用时需要校准。该参数将永久保存。
 *   @param negativeInput Float32，负输入分压 。
 *   @param vref Float32，参考电压。
 *   @param vcal Float32，校准电压。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_TCI_SET_CALIBRATE_FACTOR           (DSCP_TCI_CBASE + 0x01)

/**
 * @brief 查询当前温度。
 * @return 当前温度，包括控制室温度和环境温度，格式如下：
 *     - thermostatTemp Float32，恒温室温度，单位为摄氏度。
 *     - environmentTemp Float32，环境温度，单位为摄氏度。
 */
#define DSCP_CMD_TCI_GET_TEMPERATURE                (DSCP_TCI_CBASE + 0x02)

/**
 * @brief 查询恒温控制参数。
 * @return 恒温控制参数，格式如下：
 *  - proportion Float32，PID的比例系数。
 *  - integration Float32，PID的积分系数。
 *  - differential Float32，PID的微分系数。
 * @see DSCP_CMD_TCI_SET_THERMOSTAT_PARAM
 */
#define DSCP_CMD_TCI_GET_THERMOSTAT_PARAM           (DSCP_TCI_CBASE + 0x03)

/**
 * @brief 设置恒温控制参数。
 * @details 恒温系统将根据设置的参数进行温度调节。该参数将永久保存。
 * @param proportion Float32，PID的比例系数。
 * @param integration Float32，PID的积分系数。
 * @param differential Float32，PID的微分系数。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_TCI_SET_THERMOSTAT_PARAM           (DSCP_TCI_CBASE + 0x04)

/**
 * @brief 查询恒温器的工作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
#define DSCP_CMD_TCI_GET_THERMOSTAT_STATUS          (DSCP_TCI_CBASE + 0x05)

/**
 * @brief 恒温模式。
 */
typedef enum
{
    THERMOSTAT_MODE_AUTO = 0,           ///< 自动模式，根据需要及硬件能力综合使用加热器和制冷器。
    THERMOSTAT_MODE_HEATER = 1,         ///< 纯加热模式，不使用制冷器。
    THERMOSTAT_MODE_REFRIGERATE = 2,    ///< 纯制冷模式，不使用加热器。
    THERMOSTAT_MODE_NATURAL = 3         ///< 自然模式，加热器和制冷器都不参与，靠环境传递热量，自然升温或冷却。
}ThermostatMode;

/**
 * @brief 开始恒温。
 * @details 恒温开始后，系统将根据设定的恒温控制参数进行自动温度控制，尝试达到指定温度。
 *  不管成功与否，操作结果都将以事件的形式上传给上位机，但恒温器将继续工作，
 *  直到接收到DSCP_CMD_TCI_STOP_THERMOSTAT才停止。关联的事件有：
 *   - @ref DSCP_EVENT_TCI_THERMOSTAT_RESULT
 * @param mode Uint8，恒温模式，支持的模式见： @ref ThermostatMode 。
 * @param targetTemp Float32，恒温目标温度。
 * @param toleranceTemp Float32，容差温度，与目标温度的差值在该参数范围内即认为到达目标温度。
 * @param timeout Float32，超时时间，单位为秒。超时仍未达到目标温度，也将返回结果事件。
 * @return 状态回应，Uint16，支持的状态有：
 *   - @ref DSCP_OK  操作成功；
 *   - @ref DSCP_BUSY 操作失败，如恒温已经工作，需要先停止；
 *   - @ref DSCP_ERROR_PARAM 恒温参数错误；
 *   - @ref DSCP_ERROR 温度传感器异常；
 * @note 该命令将立即返回，恒温完成将以事件的形式上报。
 */
#define DSCP_CMD_TCI_START_THERMOSTAT               (DSCP_TCI_CBASE + 0x06)

/**
 * @brief 停止恒温控制。
 * @details 停止后，加热器和冷却器将不工作。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 该命令将立即返回。
 */
#define DSCP_CMD_TCI_STOP_THERMOSTAT                (DSCP_TCI_CBASE + 0x07)

/**
 * @brief 设置温度上报周期。
 * @details 系统将根据设定的周期，定时向上发出温度上报事件。
 * @param period Float32，温度上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_TCI_TEMPERATURE_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。
 */
#define DSCP_CMD_TCI_SET_TEMPERATURE_NOTIFY_PERIOD  (DSCP_TCI_CBASE + 0x08)

/**
 * @brief 设置机箱风扇。
 * @details 根据设定的占空比，调节风扇速率
 * @param level float ,风扇速率，对应高电平占空比。默认为0，机箱风扇关闭。
 * @see
 * @note
 */
#define DSCP_CMD_TCI_TURN_BOXFAN                    (DSCP_TCI_CBASE + 0x09)

/**
 * @brief 设置恒温器风扇。
 * @details 根据设定的占空比，调节风扇速率
 * @param level float ,风扇速率，对应高电平占空比。默认为0，风扇关闭。
 * @see
 * @note
 */
#define DSCP_CMD_TCI_TURN_FAN                       (DSCP_TCI_CBASE + 0x0A)

// *******************************************************************
// 事件

/**
 * @brief 恒温操作结果。
 */
typedef enum
{
    THERMOSTAT_RESULT_REACHED = 0,          ///< 恒温目标达成，目标温度在规定时间内达成，后续将继续保持恒温，直到用户停止。
    THERMOSTAT_RESULT_FAILED = 1,           ///< 温度传感器硬件异常。
    THERMOSTAT_RESULT_STOPPED = 2,          ///< 恒温被停止。
    THERMOSTAT_RESULT_TIMEOUT = 3           ///< 恒温超时，指定时间内仍未达到目标温度。
}ThermostatResult;

/**
 * @brief 恒温结果事件。
 * @details 恒温操作结束时将产生该事件。
 * @param result Uint8，恒温结果码（ @ref ThermostatResult ）。
 * @param temp Float32，当前温度
 */
#define DSCP_EVENT_TCI_THERMOSTAT_RESULT                (DSCP_TCI_EBASE + 0x00)

/**
 * @brief 温度定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报温度。
 *  上报周期可通过命令 @ref DSCP_CMD_TCI_SET_TEMPERATURE_NOTIFY_PERIOD 设定。
 * @param thermostatTemp Float32，恒温室温度，单位为摄氏度。
 * @param environmentTemp Float32，环境温度，单位为摄氏度。
 */
#define DSCP_EVENT_TCI_TEMPERATURE_NOTICE               (DSCP_TCI_EBASE + 0x01)


// *******************************************************************
// 状态返回



#endif // DSCP_TEMPERATURE_CONTROL_INTERFACE_H_

/** @} */
