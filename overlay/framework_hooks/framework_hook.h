#ifndef __FRAMEWORK_HOOK_H__
#define __FRAMEWORK_HOOK_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_api.h"

/**@brief Firmware information define*/

/*
 * GLOBAL FUNCTION DECLARATION
 *****************************************************************************************
 */
 void register_motor_hook(motor_manager_context_t *ctx);
#ifdef __cplusplus
}
#endif

#endif 
