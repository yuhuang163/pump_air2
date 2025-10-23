/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" 
#include "windows_internal_api.h" // IWYU pragma: keep
#include <stdint.h>

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_STATIC_NAME logo
#define PAGE_ANIMATION_FREQ (12.5f)
#define PAGE_ANIMATION_TIME_MS (uint16_t)((1000.0f / PAGE_ANIMATION_FREQ) + 0.5f)
#define PAGE_ANIMATION_CNT UI_ANIM(LOGO, COMMON, CNT)

typedef struct
{
    uint32_t ani_timer_cnt;
    window_context_t args;
    // ui_page_handle_t bridge;
} ui_proc_logo_t;

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
INIT_WINDOW_CONTEXT();

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
    window_context_t *args = user_data;

    // switch (state) {
    //     case ANI_TIMER_DRAW_BEFORE:
    //     break;

    //     case ANI_TIMER_DRAW_AFTER:
    //     break;

    //     case ANI_TIMER_DRAW_FINISH:
    //     local_work_completed(args->wm);
    //     break;

    //     default:
    //     break;
    // }
}

static int exit_event_cb(window_lift_cycle_e new_state,
                         const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_logo_t *proc = CONTAINER_OF(thiz, ui_proc_logo_t, args);
    switch (new_state) {

        case ID_SCR_STATE_ON_DESTORY:
            window_dispatch_lock("logo-exit");
            LOG_UI("ON_DESTORY: u_free: %p", proc);
            u_free(proc);
            SET_WINDOW_CONTEXT(NULL);
            LOG_UI("ON_DESTORY--end");
            window_dispatch_unlock("logo-exit");
            break;
        default:
            break;
    }

    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
window_enter_result_e PAGE_HANDLE_NAME(PAGE_STATIC_NAME)(window_manager_context_t *wm, uint8_t window_id)
{
    LOG_UI("enter, time: %ld.", get_timestamp());
    ui_proc_logo_t *proc = u_malloz(sizeof(ui_proc_logo_t));
    SET_WINDOW_CONTEXT(proc);
    window_context_t *args = &proc->args;
    args->wm = wm;
    args->new_window_id = window_id;
    args->evt_cb_exit = exit_event_cb;
    args->not_push_history = 1;
    // args->user_data = &proc->bridge;
    dispatch_new_window(args);

    /// 配置动画播放

    return ID_WINDOW_DISPLAY_OK;
}

