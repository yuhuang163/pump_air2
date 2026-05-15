#include "stk33562.h"
#include <string.h>
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_i2c.h"
#include "hal_gpio.h"
#include "cmsis_os_api.h"

#define STK33562_PS_FAR_THD_RATIO_DEFAULT   65u
#define STK33562_FACTORY_PS_DIFF_MIN        150u
#define STK33562_I2C_RETRY                  3u
#define STK33562_IRQ_MSG                    0xA5u

#define STK_STATE_REG                       0x00u
#define STK_PSCTRL_REG                      0x01u
#define STK_LEDCTRL_REG                     0x03u
#define STK_WAIT_REG                        0x05u
#define STK_THDH1_PS_REG                    0x06u
#define STK_THDH2_PS_REG                    0x07u
#define STK_THDL1_PS_REG                    0x08u
#define STK_THDL2_PS_REG                    0x09u
#define STK_FLAG_REG                        0x10u
#define STK_DATA1_PS_REG                    0x11u
#define STK_DATA2_PS_REG                    0x12u
#define STK_GAINCTRL_REG                    0x4Eu
#define STK_AGCTRL_REG                      0xDBu
#define STK_PDCTRL_REG                      0xA1u

#define STK_PS_PRST_SHIFT                   6u
#define STK_PS_GAIN_SHIFT                   4u
#define STK_PS_IT_SHIFT                     0u
#define STK_LED_IRDR_SHIFT                  4u
#define STK_STATE_EN_WAIT_MASK              0x04u
#define STK_STATE_EN_PS_MASK                0x01u
#define STK_FLG_INVALID_PS_MASK             0x02u
#define STK_FLG_PSINT_MASK                  0x10u

#define STK_PS_PRS1                         0x00u
#define STK_PS_GAIN8                        0x03u
#define STK_PS_IT200                        0x01u
#define STK_IRDR_LED_100mA                  0x06u
#define STK_WAIT_100                        0x40u

static const stk33562_reg_table_t s_default_regs[] = {
    { STK_PSCTRL_REG, ((STK_PS_PRS1 << STK_PS_PRST_SHIFT) | (STK_PS_GAIN8 << STK_PS_GAIN_SHIFT) | (STK_PS_IT200 << STK_PS_IT_SHIFT)), 0xFF },
    { STK_LEDCTRL_REG, (STK_IRDR_LED_100mA << STK_LED_IRDR_SHIFT), 0xFF },
    { STK_WAIT_REG, STK_WAIT_100, 0xFF },
    { STK_GAINCTRL_REG, 0x00, 0x01 },
    { STK_AGCTRL_REG, 0x01, 0x03 },
    { 0xA0, 0x10, 0xFF },
    { STK_PDCTRL_REG, 0x7C, 0xFF },
    { 0xAA, 0x0C, 0x7F },
    { 0xA8, 0xFF, 0xFF },
    { 0xA9, 0xFF, 0xFF },
    { 0x81, 0x63, 0xFF },
    { 0x45, 0x20, 0xFF },
};

static bool s_dev_ready;
static bool s_worker_inited;
static uint16_t s_near_threshold;
static uint16_t s_far_threshold;
static uint8_t s_far_ratio_percent = STK33562_PS_FAR_THD_RATIO_DEFAULT;
static proximity_status_t s_cached_status;
static bool s_cache_valid;
static stk33562_notify_fptr s_notify_cb;
static thread_context_t s_worker_thd;
static volatile uint8_t s_irq_msg_pending;

static int stk33562_write_reg(uint8_t reg, uint8_t val)
{
    i2c_master_data_t i2c_data = {
        .dev_address = STK33562_I2C_ADDR_7BIT,
        .reg_addr = &reg,
        .reg_addr_len = 1,
        .data = &val,
        .len = 1,
    };

    for (uint8_t i = 0; i < STK33562_I2C_RETRY; i++) {
        if (bsp_i2c_write_byte(P_SENSOR_I2C_ID, &i2c_data) == BSP_OK) {
            return BSP_OK;
        }
        msleep(2);
    }
    return BSP_ERROR;
}

static int stk33562_read_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
    i2c_master_data_t i2c_data = {
        .dev_address = STK33562_I2C_ADDR_7BIT,
        .reg_addr = &reg,
        .reg_addr_len = 1,
        .data = data,
        .len = len,
    };

    for (uint8_t i = 0; i < STK33562_I2C_RETRY; i++) {
        if (bsp_i2c_read_byte(P_SENSOR_I2C_ID, &i2c_data) == BSP_OK) {
            return BSP_OK;
        }
        msleep(2);
    }
    return BSP_ERROR;
}

static void stk33562_sw_reset(void)
{
    (void)stk33562_write_reg(STK33562_SW_RESET_REG, STK33562_SW_RESET_VAL);
}

