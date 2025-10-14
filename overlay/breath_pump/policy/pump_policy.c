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
    motor_policy_t *node = (motor_policy_t *)pos;
    u_free(node);

    return node;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
motor_policy_t *pump_policy_init(motor_cfg_t cfg) 
{
    motor_policy_t *policy_obj = u_malloz(sizeof(motor_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail");
        return NULL;
    }

    motor_para_t *m_para = &policy_obj->para;
    const negative_pressure_para_t *n_pressure = &n_pressure_level[cfg.level][cfg.speed];
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->un.pump.pump_hold_time = cfg.pump_hold_time;
    memcpy(&m_para->un.pump.n_pressure, n_pressure, sizeof(negative_pressure_para_t));

    return policy_obj;
}

int pump_policy_add(motor_time_policy_t *time_policy, motor_policy_t *next_policy) 
{
    if (time_policy == NULL) {
        return -1;       
    }
    list_head_t *head = &time_policy->policy.para_list; 
    __list_add_tail(&next_policy->para_list, head);
    time_policy->para_num++;

    return 0;
}

int pump_policy_del(motor_time_policy_t *time_policy) 
{
    EXCEPTION_CHECK_INT(time_policy);
    if (time_policy->para_num > 1) {
        list_head_t *head = &time_policy->policy.para_list; 
        EXCEPTION_CHECK_INT(head);
        __list_empty(head, list_del_condition_callback);
    }
    time_policy->para_num = 1;

    return 0;
}