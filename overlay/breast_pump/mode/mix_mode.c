/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "persist_storage.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "breast_pump_mode.h"
#include "breast_pump_policy.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/// 混合模式
static motor_policy_manager_t *s_mix_obj = NULL;

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
const motor_policy_manager_t *mix_mode_policy_get(void *user_data)
{
    return s_mix_obj;
}

int mix_mode_parameter_deinit(void)
{
    /// 删除定时策略
    pump_time_policy_empty(s_mix_obj);
    /// 删除默认定时策略下的子策略
    pump_policy_del(&s_mix_obj->head);
    /// 删除总策略
    pump_total_policy_deint(s_mix_obj);
    return 0;
}

int mix_mode_parameter_init(motor_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    motor_policy_node_t *current_policy = NULL;
    motor_policy_node_t *new_policy = NULL;
    motor_timed_policy_node_t *current_time_policy = NULL;
    motor_timed_policy_node_t *new_time_policy = NULL;
    const motor_policy_cfg_t *cfg = get_breast_pump_mode_para(BREAST_PUMP_MIX_MODE);
    EXCEPTION_CHECK_INT(cfg);

    s_mix_obj = pump_total_policy_init(cfg[0]);
    current_time_policy = &s_mix_obj->head;
    current_policy = &s_mix_obj->head.policy;
    for (uint8_t i = 1; i < MIX_MODE_LEN; i++) {
        /// 新增定时策略
        if (cfg[i].policy_type == MOTOR_TIME_POLICY) {
            new_time_policy = pump_time_policy_init(cfg[1]);
            if (new_time_policy == NULL) {
                LOGE("add time policy failed !!!");
                break;
            }
            pump_time_policy_add(current_time_policy, new_time_policy);
            current_time_policy = new_time_policy;
            current_policy = &current_time_policy->policy;
            s_mix_obj->timed_policy_count++; /// 定时策略个数统计
            continue;
        } 
        
        /// 新增子策略实体
        new_policy = pump_policy_init(cfg[i]);
        if (new_policy == NULL) {
            LOGE("add policy failed !!!");
            break;
        }
        current_time_policy->sub_policy_count++; /// 子策略个数统计
        pump_policy_add(current_policy, new_policy);
    }
    return 0;

ERROR_EXIT:
    mix_mode_parameter_deinit();
    return -1;
}


