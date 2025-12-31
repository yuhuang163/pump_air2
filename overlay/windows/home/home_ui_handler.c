/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ui_page_define.h"  // IWYU pragma: keep
#include "window_screen_home.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define HOME_DISPLAY_TASK_NAME              "home_display_cmp"

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
static void home_display_completed_cb(hdr_post_args_t *args)
{
    ui_proc_home_t *proc = (ui_proc_home_t *)args->user_data;
    EXCEPTION_CHECK_VOID(proc);
    LOG_UI("logo play finish.\n");
    window_manager_context_t *wm = proc->wm;
    EXCEPTION_CHECK_VOID(wm);
    storage_manager_context_t *st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    EXCEPTION_CHECK_VOID(st);
    uint8_t airplane_mode_state = 0;
    if (wm->is_current_window(wm, ID_WINDOW_HOME) == 0) {
        return;
    }
    multi_func_msg_data_t msg = {0};
    msg.union_body.extra.window_id = ID_WINDOW_DUMMY;
    wm->dispatch_new_window_args(wm, &msg);
    return;
}

static void home_ui_before_policy(ui_manager_context_t *ui, int index)
{

}

static void home_ui_after_policy(ui_manager_context_t *ui, int index)
{
    ui_display_home_process(ui);
    ui_proc_home_t *proc = (ui_proc_home_t *)ui->priv.play_info.user_data;
    EXCEPTION_CHECK_VOID(proc);
    if (get_timestamp_ms(proc->last_progress_ani_time) >= HOME_ANIMATION_DISPLAY_TIME) {
        POST_TASK_HANDLER_NO_DELAY(home_display_completed_cb, proc, HOME_DISPLAY_TASK_NAME);
    }
}

static void home_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    static int ani_interval = 0;
    ui_manager_context_t *ui = user_data;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            break;
        case ANI_TIMER_DRAW_BEFORE:
            home_ui_before_policy(ui, index);
            break;
        case ANI_TIMER_DRAW_AFTER:
            home_ui_after_policy(ui, index);
            break;
        default:
            break;
    }
}

static void home_ui_display_prepare(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_home_prepare(ui);
}

static void home_ui_display_remove(void *user_data)
{
    ui_manager_context_t *ui = user_data;
    EXCEPTION_CHECK_VOID(ui);
    ui_display_home_remove(ui);
}

static void home_ui_hdr_remove(void)
{
    REMOVE_TASK_HANDLER_DIRECTLY(HOME_DISPLAY_TASK_NAME);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void start_home_ui_animation(ui_proc_home_t *proc)
{    
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_init);
    ui_play_info_t play_info = {
        .tag = "home_screen",
        .fps = HOME_ANIMATION_FREQ,
        .progress_cb = home_animation_progress_cb,
        .display_prepare_cb = home_ui_display_prepare,
        .display_remove_cb = home_ui_display_remove,
        .user_data = proc,
    };
    ui->hook->ui_animation_play_init(ui, play_info);
    proc->last_progress_ani_time = get_timestamp();

}

void stop_home_ui_animation(ui_proc_home_t *proc)
{
    home_ui_hdr_remove();
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_deinit);
    ui->hook->ui_animation_play_deinit(ui);
    ui->animation_play_status_update(ui, false);
    proc->last_progress_ani_time = get_timestamp();
}