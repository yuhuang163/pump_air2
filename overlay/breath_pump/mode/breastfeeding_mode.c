/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "breath_pump_policy.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/// 吸乳模式
static motor_total_policy_t *s_breastfeeding_obj = NULL;

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
const motor_total_policy_t *breastfeeding_mode_policy_get(void *user_data)
{
    return s_breastfeeding_obj;
}

int breastfeeding_mode_parameter_deinit(void)
{
    /// 删除定时策略
    pump_time_policy_empty(s_breastfeeding_obj);
    /// 删除默认子策略
    pump_policy_del(&s_breastfeeding_obj->body);
    /// 删除总策略
    pump_total_policy_deint(s_breastfeeding_obj);

    return 0;
}

int breastfeeding_mode_parameter_init(motor_manager_context_t *thiz)
{
    EXCEPTION_CHECK_INT(thiz);
    motor_policy_t *policy = NULL;
    motor_time_policy_t *time_policy = NULL;
    motor_cfg_t *cfg = thiz->priv.motor_cfg.breastfeeding;

    s_breastfeeding_obj = pump_total_policy_init(cfg[0]);
    time_policy = &s_breastfeeding_obj->body;
    for (uint8_t i = 1; i < HYBRID_MODE_LEN; i++) {
        /// 新增定时策略
        if (cfg[i].new_time_policy > 0) {
            time_policy = pump_time_policy_init(cfg[1]);
            if (time_policy == NULL) {
                LOGE("add time policy failed !!!");
                break;
            }
            s_breastfeeding_obj->time_policy_num++; /// 定时策略个数统计
            continue;
        } 
        
        /// 新增子策略实体
        policy = pump_policy_init(cfg[i]);
        if (policy == NULL) {
            LOGE("add policy failed !!!");
            break;
        }
        time_policy->para_num++; /// 子策略个数统计
        pump_policy_add(time_policy, policy);
    }

    return 0;

ERROR_EXIT:
    breastfeeding_mode_parameter_deinit();
    
    return -1;
}


