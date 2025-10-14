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

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
motor_total_policy_t *pump_total_policy_init(motor_cfg_t cfg) 
{
    motor_total_policy_t *policy_obj = u_malloz(sizeof(motor_total_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail !!!");
        return NULL;
    }
    motor_para_t *m_para = &policy_obj->body.policy.para;
    const negative_pressure_para_t *n_pressure = &n_pressure_level[cfg.level][cfg.speed];

    policy_obj->time_policy_num = 1;
    policy_obj->body.para_num = 1;
    policy_obj->body.hold_time_s = cfg.hold_time_s;
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->un.pump.pump_hold_time = cfg.pump_hold_time;
    memcpy(&m_para->un.pump.n_pressure, n_pressure, sizeof(negative_pressure_para_t));

    return policy_obj;
}

int pump_total_policy_deint(motor_total_policy_t *total_policy) 
{
    EXCEPTION_CHECK_INT(total_policy);
    if (total_policy->time_policy_num > 1) {
        LOGE("total policy memory leak !!!!!!!!");
    }
    u_free(total_policy);
    total_policy = NULL;

    return 0;
}