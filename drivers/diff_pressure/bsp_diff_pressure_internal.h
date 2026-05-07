#ifndef __BSP_DIFF_PRESSURE_INTERNAL_H__
#define __BSP_DIFF_PRESSURE_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h" // IWYU pragma: keep

/**@brief Firmware information define*/

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_diff_pressure_init(void);
int bsp_diff_pressure_deinit(void);
int bsp_read_diff_pressure_status(void *args);
int bsp_diff_pressure_is_ready(void *args);
int bsp_diff_pressure_get_flow_status(void *args);

#ifdef __cplusplus
}
#endif

#endif 
