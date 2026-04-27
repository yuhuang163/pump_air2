#ifndef __DIFF_PRESSURE_MODULE_H__
#define __DIFF_PRESSURE_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Include files
******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep

#define DIFF_PRESSURE_POLLING_PERIOD              (20)

struct rt_device_node;
/// 压差传感器上下文
typedef struct diff_pressure_context {
    /// private
    struct __diff_pressure_private {
        struct rt_device_node *dev;
        uint32_t polling_cnt;
        uint16_t sampling_interval;
    } priv;
    /// public
    uint16_t (*get_sampling_interval)(void);
    void (*set_sampling_interval)(uint16_t interval);
} diff_pressure_context_t;

#ifdef __cplusplus
}
#endif

#endif 







