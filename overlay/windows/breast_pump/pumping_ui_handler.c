/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common.h"
#include "ui_img_define.h"
#include "ui_page_define.h" 
#include "window_screen_pump.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
/// 泵奶工作UI动画资源ID.
static const uint16_t s_pumping_res_array[1] = {
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
static void pumping_work_ui_finished_policy(ui_manager_context_t *ui)
{
    ui_proc_pump_t *proc = (ui_proc_pump_t *)ui->priv.play_info.user_data;
    EXCEPTION_CHECK_VOID(proc);
    breast_pump_manager_context_t *pump = proc->wm->priv.pump;
    EXCEPTION_CHECK_VOID(pump);
    /// 如果充电状态发生变化，则刷新UI.
    if (proc->charging_status != proc->wm->priv.misc->is_charge_plug_in()) {
        proc->charging_status = proc->wm->priv.misc->is_charge_plug_in();
        LOG_UI("charging status changed: %d", proc->charging_status);
        goto UI_DISPLAY_REFRESH;
    }

    /// 如果上次动画时间小于更新间隔，则不刷新UI.
    if (get_timestamp_ms(proc->last_progress_ani_time) < PUMP_RUNNING_TIME_UPDATE_INTERVAL) {
        return;
    }
    
    /// 如果泵奶停止且电池电量百分比未发生变化，则不刷新UI.
    if (pump->is_on_working() == 0 && proc->battery_percent == proc->wm->priv.misc->get_battery_percent()) {
        return;
    }

UI_DISPLAY_REFRESH: 
    proc->battery_percent = proc->wm->priv.misc->get_battery_percent();
    proc->last_progress_ani_time = get_timestamp();
    if (ui->priv.play_info.reset_play_count_cb) {
        ui->priv.play_info.reset_play_count_cb(&ui->priv.play_info);
    }
}

static void pumping_work_ui_before_policy(ui_manager_context_t *ui, int index)
{

}

static void pumping_work_ui_after_policy(ui_manager_context_t *ui, int index)
{
    ui_display_pump_home_screen(ui);
}

static void pumping_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            pumping_work_ui_finished_policy(ui);
            break;
        case ANI_TIMER_DRAW_BEFORE:
            pumping_work_ui_before_policy(ui, index);
            break;
        case ANI_TIMER_DRAW_AFTER:
            pumping_work_ui_after_policy(ui, index);
            break;
        default:
            break;
    }
}

static void pumping_ui_display_prepare(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_pump_home_screen_prepare(ui);
}

static void pumping_ui_display_remove(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_pump_home_screen_remove(ui);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void show_pump_home_ui(ui_proc_pump_t *proc)
{
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_init);
    ui_play_info_t play_info = {
        .tag = "pumping_screen",
        .fps = PUMP_ANIMATION_FREQ,
        .res_array = (uint16_t *)s_pumping_res_array,
    #if IS_SUPPORT_LVGL
        .res_count = 1000,
    #else 
        .res_count = LENGTH_OF_ARRAY(s_pumping_res_array),
    #endif
        .progress_cb = pumping_animation_progress_cb,
        .display_prepare_cb = pumping_ui_display_prepare,
        .display_remove_cb = pumping_ui_display_remove,
        .user_data = proc,
    };
    EXCEPTION_CHECK_VOID(proc->wm->priv.misc);
    proc->charging_status = proc->wm->priv.misc->is_charge_plug_in();
    ui->hook->ui_animation_play_init(ui, play_info);
    proc->last_progress_ani_time = get_timestamp();
    proc->current_ui_type = PUMP_UI_TYPE_PUMPING;
}
