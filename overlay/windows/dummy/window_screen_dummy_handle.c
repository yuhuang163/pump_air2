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
#define PAGE_NAME_STR                           "dummy"
#define DUMMY_TIME_INTERVAL                     (60000 * 1) /// 1分钟
#define DUMMY_IDLE_POWER_OFF_TIME               (60000 * 5) /// 5分钟
#define DUMMY_NIGHT_LIGHT_ONOFF_HDR_TASK_NAME   "dummy_night_light_onoff"
#define DUMMY_NIGHT_LIGHT_LEVEL_HDR_TASK_NAME   "dummy_night_light_level"
#define DUMMY_SYSTEM_POWER_OFF_HDR_TASK_NAME    "dummy_system_power_off"

#define REMOVE_ALL_HANDER_TASK() \
do { \
    LOG_WM("==>REMOVE_ALL_HANDER_TASK"); \
    REMOVE_TASK_HANDLER_DIRECTLY(DUMMY_NIGHT_LIGHT_ONOFF_HDR_TASK_NAME); \
    REMOVE_TASK_HANDLER_DIRECTLY(DUMMY_NIGHT_LIGHT_LEVEL_HDR_TASK_NAME); \
    REMOVE_TASK_HANDLER_DIRECTLY(DUMMY_SYSTEM_POWER_OFF_HDR_TASK_NAME); \
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
static void system_power_off_cb(hdr_post_args_t *args)
{
    ui_proc_dummy_t *proc = (ui_proc_dummy_t *)args->user_data;
    EXCEPTION_CHECK_VOID(proc);
    uint32_t current_time = platform_get_utc_time();
    if (current_time - proc->enter_dummy_time > DUMMY_IDLE_POWER_OFF_TIME) {
        LOG_WM("system power off");
        power_manager_context_t *pm =  (power_manager_context_t *)find_module(ID_MODULE_POWER);
        pm->system_power_control(false);
        return;
    }
    POST_TASK_HANDLER_DELAYED_ARG1(system_power_off_cb, DUMMY_TIME_INTERVAL, proc, 
        DUMMY_SYSTEM_POWER_OFF_HDR_TASK_NAME);
    LOG_WM("dummy time: %d", current_time - proc->enter_dummy_time);
}

static int wakup_enter_new_window_policy(ui_proc_dummy_t *proc, int wakeup_reason)
{
    window_manager_context_t *wm = proc->wm;
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
    window_manager_context_t *wm = proc->wm;
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
    window_manager_context_t *wm = proc->wm;
    if (keycode == KEY_CODE_POWER && keytype == KEY_EVENT_CLICK) {
        switch_dummy_night_light_onoff_ui(proc);
    }
    return 0;
}

static int entry_event_cb(window_lift_cycle_e new_state,
    const struct window_context *thiz, const struct window_context *old_scr)
{
    ui_proc_dummy_t *proc = CONTAINER_OF(thiz, ui_proc_dummy_t, w_ctx);
    window_manager_context_t *wm = proc->wm;
    ui_manager_context_t *ui = proc->ui;
    if (new_state == ID_SCR_STATE_ON_RESUME) {
        LOG_WM("==>%s window on resume", PAGE_NAME_STR);
        wm->priv.key->add_keyevent_handler("dummy_wm", proc, KEY_CODE_1, KEY_EVENT_CLICK,
             on_dummy_keyevent_hdr);
        proc->enter_dummy_time = platform_get_utc_time();
        POST_TASK_HANDLER_DELAYED_ARG1(system_power_off_cb, DUMMY_TIME_INTERVAL, proc, DUMMY_SYSTEM_POWER_OFF_HDR_TASK_NAME);
    } else if (new_state == ID_SCR_STATE_ON_CREATE) {
        LOG_WM("==>%s window on create", PAGE_NAME_STR);
        if (ui && ui->hook && ui->hook->set_ui_backlight) {
            ui->hook->set_ui_backlight(ui, 0);
            LOG_WM("%s window set backlight off", PAGE_NAME_STR);
        }
    }
    return 0;
}

static int exit_event_cb(window_lift_cycle_e new_state, 
    const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_dummy_t *proc = CONTAINER_OF(thiz, ui_proc_dummy_t, w_ctx);
    window_manager_context_t *wm = proc->wm;
    ui_manager_context_t *ui = proc->ui;
    switch (new_state) {
        case ID_SCR_STATE_ON_PAUSE:
            LOG_WM("==>%s window on pause", PAGE_NAME_STR);
            unregister_broadcast_receiver(&proc->receiver);
            wm->priv.key->remove_keyevent_handler(on_dummy_keyevent_hdr);
            REMOVE_ALL_HANDER_TASK();
        break;
        case ID_SCR_STATE_ON_DESTORY:
            LOG_WM("==>%s window on destory", PAGE_NAME_STR);
            u_free(proc);
            proc = NULL;
            SET_WINDOW_CONTEXT(NULL);
            LOG_WM("%s window on destory--end", PAGE_NAME_STR);
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
    LOG_WM("%s window enter, time: %ld.", PAGE_NAME_STR, get_timestamp());
    ui_proc_dummy_t *proc = u_malloz(sizeof(ui_proc_dummy_t));
    SET_WINDOW_CONTEXT(proc);
    proc->margic = UI_WINDOW_MARGIC_CODE;
    proc->wm = wm;
    proc->ui = (ui_manager_context_t *)find_module(ID_MODULE_UI);
    /// 监听PM状态改变.
    proc->receiver.id.code = ID_BROADCAST_PM_STATE_CHANGE;
    proc->receiver.context = proc;
    proc->receiver.name = "dummy_screen";
    proc->receiver.receiver_cb = on_dummy_window_receive;
    register_broadcast_receiver(&proc->receiver);

    window_context_t *w_ctx = &proc->w_ctx;
    w_ctx->new_window_id = window_id;
    w_ctx->evt_cb_entry = entry_event_cb;
    w_ctx->evt_cb_exit = exit_event_cb;
    w_ctx->not_push_history = 1;
    w_ctx->user_data = NULL;
    wm->dispatch_new_window(w_ctx);

    return ID_WINDOW_DISPLAY_OK;
}



