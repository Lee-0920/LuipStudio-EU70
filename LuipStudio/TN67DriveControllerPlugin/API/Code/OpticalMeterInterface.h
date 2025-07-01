/**
 * @page page_OpticalMeterInterface 光学定量接口
 *  光学定量接口提供了控制蠕动泵运转的相关操作。
 *
 *  具体命令见： @ref module_OpticalMeterInterface
 *
 * @section sec_OMI_ChangeLog 版本变更历史
 *  接口历史变更记录：
 *  - 1.0.0 基本版本 (2016.4.26)
 *
 */

/**
 * @addtogroup module_OpticalMeterInterface 光学定量接口
 * @{
 */

/**
 * @file
 * @brief 光学定量接口。
 * @details 定义了一序列光学定量相关的操作。
 * @version 1.0.0
 * @author xiejinqiang
 * @date 2016.4.26
 */

#ifndef DSCP_OPTICAL_METER_INTERFACE_H_
#define DSCP_OPTICAL_METER_INTERFACE_H_

#define DSCP_OMI_CBASE                  0x0000 + 0x0900     ///< 命令基值
#define DSCP_OMI_EBASE                  0x8000 + 0x0900     ///< 事件基值
#define DSCP_OMI_SBASE                  0x0000 + 0x0900     ///< 状态基值


// *******************************************************************
// 命令和回应

/**
 * @brief 自动校准定量泵，并永久保存计算出的校准系数。
 * @details 将根据设定的定量点，计算出两点间的体积，并测量泵的转动角度。
 * @param map Uint32，自动校准所用到的液体，需要控制的阀的映射图。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @see DSCP_CMD_OMI_SET_METER_POINTS
 * @see DSCP_EVENT_OMI_AUTO_CALIBRATE_RESULT
 */
#define DSCP_CMD_OMI_AUTO_CALIBRATE_PUMP            (DSCP_OMI_CBASE + 0x00)

/**
 * @brief 查询定量泵的校准系数。
 * @return 校准系数， Float32，每步泵出的体积，单位为 ml/步。
 * @see DSCP_CMD_OMI_SET_PUMP_FACTOR
 */
#define DSCP_CMD_OMI_GET_PUMP_FACTOR                (DSCP_OMI_CBASE + 0x01)

/**
 * @brief 设置定量泵的校准系数。
 * @details 因为蠕动泵的生产工艺及工作时长，每个泵转一步对应体积都不同，出厂或使用时需要校准。
 *  该参数将永久保存。
 * @param factor Float32，要设置的校准系数， 每步泵出的体积，单位为 ml/步。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 *  - @ref DSCP_ERROR_PARAM 参数错误，传入的参数有问题
 * @see DSCP_CMD_OMI_GET_PUMP_FACTOR
 */
#define DSCP_CMD_OMI_SET_PUMP_FACTOR                (DSCP_OMI_CBASE + 0x02)

/**
 * @brief 查询定量点体积。
 * @return 定量点参数， 数据格式为：
 *     - num Uint8，定量点数目。决定了后面的变长参数长度。
 *     - setVolume1 Float32，定量点1设定体积，单位为 ml。
 *     - realVolume1 Float32，定量点1实际体积，单位为 ml。
 *     - setVolume2 Float32，定量点2设定体积，单位为 ml。
 *     - realVolume2 Float32，定量点2实际体积，单位为 ml。
 *     - setVolume3 Float32，定量点3设定体积，单位为 ml。
 *     - realVolume3 Float32，定量点3实际体积，单位为 ml。
 *     - ...
 * @see DSCP_CMD_OMI_SET_METER_POINTS
 */
#define DSCP_CMD_OMI_GET_METER_POINTS               (DSCP_OMI_CBASE + 0x03)

/**
 * @brief 设置定量点体积。
 * @details 系统将根据设定的定量点体积，进行最佳的定量分配。实际体积将被用于定量校准。
 *   定量点参数将永久保存。
 * @param num Uint8，定量点数目。决定了后面变长参数的个数。
 * @param setVolume1 Float32，定量点1设定体积，单位为 ml。
 * @param realVolume1 Float32，定量点1实际体积，单位为 ml。
 * @param setVolume2 Float32，定量点2设定体积，单位为 ml。
 * @param realVolume2 Float32，定量点2实际体积，单位为 ml。
 * @param setVolume3 Float32，定量点3设定体积，单位为 ml。
 * @param realVolume3 Float32，定量点3实际体积，单位为 ml。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 *  - @ref DSCP_ERROR_PARAM 参数错误，传入的参数有问题
 * @see DSCP_CMD_OMI_GET_METER_POINTS
 */
#define DSCP_CMD_OMI_SET_METER_POINTS               (DSCP_OMI_CBASE + 0x04)

