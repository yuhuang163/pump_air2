#ifndef __BREAST_PUMP_MODE_H__
#define __BREAST_PUMP_MODE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/
const motor_policy_manager_t *pump_mode_policy_get(void *user_data);
int pump_mode_parameter_deinit(void);
int pump_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *mix_mode_policy_get(void *user_data);
int mix_mode_parameter_deinit(void);
int mix_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *lactation_start_mode_policy_get(void *user_data);
int lactation_start_mode_parameter_deinit(void);
int lactation_start_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *custom_mode_policy_get(void *user_data);
int custom_mode_parameter_deinit(void);
int custom_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *pp_milk_track_mode_policy_get(void *user_data);
int pp_milk_track_mode_parameter_deinit(void);
int pp_milk_track_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *stimulation_mode_policy_get(void *user_data);
int stimulation_mode_parameter_deinit(void);
int stimulation_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_manager_t *milk_boost_mode_policy_get(void *user_data);
int milk_boost_mode_parameter_deinit(void);
int milk_boost_mode_parameter_init(motor_manager_context_t *thiz);

const motor_policy_cfg_t *get_breast_pump_mode_para(int mode);
#ifdef __cplusplus
}
#endif

#endif 







