/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "pmu.h"
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
#include "fuel_gauge/chip/cw221x/cw221x.h"


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
static void app_battery_charger_handler(enum PMU_CHARGER_STATUS_T status)
{
    LOG_PBUS("DCIN status=%d", status);

    if (status == PMU_CHARGER_PLUGIN) {
        s_charge_status.plug_in = true; 
    } else if (status == PMU_CHARGER_PLUGOUT) {
        s_charge_status.plug_in = false;
    }
}

static void charge_gpio_init(void)
{
    struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux = {
        (enum HAL_IOMUX_PIN_T)CHARGE_POWER_PIN, 
        HAL_IOMUX_FUNC_AS_GPIO, 
        HAL_IOMUX_PIN_VOLTAGE_VIO, 
        HAL_IOMUX_PIN_NOPULL,
    };
    hal_iomux_init(&pinmux, 1);
    hal_gpio_pin_set_dir(CHARGE_POWER_PIN, HAL_GPIO_DIR_OUT, 0);

    pinmux.pin = (enum HAL_IOMUX_PIN_T)CHARGE_STATUS_PIN;
    hal_iomux_init(&pinmux, 1);
    hal_gpio_pin_set_dir(CHARGE_STATUS_PIN, HAL_GPIO_DIR_IN, 0);
}

static void charge_enable_control(bool enable)
{
    GPIO_PIN_CTRL(CHARGE_POWER_PIN, enable ? 0 : 1);
    is_charging = enable ? true:false;
    LOG_PBUS("charge_enable_control: %d", enable);
}

/// 通过充电状态引脚判断是否充满
static void check_charge_status(void)
{
    static uint8_t charge_full_count = 0;
    if ((hal_gpio_pin_get_val(CHARGE_STATUS_PIN) == 1) && is_charging) {
        if (charge_full_count < (BATT_CHARGE_FULL_DEBOUNCE * 2)) {
            charge_full_count++;
        }
    } else {
        charge_full_count = 0;
    }
    if (charge_full_count == (BATT_CHARGE_FULL_DEBOUNCE * 2)) {
        s_charge_status.charge_full = 1;
    } else {
        s_charge_status.charge_full = 0;
    }
}


/// TODO: 温度检测放到电量计中
static int charge_fg_status_poll_retry(fuel_gauge_status_t *fg)
{
    unsigned int fg_vol_mv = 0;
    unsigned int fg_alert = 0;
    long fg_current_ma = 0;
    int fg_temp_deci_c = 0;
    int fg_soc = 0;
    int fg_cycle = 0;
    int fg_soh = 0;
    const uint8_t max_retry = 5;
    if (fg == NULL) {
        return BSP_ERROR;
    }
    for (uint8_t i = 0; i < max_retry; i++) {
        if (cw221x_get_vol(&fg_vol_mv) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_current(&fg_current_ma) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_temp(&fg_temp_deci_c) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_capacity(&fg_soc) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_cycle_count(&fg_cycle) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_soh(&fg_soh) != 0) {
            msleep(5);
            continue;
        }
        if (cw221x_get_alert(&fg_alert) != 0) {
            msleep(5);
            continue;
        }

        fg->ready = 1;
        fg->voltage_mv = (uint16_t)fg_vol_mv;
        fg->current_ma = (int32_t)fg_current_ma;
        fg->temperature_c = (int16_t)(fg_temp_deci_c / 10);
        fg->soc = (uint8_t)((fg_soc < 0) ? 0 : (fg_soc > 100 ? 100 : fg_soc));
        fg->cycle_count = (uint16_t)((fg_cycle < 0) ? 0 : fg_cycle);
        fg->soh = (uint8_t)((fg_soh < 0) ? 0 : (fg_soh > 100 ? 100 : fg_soh));
        fg->alert = (uint16_t)fg_alert;
        return BSP_OK;
    }
    fg->ready = 0;
    return BSP_ERROR;
}