/**
 * @brief 查询定量的工作状态。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_IDLE 空闲；
 *  - @ref DSCP_BUSY 忙碌，需要停止后才能做下一个动作；
 */
#define DSCP_CMD_OMI_GET_METER_STATUS               (DSCP_OMI_CBASE + 0x05)

/**
 * @brief 定量模式。
 */
enum MeterMode
{
    METER_MODE_ACCURATE = 0,            ///< 精准定量模式，精准定量到指定体积的定量点。
    METER_MODE_FAST = 1,                ///< 快速定量模式，快速定量到指定体积的定量点。
    METER_MODE_SMART = 2,               ///< 智能定量模式，结合定量点和泵计步综合定量出任意体积。
    METER_MODE_SLOW = 3                 ///< 慢速定量模式，过程与快速一样，速度用很慢的速度。
};

/**
 * @brief 开始定量。
 * @details 定量开始后，不管成功与否，操作结果都将以事件的形式上传给上位机。关联的事件有：
 *   - @ref DSCP_EVENT_OMI_METER_RESULT
 * @param dir Uint8，定量方向，0为正向转动（抽取），1为反向转动（排空）。
 * @param mode Uint8，定量模式，支持的模式见： @ref MeterMode 。
 * @param volume Float32，定量体积，定量体积必须大于等于最小定量点体积，除智能模式，其他模式的定量体积等于
 * 定量点体积中一个。单位为 ml。
 * @param limitVolume Float32，限量体积，在定量过程中总的泵取体积不能超过该值，0表示不进行限量判定。
 * 此体积必须大于定量体积。单位为 ml。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败，如定量正在工作，需要先停止；
 *  - @ref DSCP_ERROR_PARAM 参数错误，传入的参数有问题
 * @note 该命令将立即返回，定量完成将以事件的形式上报。
 */
#define DSCP_CMD_OMI_START_METER                    (DSCP_OMI_CBASE + 0x06)

/**
 * @brief 停止定量。
 * @details 当前定量动作将被尽快停止，已经定量的溶液不会自动排到废液池，需要显式调用。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 * @note 该命令将立即返回。
 */
#define DSCP_CMD_OMI_STOP_METER                     (DSCP_OMI_CBASE + 0x07)

/**
 * @brief 设置定量点光信号AD上报周期。
 * @details 系统将根据设定的周期，定时向上发出定量点光信号AD上报事件。
 * @param period Float32，定量点光信号AD上报周期，单位为秒。0表示不需要上报，默认为0。
 * @see DSCP_EVENT_OMI_OPTICAL_AD_NOTICE
 * @note 所设置的上报周期将在下一次启动时丢失，默认为0，不上报。通信失败时也不会上报事件。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_SET_OPTICAL_AD_NOTIFY_PERIOD  (DSCP_OMI_CBASE + 0x08)


/**
 * @brief 打开定量点NUM的LED。
 * @param num Uint8，需要控制的定量点 。NUM范围 1 - POINTS_MAX
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_TURN_ON_LED                    (DSCP_OMI_CBASE + 0x09)

/**
 * @brief 关闭定量点NUM的LED。
 * @param num Uint8，需要控制的定量点 。NUM范围 1 - POINTS_MAX
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_TURN_OFF_LED                   (DSCP_OMI_CBASE + 0x0A)

 /**
  * @brief 定量结束自动关闭阀开关
  * @param isautoCloseValve Bool 是否自动关闭。TRUE 自动关闭，FALSE 定量结束不关闭阀
  * @return 状态回应，Uint16，支持的状态有：
  *  - @ref DSCP_OK  操作成功；
  *  - @ref DSCP_ERROR 操作失败；
  * @note 为防止定量完成时，液体往下降，所有每次定量结束都将自动关闭所有的阀。可用此命令
  * 开关 此功能。
  */
#define DSCP_CMD_OMI_AUTO_CLOSE_VALVE               (DSCP_OMI_CBASE + 0x0B)

/**
 * @brief 设置定量速度
 * @param speed float 定量的速度
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_SET_METER_SPEED                (DSCP_OMI_CBASE + 0x0C)
/**
 * @brief 获取定量速度
 * @return speed float 定量的速度
 */
#define DSCP_CMD_OMI_GET_METER_SPEED                (DSCP_OMI_CBASE + 0x0D)

 /**
  * @brief 设置定量结束时阀要改变成的状态
  * @param map Uint32，阀通道映射图，每位表示一个通道的开关状态，1为打开，0为闭合，低位开始。
  * @return 状态回应，Uint16，支持的状态有：
  *  - @ref DSCP_OK  操作成功；
  *  - @ref DSCP_ERROR 操作失败；
  */
 #define DSCP_CMD_OMI_SET_METER_FINISH_VALVE_MAP     (DSCP_OMI_CBASE + 0x0E)

 /**
  * @brief 获取获取某个定量点光信号AD
  * @param num Uint8，需要获取的定量点。NUM范围 1 - POINTS_MAX
  * @return adValue Uint32 定量点光信号AD
  */
 #define DSCP_CMD_OMI_GET_SINGLE_OPTICAL_AD          (DSCP_OMI_CBASE + 0x0F)

