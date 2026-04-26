#ifndef __BSP_PRESSURE_SENSOR_INTERNAL_H__
#define __BSP_PRESSURE_SENSOR_INTERNAL_H__

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
int bsp_pressure_sensor_init(void);
int bsp_pressure_sensor_deinit(void);
int bsp_read_pressure_sensor_status(void *args);
int bsp_pressure_sensor_is_ready(void *args);

#ifdef __cplusplus
}
#endif

#endif 
