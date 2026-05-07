/*
 * SDP3x：与 Sensirion 示例 main.c 一致的软件时序（无硬件/I2C 依赖）。
 */
#include "sdp3x_sensirion_reference.h"
#include <stddef.h>

int sdp3x_sensirion_reference_bringup(const sdp3x_sensirion_reference_ops_t *ops,
                                      uint32_t delay_ms_after_soft_reset,
                                      uint32_t delay_ms_after_start_continuous)
{
    int err;

    if (ops == NULL || ops->soft_reset == NULL || ops->start_continuous_measurement == NULL) {
        return -1;
    }

    err = ops->soft_reset();
    if (err != 0) {
        return err;
    }

    if (ops->delay_ms != NULL && delay_ms_after_soft_reset > 0U) {
        ops->delay_ms(delay_ms_after_soft_reset);
    }

    err = ops->start_continuous_measurement();
    if (err != 0) {
        return err;
    }

    if (ops->delay_ms != NULL && delay_ms_after_start_continuous > 0U) {
        ops->delay_ms(delay_ms_after_start_continuous);
    }

    return 0;
}

void sdp3x_sensirion_reference_delay_before_read(const sdp3x_sensirion_reference_ops_t *ops,
                                                uint32_t delay_ms_before_read)
{
    if (ops != NULL && ops->delay_ms != NULL && delay_ms_before_read > 0U) {
        ops->delay_ms(delay_ms_before_read);
    }
}
