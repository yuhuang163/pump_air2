/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "pump_table.h"
#include "breath_pump_policy.h"

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
static void *list_del_condition_callback(void *data, list_head_t *pos)
{
    motor_time_policy_t *node = (motor_time_policy_t *)pos;

    pump_policy_del(node);    /// 删除当前定时策略下的子策略
    if (node->para_num > 1) {
        LOGE("time policy memory leak !!!!!!!!");
    }
    u_free(node);

    return node;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
motor_time_policy_t *pump_time_policy_init(motor_cfg_t cfg) 
{
    motor_time_policy_t *policy_obj = u_malloz(sizeof(motor_time_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail !!!");
        return NULL;
    }

    motor_para_t *m_para = &policy_obj->policy.para;
    const negative_pressure_para_t *n_pressure = &n_pressure_level[cfg.level][cfg.speed];

    policy_obj->para_num = 1;
    policy_obj->hold_time_s = cfg.hold_time_s;
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->un.pump.pump_hold_time = cfg.pump_hold_time;
    memcpy(&m_para->un.pump.n_pressure, n_pressure, sizeof(negative_pressure_para_t));

    return policy_obj;
}

int pump_time_policy_add(motor_total_policy_t *total_policy, motor_time_policy_t *time_policy) 
{
    if (total_policy == NULL) {
        return -1;       
    }

    list_head_t *head = &total_policy->body.policy_list; 
    __list_add_tail(&time_policy->policy_list, head);
    total_policy->time_policy_num++;

    return 0;
}

int pump_time_policy_empty(motor_total_policy_t *total_policy) 
{ 
    EXCEPTION_CHECK_INT(total_policy);
    if (total_policy->time_policy_num > 1) {
        list_head_t *head = &total_policy->body.policy_list; 
        EXCEPTION_CHECK_INT(head);
        __list_empty(head, list_del_condition_callback);
    }
    total_policy->time_policy_num = 1;

    return 0;
}