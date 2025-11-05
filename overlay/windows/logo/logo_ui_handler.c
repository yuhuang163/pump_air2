/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "window_screen_logo.h" // IWYU pragma: keep
#include "ui_img_define.h"

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_ANIMATION_FREQ          (12)

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
/// 泵奶首页动画资源ID.
static const uint16_t s_logo_screen_res_array[1] = {
    UI_IMG_ID_LOGO,
};

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void local_work_completed(ui_manager_context_t *ui)
{
    LOG_UI("logo play finish.\n");
    window_manager_context_t *wm = (window_manager_context_t *)find_module(ID_MODULE_WM);
    EXCEPTION_CHECK_VOID(wm);
    multi_func_msg_data_t msg = {0};
    msg.union_body.extra.window_id = ID_WINDOW_PUMP;
    wm->dispatch_new_window_args(wm, &msg);
}

static void logo_animation_progress_cb(void *user_data, int index, ui_ani_progress_state_e state)
{
    static int ani_interval = 0;
    ui_manager_context_t *ui = user_data;
    switch (state) {
        case ANI_TIMER_DRAW_FINISH:
            local_work_completed(ui);
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
void start_logo_ui_animation(ui_proc_logo_t *proc)
{
    power_manager_context_t *pm = proc->wm->priv.pm;
    EXCEPTION_CHECK_VOID(pm);
    while (get_timestamp_ms(0) < BOOT_LONG_PRESS_TIME) {
        if (pm->is_system_power_on()) {
            break;
        }
        msleep(10);
    }
    LOG_UI("start logo ui animation.");
    ui_manager_context_t *ui = proc->ui;
    if (ui->hook && ui->hook->ui_animation_play_init) {
        ui_play_info_t play_info = {
            .tag = "logo_screen",
            .fps = PAGE_ANIMATION_FREQ,
            .res_array = (uint16_t *)s_logo_screen_res_array,
            .res_count = LENGTH_OF_ARRAY(s_logo_screen_res_array),
            .progress_cb = logo_animation_progress_cb,
            .user_data = proc,
        };
        ui->hook->ui_animation_play_init(ui, play_info);
    }

}
