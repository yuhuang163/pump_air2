#ifndef __HAL_DIFF_PRESSURE_H__
#define __HAL_DIFF_PRESSURE_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h> // IWYU pragma: keep

/**
 * @brief 描述压差传感器操作码
*/
typedef enum {
    OP_CODE_DIFF_PRESSURE_GET_STATUS = 1,
    OP_CODE_DIFF_PRESSURE_IS_READY = 2,
    OP_CODE_DIFF_PRESSURE_IS_SUPPORT = 3, /// 查询是否支持压差传感器
} diff_pressure_dev_op_code_e;

/// 压差传感器状态
typedef struct {
    float pressure;
    float temp; 
    uint32_t raw_data;
} diff_pressure_status_t;

#ifdef __cplusplus
}
#endif

#endif 
