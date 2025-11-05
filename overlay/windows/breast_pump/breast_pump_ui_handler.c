/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common.h"
#include "ui_img_define.h"
#include "window_screen_pump.h"
#include "breast_pump_manager.h" 

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_ANIMATION_FREQ                 (12)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
/// 泵奶首页动画资源ID.
static const uint16_t s_pump_home_res_array[1] = {
    UI_IMG_ID_HOME_SCREEN,
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void pump_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    static int ani_interval = 0;
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_animation_play_t *ani = &ui->priv.play_info;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            ani_interval++;
            if (ani_interval >= PAGE_ANIMATION_FREQ) {
                ani_interval = 0;
                // if (ani->reset_play_count_cb) {
                //     ani->reset_play_count_cb(ani);
                // }
            }
            break;
        case ANI_TIMER_DRAW_BEFORE:
            break;
        case ANI_TIMER_DRAW_AFTER:
            break;
        default:
            break;
    }
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void start_pump_ui_animation(ui_proc_pump_t *proc)
{
    if (proc->user_pause == 1) {
        LOG_UI("user_pause ignore.");
        return;
    }
    ui_manager_context_t *ui = proc->ui;
    if (ui->hook && ui->hook->ui_animation_play_init) {
        ui_play_info_t play_info = {
            .tag = "pump_home_screen",
            .fps = PAGE_ANIMATION_FREQ,
            .res_array = (uint16_t *)s_pump_home_res_array,
            .res_count = LENGTH_OF_ARRAY(s_pump_home_res_array),
            .progress_cb = pump_animation_progress_cb,
            .user_data = proc,
        };
        ui->hook->ui_animation_play_init(ui, play_info);
    }
}

int switch_pump_common_mode_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, 
                                breast_pump_info_t *pump_info)
{
    proc->info.common_mode++;
    if (proc->info.common_mode > BREAST_PUMP_MIX_MODE) {
        proc->info.common_mode = BREAST_PUMP_STIMULATION_MODE;
    }
    pump->set_breast_pump_mode(proc->info.common_mode);
    LOG_UI("switch pump common mode: %d, mode: %s", proc->info.common_mode, pump->get_breast_pump_mode_name(proc->info.common_mode));
    return 0;
}

int switch_pump_power_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump)
{
    struct storage_manager_context *st = pump->priv.st;
    EXCEPTION_CHECK_INT(st);
    peripheral_bus_context_t *pbus = pump->priv.pbus;
    EXCEPTION_CHECK_INT(pbus);
    struct rt_device_node *led_dev = pbus->find_device(ID_DEVICE_LED);
    EXCEPTION_CHECK_INT(led_dev);
    night_light_info_t night_light_info = {0};
    st->get_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &night_light_info);
    if (night_light_info.adaptive) {
        LOG_UI("night light is adaptive, ignore.");
        return 0;
    }
    night_light_info.onoff = !night_light_info.onoff;
    if (night_light_info.level == 0) {
        night_light_info.level = 5;
    } 
    st->put_store_ram_val(STORE_ID_SYS_NIGHT_LIGHT_INFO, 0, 0, &night_light_info);
    led_status_t led_status = {
        .mode = LED_MODE_LEVEL,
        .un.luminous.level = night_light_info.onoff ? night_light_info.level : 0,
    };
    int args[] = {NIGHT_LIGHT_LED_ID_0, (int)&led_status};
    led_dev->ioctl(led_dev, OP_CODE_NIGHT_LIGHT_STATUS_SET, (void *)args);
    LOG_UI("switch pump power: %d, level: %d", night_light_info.onoff, night_light_info.level);
    return 0;
}

int switch_pump_status_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, int status)
{
    if (status == BREAST_PUMP_PAUSE) {
        LOG_UI("pump status:PAUSE => set_breast_pump_status:CONTINUE");
        pump->set_breast_pump_status(BREAST_PUMP_CONTINUE);
    } else if (status == BREAST_PUMP_IDLE || status == BREAST_PUMP_FINISH) {
        LOG_UI("first enter: ->current_ui_mode: %d", proc->current_ui_mode);
        pump->set_breast_pump_status(BREAST_PUMP_START);
    } else {
        LOG_UI("pump status:%d => set_breast_pump_status:PAUSE", pump->get_breast_pump_status_name(status));
        pump->set_breast_pump_status(BREAST_PUMP_PAUSE);
    }
    return 0;
}

int switch_pump_frequency_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump,
                        breast_pump_info_t *pump_info)
{
    struct storage_manager_context *st = pump->priv.st;
    EXCEPTION_CHECK_INT(st);

    if (!BREAST_PUMP_IS_COMMON_MODE(pump_info->mode)) {
        LOG_UI("pump is not common mode, ignore.");
        return 0;
    }

    pump->get_breast_pump_setting(&proc->setting);
    proc->setting.speed++;
    if (proc->setting.speed > BREAST_PUMP_SPEED_MAX) {
        proc->setting.speed = BREAST_PUMP_SPEED_LOW;
    }
    pump->set_breast_pump_setting(&proc->setting);
    st->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, pump_info->mode, 0, &proc->setting);
    LOG_UI("switch pump frequency: %d, mode: %s", proc->setting.speed,
         pump->get_breast_pump_mode_name(pump_info->mode));
    return 0;
}

