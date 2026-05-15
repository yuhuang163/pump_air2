/*
 * ICM-42670-P BSP glue: I2C serif, init/deinit, DRDY polling read, optional GPIO IRQ.
 */
#include "bsp_common.h"
#include "bsp_config.h"
#include "bsp_i2c.h"
#include "bsp_imu_internal.h"
#include "bsp_internal.h"
#include "hal_gpio.h"
#include "hal_imu.h"
#include "imu/inv_imu_defs.h"
#include "imu/inv_imu_driver.h"

#include <stddef.h>
#include <string.h>

#define ICM42670_I2C_ADDR_7BIT         0x69u    ///0x68u
#define ICM_I2C_MAX_XFER               256u

typedef struct {
    i2c_master_id_e bus;
    uint8_t addr_7bit;
} icm42670_i2c_ctx_t;

static struct inv_imu_device s_imu;
static icm42670_i2c_ctx_t s_i2c_ctx;
static imu_raw_data_t s_last_raw;
static void (*s_irq_user_cb)(uint8_t *data);

static int icm_i2c_read_reg(struct inv_imu_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len)
{
    icm42670_i2c_ctx_t *ctx = (icm42670_i2c_ctx_t *)serif->context;
    uint8_t reg8 = reg;

    if (ctx == NULL || buf == NULL || len == 0 || len > ICM_I2C_MAX_XFER) {
        return -1;
    }

    i2c_master_data_t d = {
        .dev_address = ctx->addr_7bit,
        .reg_addr = &reg8,
        .reg_addr_len = 1,
        .data = buf,
        .len = (uint16_t)len,
    };
    return (bsp_i2c_read_byte(ctx->bus, &d) == BSP_OK) ? 0 : -1;
}

static int icm_i2c_write_reg(struct inv_imu_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len)
{
    icm42670_i2c_ctx_t *ctx = (icm42670_i2c_ctx_t *)serif->context;
    uint8_t reg8 = reg;

    if (ctx == NULL || buf == NULL || len == 0 || len > ICM_I2C_MAX_XFER) {
        return -1;
    }

    i2c_master_data_t d = {
        .dev_address = ctx->addr_7bit,
        .reg_addr = &reg8,
        .reg_addr_len = 1,
        .data = (uint8_t *)buf,
        .len = (uint16_t)len,
    };
    return (bsp_i2c_write_byte(ctx->bus, &d) == BSP_OK) ? 0 : -1;
}

static void imu_sensor_event_cb(inv_imu_sensor_event_t *event)
{
    s_last_raw.acc.x = event->accel[0];
    s_last_raw.acc.y = event->accel[1];
    s_last_raw.acc.z = event->accel[2];
    s_last_raw.gyro.x = event->gyro[0];
    s_last_raw.gyro.y = event->gyro[1];
    s_last_raw.gyro.z = event->gyro[2];
}

static void imu_gpio_irq_handler(enum HAL_GPIO_PIN_T pin)
{
    (void)pin;
    if (s_irq_user_cb) {
        s_irq_user_cb(NULL);
    }
}

void imu_irq_event_callback(void)
{
    /* Reserved for platform vector routing; GPIO path uses imu_gpio_irq_handler. */
}

static int icm42670_apply_streaming_config(void)
{
    int st;

    st = inv_imu_configure_fifo(&s_imu, INV_IMU_FIFO_DISABLED);
    if (st != 0) {
        return BSP_ERROR;
    }

    st = inv_imu_set_accel_fsr(&s_imu, ACCEL_CONFIG0_FS_SEL_4g); /* 4 g */
    st |= inv_imu_set_gyro_fsr(&s_imu, GYRO_CONFIG0_FS_SEL_2000dps); /* 2000 dps */
    st |= inv_imu_set_accel_frequency(&s_imu, ACCEL_CONFIG0_ODR_100_HZ);
    st |= inv_imu_set_gyro_frequency(&s_imu, GYRO_CONFIG0_ODR_100_HZ);
    st |= inv_imu_enable_accel_low_noise_mode(&s_imu);
    st |= inv_imu_enable_gyro_low_noise_mode(&s_imu);

    return (st == 0) ? BSP_OK : BSP_ERROR;
}

