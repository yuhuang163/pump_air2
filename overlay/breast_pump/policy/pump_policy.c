/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "motor_policy.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "pump_table.h" 
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
motor_policy_t *pump_policy_init(motor_policy_cfg_t cfg) 
{
    motor_policy_t *policy_obj = u_malloz(sizeof(motor_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail");
        return NULL;
    }

    motor_policy_para_t *m_para = &policy_obj->para;
    m_para->level = cfg.level;
    m_para->speed = cfg.speed;
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->pump_hold_time = cfg.pump_hold_time;

    return policy_obj;
}

int pump_policy_add(motor_policy_t *policy, motor_policy_t *next_policy) 
{
    if (policy == NULL || next_policy == NULL) {
        return -1;       
    }
    policy->next_policy = next_policy;

    return 0;
}

int pump_policy_del(motor_time_policy_t *time_policy) 
{
    motor_policy_t *next_policy = NULL;
    motor_policy_t *current_policy = time_policy->policy.next_policy;
    EXCEPTION_CHECK_INT(time_policy);
    
    for (;;) {
        if (time_policy->para_num <= 1 || current_policy == NULL) {
            break;
        }
        next_policy = current_policy->next_policy;
        u_free(current_policy);
        current_policy = next_policy;
        time_policy->para_num--;
    }

    return 0;
}

int pump_policy_update(negative_pressure_info_t *info) 
{ 
    switch (info->status) {
        case N_PRESSURE_IDLE:
            info->status = N_PRESSURE_GENERATE;
            info->action = MOTOR_PUMP_ON;
            break;

        case N_PRESSURE_GENERATE:
            if (get_timestamp_ms(info->start_time) >= info->n_pressure.pumping_time) {
                info->status = N_PRESSURE_HOLD;
                info->action = MOTOR_PUMP_OFF;
            }
            break;

        case N_PRESSURE_HOLD:
            if (get_timestamp_ms(info->start_time) >= info->pump_hold_time) {
                info->status = N_PRESSURE_RELEASE;
                info->action = MOTOR_VALVE_ON;
            }
            break;

        case N_PRESSURE_RELEASE:
            if (get_timestamp_ms(info->start_time) >= info->n_pressure.valve_on_time) {
                info->status = N_PRESSURE_SLEEP;
                info->action = MOTOR_VALVE_OFF;
            }
            break;

        case N_PRESSURE_SLEEP:
            if (get_timestamp_ms(info->start_time) >= info->cycles_interval) {
                info->cycles_count++;
                if (info->cycles_count < info->cycles_num) {
                    info->status = N_PRESSURE_GENERATE;
                    info->action = MOTOR_PUMP_ON;
                } else {
                    info->policy_update_flag = 1;
                }
            }
            break;

        default:
            break;

    }
    return 0;
}

void pump_policy_info_reset(motor_policy_t *policy, negative_pressure_info_t *info) 
{
    info->status = N_PRESSURE_IDLE;
    info->action = MOTOR_ACTION_IDLE; 
    info->n_pressure_flag = 0;
    info->policy_update_flag = 0;
    info->cycles_count = 0;
    info->start_time = 0;

    info->pump_hold_time = policy->para.pump_hold_time;
    info->cycles_num = policy->para.cycles_num;
    info->cycles_interval = policy->para.cycles_interval;
    
    uint8_t level = policy->para.level;
    uint8_t speed = policy->para.speed;
    /// 可以在此处导入设置
    const negative_pressure_para_t *n_pressure = &n_pressure_level[level][speed];
    info->n_pressure.pump_level = n_pressure->pump_level;
    info->n_pressure.pumping_time = n_pressure->pumping_time;
    info->n_pressure.valve_on_time = n_pressure->valve_on_time;
}