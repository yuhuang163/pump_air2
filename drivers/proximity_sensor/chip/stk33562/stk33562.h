#ifndef __STK33562_H__
#define __STK33562_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_proximity_sensor.h"

#define STK33562_I2C_ADDR_7BIT          0x46u
#define STK33562_PRODUCT_ID_REG         0x3Eu
#define STK33562_SW_RESET_REG           0x80u
#define STK33562_SW_RESET_VAL           0xB6u

typedef struct {
    uint8_t address;
    uint8_t value;
    uint8_t mask;
} stk33562_reg_table_t;

/** 中断后在 worker 线程里更新完缓存后回调（禁止在回调里做 I2C） */
typedef void (*stk33562_notify_fptr)(void);

int stk33562_init(const proximity_cali_data_t *cali);
int stk33562_deinit(void);
int stk33562_get_chip_id(uint8_t *chip_id);
/** 读取线程更新后的缓存，不访问 I2C */
int stk33562_get_cached_status(proximity_status_t *status);
bool stk33562_get_is_worn(void);
int stk33562_set_cali(const proximity_cali_data_t *cali);
int stk33562_enable_irq(void);
int stk33562_disable_irq(void);
int stk33562_set_notify_callback(stk33562_notify_fptr cb);
bool stk33562_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif
