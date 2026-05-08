/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "bsp_common.h" // IWYU pragma: keep
#include "bsp_config.h"
#include "bsp_i2c.h" // IWYU pragma: keep
#include "bsp_charge.h"
#include "bsp_ntc_map.h" // IWYU pragma: keep
#include "bsp_battery_internal.h"
#if IS_SUPPORT_PD_CHARGE
#include "bsp_charge_pd.h"
#endif
#if IS_SUPPORT_CHARGE_IC_SGM41519
#include "chip/sgm41519/sgm41519.h"
#endif


#if IS_SUPPORT_DEVICE_CHARGE 
/*
* DEFINES
*****************************************************************************************
*/
#define IS_SUPPORT_CHARGE_CURRENT_ADJUST    1


#define CHARGE_ABNORMAL_MAX_COUNT           10
#define CHARGE_THREAD_POLL_INTERVAL         100
#define CHARGE_TEMP_POLICY_POLL_INTERVAL    3
/*
* LOCAL VARIABLE DEFINITIONS)
*****************************************************************************************
*/
static uint16_t s_charge_abnormal_count = 0;
static bool s_charge_is_ready = false;
static bool is_charging = false;
static charge_chip_status_t s_charge_status = {0};
static uint8_t s_charge_full_debounce = 0;
static uint8_t s_charge_temp_status = 0;
static uint16_t s_charge_temp_count = 0;
static uint16_t s_charging_current_max = BATT_INPUT_CURRENT_LIMIT;
static uint16_t s_charging_current = 0;
static charge_event_cb s_charge_event_callback = NULL;
static thread_context_t s_charge_device_thd  = {0};
static os_timer_id s_charge_timer_id;
static const char *charge_status_arr[] = {
    "charge error",
    "charge full",
    "charging", 
    "uncharge",
    "no battery",
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void charge_enable_gpio_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux = {
        (enum HAL_IOMUX_PIN_T)CHARGE_POWER_PIN, 
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_NOPULL,
    };
    hal_iomux_init(&pinmux, 1);
    hal_gpio_pin_set_dir(CHARGE_POWER_PIN, HAL_GPIO_DIR_OUT, 0);
}

static void charge_enable_control(bool enable)
{
    // GPIO_PIN_CTRL(CHARGE_POWER_PIN, enable ? 0 : 1);
    // is_charging = enable ? true:false;
    LOG_PBUS("charge_enable_control: %d", enable);
}

static int charge_ntc_temperature_get(float resistance)
{
    int left = 0;
    int right = NTC_TEMP_TABLE_SIZE - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (resistance >= ntc_temp_table[mid].r_min && resistance <= ntc_temp_table[mid].r_max) {
            return ntc_temp_table[mid].temp;
        }
        if (resistance > ntc_temp_table[mid].r_max) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    if (left >= NTC_TEMP_TABLE_SIZE) {
        return ntc_temp_table[NTC_TEMP_TABLE_SIZE - 1].temp + 1;
    }
    return ntc_temp_table[0].temp - 1;
}

static void charge_chip_init(void)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    sgm41519_chip_init();
#endif
}

static void charge_ibat_config(uint16_t current_ma)
{
    s_charging_current = current_ma;
#if IS_SUPPORT_CHARGE_IC_SGM41519
    sgm41519_set_charge_current(current_ma);
#endif
}

