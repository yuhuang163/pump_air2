/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "pump_mode_default_config.h"
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
const motor_policy_cfg_t *get_breast_pump_mode_para (int mode)
{
    switch (mode) {
        case BREAST_PUMP_STIMULATION_MODE:   
            return stimulation_default;
        case BREAST_PUMP_PUMP_MODE:   
            return pump_default;
        case BREAST_PUMP_MIX_MODE:   
            return mix_default;
        case BREAST_PUMP_CUSTOM_MODE:   
            return custom_default;
        case BREAST_PUMP_PP_MILK_TRACK_MODE:   
            return pp_milk_track_default;
        case BREAST_PUMP_MILK_BOOST_MODE:   
            return milk_boost_default;
        case BREAST_PUMP_LACTATION_START_MODE:   
            return lactation_start_default;  
    }
    return NULL;
}

int breast_pump_mode_parameter_deinit(motor_policy_manager_t *obj)
{
    EXCEPTION_CHECK_INT(obj);
    /// 删除定时策略
    pump_time_policy_empty(obj);
    /// 删除默认定时策略下的子策略
    pump_policy_del(&obj->head);
    /// 删除总策略
    pump_total_policy_deint(obj);

    return 0;
}