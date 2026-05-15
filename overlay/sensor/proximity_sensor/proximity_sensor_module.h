#ifndef __PROXIMITY_SENSOR_MODULE_H__
#define __PROXIMITY_SENSOR_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "root_common_base.h"
#include "hal_proximity_sensor.h"

struct rt_device_node;

typedef struct proximity_sensor_context {
    struct __proximity_sensor_private {
        struct rt_device_node *dev;
    } priv;

    /** 读取缓存的佩戴状态（不触发 I2C） */
    bool (*is_worn)(void);
    int (*get_status)(proximity_status_t *status);
} proximity_sensor_context_t;

#ifdef __cplusplus
}
#endif

#endif