static void charging_temp_policy_handler(int8_t temp)
{
    if (temp >= CHARGE_NORMAL_TEMP_MIN && temp <= CHARGE_NORMAL_TEMP_MAX) {
        if (s_charge_temp_status != CHARGE_TEMP_NORMAL) {
            s_charge_temp_status = CHARGE_TEMP_NORMAL;
            s_charge_temp_count = 0;
        }
        s_charge_temp_count++;
        if (s_charge_temp_count > CHARGE_TEMP_POLICY_POLL_INTERVAL) {
            s_charge_temp_count = 0;
            if (s_charging_current != s_charging_current_max) {
                charge_ibat_config(s_charging_current_max);
                LOG_PBUS("normal current update:%dmA", s_charging_current);
            }
        }
    } else if ((temp > CHARGE_DERATED_TEMP_MIN && temp< CHARGE_NORMAL_TEMP_MIN) ||
               (temp> CHARGE_NORMAL_TEMP_MAX && temp < CHARGE_DERATED_TEMP_MAX)) {
        if (s_charge_temp_status != CHARGE_TEMP_DERATED) {
            s_charge_temp_status = CHARGE_TEMP_DERATED;
            s_charge_temp_count = 0;
        }
        s_charge_temp_count++;
        if (s_charge_temp_count > CHARGE_TEMP_POLICY_POLL_INTERVAL) {
            s_charge_temp_count = 0;
            if (s_charging_current != TEMP_ABNORMAL_CHARGING_CURRENT) {
                charge_ibat_config(TEMP_ABNORMAL_CHARGING_CURRENT);
                LOG_PBUS("derated current update:%dmA", s_charging_current);
            }
        }
    } else {
        if (s_charge_temp_status != CHARGE_TEMP_DISABLE) {
            s_charge_temp_status = CHARGE_TEMP_DISABLE;
            s_charge_temp_count = 0;
        }
        s_charge_temp_count++;
        if (s_charge_temp_count > CHARGE_TEMP_POLICY_POLL_INTERVAL) {
            s_charge_temp_count = 0;
            if (is_charging) {
                charge_enable_control(false);
                LOG_PBUS("temp abnormal disable charing");
            }
        }
    }
}

static void charge_device_thread(void *arg)
{
    uint8_t msg = 0;
    uint32_t log_timer = 0;
    uint16_t test_count = 0;
    while (1) {
        MSG_QUEUE_RECV(s_charge_device_thd.msg_id, &msg, OS_WAIT_FOREVER);
#if IS_SUPPORT_PD_CHARGE
        charge_pd_status_poll();
#endif
#if IS_SUPPORT_DEVICE_CHARGE
        charge_status_poll(&s_charge_status);
        s_charge_status.charge_full = (s_charge_status.status & CHARGE_STATUS_BAT_FULL_MSK) > 0 ? true : false;        
        s_charge_status.temp = charge_ntc_temperature_get(s_charge_status.ntc_resistance);
#if IS_SUPPORT_CHARGE_CURRENT_ADJUST
        charging_temp_policy_handler(s_charge_status.temp);
#endif
        if (get_timestamp_ms(log_timer) > 2000) {
            log_timer = get_timestamp();
            BSP_LOG("charge status", "vbus:%d mV, ibus:%d mA, vbat:%d mV, ibat:%d mA, ntc:%.2f KOhm, temp:%d C, status:0x%04X", 
                     s_charge_status.vbus, s_charge_status.ibus, s_charge_status.vbat, s_charge_status.ibat, 
                     s_charge_status.ntc_resistance, s_charge_status.temp, s_charge_status.status);
        }
#endif
    }
}