static int stk33562_reg_init(void)
{
    uint8_t data;

    for (uint32_t i = 0; i < (sizeof(s_default_regs) / sizeof(s_default_regs[0])); i++) {
        const stk33562_reg_table_t *entry = &s_default_regs[i];

        if (entry->mask == 0xFF) {
            if (stk33562_write_reg(entry->address, entry->value) != BSP_OK) {
                return BSP_ERROR;
            }
        } else {
            if (stk33562_read_reg(entry->address, &data, 1) != BSP_OK) {
                return BSP_ERROR;
            }
            data = (uint8_t)((data & (uint8_t)(~entry->mask)) | (entry->value & entry->mask));
            if (stk33562_write_reg(entry->address, data) != BSP_OK) {
                return BSP_ERROR;
            }
        }
    }
    return BSP_OK;
}

static int stk33562_apply_threshold(void)
{
    uint8_t reg;

    if (s_near_threshold == 0 || s_far_threshold == 0) {
        return BSP_OK;
    }

    reg = (uint8_t)((s_near_threshold & 0xFF00) >> 8);
    if (stk33562_write_reg(STK_THDH1_PS_REG, reg) != BSP_OK) {
        return BSP_ERROR;
    }
    reg = (uint8_t)(s_near_threshold & 0x00FF);
    if (stk33562_write_reg(STK_THDH2_PS_REG, reg) != BSP_OK) {
        return BSP_ERROR;
    }
    reg = (uint8_t)((s_far_threshold & 0xFF00) >> 8);
    if (stk33562_write_reg(STK_THDL1_PS_REG, reg) != BSP_OK) {
        return BSP_ERROR;
    }
    reg = (uint8_t)(s_far_threshold & 0x00FF);
    if (stk33562_write_reg(STK_THDL2_PS_REG, reg) != BSP_OK) {
        return BSP_ERROR;
    }
    return BSP_OK;
}

