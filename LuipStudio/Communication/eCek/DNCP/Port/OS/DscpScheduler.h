/**
 * @addtogroup module_DNCP
 * @{
 */

/**
 * @file
 * @brief DSCP 命令处理调度器。
 * @details
 * @version 1.0.0
 * @author kim.xiejinqiang
 * @date 2016-6-28
 */

#ifndef DNCP_PORT_OS_DSCPSCHEDULER_H_
#define DNCP_PORT_OS_DSCPSCHEDULER_H_

#ifdef __cplusplus
extern "C"
{
#endif

void DscpScheduler_Init(DscpDevice* dscp);
void DscpScheduler_Uninit();
void DscpScheduler_Active(void);

#ifdef __cplusplus
}
#endif

#endif /* DNCP_PORT_OS_DSCPSCHEDULER_H_ */

/** @} */
