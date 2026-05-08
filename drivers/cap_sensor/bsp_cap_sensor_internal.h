#ifndef __BSP_CAP_SENSOR_INTERNAL_H__
#define __BSP_CAP_SENSOR_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "bsp_common.h" // IWYU pragma: keep

/*
* GLOBAL FUNCTION DECLARATION
*****************************************************************************************
*/
int bsp_cap_sensor_init(void);
int bsp_cap_sensor_deinit(void);
int bsp_cap_sensor_get_wear_flag(void *args);
int bsp_cap_sensor_get_touch_flag(void *args);

#ifdef __cplusplus
}
#endif

#endif
