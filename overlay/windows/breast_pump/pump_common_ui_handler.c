/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "window_screen_pump.h"

/*
* DEFINES
*****************************************************************************************
*/

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

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int switch_pump_night_light_ui(ui_proc_pump_t *proc, breast_pump_manager_context_t *pump)
{
    struct storage_manager_context *st = pump->priv.st;
    EXCEPTION_CHECK_INT(st);
    EXCEPTION_CHECK_INT(proc->ui && proc->ui->hook && proc->ui->hook->set_led_level);
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
    proc->ui->hook->set_led_level(proc->ui, NIGHT_LIGHT_LED_ID_0, 
                night_light_info.onoff ? night_light_info.level : 0);
    LOG_UI("switch pump power: %d, level: %d", night_light_info.onoff, night_light_info.level);
    
    return 0;
}

void stop_pump_ui_animation(ui_proc_pump_t *proc)
{
    pump_result_ui_hdr_remove();
    pump_level_ui_hdr_remove();
    pump_speed_ui_hdr_remove();
    pump_mode_ui_hdr_remove();
    breastfeeding_mode_ui_hdr_remove();
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_deinit);
    ui->hook->ui_animation_play_deinit(ui);
    ui->animation_play_status_update(ui, false);
    proc->last_progress_ani_time = get_timestamp();
}