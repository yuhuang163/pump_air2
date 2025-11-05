/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "input_key_code.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" 
#include "windows_map_define.h" // IWYU pragma: keep
#include "window_screen_dummy.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_STATIC_NAME                        dummy
#define DUMMY_NIGHT_LIGHT_ONOFF_HDR_TASK_NAME   "dummy_night_light_onoff"
#define DUMMY_NIGHT_LIGHT_LEVEL_HDR_TASK_NAME   "dummy_night_light_level"

#define REMOVE_ALL_HANDER_TASK() \
do { \
    LOG_WM("==>REMOVE_ALL_HANDER_TASK"); \
    REMOVE_TASK_HANDLER_DIRECTLY(DUMMY_NIGHT_LIGHT_ONOFF_HDR_TASK_NAME); \
    REMOVE_TASK_HANDLER_DIRECTLY(DUMMY_NIGHT_LIGHT_LEVEL_HDR_TASK_NAME); \
} while (0)
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
static int wakup_enter_new_window_policy(ui_proc_dummy_t *proc, int wakeup_reason)
{
    window_manager_context_t *wm = proc->w_ctx.wm;
    power_manager_context_t *pm =  (power_manager_context_t *)find_module(ID_MODULE_POWER);
    multi_func_msg_data_t msg = {0};
    LOG_WM("goto pump window.");
    msg.union_body.extra.window_id = ID_WINDOW_PUMP;
    wm->dispatch_new_window_args(wm, &msg);
    return 0;
}

static void on_dummy_receive_internal(broadcast_mask_id_t id, void *context,
                                 int pm_state, pm_wakeup_reason_e wakeup_reason, broadcast_priv_data_t *pdata)
{
    ui_proc_dummy_t *proc = context;
    window_manager_context_t *wm = proc->w_ctx.wm;
    if (pm_state != ID_SYS_PM_RESUME) {
        LOG_WM("ignore: %d\n", wakeup_reason);
        return;
    }

    if (proc == NULL) {
        LOG_WM("dummy is destory");
        return;
    }
    LOG_WM("PM_RESUME, wakeup_reason: %d.", wakeup_reason);
    wakup_enter_new_window_policy(proc, wakeup_reason);
}

static void on_dummy_window_receive(broadcast_mask_id_t id, void *context,
                                 int32_t pm_state, int32_t wakeup_reason, broadcast_priv_data_t *pdata)
{
    IS_WINDOW_CONTEXT_PRESENT({
        switch (id.code) {
            case ID_BROADCAST_PM_STATE_CHANGE: {
                LOG_WM("received PM_STATE_CHANGE.");
                on_dummy_receive_internal(id, context, pm_state, (pm_wakeup_reason_e)wakeup_reason, pdata);
            }
            break;
            case ID_BROADCAST_BREAST_PUMP_MODE_CHANGE: {
                LOG_WM("received BREAST_PUMP_MODE_CHANGE.");
                break;
            }
            default:
            break;
        }
    });
}

static int on_dummy_keyevent_hdr(void *user_data, uint16_t keycode, uint16_t keytype)
{
    ui_proc_dummy_t *proc = user_data;
    window_manager_context_t *wm = proc->w_ctx.wm;
    if (keycode == KEY_CODE_POWER && keytype == KEY_EVENT_CLICK) {
        switch_dummy_night_light_onoff_ui(proc);
    }
    return 0;
}

static int entry_event_cb(window_lift_cycle_e new_state,
    const struct window_context *thiz, const struct window_context *old_scr)
{
    window_manager_context_t *wm = thiz->wm;
    ui_manager_context_t *ui = wm->priv.ui;
    ui_proc_dummy_t *proc = CONTAINER_OF(thiz, ui_proc_dummy_t, w_ctx);
    if (new_state == ID_SCR_STATE_ON_RESUME) {
        LOG_WM("==>ON_RESUME #add dummy keyevent handler");
        wm->priv.key->add_keyevent_handler("dummy_wm", proc, KEY_CODE_1, KEY_EVENT_CLICK,
             on_dummy_keyevent_hdr);
    } else if (new_state == ID_SCR_STATE_ON_CREATE) {
        LOG_WM("==>ON_CREATE");
        if (ui && ui->hook && ui->hook->set_ui_backlight) {
            ui->hook->set_ui_backlight(ui, 0);
            LOG_WM("set backlight off");
        }
    }
    return 0;
}

static int exit_event_cb(window_lift_cycle_e new_state, 
    const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_dummy_t *proc = CONTAINER_OF(thiz, ui_proc_dummy_t, w_ctx);

    switch (new_state) {
        case ID_SCR_STATE_ON_PAUSE:
            LOG_WM("ON_PAUSE\n");
            unregister_broadcast_receiver(&proc->receiver);
            REMOVE_ALL_HANDER_TASK();
        break;
        case ID_SCR_STATE_ON_DESTORY:
            LOG_WM("ON_DESTORY--begin\n");
            u_free(proc);
            SET_WINDOW_CONTEXT(NULL);
            LOG_WM("ON_DESTORY--end\n");
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
    ui_manager_context_t *ui = wm->priv.ui;
    ui_proc_dummy_t *proc = u_malloz(sizeof(ui_proc_dummy_t));
    SET_WINDOW_CONTEXT(proc);
    /// 监听PM状态改变.
    LOG_WM("register receiver\n");
    proc->receiver.id.code = ID_BROADCAST_PM_STATE_CHANGE;
    proc->receiver.context = proc;
    proc->receiver.name = "dummy_screen";
    proc->receiver.receiver_cb = on_dummy_window_receive;
    register_broadcast_receiver(&proc->receiver);

    window_context_t *w_ctx = &proc->w_ctx;
    w_ctx->wm = wm;
    w_ctx->new_window_id = window_id;
    w_ctx->evt_cb_entry = entry_event_cb;
    w_ctx->evt_cb_exit = exit_event_cb;
    w_ctx->not_push_history = 1;
    w_ctx->user_data = NULL;
    wm->dispatch_new_window(w_ctx);

    return ID_WINDOW_DISPLAY_OK;
}



