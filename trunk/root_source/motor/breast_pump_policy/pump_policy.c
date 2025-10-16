/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "pump_parameters_table.h" 
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
motor_policy_node_t *pump_policy_init(motor_policy_cfg_t cfg) 
{
    motor_policy_node_t *policy_obj = u_malloz(sizeof(motor_policy_node_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail");
        return NULL;
    }

    motor_policy_core_t *m_para = &policy_obj->core;
    memcpy(m_para, &cfg.core, sizeof(motor_policy_core_t));

    return policy_obj;
}

int pump_policy_add(motor_policy_node_t *policy, motor_policy_node_t *next_policy) 
{
    if (policy == NULL || next_policy == NULL) {
        return -1;       
    }
    policy->next = next_policy;

    return 0;
}

int pump_policy_del(motor_timed_policy_node_t *time_policy) 
{
    motor_policy_node_t *next_policy = NULL;
    motor_policy_node_t *current_policy = time_policy->policy.next;
    EXCEPTION_CHECK_INT(time_policy);
    
    for (;;) {
        if (time_policy->sub_policy_count <= 1 || current_policy == NULL) {
            break;
        }
        next_policy = current_policy->next;
        u_free(current_policy);
        current_policy = next_policy;
        time_policy->sub_policy_count--;
    }

    return 0;
}

int pump_policy_update(motor_policy_info_t *info) 
{ 
    switch (info->state.status) {
        case SUCTION_STATUS_IDLE:
            info->state.status = SUCTION_STATUS_GENERATE;
            info->state.action = MOTOR_ACTION_PUMP_ON;
            break;

        case SUCTION_STATUS_GENERATE:
            if (get_timestamp_ms(info->state.start_time) >= info->parameters.pumping_time) {
                info->state.status = SUCTION_STATUS_HOLD;
                info->state.action = MOTOR_ACTION_PUMP_OFF;
            }
            break;

        case SUCTION_STATUS_HOLD:
            if (get_timestamp_ms(info->state.start_time) >= info->config.hold_time) {
                info->state.status = SUCTION_STATUS_RELEASE;
                info->state.action = MOTOR_ACTION_VALVE_ON;
            }
            break;

        case SUCTION_STATUS_RELEASE:
            if (get_timestamp_ms(info->state.start_time) >= info->parameters.valve_on_time) {
                info->state.status = SUCTION_STATUS_SLEEP;
                info->state.action = MOTOR_ACTION_VALVE_OFF;
            }
            break;

        case SUCTION_STATUS_SLEEP:
            if (get_timestamp_ms(info->state.start_time) >= info->config.loop_intv) {
                info->state.cycles_count++;
                if (info->state.cycles_count < info->config.loop_num) {
                    info->state.status = SUCTION_STATUS_GENERATE;
                    info->state.action = MOTOR_ACTION_PUMP_ON;
                } else {
                    info->state.policy_update_req = 1;
                }
            }
            break;

        default:
            break;

    }
    return 0;
}

void pump_policy_info_reset(struct motor_manager_context *thiz, motor_policy_node_t *policy, motor_policy_info_t *info) 
{
    suction_strategy_mapping_t map_para = {0};
    const suction_parameters_t *n_pressure = NULL;
    
    /// 默认策略
    memset(&info->state, 0, sizeof(motor_policy_state_t));
    memcpy(&info->config, &policy->core.config, sizeof(suction_policy_config_t));
    memcpy(&map_para, &policy->core.mapping, sizeof(suction_strategy_mapping_t));
    /// 空闲模式，吸力设定为0
    if (map_para.mode_type == SUCTION_MODE_IDLE) {
        memset(&info->parameters, 0, sizeof(suction_parameters_t));
        return;
    }
    /// 用户设定力度与频率(速度)策略
    pump_user_policy_update(thiz, &map_para); 
    /// 通过map参数获取吸力参数
    if (map_para.mode_type == SUCTION_MODE_SHORT) {
        n_pressure = &short_suction_table[map_para.level][map_para.speed];
    } else {
        n_pressure = &long_suction_table[map_para.level][map_para.speed];
    }
    info->parameters.pump_level = n_pressure->pump_level;
    info->parameters.pumping_time = n_pressure->pumping_time;
    info->parameters.valve_on_time = n_pressure->valve_on_time;
    /// 通过算法线性补偿
    pump_compensation_policy_update(thiz, &info->parameters); 
}