int switch_pump_breastfeeding_mode_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump,
                        breast_pump_info_t *pump_info)
{
    proc->info.breastfeeding_mode++;
    if (proc->info.breastfeeding_mode > BREAST_PUMP_LACTATION_START_MODE ||
         proc->info.breastfeeding_mode < BREAST_PUMP_CUSTOM_MODE) {
        proc->info.breastfeeding_mode = BREAST_PUMP_CUSTOM_MODE;
    }
    pump->set_breast_pump_mode(proc->info.breastfeeding_mode);
    LOG_UI("switch pump breastfeeding mode: %d, mode: %s", proc->info.breastfeeding_mode, 
        pump->get_breast_pump_mode_name(proc->info.breastfeeding_mode));
    return 0;
}

int switch_pump_control_type_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump, uint8_t control_type)
{
    if (control_type >= BREAST_PUMP_CONTROL_TYPE_MAX) {
        LOGE("switch pump control type: %d, out of range", control_type);
        return -1;
    }

    struct storage_manager_context *st = pump->priv.st;
    EXCEPTION_CHECK_INT(st);
    if (proc->info.control_type != control_type) {
        proc->info.control_type = control_type;
        st->put_store_ram_val(STORE_ID_PUMP_CONTROL_TYPE, 0, 0, &proc->info.control_type);
    }

    peripheral_bus_context_t *pbus = pump->priv.pbus;
    EXCEPTION_CHECK_INT(pbus);
    rt_device_node_t *led_dev = pbus->find_device(ID_DEVICE_LED);
    EXCEPTION_CHECK_INT(led_dev);
    led_status_t led_status = {0};
    int args[2] = {0, (int)&led_status};

    if (control_type == BREAST_PUMP_CONTROL_TYPE_LEFT) {
        args[0] = PUMP_RIGHT_CONTROL_BL;
        led_status.mode = LED_MODE_LEVEL;
        led_status.un.luminous.level = LED_LEVEL_MAX / 2;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
        args[0] = PUMP_LEFT_CONTROL_BL;
        led_status.mode = LED_MODE_BLINK;
        led_status.un.blink.period = 500;
        led_status.un.blink.continue_time = 3000;
        led_status.un.blink.first_status = LED_STATUS_ON;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
    } else if (control_type == BREAST_PUMP_CONTROL_TYPE_RIGHT) {
        args[0] = PUMP_LEFT_CONTROL_BL;
        led_status.mode = LED_MODE_LEVEL;
        led_status.un.luminous.level = LED_LEVEL_MAX / 2;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
        args[0] = PUMP_RIGHT_CONTROL_BL;
        led_status.mode = LED_MODE_BLINK;
        led_status.un.blink.period = 500;
        led_status.un.blink.continue_time = 3000;
        led_status.un.blink.first_status = LED_STATUS_ON;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
    } else {
        led_status.mode = LED_MODE_LEVEL;
        led_status.un.luminous.level = LED_LEVEL_MAX;
        args[0] = PUMP_RIGHT_CONTROL_BL;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
        args[0] = PUMP_LEFT_CONTROL_BL;
        led_dev->ioctl(led_dev, OP_CODE_LED_STATUS_SET, (void *)args);
    }
    LOG_UI("switch pump control type: %d, led status: %d", control_type, led_status.un.luminous.level);
    return 0;
}

int switch_pump_level_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump,
                    breast_pump_info_t *pump_info)
{
    struct storage_manager_context *st = pump->priv.st;
    EXCEPTION_CHECK_INT(st);
    pump->get_breast_pump_setting(&proc->setting);
    if (proc->info.control_type == BREAST_PUMP_CONTROL_TYPE_LEFT) {
        proc->setting.level[0]++;
    } else if (proc->info.control_type == BREAST_PUMP_CONTROL_TYPE_RIGHT) {
        proc->setting.level[1]++;
    } else if (proc->info.control_type == BREAST_PUMP_CONTROL_TYPE_BILATERAL) {
        for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
            proc->setting.level[i]++;
        }
    }
    pump->set_breast_pump_setting(&proc->setting);
    st->put_store_ram_val(STORE_ID_PUMP_SETTING_INFO, pump_info->mode, 0, &proc->setting);
    for (uint8_t i = 0; i < BREAST_PUMP_NUM; i++) {
        LOG_UI("switch pump %d level: %d, mode: %s", i, proc->setting.level[i],
             pump->get_breast_pump_mode_name(pump_info->mode));
    }   
    return 0;
}   

void show_pump_result_loop(ui_proc_pump_t *proc)
{

}

void show_pump_progress_time_policy(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump)
{
    EXCEPTION_CHECK_VOID(pump);
    int pump_status = pump->get_breast_pump_status_async();
    LOG_UI("pump status: %s", pump->get_breast_pump_status_name(pump_status));
    if (PUMP_IS_GOING(pump_status)) {
        if (proc->pump_result.real_valid_time_sec < (SINGLE_PUMP_TIME_MAX / 1000)) {
            LOG_UI("=>set_breast_pump_status:PAUSE");
            pump->set_breast_pump_status(BREAST_PUMP_PAUSE);
        } else {
            LOG_UI("=>set_breast_pump_status:FINISH");
            pump->set_breast_pump_status(BREAST_PUMP_FINISH);
        }
    }

    if (IS_PUMPTIME_LESS_THEN_30S(&proc->pump_result)) {
        LOG_UI("PUMPTIME < 30s, do not loop show!");
    } else {
        show_pump_result_loop(proc);
    }
}