static int stk33562_read_ps_raw(uint16_t *ps_raw)
{
    uint8_t data_l = 0;
    uint8_t data_h = 0;

    if (ps_raw == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    if (stk33562_read_reg(STK_DATA1_PS_REG, &data_h, 1) != BSP_OK) {
        return BSP_ERROR;
    }
    if (stk33562_read_reg(STK_DATA2_PS_REG, &data_l, 1) != BSP_OK) {
        return BSP_ERROR;
    }
    *ps_raw = (uint16_t)(((uint16_t)data_h << 8) | data_l);
    return BSP_OK;
}

static void stk33562_clear_interrupt(void)
{
    uint8_t flag = 0;

    if (stk33562_read_reg(STK_FLAG_REG, &flag, 1) != BSP_OK) {
        return;
    }
    (void)stk33562_write_reg(STK_FLAG_REG, (uint8_t)(flag & (uint8_t)(~STK_FLG_PSINT_MASK)));
}

static int stk33562_fetch_status(proximity_status_t *status)
{
    uint8_t flag = 0;

    if (!s_dev_ready) {
        return BSP_NOT_READY;
    }
    if (status == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    memset(status, 0, sizeof(*status));
    if (stk33562_read_reg(STK_FLAG_REG, &flag, 1) != BSP_OK) {
        return BSP_ERROR;
    }
    if (stk33562_read_ps_raw(&status->ps_raw) != BSP_OK) {
        return BSP_ERROR;
    }
    status->flag_reg = flag;
    status->is_worn = ((flag & STK_FLG_INVALID_PS_MASK) == 0);
	LOG_PBUS("stk33562_fetch_status: flag=0x%02x, ps_raw=%d, is_worn=%d", flag, status->ps_raw, status->is_worn);
    return BSP_OK;
}

static void stk33562_irq_work_in_thread(void)
{
    proximity_status_t status;

    s_irq_msg_pending = 0;
    if (!s_dev_ready) {
        return;
    }

    if (stk33562_fetch_status(&status) != BSP_OK) {
        return;
    }

    s_cached_status = status;
    s_cache_valid = true;
    stk33562_clear_interrupt();
    (void)stk33562_enable_irq();

    if (s_notify_cb) {
        s_notify_cb();
    }
}

static void stk33562_worker_thread(void *arg)
{
    uint8_t msg = 0;

    (void)arg;
    while (1) {
        MSG_QUEUE_RECV(s_worker_thd.msg_id, &msg, OS_WAIT_FOREVER);
        if (msg == STK33562_IRQ_MSG) {
            stk33562_irq_work_in_thread();
        }
    }
}

static bool stk33562_worker_start(void)
{
    if (s_worker_inited) {
        return true;
    }

    CREATE_MSG_TYPE_QUEUE(&s_worker_thd.msg_id, 4, 1);
    if (s_worker_thd.msg_id == NULL) {
        LOGE("stk33562 worker queue create fail");
        return false;
    }
    if (RUN_NEW_THREAD(&s_worker_thd, "stk33562", stk33562_worker_thread,
            RT_PRIO_SENSOR_INPUT_DISPATCH_THREAD, INPUT_SENSOR_THREAD_STACKSIZE) != 0) {
        LOGE("stk33562 worker thread create fail");
        return false;
    }
    s_worker_inited = true;
    return true;
}

static void stk33562_post_irq_msg(void)
{
    uint8_t msg = STK33562_IRQ_MSG;

    if (s_worker_thd.msg_id == NULL) {
        return;
    }
    (void)MSG_QUEUE_SEND(s_worker_thd.msg_id, &msg, 0);
}

static void stk33562_gpio_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    (void)pin;
    /* 中断里禁止 I2C：仅关 GPIO 中断并通知 worker 线程 */
    (void)stk33562_disable_irq();
    if (!s_irq_msg_pending) {
        s_irq_msg_pending = 1;
        stk33562_post_irq_msg();
    }
}

bool stk33562_is_ready(void)
{
    return s_dev_ready;
}

int stk33562_get_chip_id(uint8_t *chip_id)
{
    if (chip_id == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    return stk33562_read_reg(STK33562_PRODUCT_ID_REG, chip_id, 1);
}

static int stk33562_enable_ps(void)
{
    return stk33562_write_reg(STK_STATE_REG, (uint8_t)(STK_STATE_EN_WAIT_MASK | STK_STATE_EN_PS_MASK));
}

static int stk33562_disable_ps(void)
{
    return stk33562_write_reg(STK_STATE_REG, 0x00);
}

int stk33562_set_cali(const proximity_cali_data_t *cali)
{
	s_near_threshold = 30000;
	s_far_threshold = 20000;

	return stk33562_apply_threshold();

    // uint16_t diff;

    // if (cali == NULL) {
    //     s_near_threshold = 0;
    //     s_far_threshold = 0;
    //     return BSP_OK;
    // }

    // if (cali->far_ratio_percent != 0) {
    //     s_far_ratio_percent = cali->far_ratio_percent;
    // }

    // if (cali->near_ps > (uint16_t)(cali->uncover_ps + STK33562_FACTORY_PS_DIFF_MIN) && cali->uncover_ps != 0) {
    //     diff = (uint16_t)(cali->near_ps - cali->uncover_ps);
    //     s_near_threshold = cali->near_ps;
    //     s_far_threshold = (uint16_t)(cali->uncover_ps + (diff * s_far_ratio_percent) / 100u);
    //     return stk33562_apply_threshold();
    // }

    // s_near_threshold = 0;
    // s_far_threshold = 0;
    // return BSP_OK;
}

int stk33562_init(const proximity_cali_data_t *cali)
{
    uint8_t chip_id = 0;

    if (!stk33562_worker_start()) {
        return BSP_ERROR;
    }

    s_dev_ready = false;
    s_cache_valid = false;
    stk33562_sw_reset();
    msleep(60);

    if (stk33562_reg_init() != BSP_OK) {
        LOGE("stk33562 reg init failed");
        return BSP_ERROR;
    }
    msleep(50);

    if (stk33562_get_chip_id(&chip_id) != BSP_OK) {
        LOGE("stk33562 read chip id failed");
        return BSP_ERROR;
    }

    if (stk33562_set_cali(cali) != BSP_OK) {
        LOGE("stk33562 set threshold failed");
        return BSP_ERROR;
    }

    if (stk33562_enable_ps() != BSP_OK) {
        LOGE("stk33562 enable ps failed");
        return BSP_ERROR;
    }

    // if (stk33562_fetch_status(&s_cached_status) == BSP_OK) {
    //     s_cache_valid = true;
    // }

    s_dev_ready = true;
    LOG_PBUS("stk33562 init ok, chip_id=0x%02x", chip_id);
    return BSP_OK;
}

int stk33562_deinit(void)
{
    (void)stk33562_disable_irq();
    (void)stk33562_disable_ps();
    s_dev_ready = false;
    s_cache_valid = false;
    s_notify_cb = NULL;
    s_irq_msg_pending = 0;
    return BSP_OK;
}

int stk33562_get_cached_status(proximity_status_t *status)
{
    if (status == NULL) {
        return BSP_INVALID_PARAMETER;
    }
    if (!s_dev_ready || !s_cache_valid) {
        return BSP_NOT_READY;
    }
    *status = s_cached_status;
    return BSP_OK;
}

bool stk33562_get_is_worn(void)
{
    if (!s_dev_ready || !s_cache_valid) {
        return false;
    }
    return s_cached_status.is_worn;
}

int stk33562_enable_irq(void)
{
    struct HAL_GPIO_IRQ_CFG_T cfg = {
        1, 1, HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, stk33562_gpio_irq_handler,
    };
    s_irq_msg_pending = 0;
    return (hal_gpio_setup_irq(IR_SENSOR_INT_PIN, &cfg) == 0) ? BSP_OK : BSP_ERROR;
}

int stk33562_disable_irq(void)
{
    struct HAL_GPIO_IRQ_CFG_T cfg = {
        0, 0, HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, stk33562_gpio_irq_handler,
    };
    return (hal_gpio_setup_irq(IR_SENSOR_INT_PIN, &cfg) == 0) ? BSP_OK : BSP_ERROR;
}

int stk33562_set_notify_callback(stk33562_notify_fptr cb)
{
    s_notify_cb = cb;
    return BSP_OK;
}