/**
 * @brief 设置粘稠模式定量过冲量
 * @param value Uint16 定量过冲量
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_SET_ROPINESS_METER_OVER_VALUE     (DSCP_OMI_CBASE + 0x10)

/**
 * @brief 获取粘稠模式定量过冲量
 * @return value Uint16 定量过冲量
 */
#define DSCP_CMD_OMI_GET_ROPINESS_METER_OVER_VALUE     (DSCP_OMI_CBASE + 0x11)

/**
 * @brief 设置定量LED光强等级
 * @param index Uint8 定量LED索引, 1-2
 * @param value Uint16 定量LED光强等级, 0低/1中/2高/3很高
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_SET_METER_LED_LEVEL                (DSCP_OMI_CBASE + 0x12)

/**
 * @brief 获取定量LED光强等级
 * @return value Uint16 定量LED光强等级
 */
#define DSCP_CMD_OMI_GET_METER_LED_LEVEL                   (DSCP_OMI_CBASE + 0x13)

/**
 * @brief 设置精确定量模式下需要达到判定终点的次数
 * @param cnt Uint16 精确定量模式下需要达到判定终点的次数
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
 */
#define DSCP_CMD_OMI_SET_ACCURATE_MODE_STANDARD_CNT                   (DSCP_OMI_CBASE + 0x14)

/**
 * @brief 获取精确定量模式下需要达到判定终点的次数
 * @return cnt Uint16 精确定量模式下需要达到判定终点的次数
 */
#define DSCP_CMD_OMI_GET_ACCURATE_MODE_STANDARD_CNT                   (DSCP_OMI_CBASE + 0x15)


// *******************************************************************
// 事件

/**
 * @brief 定量操作结果。
 */
enum MeterResult
{
    METER_RESULT_FINISHED = 0,          ///< 定量正常完成。
    METER_RESULT_FAILED = 1,            ///< 定量中途出现故障，未能完成。
    METER_RESULT_STOPPED = 2,           ///< 定量被停止。
    METER_RESULT_OVERFLOW = 3,          ///< 定量溢出。
    METER_RESULT_UNFINISHED = 4,        ///< 定量目标未达成。
    METER_RESULT_AIRBUBBLE = 5          ///< 定量遇到气泡。
};

/**
 * @brief 定量结果事件。
 * @details 定量操作结束时将产生该事件。
 * @param result Uint8，定量结果码（ @ref MeterResult ），定义如下：
 *  - @ref METER_RESULT_FINISHED  定量正常完成；
 *  - @ref METER_RESULT_FAILED  定量中途出现故障，未能完成。
 *  - @ref METER_RESULT_STOPPED  定量被停止。
 *  - @ref METER_RESULT_OVERFLOW  定量溢出。
 *  - @ref METER_RESULT_UNFINISHED  定量目标未达成。
 *  - @ref METER_RESULT_AIRBUBBLE   定量遇到气泡
 */
#define DSCP_EVENT_OMI_METER_RESULT                 (DSCP_OMI_EBASE + 0x00)

/**
 * @brief 自动校准结果事件。
 * @details 自动校准操作结束时将产生该事件。
 * @param result Uint8，定量结果码（ @ref MeterResult ），定义如下：
 *  - @ref METER_RESULT_FINISHED  自动校准正常完成；
 *  - @ref METER_RESULT_FAILED  自动校准中途出现故障，未能完成。
 *  - @ref METER_RESULT_STOPPED  自动校准被停止。
 */
#define DSCP_EVENT_OMI_AUTO_CALIBRATE_RESULT        (DSCP_OMI_EBASE + 0x01)

/**
 * @brief 定量点光信号AD定时上报事件。
 * @details 系统将根据设置的上报周期，定时向上汇报定量点光信号AD。
 *  上报周期可通过命令 @ref DSCP_CMD_OMI_SET_OPTICAL_AD_NOTIFY_PERIOD 设定。
 * @return 定量点AD信息， 数据格式为：
 *     - num Uint8，定量点数目。决定了后面的变长参数长度。
 *     - adValue[num] Uint32[num]，定量点AD值数组，AD值为裸值，无单位。
 */
#define DSCP_EVENT_OMI_OPTICAL_AD_NOTICE               (DSCP_OMI_EBASE + 0x02)



// *******************************************************************
// 状态返回



#endif // DSCP_OPTICAL_METER_INTERFACE_H_

/** @} */
