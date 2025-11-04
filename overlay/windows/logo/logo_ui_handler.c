/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "window_screen_logo.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_ANIMATION_FREQ (12.5f)
#define PAGE_ANIMATION_TIME_MS (uint16_t)((1000.0f / PAGE_ANIMATION_FREQ) + 0.5f)
#define PAGE_ANIMATION_CNT UI_ANIM(LOGO, COMMON, CNT)

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
static void local_work_completed(window_manager_context_t *wm)
{
    LOG_UI("logo play finish.\n");
}

static void animation_progress_cb(void *user_data, int index, int state)
{

}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void start_logo_ui_animation(ui_proc_logo_t *proc)
{
    LOG_UI("start_logo_ui_animation");
    /// 开机动画
    /// 临时策略，实际应该等待动画播放完成后跳转吸奶界面/主界面
    window_manager_context_t *wm = proc->w_ctx.wm;
    EXCEPTION_CHECK_VOID(wm);
    multi_func_msg_data_t msg = {0};
    msg.union_body.extra.window_id = ID_WINDOW_PUMP;
    wm->dispatch_new_window_args(wm, &msg);
}
