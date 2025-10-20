/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "hal_iomux.h" // IWYU pragma: keep
#include "hal_gpio.h" // IWYU pragma: keep
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_i2c.h" // IWYU pragma: keep
#include "bsp_key_internal.h" // IWYU pragma: keep
#include "bsp_sc09b.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define TOUCH_INT_PIN               HAL_IOMUX_PIN_P0_0
#define TOUCH_KEY_I2C_ID            I2C_MASTER_ID_0
#define TOUCH_KEY_NUM               6
#define TOUCH_KEY_RETRY_COUNT       1
#define TOUCH_KEY_IDLE_COUNT        3
#define TOUCH_I2C_ADDR              0x40  /// SC09B 只有一个固定地址

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static thread_context_t s_key_device_thd  = {0};
static uint8_t s_touch_key[TOUCH_KEY_NUM] = {0};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int sc09b_write_nbyte(i2c_master_data_t *data)
{
    int ret = 0;

    data->dev_address = TOUCH_I2C_ADDR;
    for (uint8_t i = 0; i < TOUCH_KEY_RETRY_COUNT; i++) {
        ret = bsp_i2c_write_byte(TOUCH_KEY_I2C_ID, data);
        if (ret == BSP_OK) {
            break;
        }
        msleep(10);
    }

    return ret;
}

static int sc09b_read_nbyte(i2c_master_data_t *data)
{
    int ret = 0;

    data->dev_address = TOUCH_I2C_ADDR;
    for (uint8_t i = 0; i < TOUCH_KEY_RETRY_COUNT; i++) {
        ret = bsp_i2c_read_byte(TOUCH_KEY_I2C_ID, data);
        if (ret == BSP_OK) {
            break;
        }
        msleep(10);
    }

    return ret;
}

static void sc09b_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    uint8_t msg = 0;
    LOG_PBUS("sc09b pin[%d] irq triggered", pin);
    if (pin == (enum HAL_GPIO_PIN_T)TOUCH_INT_PIN && s_key_device_thd.msg_id) {
        MSG_QUEUE_SEND(s_key_device_thd.msg_id, &msg, 0);
        bsp_irq_msg_send(INT_NUM_TOUCH_KEY);
    }
}

static void sc09b_irq_control(bool enable)
{
    struct HAL_GPIO_IRQ_CFG_T gpio_irq_cfg = {
        enable, 
        0, 
        HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE, 
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, 
        sc09b_irq_callback
    };
    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)TOUCH_INT_PIN, &gpio_irq_cfg);
}

static void sc09b_irq_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        TOUCH_INT_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)TOUCH_INT_PIN,  HAL_GPIO_DIR_IN,  1);
}

static void sc09b_touch_key_scan(void)
{
    uint16_t key_value = 0, key_idle_count = 0;
    i2c_master_data_t key_data = {
        .data = (uint8_t *)&key_value,
        .len  = 2,
        .reg_addr = SC09B_OUTPUT1_REG
    };

    while(1) {
        if (sc09b_read_nbyte(&key_data) == BSP_OK) {
            for (uint8_t i = 0; i < TOUCH_KEY_NUM; i++) {
                s_touch_key[i] = key_value & (1 << (12 - i));
            }

            if (key_value == 0) {
                key_idle_count++;
                if (key_idle_count > TOUCH_KEY_IDLE_COUNT) {
                    break;
                }
            } else {
                key_idle_count = 0;
            }
        }
        msleep(10);
    }
}

static void key_device_thread(void *arg)
{
    int ret = 0;
    uint8_t msg = 0;
    uint32_t timestamp = 0;
    while (1) {
        MSG_QUEUE_RECV(s_key_device_thd.msg_id, &msg, OS_WAIT_FOREVER);
        timestamp = get_timestamp();
        sc09b_touch_key_scan();
        LOG_PBUS("key release time:", get_timestamp_ms(timestamp));
    }
}

static void sc09b_reference_cap_config(void)
{
    uint8_t reg_val = SC09B_CAP_DEFALUT;
    i2c_master_data_t key_data = {
        .data = &reg_val,
        .len  = 1,
        .reg_addr = SC09B_CTRL1_REG 
    };
    sc09b_write_nbyte(&key_data);
}

static void sc09b_chip_init(void)
{
    // bsp_key_sensitivity_set(0, SC09B_SEN_DEFAULT);
    // bsp_key_sensitivity_set(1, SC09B_SEN_DEFAULT);
    // sc09b_reference_cap_config();
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_key_init(void)
{
    sc09b_irq_init();
    bsp_i2c_init(I2C_MASTER_ID_0);
    CREATE_MSG_TYPE_QUEUE(&s_key_device_thd.msg_id, 1, 1);
    RUN_NEW_THREAD(&s_key_device_thd, "key", key_device_thread, RT_PRIO_KEY_DEVICE_THREAD, (KEY_DEVICE_THREAD_STACKSIZE));
    sc09b_irq_control(true);
    sc09b_chip_init();

    return BSP_OK;
}

int bsp_key_deinit(void)
{
    sc09b_irq_control(false);
    bsp_i2c_deinit(I2C_MASTER_ID_0);
    
    return BSP_OK;
}

int bsp_read_key_value(uint8_t key_index)
{
    if (key_index >= TOUCH_KEY_NUM) {
        return 0;
    }

    return s_touch_key[key_index];
}

int bsp_key_sensitivity_set(uint8_t key_index, uint8_t sensitivity)
{
    uint8_t reg_val = sensitivity;
    i2c_master_data_t key_data = {
        .data = &reg_val,
        .len  = 2,
        .reg_addr = key_index == 0 ? SC09B_SEN_REG0 : SC09B_SEN_REG_COM 
    };

    return sc09b_write_nbyte(&key_data);
}