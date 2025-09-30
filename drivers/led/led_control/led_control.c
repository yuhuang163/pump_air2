/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_led.h"
#include "hal_timer.h"
#include "hwtimer_list.h"
#include "bsp_common.h" 
#include "bsp_led_internal.h"
#include <stdint.h>
#include <string.h>

/*
* DEFINES
*****************************************************************************************
*/
#define LED_TOTAL_SIZE               (6)
#define LED_PWM_PERIOD               800 // 800 - 1ms  350 - 0.5ms  120 - 0.25ms
#define LED_THREAD_POLL_INTERVAL     (10)

#define LED1_PIN                     HAL_GPIO_PIN_P1_2  // 启动与暂停背光灯
#define LED2_PIN                     HAL_GPIO_PIN_P1_3  // 模式选择背光灯
#define LED3_PIN                     HAL_GPIO_PIN_P1_4  // 频率选择背光灯
#define LED4_PIN                     HAL_GPIO_PIN_P1_5  // 追奶模式背光灯
#define LED5_PIN                     HAL_GPIO_PIN_P1_6  // 双边控制背光灯
#define LED6_PIN                     HAL_GPIO_PIN_P1_7  // 左边控制背光灯
#define LED7_PIN                     HAL_GPIO_PIN_P4_3  // 右边控制背光灯

#define LED_PIN_CTRL(pin, status)             ((status) ? hal_gpio_pin_set(pin) : hal_gpio_pin_clr(pin))

typedef struct {
    enum HAL_GPIO_PIN_T pin;
} led_cfg_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static HWTIMER_ID hwt = NULL;
static uint8_t led_time_is_running = 0;
static led_status_t led_status[LED_TOTAL_SIZE];
static thread_context_t s_led_device_thd;
static os_timer_id s_led_timer_id;

static const led_cfg_t led_cfg[LED_TOTAL_SIZE] = {
    {LED1_PIN},
    {LED2_PIN},
    {LED3_PIN},
    {LED4_PIN},
    {LED6_PIN},
    {LED7_PIN},
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void led_timer_stop(void)
{
    if (hwt) {
        hwtimer_stop(hwt);
    }
    led_time_is_running = 0;
}

static void led_timer_start(uint32_t period)
{
    uint32_t period_ticks = US_TO_TICKS(period);
    hwtimer_start(hwt, period_ticks);
    led_time_is_running = 1;
}

static void led_timeout_handler(void *param)
{
    uint8_t level_count = 0;

    for (uint8_t i = 0; i < LED_TOTAL_SIZE; i++) {
        if (led_status[i].mode != LED_MODE_LEVEL) {
            continue;
        }
        level_count++;
        led_status[i].un.onoff.status = !led_status[i].un.onoff.status;
        LED_PIN_CTRL(led_cfg[i].pin, led_status[i].un.onoff.status > 0 ? 1 : 0);
    }

    if (level_count > 0) {
        led_timer_start(LED_PWM_PERIOD);
    } else {
        led_time_is_running = 0;
    }
}

static void led_timer_init(void)
{
    if (hwt == NULL) {
        hwt = hwtimer_alloc(led_timeout_handler, 0);
    }
}

static void led_timer_deinit(void)
{
    if (hwt) {
        hwtimer_stop(hwt);
        hwtimer_free(hwt);
        hwt = NULL;
    }
}

static void led_pin_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)LED1_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE
    };

    for (uint8_t i = 0; i < LED_TOTAL_SIZE; i++) {
        gpio_map.pin = (enum HAL_IOMUX_PIN_T)led_cfg[i].pin;
        hal_iomux_init(&gpio_map, 1);
        hal_gpio_pin_set_dir(led_cfg[i].pin, HAL_GPIO_DIR_OUT, 1);
        LED_PIN_CTRL(led_cfg[i].pin, 0);
    }
}

static void led_blink_handle(uint8_t index)
{
    if (led_status[index].mode != LED_MODE_BLINK) {
        return;
    }

    led_blink_t *led = &led_status[index].un.blink;

    /// 闪烁完成
    if (get_timestamp_ms(led->start_time) >= led->continue_time && led->continue_time != 0) {
        if (led->cb) {
            led->cb();
        }
        return;
    }
    
    /// 闪烁计时
    led->count += LED_THREAD_POLL_INTERVAL;
    if (led->count >= led->period) {
        led->status = led->status > 0 ? 0 : 1;
        LED_PIN_CTRL(led_cfg[index].pin, led->status);
        led->count = 0;
    }
}

