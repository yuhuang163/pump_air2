/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_config.h"
#include "bsp_i2c.h"
#include "bsp_charge.h"
#include "bsp_battery_internal.h"


/*
* DEFINES
*****************************************************************************************
*/
#define CHARGING_STATUS_VALUE               0x02
#define CHARHE_FULL_STATUS_VALUE            0x01
#define UNCHARGE_STATUS_VALUE               0x00 

#define CHARHE_CHECK_NO_BATT_0              0xFF 
#define CHARHE_CHECK_NO_BATT_1              0xFE 
#define CHARGE_I2C_ID                       I2C_MASTER_ID_0
#define CHARGE_I2C_ADDR                     0x75
#define CHARGE_RETRY_COUNT                  3

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static thread_context_t s_charge_device_thd  = {0};
static const char *charge_status_arr[] = {
    "charge error",
    "charge full",
    "charging", 
    "uncharge",
    "no battery",
};

// static uint16_t charge_full_debounce = 0;
// static void (*s_charge_irq_callback)(void *param);

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static int charge_write_nbyte(i2c_master_data_t *data)
{
    int ret = 0;

    data->dev_address = CHARGE_I2C_ADDR;
    for (uint8_t i = 0; i < CHARGE_RETRY_COUNT; i++) {
        ret = bsp_i2c_write_byte(CHARGE_I2C_ID, data);
        if (ret == BSP_OK) {
            break;
        }
        msleep(10);
    }

    return ret;
}

static int charge_read_nbyte(i2c_master_data_t *data)
{
    int ret = 0;

    data->dev_address = CHARGE_I2C_ADDR;
    for (uint8_t i = 0; i < CHARGE_RETRY_COUNT; i++) {
        ret = bsp_i2c_read_byte(CHARGE_I2C_ID, data);
        if (ret == BSP_OK) {
            break;
        }
        msleep(10);
    }

    return ret;
}

static void charge_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    uint8_t msg = 0;
    LOG_PBUS("charge irq pin[%d] triggered", pin);
    if (pin == CHARGE_INT_PIN && s_charge_device_thd.msg_id) {
        MSG_QUEUE_SEND(s_charge_device_thd.msg_id, &msg, 0);
    }
}

static void charge_irq_control(bool enable)
{
    struct HAL_GPIO_IRQ_CFG_T gpio_irq_cfg = {
        enable, 
        0, 
        HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE, 
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, 
        charge_irq_callback
    };

    hal_gpio_setup_irq(CHARGE_INT_PIN, &gpio_irq_cfg);
}

static void charge_irq_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)CHARGE_INT_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(CHARGE_INT_PIN,  HAL_GPIO_DIR_IN,  1);
}

static void charge_device_thread(void *arg)
{
    while (1) {
        msleep(100);
    }
}

static int charge_device_is_ready(void)
{
    uint8_t value = 0;
    i2c_master_data_t data = {0};
    data.reg_addr = CHARGE_ID_REG;
    data.data = &value;
    data.len = 1;
    int ret = charge_read_nbyte(&data);
    LOG_PBUS("charge id: %02X, ret: %d", value, ret);
    if (value == CHARGE_ID_VALUE) {
        return BSP_OK;
    }
    return BSP_ERROR;
}

static int charge_device_interrupt_control(bool enable)
{
    return BSP_OK;
}

static int charge_device_read_status(void)
{
    uint16_t status = 0;
    i2c_master_data_t data = {0};
    data.reg_addr = CHARGE_STATUS0_REG;
    data.data = (uint8_t *)&status;
    data.len = 2;
    int ret = charge_read_nbyte(&data);
    LOG_PBUS("charge status: %04X, ret: %d", status, ret);
    return ret;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
bool vbus_is_plug_in(void)
{
    return false;
}

int charge_device_init(void)
{
    charge_irq_init();
    bsp_i2c_init(CHARGE_I2C_ID);
    CREATE_MSG_TYPE_QUEUE(&s_charge_device_thd.msg_id, 1, 1);
    RUN_NEW_THREAD(&s_charge_device_thd, "charge", charge_device_thread, RT_PRIO_CHARGE_DEVICE_THREAD, 
        (CHARGE_DEVICE_THREAD_STACKSIZE));
    charge_irq_control(true);
    msleep(10);
    return charge_device_is_ready();
}

void add_charge_irq_callback(void (*cb)(void *param))
{
    // s_charge_irq_callback = cb;
}

int get_charge_state(void) 
{
    return 0;
}

int get_charging_temperature(void) 
{
    return 0;
}

int get_charge_full_debounce(void) 
{
    return 0;
}

void charge_irq_event_callback(void)
{

}

