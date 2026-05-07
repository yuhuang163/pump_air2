#ifndef SDP3X_SENSIRION_REFERENCE_H
#define SDP3X_SENSIRION_REFERENCE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sdp3x_sensirion_reference.h
 * @brief 与 Sensirion 官方 SDP3x 示例 `main.c`（2016-08）等价的逻辑时序，仅描述算法步骤；
 *        寄存器/I2C 由驱动层实现后通过 ops 回调注入。
 *
 * 参考流程：
 *   Sdp3x_SoftReset();
 *   Sdp3x_StartContinousMeasurement(...);
 *   循环：DelayMicroSeconds(1000); Sdp3x_ReadMeasurementResults(...);
 */

typedef void (*sdp3x_sensirion_delay_ms_fn)(uint32_t ms);

/** 返回 0 成功，非 0 失败（与 BSP 无关，便于本目录纯算法语义） */
typedef int (*sdp3x_sensirion_cmd_fn)(void);

typedef struct sdp3x_sensirion_reference_ops {
    sdp3x_sensirion_cmd_fn soft_reset;
    sdp3x_sensirion_cmd_fn start_continuous_measurement;
    sdp3x_sensirion_delay_ms_fn delay_ms; /**< 可为 NULL：跳过时序中的延时 */
} sdp3x_sensirion_reference_ops_t;

/** 默认值与当前 BSP 对齐，可按板级调教 */
#ifndef SDP3X_SENSIRION_MS_AFTER_SOFT_RESET
#define SDP3X_SENSIRION_MS_AFTER_SOFT_RESET 40u
#endif
#ifndef SDP3X_SENSIRION_MS_AFTER_START_CONTINUOUS
#define SDP3X_SENSIRION_MS_AFTER_START_CONTINUOUS 10u
#endif

/**
 * @brief Sensirion 示例中的「复位 → 等待 → 启动连续测量 → 再等待」
 */
int sdp3x_sensirion_reference_bringup(const sdp3x_sensirion_reference_ops_t *ops,
                                      uint32_t delay_ms_after_soft_reset,
                                      uint32_t delay_ms_after_start_continuous);

/**
 * @brief 示例在读数前插入的短时等待（如 1 ms 对应 DelayMicroSeconds(1000)）
 */
void sdp3x_sensirion_reference_delay_before_read(const sdp3x_sensirion_reference_ops_t *ops,
                                                 uint32_t delay_ms_before_read);

#ifdef __cplusplus
}
#endif

#endif /* SDP3X_SENSIRION_REFERENCE_H */