static void led_thread_timer_handle(void)
{
    uint8_t msg = 0;
    MSG_QUEUE_SEND(s_led_device_thd.msg_id, (void *)&msg, 0);
}

static void led_device_thread(void *arg)
{
    uint8_t msg = 0;
    CREATE_MSG_TYPE_QUEUE(&s_led_device_thd.msg_id, 1, 1);
    CREATE_TIMER(&s_led_timer_id, "led", NULL, OS_TIMER_PERIODIC, led_thread_timer_handle);
    START_TIMER(s_led_timer_id, LED_THREAD_POLL_INTERVAL);
    while (1) {
        MSG_QUEUE_RECV(s_led_device_thd.msg_id, (void *)&msg, OS_WAIT_FOREVER);
        for (uint8_t i = 0; i < LED_TOTAL_SIZE; i++) {
            led_blink_handle(i);
        }
    }
}

static void led_onoff_set(uint8_t index, led_onoff_t onoff)
{
    led_status[index].mode = LED_MODE_ONOFF; 
    LED_PIN_CTRL(led_cfg[index].pin, onoff.status);
}

static void led_level_set(uint8_t index, led_luminous_t luminous)
{
    /// 灭
    if (luminous.level == 0) {
        led_status[index].mode = LED_MODE_ONOFF; 
        LED_PIN_CTRL(led_cfg[index].pin, 0);
        return;
    }

    /// 全亮
    if (luminous.level >= LED_LEVEL_MAX) {
        led_status[index].mode = LED_MODE_ONOFF; 
        LED_PIN_CTRL(led_cfg[index].pin, 1);
        return;
    }

    /// 半亮
    led_status[index].mode = LED_MODE_LEVEL; 
    led_status[index].un.luminous.level = luminous.level;
    if (led_time_is_running == 0) {
        led_timer_start(LED_PWM_PERIOD); 
    }
}

static void led_blink_set(uint8_t index, led_blink_t blink)
{
    led_status[index].mode = LED_MODE_BLINK; 
    memcpy(&led_status[index].un.blink, &blink, sizeof(led_blink_t));
    LED_PIN_CTRL(led_cfg[index].pin, blink.first_status);
    led_status[index].un.blink.start_time = get_timestamp();
    led_status[index].un.blink.status = led_status[index].un.blink.first_status;
    led_status[index].un.blink.count = 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int bsp_led_init(void)
{
    led_timer_init();
    led_pin_init();
    RUN_NEW_THREAD(&s_led_device_thd, "led", led_device_thread, RT_PRIO_LED_DEVICE_THREAD, (LED_DEVICE_THREAD_STACKSIZE));
    return BSP_OK;
}

void bsp_led_resume(void)
{
    led_timer_init();
    START_TIMER(s_led_timer_id, LED_THREAD_POLL_INTERVAL);
}

void bsp_led_suspend(void)
{
    STOP_TIMER(s_led_timer_id);
    led_timer_deinit();
}

int bsp_led_status_set(void *args)
{
    uint32_t *tmp = (uint32_t *)args;
    if (tmp == NULL) {
        return BSP_ERROR;
    }
    
    uint8_t index = tmp[0];
    if (index >= LED_TOTAL_SIZE) {
        return BSP_ERROR;
    }

    led_status_t *led = (led_status_t *)tmp[1];
    if (led == NULL) {
        return BSP_ERROR;
    }

    switch (led->mode) {
        case LED_MODE_ONOFF:
            led_onoff_set(index,  led->un.onoff);
        break;

        case LED_MODE_BLINK:
            led_blink_set(index, led->un.blink);
        break;

        case LED_MODE_LEVEL:
            led_level_set(index, led->un.luminous);
        break;

        case LED_MODE_BREATH:
            LOGE("LED breathing is not supported");
        break;
    }

    return BSP_OK;
}

void *bsp_led_status_get(void *args)
{
    uint32_t *tmp = (uint32_t *)args;
    
    if (!tmp) {
        return NULL;
    }

    if (tmp[0] >= LED_TOTAL_SIZE) {
        return NULL;
    }
    
    return (void *)&led_status[tmp[0]];
}
