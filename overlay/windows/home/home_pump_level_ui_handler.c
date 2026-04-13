/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_page_define.h" // IWYU pragma: keep
#include "window_screen_home.h"

/*
* DEFINES
*****************************************************************************************
*/
#define HOME_PUMP_LEVEL_ADJUST_DISPLAY_TASK_NAME                     "home_pump_level_adjust_display_cmp"

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
static void home_pump_level_ui_display_completed_cb(hdr_post_args_t *args)
{
    ui_proc_home_t *proc = (ui_proc_home_t *)args->user_data;
    EXCEPTION_CHECK_VOID(proc);
    WINDOW_ID_GUARD_VOID(proc, ID_WINDOW_HOME);
    if (proc->current_ui_type != HOME_UI_TYPE_LEVEL_ADJUST) {
        return;
    }

    if (proc->control_type == BREAST_PUMP_CONTROL_TYPE_BILATERAL) {
        LOGE("control type is bilateral, ignore.");
        return;
    }

    int time_index = 0;
    READ_PUMP_MODE_STEP_INDEX(proc->pump, time_index);
    uint8_t level_idx = proc->control_type == BREAST_PUMP_CONTROL_TYPE_LEFT ? 0 : 1;
    if (proc->level_change > 0) {
            proc->setting.level[level_idx] = proc->setting.level[level_idx] + proc->level_change;
            if (proc->setting.level[level_idx] >= BREAST_PUMP_LEVEL_MAX) {
                proc->setting.level[level_idx] = BREAST_PUMP_LEVEL_MAX - 1;
            }
        WRITE_PUMP_SETTING_INFO(proc->pump, time_index, true, proc->setting);
    } else if (proc->level_change < 0) {
        uint8_t abs_level_change = abs(proc->level_change);
        proc->setting.level[level_idx] = proc->setting.level[level_idx] < abs_level_change ?
            0 : (proc->setting.level[level_idx] - abs_level_change);
        WRITE_PUMP_SETTING_INFO(proc->pump, time_index, true, proc->setting);
    }
    proc->level_change = 0;
    start_home_ui_animation(proc);
}

static void home_pump_level_ui_after_policy(ui_manager_context_t *ui, int index)
{
    ui_display_home_pump_level_process(ui);
    ui_proc_home_t *proc = (ui_proc_home_t *)ui->priv.play_info.user_data;
    EXCEPTION_CHECK_VOID(proc);
    uint32_t time_diff = get_timestamp_ms(proc->last_progress_ani_time);
    if (time_diff >= HOME_PUMP_LEVEL_DISPLAY_TIMEOUT) {
        proc->last_progress_ani_time = get_timestamp();
        POST_TASK_HANDLER_NO_DELAY(home_pump_level_ui_display_completed_cb, proc, HOME_PUMP_LEVEL_ADJUST_DISPLAY_TASK_NAME);
    }
}

static void home_pump_level_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_animation_play_t *ani = &ui->priv.play_info;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            break;
        case ANI_TIMER_DRAW_BEFORE:
            break;
        case ANI_TIMER_DRAW_AFTER:
            home_pump_level_ui_after_policy(ui, index);
            break;
        default:
            break;
    }
}

static void home_pump_level_ui_prepare(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_home_pump_level_prepare(ui);
}

static void home_pump_level_ui_remove(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_home_pump_level_remove(ui);
}

static void show_home_pump_level_ui(ui_proc_home_t *proc)
{
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_init);
    proc->last_progress_ani_time = get_timestamp();
    if (proc->current_ui_type == HOME_UI_TYPE_LEVEL_ADJUST) {
        return;
    } else {
        home_ui_change_hdr_remove(proc);
    }
    ui_play_info_t play_info = {
        .tag = "home_pump_level_screen",
        .fps = HOME_ANIMATION_FREQ,
        .progress_cb = home_pump_level_animation_progress_cb,
        .display_prepare_cb = home_pump_level_ui_prepare,
        .display_remove_cb = home_pump_level_ui_remove,
        .user_data = proc,
    };
    proc->last_progress_ani_time = get_timestamp();
    proc->current_ui_type = HOME_UI_TYPE_LEVEL_ADJUST;
    ui->hook->ui_animation_play_init(ui, play_info);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void switch_home_pump_level_control_type(ui_proc_home_t *proc, uint8_t control_type)
{
    EXCEPTION_CHECK_VOID(proc );
    proc->level_change = 0;
    if (proc->current_ui_type != HOME_UI_TYPE_LEVEL_ADJUST) {
        int time_index = 0;
        READ_PUMP_MODE_STEP_INDEX(proc->pump, time_index);
        READ_PUMP_SETTING_INFO(proc->pump, time_index, proc->setting);
    } 
    if (proc->control_type != control_type) {
        proc->control_type = control_type;
        // WRITE_PUMP_LEVEL_CONTROL_TYPE(proc->pump, control_type, 1);
    }
    show_home_pump_level_ui(proc);
    LOG_WM("switch home pump level adjust type: %d", control_type);
}

void switch_home_pump_level(ui_proc_home_t *proc, bool is_increase)
{
    EXCEPTION_CHECK_VOID(proc && proc->pump);
    uint8_t level = proc->control_type == BREAST_PUMP_CONTROL_TYPE_LEFT ? \
    proc->setting.level[0] : proc->setting.level[1];
    if (is_increase) {
        if ((proc->level_change + level) < BREAST_PUMP_LEVEL_15) {
            proc->level_change++;
        } 
    } else {
        if ((proc->level_change + level) > BREAST_PUMP_LEVEL_0) {
            proc->level_change--;
        } 
    }
    LOG_WM("switch home pump level: %d, level_change: %d", is_increase, proc->level_change);
    show_home_pump_level_ui(proc);
}

void home_pump_level_ui_hdr_remove(void)
{
    REMOVE_TASK_HANDLER_DIRECTLY(HOME_PUMP_LEVEL_ADJUST_DISPLAY_TASK_NAME);
}