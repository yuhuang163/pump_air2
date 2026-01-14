/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "adaptive_dimming.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static adaptive_dimming_info_t *adaptive = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void on_light_sensor_data_cb(void *user_data, uint8_t *data, int data_len)
{
    EXCEPTION_CHECK_VOID(adaptive);
    EXCEPTION_CHECK_VOID(user_data);
    sensor_wrapper_data_t *sensor_data = (sensor_wrapper_data_t *)data;
#if IS_SUPPORT_DEVICE_LIGHT_SENSOR
    if (adaptive->capture_data.is_acquire_complete == 0 && 
        adaptive->capture_data.index < ADAPTIVE_DIMMING_DATA_SIZE) {
        adaptive->capture_data.buff[adaptive->capture_data.index] = sensor_data->light_sensor_data.lux_level;
        adaptive->capture_data.index++;
        if (adaptive->capture_data.index >= ADAPTIVE_DIMMING_DATA_SIZE) {
            adaptive->capture_data.is_acquire_complete = 1;
            adaptive->capture_data.index = 0;
        }
    }
#endif
}

static uint8_t adaptive_dimming_duty_calculate(uint16_t lux_avg)
{
    /// TODO: 根据lux_avg计算duty, 需要添加临界区保护，避免来回切换亮度导致闪烁
    return 80;
}

static void adaptive_dimming_polling_thread(void *user_data)
{
    rt_device_node_t *night_light = (rt_device_node_t *)user_data;
    EXCEPTION_CHECK_VOID(night_light);
    EXCEPTION_CHECK_VOID(adaptive);
    if (!adaptive->is_adaptive_dimming) {
        return;
    }

    if (adaptive->capture_data.is_acquire_complete == 0) {
        return;
    }
    uint32_t lux_sum = 0, lux_avg = 0;
    for (int i = 0; i < ADAPTIVE_DIMMING_DATA_SIZE; i++) {
        lux_sum += adaptive->capture_data.buff[i];
    }
    adaptive->lux_level = lux_sum / ADAPTIVE_DIMMING_DATA_SIZE;
    if (adaptive->lux_level == adaptive->last_lux_level) {
        return;
    }
    adaptive->last_lux_level = adaptive->lux_level;
    uint8_t duty = adaptive_dimming_duty_calculate(lux_avg);
    led_status_msg_t msg = {
        .led_index = NIGHT_LIGHT_LED_ID_0,
        .led_status = {
            .mode = LED_MODE_LEVEL,
            .un.luminous.level = duty,
        },
    };
    night_light->ioctl(night_light, OP_CODE_NIGHT_LIGHT_STATUS_SET, (void *)&msg);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void set_adaptive_dimming_state(bool state)
{
    EXCEPTION_CHECK_VOID(adaptive);
    adaptive->is_adaptive_dimming = state;
    storage_manager_context_t *st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_VOID(st);
    night_light_info_t light_info = {0};
    light_info.adaptive = state;
    st->put_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &light_info);
}

bool get_adaptive_dimming_state(void)
{
    EXCEPTION_CHECK_BOOL(adaptive);
    return adaptive->is_adaptive_dimming;
}

void adaptive_dimming_init(void)
{
    adaptive = (adaptive_dimming_info_t *)u_malloz(sizeof(adaptive_dimming_info_t));
    EXCEPTION_CHECK_VOID(adaptive);
    misc_manager_context_t *misc = (misc_manager_context_t *)find_module(ID_MODULE_MISC);
    EXCEPTION_CHECK_VOID(misc);
    storage_manager_context_t *st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_VOID(st);
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_VOID(pbus);
    sensor_manager_context_t *sensor = (sensor_manager_context_t *)find_module(ID_MODULE_SENSOR);
    EXCEPTION_CHECK_VOID(sensor);
    rt_device_node_t *night_light = pbus->find_device(ID_DEVICE_LED);
    night_light_info_t light_info = {0};
    sensor->add_sensor_evt_lisenser(sensor, LIGHT_SENSOR_EVT_POLLING, 1, on_light_sensor_data_cb);
    st->get_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &light_info);
    adaptive->is_adaptive_dimming = light_info.adaptive;
    misc->add_misc_task(night_light, MISC_TASK_ID_ADAPTIVE_DIMMING, 100,
         MISC_WORK_MSG_ID_TIMER, adaptive_dimming_polling_thread);
    LOG_ALGO("adaptive_dimming_state_sync: %d", adaptive->is_adaptive_dimming);
}