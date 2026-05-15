#ifndef __BSP_PROXIMITY_SENSOR_INTERNAL_H__
#define __BSP_PROXIMITY_SENSOR_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_common.h"
#include "hal_proximity_sensor.h"

int bsp_proximity_sensor_init(void);
int bsp_proximity_sensor_deinit(void);
int bsp_read_proximity_status(void *args);
int bsp_proximity_sensor_is_ready(void *args);
int bsp_proximity_set_cali(const proximity_cali_data_t *cali);
int bsp_proximity_get_chip_id(void *args);
int bsp_proximity_set_notify_callback(void (*cb)(void));
int bsp_proximity_enable_irq(void);
int bsp_proximity_disable_irq(void);

#ifdef __cplusplus
}
#endif

#endif
