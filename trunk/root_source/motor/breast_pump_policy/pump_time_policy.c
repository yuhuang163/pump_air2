/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "breast_pump_policy.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
motor_timed_policy_node_t *pump_time_policy_init(motor_policy_cfg_t cfg) 
{
    motor_timed_policy_node_t *policy_obj = u_malloz(sizeof(motor_timed_policy_node_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail !!!");
        return NULL;
    }

    motor_policy_core_t *m_para = &policy_obj->policy.core;
    policy_obj->sub_policy_count = 1;
    policy_obj->duration_seconds = cfg.duration;
    memcpy(m_para, &cfg.core, sizeof(motor_policy_core_t));
    return policy_obj;
}

int pump_time_policy_add(motor_timed_policy_node_t *time_policy, motor_timed_policy_node_t *next_time_policy) 
{
    if (time_policy == NULL || next_time_policy == NULL) {
        return -1;       
    }
    time_policy->next = next_time_policy;
    return 0;
}

int pump_time_policy_empty(motor_policy_manager_t *total_policy) 
{ 
    motor_timed_policy_node_t *next_policy = NULL;
    motor_timed_policy_node_t *current_policy = total_policy->head.next;
    EXCEPTION_CHECK_INT(total_policy);
    for (;;) {
        if (total_policy->timed_policy_count <= 1 || current_policy == NULL) {
            break;
        }
        next_policy = current_policy->next;
        /// 删除定时策略前先删除该定时策略下的子策略
        pump_policy_del(current_policy);
        /// 删除定时策略
        u_free(current_policy);
        current_policy = next_policy;
        total_policy->timed_policy_count--;
    }
    return 0;
}

int pump_time_policy_update(struct motor_manager_context *thiz, motor_policy_runtime_t *info) 
{ 
    /// 吸力状态下不允许切换策略，跑完放气流程后才能切换
    if (info->current_info.state.n_pressure_flag) {
        return -2;
    }
    
    /// 初始化策略
    if (info->current_timed_policy == NULL) {
        info->current_timed_policy = &info->manager->head;
        info->timed_policy_start_time = get_timestamp_ms(0);
        info->manager_start_time = info->timed_policy_start_time;
        info->current_policy = &info->current_timed_policy->policy;
        pump_policy_info_reset(thiz, info->current_policy, &info->current_info);
        return 0;
    }
   
    /// 检测当前时间策略是否执行完毕
    uint16_t hold_time = get_timestamp_ms(info->timed_policy_start_time) / 1000;
    if (hold_time >= info->current_timed_policy->duration_seconds) {
        if (info->current_timed_policy->next == NULL) {
            LOG_MOTOR("time policy completed");
            return -1;
        }
        /// 执行下一个时间策略
        info->current_timed_policy = info->current_timed_policy->next;
        info->timed_policy_start_time = get_timestamp_ms(0);
        info->current_policy = &info->current_timed_policy->policy;
        pump_policy_info_reset(thiz, info->current_policy, &info->current_info);
        return 0;
    }

    /// 检测到子策略切换请求
    if (info->current_info.state.policy_update_req) {
        if (info->current_policy->next) {
            info->current_policy = info->current_policy->next;  /// 切换到下一个子策略
        } else {
            info->current_policy = &info->current_timed_policy->policy; /// 切换到当前时间策略下的第一个子策略 
        }
        pump_policy_info_reset(thiz, info->current_policy, &info->current_info);
    }

    return 0;
}