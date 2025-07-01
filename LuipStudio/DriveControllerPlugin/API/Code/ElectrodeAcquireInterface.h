/**
 * @page page_ElectrodeAcquireInterface 电极信号采集接口
 *  电极信号采集接口提供了电极信号获取相关操作。
 *
 *  具体命令见： @ref module_ElectrodeAcquireInterface
 *
 * @section sec_EAI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2017.03.27)
 *  - 1.0.1 增加运放增益控制命令 (2017.03.29)
 *  - 1.0.2 增加运放增益获取及更改数据采集命令 (2017.03.30)
 *  - 1.0.3 增加数模转换器电位输出控制及电极扫描采集等命令 (2017.07.11)
 */

/**
 * @addtogroup module_ElectrodeAcquireInterface 电极信号采集接口
 * @{
 */

/**
 * @file
 * @brief 电极信号采集接口。
 * @details 定义了一序列电极信号采集相关的操作。
 * @version 1.0.3
 * @author yongxi
 * @date 2017.07.11
 */

#ifndef DSCP_ELECTRODE_ACQUIRE_INTERFACE_H_
#define DSCP_ELECTRODE_ACQUIRE_INTERFACE_H_

#define DSCP_EAI_CBASE                  0x0000 + 0x0E00     ///< 命令基值
#define DSCP_EAI_EBASE                  0x8000 + 0x0E00     ///< 事件基值
#define DSCP_EAI_SBASE                  0x0000 + 0x0E00     ///< 状态基值

// *******************************************************************
// 命令和回应

/**
 * @brief 设置AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出AD上报事件。
 * @param period Float32，AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_EAI_SIGNAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。而且通信失败时不上报。
 */
#define DSCP_CMD_EAI_SET_SIGNAL_AD_NOTIFY_PERIOD    (DSCP_EAI_CBASE + 0x00)

/**
 * @brief 启动电极扫描采集过程
 * @details 启动后，DAC根据设置的起始电位、终止电位和扫描速率，依次改变输出电位值，
 *  同时启动ADC采集电极AD值，将采集数据存入缓冲，扫描完成后上报完成事件。
 * @param startPotential Float32，扫描起始电位，单位：mV；
 * @param stopPotential Float32，扫描终止电位，单位：mV；
 * @param scanRate Float32，扫描速率，单位：mV/s；
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 该命令将立即返回，电极扫描完成情况将以事件的形式上报。
 */
#define DSCP_CMD_EAI_START_SCAN                     (DSCP_EAI_CBASE + 0x01)

/**
 * @brief 停止电极扫描采集过程。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EAI_STOP_ACQUIRER                  (DSCP_EAI_CBASE + 0x02)

/**
 * @brief 设置DAC输出电位
 * @param potential Float32，DAC输出电位，单位：mV，范围-2000~2000。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EAI_SET_POTENTIAL                  (DSCP_EAI_CBASE + 0x03)

/**
 * @brief 获取DAC输出电位
 * @return potential Float32，DAC输出电位，单位：mV，范围-2000~2000。
 */
#define DSCP_CMD_EAI_GET_POTENTIAL                  (DSCP_EAI_CBASE + 0x04)

/**
 * @brief 关闭DAC输出
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EAI_DAC_OFF                        (DSCP_EAI_CBASE + 0x05)

/**
 * @brief 设置ADC灵敏度等级
 * @param sensitivity Uint8，ADC采集灵敏度等级，范围0~2。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_EAI_SET_SENSITIVITY                (DSCP_EAI_CBASE + 0x06)

/**
 * @brief 获取ADC灵敏度等级
 * @return sensitivity Uint8，ADC采集灵敏度等级，范围0~2。
 */
#define DSCP_CMD_EAI_GET_SENSITIVITY                (DSCP_EAI_CBASE + 0x07)

/**
 * @brief 获取扫描分辨率
 * @return scanResolution Float32，扫描分辨率,单位：mV。
 */
#define DSCP_CMD_EAI_GET_SCAN_RESOLUTION            (DSCP_EAI_CBASE + 0x08)

/**
 * @brief 获取ADC采集的电极扫描数据
 * @param pos Uint16，起始位置索引
 * @param length Uint16，数据个数
 * @return current[length]，Float32，电流值数组，单位：uA
 */
#define DSCP_CMD_EAI_GET_SCAN_DATA                  (DSCP_EAI_CBASE + 0x09)

/**
 * @brief 查询电极扫描的工作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
#define DSCP_CMD_EAI_GET_SCAN_STATUS                (DSCP_EAI_CBASE + 0x0A)


// *******************************************************************
// 事件
/**
 * @brief 电极扫描结果。
 */
enum ElectrodeScanResult
{
    ELCTRODE_SCAN_RESULT_FINISHED = 0,       ///< 电极扫描操作正常完成。
    ELCTRODE_SCAN_RESULT_FAILED = 1,         ///< 电极扫描操作失败。
};

/**
 * @brief 信号AD采集完成事件。
 * @details 启动采集后，采集数据时将产生该事件。
 * @param number Uint32，采样时间内得到的数据个数。
 * @param value[number] Uint32，采样时间内得到的AD值。
 * @see DSCP_CMD_EAI_START_ACQUIRER
 */
#define DSCP_EVENT_EAI_AD_ACQUIRED                  (DSCP_EAI_EBASE + 0x00)

/**
 * @brief 信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_EAI_SET_SIGNAL_AD_NOTIFY_PERIOD 设定。
 * @param electrodeValue Uint32，电极信号值。
 */
#define DSCP_EVENT_EAI_SIGNAL_AD_NOTICE             (DSCP_EAI_EBASE + 0x01)

/**
 * @brief 电极扫描完成事件。
 * @details 电极扫描完成或终止会产生该事件。
 * @param result Uint8，电极扫描操作结果码（ @ref ElectrodeScanResult ），定义如下：
 *  - @ref ELCTRODE_SCAN_RESULT_FINISHED  电极扫描操作正常完成；
 *  - @ref ELCTRODE_SCAN_RESULT_FAILED  电极扫描操作失败。
 */
#define DSCP_EVENT_EAI_SCAN_RESULT                  (DSCP_EAI_EBASE + 0x02)

// *******************************************************************
// 状态返回


#endif // DSCP_ELECTRODE_ACQUIRE_INTERFACE_H_

/** @} */
