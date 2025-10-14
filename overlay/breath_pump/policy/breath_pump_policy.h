#ifndef __BREATH_PUMP_POLICY_H__
#define __BREATH_PUMP_POLICY_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "motor_policy.h"

/// 策略集合
motor_total_policy_t *pump_total_policy_init(motor_cfg_t cfg);
int pump_total_policy_deint(motor_total_policy_t *total_policy);

/// 定时策略
motor_time_policy_t *pump_time_policy_init(motor_cfg_t cfg);
int pump_time_policy_add(motor_total_policy_t *total_policy, motor_time_policy_t *time_policy);
int pump_time_policy_empty(motor_total_policy_t *total_policy);

/// 策略实体
motor_policy_t *pump_policy_init(motor_cfg_t cfg);
int pump_policy_del(motor_time_policy_t *time_policy);
int pump_policy_add(motor_time_policy_t *time_policy, motor_policy_t *next_policy);

#ifdef __cplusplus
}
#endif

#endif 
