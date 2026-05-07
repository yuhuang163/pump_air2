#ifndef __HAL_DIFF_PRESSURE_H__
#define __HAL_DIFF_PRESSURE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h> // IWYU pragma: keep
#include "hal_air_pressure.h" // air_pressue_status_t

/// SDP3x + library/root_algo/remain_detection/flow_algo 输出的流量快照（最近一次批处理更新的累计量与流速）
typedef struct {
    uint8_t valid;
    float volume_ml;
    float flow_rate_ml_min;
    float algo_time_s;
} diff_pressure_flow_status_t;

/**
 * @brief 差压传感器操作码（与 air_pressure 设备层对齐）
 */
typedef enum {
    OP_CODE_DIFF_PRESSURE_GET_STATUS = 1,
    OP_CODE_DIFF_PRESSURE_IS_READY = 2,
    OP_CODE_DIFF_PRESSURE_IS_SUPPORT = 3,
    OP_CODE_DIFF_PRESSURE_GET_FLOW_STATUS = 4,
} diff_pressure_dev_op_code_e;

#ifdef __cplusplus
}
#endif

#endif
