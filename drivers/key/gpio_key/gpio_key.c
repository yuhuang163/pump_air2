/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_config.h"
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_key_internal.h" // IWYU pragma: keep

#if !IS_SUPPORT_DEVICE_TOUCH_KEY

/*
* DEFINES
*****************************************************************************************
*/
#define GPIO_KEY_INDEX_POWER  (0)
#define GPIO_KEY_INDEX_UP     (1)
#define GPIO_KEY_INDEX_DOWN   (2)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void power_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("power key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_1);
}

static void up_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("up key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_2);
}
static void down_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("down key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_3);
}

static void gpio_key_irq_control(bool enable)
{
    struct HAL_GPIO_IRQ_CFG_T gpio_irq_cfg = {
        enable, 
        0, 
        HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE, 
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, 
        power_key_irq_callback
    };

    hal_gpio_setup_irq(POWER_KEY_PIN, &gpio_irq_cfg);
    gpio_irq_cfg.irq_handler = up_key_irq_callback;
    hal_gpio_setup_irq(UP_KEY_PIN, &gpio_irq_cfg);
    gpio_irq_cfg.irq_handler = down_key_irq_callback;
    hal_gpio_setup_irq(DOWN_KEY_PIN, &gpio_irq_cfg);
}

static void gpio_key_irq_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)POWER_KEY_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(POWER_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
    gpio_map.pin = (enum HAL_IOMUX_PIN_T)UP_KEY_PIN;
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(UP_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
    gpio_map.pin = (enum HAL_IOMUX_PIN_T)DOWN_KEY_PIN;
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(DOWN_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_key_init(void)
{
    /// 初始化电源键
    gpio_key_irq_init();
    gpio_key_irq_control(true);

    return BSP_OK;
}

int bsp_key_deinit(void)
{
    return BSP_OK;
}

int bsp_read_key_value(uint8_t key_index)
{
    if (key_index == GPIO_KEY_INDEX_POWER) {
        return hal_gpio_pin_get_val(POWER_KEY_PIN) == 0 ? 1 : 0;
    }
    if (key_index == GPIO_KEY_INDEX_UP) {
        return hal_gpio_pin_get_val(UP_KEY_PIN) == 0 ? 1 : 0;
    }
    if (key_index == GPIO_KEY_INDEX_DOWN) {
        return hal_gpio_pin_get_val(DOWN_KEY_PIN) == 0 ? 1 : 0;
    }
    return 0;
}

int bsp_read_all_key_value(void)
{
    int key_value = 0;
    key_value |= bsp_read_key_value(GPIO_KEY_INDEX_POWER);
    key_value |= (bsp_read_key_value(GPIO_KEY_INDEX_UP) << 1);
    key_value |= (bsp_read_key_value(GPIO_KEY_INDEX_DOWN) << 2);
    return key_value;
}

int bsp_key_sensitivity_set(uint8_t key_index, uint8_t sensitivity)
{
    (void)key_index;
    (void)sensitivity;
    return BSP_OK;
}

int bsp_touch_is_ready(void *args)
{
    uint8_t *touch_is_ready_status = (uint8_t *)args;
    if (touch_is_ready_status) {
        *touch_is_ready_status = 1;
    }
    return BSP_OK;
}

int bsp_touch_signal_read(void *args)
{
    (void)args;
    return BSP_OK;
}

int bsp_add_touch_key_ota_event_cb(void *args)
{
    (void)args;
    return BSP_OK;
}

int bsp_start_touch_key_update_fw(void)
{
    return BSP_OK;
}

int bsp_get_touch_key_fw_version(void)
{
    return 0;
}

bool bsp_touch_key_is_upgrading(void)
{
    return false;
}

#endif
/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_config.h"
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_internal.h" // IWYU pragma: keep
#include "bsp_key_internal.h" // IWYU pragma: keep

#if !IS_SUPPORT_DEVICE_TOUCH_KEY

/*
* DEFINES
*****************************************************************************************
*/
#define GPIO_KEY_INDEX_POWER  (0)
#define GPIO_KEY_INDEX_UP     (1)
#define GPIO_KEY_INDEX_DOWN   (2)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void power_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("power key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_1);
}

static void up_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("up key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_2);
}
static void down_key_irq_callback(enum HAL_GPIO_PIN_T pin)
{
    // LOG_PBUS("down key pin[%d] irq triggered", pin);
    bsp_irq_msg_send(INT_NUM_KEY_3);
}

static void gpio_key_irq_control(bool enable)
{
    struct HAL_GPIO_IRQ_CFG_T gpio_irq_cfg = {
        enable, 
        0, 
        HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE, 
        HAL_GPIO_IRQ_POLARITY_LOW_FALLING, 
        power_key_irq_callback
    };

    hal_gpio_setup_irq(POWER_KEY_PIN, &gpio_irq_cfg);
    gpio_irq_cfg.irq_handler = up_key_irq_callback;
    hal_gpio_setup_irq(UP_KEY_PIN, &gpio_irq_cfg);
    gpio_irq_cfg.irq_handler = down_key_irq_callback;
    hal_gpio_setup_irq(DOWN_KEY_PIN, &gpio_irq_cfg);
}

static void gpio_key_irq_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)POWER_KEY_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(POWER_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
    gpio_map.pin = (enum HAL_IOMUX_PIN_T)UP_KEY_PIN;
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(UP_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
    gpio_map.pin = (enum HAL_IOMUX_PIN_T)DOWN_KEY_PIN;
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(DOWN_KEY_PIN,  HAL_GPIO_DIR_IN,  1);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_key_init(void)
{
    /// 初始化电源键
    gpio_key_irq_init();
    gpio_key_irq_control(true);

    return BSP_OK;
}

int bsp_key_deinit(void)
{
    return BSP_OK;
}

int bsp_read_key_value(uint8_t key_index)
{
    if (key_index == GPIO_KEY_INDEX_POWER) {
        return hal_gpio_pin_get_val(POWER_KEY_PIN) == 0 ? 1 : 0;
    }
    if (key_index == GPIO_KEY_INDEX_UP) {
        return hal_gpio_pin_get_val(UP_KEY_PIN) == 0 ? 1 : 0;
    }
    if (key_index == GPIO_KEY_INDEX_DOWN) {
        return hal_gpio_pin_get_val(DOWN_KEY_PIN) == 0 ? 1 : 0;
    }
    return 0;
}

int bsp_read_all_key_value(void)
{
    int key_value = 0;
    key_value |= bsp_read_key_value(GPIO_KEY_INDEX_POWER);
    key_value |= (bsp_read_key_value(GPIO_KEY_INDEX_UP) << 1);
    key_value |= (bsp_read_key_value(GPIO_KEY_INDEX_DOWN) << 2);
    return key_value;
}

int bsp_key_sensitivity_set(uint8_t key_index, uint8_t sensitivity)
{
    (void)key_index;
    (void)sensitivity;
    return BSP_OK;
}

int bsp_touch_is_ready(void *args)
{
    uint8_t *touch_is_ready_status = (uint8_t *)args;
    if (touch_is_ready_status) {
        *touch_is_ready_status = 1;
    }
    return BSP_OK;
}

int bsp_touch_signal_read(void *args)
{
    (void)args;
    return BSP_OK;
}

int bsp_add_touch_key_ota_event_cb(void *args)
{
    (void)args;
    return BSP_OK;
}

int bsp_start_touch_key_update_fw(void)
{
    return BSP_OK;
}

int bsp_get_touch_key_fw_version(void)
{
    return 0;
}

bool bsp_touch_key_is_upgrading(void)
{
    return false;
}

#endif