static int charge_status_poll_by_fg(charge_chip_status_t *status)
{  
    if (status == NULL) {
        return BSP_ERROR;
    }

    if (charge_fg_status_poll_retry(&status->fg) != BSP_OK || !status->fg.ready) {
        return BSP_ERROR;
    }

    status->vbat = status->fg.voltage_mv;
    status->ibat = (status->fg.current_ma > 0) ? (uint16_t)status->fg.current_ma : 0;
    status->temp = (int8_t)status->fg.temperature_c;

    if (status->plug_in && is_charging && status->fg.soc >= 100) {
        status->charge_full = 1;
    } else {
        status->charge_full = 0;
    }
    return BSP_OK;
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
                charge_enable_control(false); /// TODO: 什么时候恢复？？？
                LOG_PBUS("temp abnormal disable charing");
            }
        }
    }
}

static void charge_device_thread(void *arg)
{
    uint8_t msg = 0;
    uint32_t log_timer = 0;
    uint8_t last_plug_in = 0xff;
    uint8_t last_full = 0;
    uint8_t last_temp_abnormal = 0;
    while (1) {
        MSG_QUEUE_RECV(s_charge_device_thd.msg_id, &msg, OS_WAIT_FOREVER);

#if IS_SUPPORT_DEVICE_CHARGE      
        if (s_charge_status.plug_in != last_plug_in) {
            last_plug_in = s_charge_status.plug_in;
            charge_event_internal_cb(last_plug_in ? CHARGE_EVENT_PLUG_IN : CHARGE_EVENT_PLUG_OUT);
        }

        check_charge_status();
        // charge_status_poll_by_fg(&s_charge_status); 
        if (s_charge_status.charge_full && !last_full) {
            charge_event_internal_cb(CHARGE_EVENT_FULL);
        }
        last_full = s_charge_status.charge_full;

#if IS_SUPPORT_CHARGE_CURRENT_ADJUST
#if IS_SUPPORT_DEVICE_FUEL_GAUGE
        int temp = get_charging_temperature();
        s_charge_status.temp = (int8_t)(temp / 10); 
#endif
        charging_temp_policy_handler(s_charge_status.temp);
        uint8_t temp_abnormal = (s_charge_temp_status == CHARGE_TEMP_DISABLE) ? 1 : 0;
        if (temp_abnormal != last_temp_abnormal) {
            last_temp_abnormal = temp_abnormal;
            charge_event_internal_cb(temp_abnormal ? CHARGE_EVENT_TEMPERATURE_ERROR : CHARGE_EVENT_UNCHARGE);
        }
        if (get_timestamp_ms(log_timer) > 2000) {
            log_timer = get_timestamp();
            // BSP_LOG("charge status", "vbus:%d mV, ibus:%d mA, vbat:%d mV, ibat:%d mA, ntc:%.2f KOhm, temp:%d C, status:0x%04X", 
            //          s_charge_status.vbus, s_charge_status.ibus, s_charge_status.vbat, s_charge_status.ibat, 
            //          s_charge_status.ntc_resistance, s_charge_status.temp, s_charge_status.status);
        }
#endif
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
    return s_charge_status.plug_in;
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
    pmu_charger_init();
    pmu_charger_set_irq_handler(app_battery_charger_handler);

#if IS_SUPPORT_PD_CHARGE
    bsp_charge_pd_init();
#endif
#if IS_SUPPORT_DEVICE_CHARGE
    charge_gpio_init();
    bsp_i2c_init(CHARGE_I2C_ID);
    charge_chip_init();
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
        charge_enable_control(true);
    } else if (event == CHARGE_EVENT_PLUG_OUT) {
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

int charge_ship_mode_enter(void)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    return (sgm41519_enter_ship_mode() == BSP_OK) ? 0 : -1;
#else
    return -1;
#endif
}

int charge_ship_mode_exit(void)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    return (sgm41519_exit_ship_mode() == BSP_OK) ? 0 : -1;
#else
    return -1;
#endif
}

int charge_ship_mode_get(void)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    return sgm41519_get_ship_mode();
#else
    return 0;
#endif
}

int charge_boost_set(uint8_t enable)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    return (sgm41519_set_otg_boost_enable(enable) == BSP_OK) ? 0 : -1;
#else
    return -1;
#endif
}

int charge_boost_get(void)
{
#if IS_SUPPORT_CHARGE_IC_SGM41519
    return sgm41519_get_otg_boost_enable();
#else
    return 0;
#endif
}
#endif

