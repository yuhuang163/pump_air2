#ifndef __BREAST_PUMP_POLICY_H__
#define __BREAST_PUMP_POLICY_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "motor_policy.h"

struct motor_manager_context;
struct motor_task_para;

/// 策略集合
motor_total_policy_t *pump_total_policy_init(motor_policy_cfg_t cfg);
int pump_total_policy_deint(motor_total_policy_t *total_policy);

/// 定时策略
motor_time_policy_t *pump_time_policy_init(motor_policy_cfg_t cfg);
int pump_time_policy_add(motor_time_policy_t *time_policy, motor_time_policy_t *next_time_policy);
int pump_time_policy_empty(motor_total_policy_t *total_policy);
int pump_time_policy_update(motor_policy_info_t *info);

/// 策略实体
motor_policy_t *pump_policy_init(motor_policy_cfg_t cfg);
int pump_policy_del(motor_time_policy_t *time_policy);
int pump_policy_add(motor_policy_t *policy, motor_policy_t *next_policy);
int pump_policy_update(negative_pressure_info_t *info);
void pump_policy_info_reset(motor_policy_t *policy, negative_pressure_info_t *info);

/// 补偿策略
int pump_compensation_policy_update(struct motor_manager_context *thiz, motor_policy_info_t *info);

/// 用户策略，力度/频率调节
int pump_user_policy_update(struct motor_manager_context *thiz, motor_policy_info_t *info);
 
/// 吸奶策略处理
int breast_pump_mode_policy_handler(struct motor_manager_context *thiz, struct motor_task_para *play_para);
#ifdef __cplusplus
}
#endif

#endif 
