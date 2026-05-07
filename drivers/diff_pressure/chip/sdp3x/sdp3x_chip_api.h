#ifndef SDP3X_CHIP_API_H
#define SDP3X_CHIP_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_common.h" // IWYU pragma: keep
#include "project_config.h" // IWYU pragma: keep DIFF_PRESSURE_NUM

/**
 * 芯片层：连续测量帧读取（含与 BSP 一致的失败恢复），并更新内部 last 缓存。
 */
int sdp3x_chip_measurement_read(float *diff_pa, float *temp);

/**
 * 拷贝各通道 ready 标志（与 s_dev_is_ready 一致），长度至少 DIFF_PRESSURE_NUM。
 */
void sdp3x_chip_copy_ready_status(uint8_t *dst, unsigned count);

#ifdef __cplusplus
}
#endif

#endif
