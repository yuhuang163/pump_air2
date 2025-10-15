/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "breast_pump_policy.h"
#include "root_log_define.h"
#include <stdint.h>
#include <string.h>

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
motor_time_policy_t *pump_time_policy_init(motor_policy_cfg_t cfg) 
{
    motor_time_policy_t *policy_obj = u_malloz(sizeof(motor_time_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail !!!");
        return NULL;
    }

    motor_policy_para_t *m_para = &policy_obj->policy.para;
    policy_obj->para_num = 1;
    policy_obj->hold_time_s = cfg.hold_time_s;
    m_para->level = cfg.level;
    m_para->speed = cfg.speed;
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->pump_hold_time = cfg.pump_hold_time;
    return policy_obj;
}

int pump_time_policy_add(motor_time_policy_t *time_policy, motor_time_policy_t *next_time_policy) 
{
    if (time_policy == NULL || next_time_policy == NULL) {
        return -1;       
    }
    time_policy->next_time_policy = next_time_policy;
    return 0;
}

int pump_time_policy_empty(motor_total_policy_t *total_policy) 
{ 
    motor_time_policy_t *next_policy = NULL;
    motor_time_policy_t *current_policy = total_policy->body.next_time_policy;
    EXCEPTION_CHECK_INT(total_policy);
    for (;;) {
        if (total_policy->time_policy_num <= 1 || current_policy == NULL) {
            break;
        }
        next_policy = current_policy->next_time_policy;
        /// 删除定时策略前先删除该定时策略下的子策略
        pump_policy_del(current_policy);
        /// 删除定时策略
        u_free(current_policy);
        current_policy = next_policy;
        total_policy->time_policy_num--;
    }
    return 0;
}

int pump_time_policy_update(motor_policy_info_t *info) 
{ 
    /// 负压状态下不允许切换策略，跑完放气流程后才能切换
    if (info->current_info.n_pressure_flag) {
        return -2;
    }
    
    /// 初始化策略
    if (info->current_time_policy == NULL) {
        info->current_time_policy = &info->total_obj->body;
        info->time_policy_start_time = get_timestamp_ms(0);
        info->total_policy_start_time = info->time_policy_start_time;
        info->current_policy = &info->current_time_policy->policy;
        pump_policy_info_reset(info->current_policy, &info->current_info);
        return 0;
    }
   
    /// 检测当前时间策略是否执行完毕
    uint16_t hold_time = get_timestamp_ms(info->time_policy_start_time) / 1000;
    if (hold_time >= info->current_time_policy->hold_time_s) {
        if (info->current_time_policy->next_time_policy == NULL) {
            LOG_MOTOR("time policy completed");
            return -1;
        }
        /// 执行下一个时间策略
        info->current_time_policy = info->current_time_policy->next_time_policy;
        info->time_policy_start_time = get_timestamp_ms(0);
        info->current_policy = &info->current_time_policy->policy;
        pump_policy_info_reset(info->current_policy, &info->current_info);
        return 0;
    }

    /// 检测到子策略切换请求
    if (info->current_info.policy_update_flag) {
        if (info->current_policy->next_policy) {
            info->current_policy = info->current_policy->next_policy;  /// 切换到下一个子策略
        } else {
            info->current_policy = &info->current_time_policy->policy; /// 切换到当前时间策略下的第一个子策略 
        }
        pump_policy_info_reset(info->current_policy, &info->current_info);
    }

    return 0;
}