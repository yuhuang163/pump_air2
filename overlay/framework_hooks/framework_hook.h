#ifndef __FRAMEWORK_HOOK_H__
#define __FRAMEWORK_HOOK_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

/**@brief Firmware information define*/

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
void register_motor_hook(motor_manager_context_t *ctx);
void register_sensor_hook(sensor_manager_context_t *ctx);
void register_breast_pump_hook(breast_pump_manager_context_t *ctx);
void register_storage_hook(storage_manager_context_t *ctx);
void register_input_key_hook(key_input_context_t *ctx);
#ifdef __cplusplus
}
#endif

#endif 
