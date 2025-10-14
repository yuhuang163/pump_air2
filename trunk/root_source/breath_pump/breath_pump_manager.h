#ifndef __BREATH_PUMP_MANAGER_H__
#define __BREATH_PUMP_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_api.h" // IWYU pragma: keep

#define BREATH_PUMP_WORK_STEP_TIME_MS   (100)
#define BREATH_PUMP_IS_GOING(state) (state == BREATH_PUMP_START || state == BREATH_PUMP_CONTINUE)

/*
* INTERNAL FUNCTION DECLARATION
*****************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif 







