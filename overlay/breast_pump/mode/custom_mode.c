/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "breast_pump_mode.h"
#include "breast_pump_policy.h" // IWYU pragma: keep
#include "app_protocol_pump_ctrl_server.h"
#include "pump_mode_default_config.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/// 个性化模式
static motor_policy_manager_t *s_custom_obj = NULL;
static motor_policy_cfg_t s_custom_policy_cfg[CUSTOM_MODE_MOTOR_POLICY_NUMBER] = {0};
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
const motor_policy_manager_t *custom_mode_policy_get(void *user_data)
{
    return s_custom_obj;
}

motor_policy_cfg_t *custom_mode_policy_cfg_get(void)
{
    return s_custom_policy_cfg;
}

uint8_t pump_custom_step_para_to_local_motor_policy(user_def_pump_steps_data_t *pump_steps_data,
                                                                 motor_policy_cfg_t *custom_mode_policy_cfg)
{
    //memset(&custom_mode_policy_cfg, 0, sizeof(motor_policy_cfg_t)*CUSTOM_MODE_MOTOR_POLICY_NUMBER);
    uint8_t cfg_num = 0;
    for (uint8_t i = 0; i < pump_steps_data->num; i++)
    {
        if (cfg_num >= CUSTOM_MODE_MOTOR_POLICY_NUMBER) {
            LOG_MOTOR("custom pump step will over max");
        }
        LOG_MOTOR("custom pump step mode %d", pump_steps_data->data[i].mode);
        if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_SHORT) {
            motor_policy_cfg_t stimulation_default_temp[STIMULATION_MODE_LEN] = {0};
            memcpy(stimulation_default_temp, &stimulation_default, sizeof(stimulation_default));
            stimulation_default_temp[0].duration = pump_steps_data->data[i].time;
            stimulation_default_temp[0].core.un.mapping.level = pump_steps_data->data[i].level;
            stimulation_default_temp[0].core.un.mapping.speed = pump_steps_data->data[i].freq;
            memcpy(custom_mode_policy_cfg + cfg_num * sizeof(motor_policy_cfg_t), &stimulation_default_temp,
                   sizeof(stimulation_default_temp));
            cfg_num += 1;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_LONG) {
            motor_policy_cfg_t pump_default_temp[PUMP_MODE_LEN] = {0};
            memcpy(pump_default_temp, &pump_default, sizeof(pump_default));
            pump_default_temp[0].duration = pump_steps_data->data[i].time;
            pump_default_temp[0].core.un.mapping.level = pump_steps_data->data[i].level;
            pump_default_temp[0].core.un.mapping.speed = pump_steps_data->data[i].freq;
            memcpy(custom_mode_policy_cfg + cfg_num * sizeof(motor_policy_cfg_t), &pump_default_temp,
                   sizeof(pump_default_temp));
            cfg_num += 1;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_MIX1) {
            motor_policy_cfg_t mix_default_temp[MIX_MODE_LEN] = {0};
            memcpy(mix_default_temp, &mix_default, sizeof(mix_default));
            for (uint8_t j = 0; j < MIX_MODE_LEN; j++) {
                mix_default_temp[j].duration = pump_steps_data->data[i].time;
                mix_default_temp[j].core.un.mapping.level = pump_steps_data->data[i].level;
                mix_default_temp[j].core.un.mapping.speed = pump_steps_data->data[i].freq;
            }
            memcpy(custom_mode_policy_cfg + cfg_num * sizeof(motor_policy_cfg_t), &mix_default_temp,
                   sizeof(mix_default_temp));
            cfg_num += 2;
        } else if (pump_steps_data->data[i].mode == APP_OLD_COMM_MODE_MIX2) {
            motor_policy_cfg_t mix_default2_temp[MIX2_MODE_LEN] = {0};
            memcpy(mix_default2_temp, &mix_default2, sizeof(mix_default2));
            for (uint8_t j = 0; j < MIX2_MODE_LEN; j++) {
                mix_default2_temp[j].duration = pump_steps_data->data[i].time;
                mix_default2_temp[j].core.un.mapping.level = pump_steps_data->data[i].level;
                mix_default2_temp[j].core.un.mapping.speed = pump_steps_data->data[i].freq;
            }
            memcpy(custom_mode_policy_cfg + cfg_num * sizeof(motor_policy_cfg_t), &mix_default2_temp,
                   sizeof(mix_default2_temp));
            cfg_num += 2;
        } else {
            LOG_MOTOR("unknow custom pump step mode");
        }
    }
    return cfg_num;
}

int custom_mode_parameter_init(void)
{
    const motor_policy_cfg_t *cfg = get_breast_pump_mode_para(BREAST_PUMP_CUSTOM_MODE);
    user_def_pump_steps_data_t custom_pump_steps_data;
    storage_manager_context_t *storage_ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_UINT(storage_ctx);
    storage_ctx->get_store_ram_val(STORE_ID_PUMP_CUSTOM_STEPS_PARAMETER_INFO, 0, 0, &custom_pump_steps_data);
    if(custom_pump_steps_data.id.dat32 != 0x00) //标识是默认custom策略定义
    {
       uint8_t cfg_number = pump_custom_step_para_to_local_motor_policy(&custom_pump_steps_data, s_custom_policy_cfg);
       s_custom_obj = breast_pump_mode_parameter_init(s_custom_policy_cfg, CUSTOM_MODE_LEN, "custom");
    }
    else
    {
       s_custom_obj = breast_pump_mode_parameter_init(cfg, CUSTOM_MODE_LEN, "custom");
    }
    // LOG_MOTOR("obj:%p", s_custom_obj);
    return s_custom_obj == NULL ? -1 : 0;
}

int custom_mode_parameter_update(motor_policy_cfg_t *cfg, uint8_t cfg_num)
{
    motor_policy_manager_t *new_obj = NULL;
    new_obj = breast_pump_mode_parameter_init(cfg, cfg_num, "new_custom");
    if (new_obj) {
        breast_pump_mode_parameter_deinit(s_custom_obj);
        s_custom_obj = new_obj;
        // LOG_MOTOR("obj:%p", s_custom_obj);
        return 0;
    }
    // LOG_MOTOR("obj:%p", s_custom_obj);
    return -1;
}

 
 
 