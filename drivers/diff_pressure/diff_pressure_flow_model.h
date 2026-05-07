#ifndef DIFF_PRESSURE_FLOW_MODEL_H
#define DIFF_PRESSURE_FLOW_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_diff_pressure.h"

/**
 * 差压 → 体积/流速 的算法层（flow_algo），与 SDP3x 芯片 I2C 驱动解耦。
 * 由 BSP 在每帧有效采样后调用 on_sample；GET_FLOW ioctl 读 get_status。
 */
void diff_pressure_flow_model_reset(void);
void diff_pressure_flow_model_on_sample(float pressure_pa);
void diff_pressure_flow_model_get_status(diff_pressure_flow_status_t *out);

#ifdef __cplusplus
}
#endif

#endif
