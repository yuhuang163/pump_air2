#ifndef __HAL_CAP_SENSOR_H__
#define __HAL_CAP_SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stdint.h>

/**
 * @brief 描述电容佩戴检测操作码
*/
typedef enum {
    OP_CODE_CAP_SENSOR_GET_WEAR_FLAG = 1,
    OP_CODE_CAP_SENSOR_GET_TOUCH_FLAG = 2,
    OP_CODE_CAP_SENSOR_IS_READY = 3,
} cap_sensor_dev_op_code_e;

#ifdef __cplusplus
}
#endif

#endif
