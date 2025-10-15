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
const motor_total_policy_t *breastfeeding_mode_policy_get(void *user_data);
int breastfeeding_mode_parameter_deinit(void);
int breastfeeding_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *hybrid_mode_policy_get(void *user_data);
int hybrid_mode_parameter_deinit(void);
int hybrid_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *milk_initiation_mode_policy_get(void *user_data);
int milk_initiation_mode_parameter_deinit(void);
int milk_initiation_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *personalized_mode_policy_get(void *user_data);
int personalized_mode_parameter_deinit(void);
int personalized_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *pp_catch_up_mode_policy_get(void *user_data);
int pp_catch_up_mode_parameter_deinit(void);
int pp_catch_up_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *stimulation_mode_policy_get(void *user_data);
int stimulation_mode_parameter_deinit(void);
int stimulation_mode_parameter_init(motor_manager_context_t *thiz);

const motor_total_policy_t *upgrade_mode_policy_get(void *user_data);
int upgrade_mode_parameter_deinit(void);
int upgrade_mode_parameter_init(motor_manager_context_t *thiz);

#ifdef __cplusplus
}
#endif

#endif 