int bsp_imu_init(void)
{
    int rc;
    uint8_t who = 0;
    struct inv_imu_serif serif = {
        .context = &s_i2c_ctx,
        .read_reg = icm_i2c_read_reg,
        .write_reg = icm_i2c_write_reg,
        .max_read = ICM_I2C_MAX_XFER,
        .max_write = ICM_I2C_MAX_XFER,
        .serif_type = UI_I2C,
    };

    memset(&s_last_raw, 0, sizeof(s_last_raw));
    s_i2c_ctx.bus = IMU_I2C_ID;
    s_i2c_ctx.addr_7bit = ICM42670_I2C_ADDR_7BIT;

    rc = inv_imu_init(&s_imu, &serif, imu_sensor_event_cb);
    if (rc != 0) {
        LOGE("inv_imu_init failed: %d", rc);
        return BSP_ERROR;
    }

    rc = inv_imu_get_who_am_i(&s_imu, &who);
    if (rc != 0 || who != ICM42670P_WHOAMI) {
        LOGE("IMU whoami mismatch/exp:0x%02x got:0x%02x rc:%d", ICM42670P_WHOAMI, who, rc);
        return BSP_ERROR;
    }

    if (icm42670_apply_streaming_config() != BSP_OK) {
        LOGE("IMU streaming config failed");
        return BSP_ERROR;
    }
LOG_PBUS("icm42670 init ok!");
    return BSP_OK;
}

int bsp_imu_deinit(void)
{
    inv_imu_disable_gyro(&s_imu);
    inv_imu_disable_accel(&s_imu);
    memset(&s_last_raw, 0, sizeof(s_last_raw));
    return BSP_OK;
}

int bsp_imu_read_six_axis(void *buf, uint32_t n_bytes)
{
    if (buf == NULL || n_bytes < sizeof(imu_raw_data_t)) {
        return BSP_INVALID_PARAMETER;
    }

    (void)inv_imu_get_data_from_registers(&s_imu);
    memcpy(buf, &s_last_raw, sizeof(imu_raw_data_t));
    return (int)sizeof(imu_raw_data_t);
}

int bsp_imu_get_chip_id(void *args)
{
    uint8_t *out = (uint8_t *)args;

    if (out == NULL) {
        return BSP_ERR_POINTER_NULL;
    }
    if (inv_imu_get_who_am_i(&s_imu, out) != 0) {
        return BSP_ERROR;
    }
    return BSP_OK;
}

int bsp_imu_enable_irq(void)
{
    struct HAL_GPIO_IRQ_CFG_T cfg = {
        .irq_enable = true,
        .irq_debounce = false,
        .irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
        .irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING,
        .irq_handler = imu_gpio_irq_handler,
    };
    hal_gpio_setup_irq(IMU_INT_PIN, &cfg);
    return BSP_OK;
}

int bsp_imu_disable_irq(void)
{
    struct HAL_GPIO_IRQ_CFG_T cfg = {
        .irq_enable = false,
        .irq_debounce = false,
        .irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE,
        .irq_polarity = HAL_GPIO_IRQ_POLARITY_HIGH_RISING,
        .irq_handler = imu_gpio_irq_handler,
    };
    hal_gpio_setup_irq(IMU_INT_PIN, &cfg);
    return BSP_OK;
}

int bsp_imu_set_irq_callback(void (*cb)(uint8_t *data))
{
    s_irq_user_cb = cb;
    return BSP_OK;
}

int bsp_imu_get_fifo_data(void *args)
{
    (void)args;
    return BSP_ERROR;
}
