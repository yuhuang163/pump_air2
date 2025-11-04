/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "input_key_code.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" 
#include "windows_map_define.h" // IWYU pragma: keep
#include "window_screen_logo.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_STATIC_NAME                            logo

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
static int entry_event_cb(window_lift_cycle_e new_state,
    const struct window_context *thiz, const struct window_context *old_scr)
{
    window_manager_context_t *wm = thiz->wm;
    ui_manager_context_t *ui = wm->priv.ui;
    ui_proc_logo_t *proc = CONTAINER_OF(thiz, ui_proc_logo_t, w_ctx);
    if (new_state == ID_SCR_STATE_ON_RESUME) {

    } else if (new_state == ID_SCR_STATE_ON_CREATE) {
        LOG_WM("==>ON_CREATE");
        ui->ui_animation_play_status_update(ui, false); /// 清除动画播放标志
        start_logo_ui_animation(proc);
    }
    return 0;
}

static int exit_event_cb(window_lift_cycle_e new_state,
                         const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_logo_t *proc = CONTAINER_OF(thiz, ui_proc_logo_t, w_ctx);
    switch (new_state) {
        case ID_SCR_STATE_ON_DESTORY:
            LOG_WM("exit: u_free: %p", proc);
            u_free(proc);
            SET_WINDOW_CONTEXT(NULL);
            LOG_WM("exit--end");
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
    LOG_WM("enter, time: %ld.", get_timestamp());
    ui_proc_logo_t *proc = u_malloz(sizeof(ui_proc_logo_t));
    SET_WINDOW_CONTEXT(proc);
    proc->margic = UI_WINDOW_MARGIC_CODE;
    window_context_t *w_ctx = &proc->w_ctx;
    w_ctx->wm = wm;
    w_ctx->new_window_id = window_id;
    w_ctx->evt_cb_entry = entry_event_cb;
    w_ctx->evt_cb_exit = exit_event_cb;
    w_ctx->not_push_history = 1;
    // args->user_data = &proc->bridge;
    wm->dispatch_new_window(w_ctx);

    /// 配置动画播放

    return ID_WINDOW_DISPLAY_OK;
}

