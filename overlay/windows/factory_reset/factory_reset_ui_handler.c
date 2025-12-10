/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "window_screen_factory_reset.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define FACTORY_RESET_ANIMATION_FREQ                 (12)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
/// 泵奶首页动画资源ID.
static const uint16_t s_factory_reset_screen_res_array[1] = {
    UI_IMG_ID_UNLOCK_TIP,
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

static void factory_reset_ui_finished_policy(ui_manager_context_t *ui)
{

}

static void factory_reset_ui_before_policy(ui_manager_context_t *ui, int index)
{

}

static void factory_reset_ui_after_policy(ui_manager_context_t *ui, int index)
{

}

static void factory_reset_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    static int ani_interval = 0;
    ui_manager_context_t *ui = user_data;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            factory_reset_ui_finished_policy(ui);
            break;
        case ANI_TIMER_DRAW_BEFORE:
            factory_reset_ui_before_policy(ui, index);
            break;
        case ANI_TIMER_DRAW_AFTER:
            factory_reset_ui_after_policy(ui, index);
            break;
        default:
            break;
    }
}

static void show_factory_reset_ui(ui_proc_factory_reset_t *proc)
{
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_init);
    ui_play_info_t play_info = {
        .tag = "factory_reset_screen",
        .fps = FACTORY_RESET_ANIMATION_FREQ,
        .res_array = (uint16_t *)s_factory_reset_screen_res_array,
        .res_count = LENGTH_OF_ARRAY(s_factory_reset_screen_res_array),
        .progress_cb = factory_reset_animation_progress_cb,
        .user_data = proc,
    };
    ui->hook->ui_animation_play_init(ui, play_info);
    // proc->last_progress_ani_time = get_timestamp();
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void start_factory_reset_ui_animation(ui_proc_factory_reset_t *proc)
{
    power_manager_context_t *pm = proc->wm->priv.pm;
    EXCEPTION_CHECK_VOID(pm);
    while (get_timestamp_ms(0) < BOOT_LONG_PRESS_TIME) {
        if (pm->is_system_power_on()) {
            break;
        }
        msleep(10);
    }
    LOG_UI("start factory reset ui animation.");
    show_factory_reset_ui(proc);
}

void stop_factory_reset_ui_animation(ui_proc_factory_reset_t *proc)
{
    ui_manager_context_t *ui = proc->ui;
    EXCEPTION_CHECK_VOID(ui->hook && ui->hook->ui_animation_play_deinit);
    ui->hook->ui_animation_play_deinit(ui);
    ui->animation_play_status_update(ui, false);
}