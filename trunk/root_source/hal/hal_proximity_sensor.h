#ifndef __HAL_PROXIMITY_SENSOR_H__
#define __HAL_PROXIMITY_SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief 接近/佩戴传感器操作码
 */
typedef enum {
    OP_CODE_PROXIMITY_GET_STATUS = 1,
    OP_CODE_PROXIMITY_IS_READY = 2,
    OP_CODE_PROXIMITY_IS_SUPPORT = 3,
    OP_CODE_PROXIMITY_SET_CALI = 4,
    OP_CODE_PROXIMITY_GET_CHIP_ID = 5,
    OP_CODE_PROXIMITY_ADD_IRQ_CALLBACK = 6,
    OP_CODE_PROXIMITY_ENABLE_IRQ = 7,
    OP_CODE_PROXIMITY_DISABLE_IRQ = 8,
} proximity_sensor_dev_op_code_e;

/**
 * @brief 工厂校准数据（由上层 storage 读取后通过 SET_CALI 下发）
 */
typedef struct {
    uint16_t uncover_ps;
    uint16_t near_ps;
    uint8_t far_ratio_percent;
} proximity_cali_data_t;

/**
 * @brief 接近传感器状态
 */
typedef struct {
    uint16_t ps_raw;
    uint8_t flag_reg;
    bool is_worn;
} proximity_status_t;

#ifdef __cplusplus
}
#endif

#endif
