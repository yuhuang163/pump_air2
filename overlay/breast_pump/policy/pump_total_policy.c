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
motor_total_policy_t *pump_total_policy_init(motor_policy_cfg_t cfg) 
{
    motor_total_policy_t *policy_obj = u_malloz(sizeof(motor_total_policy_t));
    if (policy_obj == NULL) {
        LOGE("policy malloc fail !!!");
        return NULL;
    }
    motor_policy_para_t *m_para = &policy_obj->body.policy.para;

    policy_obj->time_policy_num = 1;
    policy_obj->body.para_num = 1;
    policy_obj->body.hold_time_s = cfg.hold_time_s;
    m_para->level = cfg.level;
    m_para->speed = cfg.speed;
    m_para->cycles_num = cfg.cycles_num;
    m_para->cycles_interval = cfg.cycles_interval;
    m_para->pump_hold_time = cfg.pump_hold_time;

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