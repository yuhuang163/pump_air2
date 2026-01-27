/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "hal_pwm.h" // IWYU pragma: keep
#include "hal_timer.h" // IWYU pragma: keep
#include "hwtimer_list.h" // IWYU pragma: keep
#include "bsp_common.h"
#include "bsp_config.h" // IWYU pragma: keep
#include "bsp_led_internal.h"

/*
* DEFINES
*****************************************************************************************
*/
#define NIGHT_LIGHT_SIZE                          1
#define IS_SUPPORT_NIGHT_LIGHT_SUSPEND            (0)
#define NIGHT_LIGHT_PERIOD                        1000

typedef struct {
    enum HAL_GPIO_PIN_T pin;
    enum HAL_PWM_ID_T id;
    void (*iomux_set)(void);
    uint16_t freq;
} night_light_cfg_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static led_status_t led_status[NIGHT_LIGHT_SIZE];

#if (V3_HARDWARE_VERSION == V3_2710IMP_HARDWARE_V1)
static const night_light_cfg_t night_light_cfg[NIGHT_LIGHT_SIZE] = {
    {NIGHT_LIGHT_PIN, NIGHT_LIGHT_PWM_ID, NIGHT_LIGHT_PWM_IMUX, 1000},
};
#elif (V3_HARDWARE_VERSION == V3_2710IMC_HARDWARE_V2)
static const night_light_cfg_t night_light_cfg[NIGHT_LIGHT_SIZE] = {
    {NIGHT_LIGHT_PIN, NIGHT_LIGHT_PWM_ID, NIGHT_LIGHT_PWM_IMUX, 1000},
};
#else
#error "Unsupported hardware version"
#endif

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void night_light_level_set(uint8_t index, led_luminous_t luminous)
{
    if (luminous.level == 0) {
        hal_pwm_disable(night_light_cfg[index].id);
        return;
    }
    
    uint8_t duty = luminous.level >= LED_LEVEL_MAX ? LED_LEVEL_MAX : luminous.level;
    struct HAL_PWM_CFG_T pwm_cfg = {
        .freq = night_light_cfg[index].freq,
        .ratio = duty,
        .inv = false,
        .sleep_on = false,
    };
    led_status[index].mode = LED_MODE_LEVEL;
    hal_pwm_enable(night_light_cfg[index].id, &pwm_cfg);
    LOG_PBUS("index: %d, duty: %d", index, duty);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void bsp_night_light_init(void)
{
    for (uint8_t i = 0; i < LENGTH_OF_ARRAY(night_light_cfg); i++) {
        night_light_cfg[i].iomux_set();
    }
}

void bsp_night_light_resume(void)
{
#if IS_SUPPORT_NIGHT_LIGHT_SUSPEND
    bsp_night_light_init();
#endif
}

void bsp_night_light_suspend(void)
{
#if IS_SUPPORT_NIGHT_LIGHT_SUSPEND
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)NIGHT_LIGHT_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE
    };

    for (int i = 0; i < ARRAY_SIZE(night_light_cfg); i++) {
        hal_pwm_disable(night_light_cfg[i].id);
        gpio_map.pin = (enum HAL_IOMUX_PIN_T)night_light_cfg[i].pin;
        hal_iomux_init(&gpio_map, 1);
        hal_gpio_pin_set_dir(night_light_cfg[i].pin, HAL_GPIO_DIR_OUT, 0);
    }
#endif
}

int bsp_night_light_status_set(void *args)
{
    led_status_msg_t *msg = (led_status_msg_t *)args;
    if (msg == NULL) {
        LOGE("msg is NULL");
        return BSP_ERROR;
    }
    
    if (msg->led_index >= NIGHT_LIGHT_SIZE) {
        LOGE("index is out of range: %d", msg->led_index);
        return BSP_ERROR;
    }

    LOG_PBUS("index: %d, mode: %d, level: %d", msg->led_index, msg->led_status.mode, msg->led_status.un.luminous.level);
    if (msg->led_status.mode == LED_MODE_LEVEL) {
        night_light_level_set(msg->led_index, msg->led_status.un.luminous);
    }

    return 0;
}

int bsp_night_light_test(void)
{
    static uint8_t level = 0;
    level++;
    if (level > LED_LEVEL_MAX) {
        level = 0;
    }

    led_status_msg_t night_light_msg = {
        .led_index = 0,
        .led_status = {
            .mode = LED_MODE_LEVEL,
            .un.luminous = {
                .level = level,
            },
        },
    };
    return bsp_night_light_status_set(&night_light_msg);
}