static void charge_thread_timer_handle(void *arg)
{
    uint8_t msg = 0;
    MSG_QUEUE_SEND(s_charge_device_thd.msg_id, (void *)&msg, 0);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void charge_vbus_init(void)
{
#if IS_SUPPORT_PD_CHARGE
    struct HAL_IOMUX_PIN_FUNCTION_MAP gpio_map = {
        (enum HAL_IOMUX_PIN_T)CHARGE_PD_INT_PIN,  
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_PULLUP_ENABLE,
    };
    hal_iomux_init(&gpio_map, 1);
    hal_gpio_pin_set_dir(CHARGE_PD_INT_PIN,  HAL_GPIO_DIR_IN,  1);
#endif
}

bool vbus_is_plug_in(void)
{
#if IS_SUPPORT_PD_CHARGE
    return hal_gpio_pin_get_val(CHARGE_PD_INT_PIN) == 0 ? true : false;
#else
    return true;
#endif
}

void add_charge_event_callback(charge_event_cb cb)
{
    s_charge_event_callback = cb;
}

int get_charge_state(void) 
{
    if (s_charge_is_ready == false) {
        return CHARGE_STATE_ERROR;
    }

    if (s_charge_status.charge_full && s_charge_status.plug_in) {
        return CHARGE_STATE_FULL;
    } else if (s_charge_status.plug_in) {
        return CHARGE_STATE_CHARGING;
    } else {
        return CHARGE_STATE_UNCHARGE;
    }
}

int get_charging_temperature(void) 
{
    return s_charge_is_ready ? s_charge_status.temp : 26;
}

int get_charge_battery_voltage(void) 
{
    return s_charge_status.vbat;
}

int get_charge_full_debounce(void) 
{
    if (s_charge_status.charge_full && s_charge_status.plug_in) {
        if (s_charge_full_debounce < BATT_CHARGE_FULL_DEBOUNCE) {
            s_charge_full_debounce++;
        }
    } else {
        s_charge_full_debounce = 0;
    }
    return s_charge_full_debounce;
}

int bsp_charge_init(void)
{
#if IS_SUPPORT_PD_CHARGE
    bsp_charge_pd_init();
#endif
#if IS_SUPPORT_DEVICE_CHARGE
    charge_enable_gpio_init();
    bsp_i2c_init(CHARGE_I2C_ID);
    charge_chip_init();
    charge_status_poll(&s_charge_status);
    s_charge_is_ready = true;
#endif
    CREATE_MSG_TYPE_QUEUE(&s_charge_device_thd.msg_id, 1, 1);
    CREATE_TIMER(&s_charge_timer_id, "charge", NULL, OS_TIMER_PERIODIC, charge_thread_timer_handle);
    START_TIMER(s_charge_timer_id, CHARGE_THREAD_POLL_INTERVAL);
    RUN_NEW_THREAD(&s_charge_device_thd, "charge", charge_device_thread, RT_PRIO_CHARGE_DEVICE_THREAD, 
        (CHARGE_DEVICE_THREAD_STACKSIZE));

    return BSP_OK;
}

int bsp_charge_resume(void)
{
    s_charge_is_ready = true;
    s_charge_abnormal_count = 0;
    s_charge_temp_count = 0;
    s_charge_temp_status = CHARGE_TEMP_UNKNOW;
    START_TIMER(s_charge_timer_id, CHARGE_THREAD_POLL_INTERVAL);
    return BSP_OK;
}

int bsp_charge_suspend(void)
{
    s_charge_is_ready = false;
    s_charge_abnormal_count = 0;
    s_charge_temp_count = 0;
    s_charge_temp_status = CHARGE_TEMP_UNKNOW;
    STOP_TIMER(s_charge_timer_id);
    return BSP_OK;
}

int bsp_charge_is_ready(void)
{
    return s_charge_is_ready ? 1 : 0;
}

void charge_event_internal_cb(charge_event_e event)
{
#if IS_SUPPORT_DEVICE_CHARGE  
    if (event == CHARGE_EVENT_PLUG_IN) {
#if IS_SUPPORT_PD_CHARGE
        int current = charge_pd_current_get();
        LOG_PBUS("pd charge imax:%dmA", current);
        if (current > BATT_INPUT_CURRENT_LIMIT) {
            current = BATT_INPUT_CURRENT_LIMIT;
        }
        s_charging_current_max = current;
#if IS_SUPPORT_CHARGE_CURRENT_ADJUST
        charge_ibat_config(current);
#endif
#endif 
        s_charge_status.plug_in = true;      
        charge_enable_control(true);
    } else if (event == CHARGE_EVENT_PLUG_OUT) {
        s_charge_status.plug_in = false;
        charge_enable_control(false);

    }
#endif

    if (s_charge_event_callback) {
        s_charge_event_callback(event);
    } 
}

void charge_current_adjust(uint16_t current)
{
    if (current > 5000) {
        LOGE("charge current adjust out of range, current: %dmA", current);
        return;
    }
#if IS_SUPPORT_DEVICE_CHARGE 
    charge_ibat_config(current);
#endif
}

int bsp_charge_get_ibat_ma(uint16_t *ibat_ma)
{
    if (ibat_ma == NULL) {
        return BSP_ERROR;
    }
    *ibat_ma = s_charge_status.ibat;
    return BSP_OK;
}
#endif

