/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "windows_manager.h" 
#include "windows_map_define.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_STATIC_NAME                dummy
#define ENTRY_AP_MODE_TIMES             (10)

typedef struct {
    window_context_t args;
    broadcast_obj_t receiver;
    misc_manager_context_t *misc;
    storage_manager_context_t *st;
    #if IS_SUPPORT_MODULE_RES
    resource_manager_context_t *res;
    #endif
} ui_proc_dummy_t;

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
    window_manager_context_t *wm = proc->args.wm;
    power_manager_context_t *pm =  (power_manager_context_t *)find_module(ID_MODULE_POWER);
    multi_func_msg_data_t msg = {0};
    if (!(pm && pm->is_background_wakeup())) {
    }

    int factory_deploy_flag = 0;
    proc->st->get_store_ram_val(STORE_ID_FACTORY_DEPLOY_FLAG, 0, 0, &factory_deploy_flag);
    if (!factory_deploy_flag) {
        LOG_WM("goto fac: true");
        msg.union_body.extra.window_id = ID_WINDOW_FACTORY_MODE;
        wm->dispatch_new_window_args(wm, &msg);
        return 0;
    }

    if (proc->misc) {
        if (proc->misc->show_battery_ui_if_need(wm, false)) {
            LOG_WM("battery,show_ui_if_need: true");
            return 0;
        }
    }

    if (!wm->is_window_guidance_deployed()) {
        LOG_WM("goto wizard: true");
        msg.union_body.extra.window_id = ID_WINDOW_GUIDE;
        msg.union_body.extra.is_background = 1;
        wm->dispatch_new_window_args(wm, &msg);
        return 0;
    }
    return -1;
}

static void on_dummy_receive_internal(broadcast_mask_id_t id, void *context,
                                 int pm_state, pm_wakeup_reason_e wakeup_reason, broadcast_priv_data_t *pdata)
{
    ui_proc_dummy_t *proc = context;
    window_manager_context_t *wm = proc->args.wm;
    multi_func_msg_data_t msg = {0};

    if (pm_state != ID_SYS_PM_RESUME) {
        LOG_WM("ignore: %d\n", wakeup_reason);
        return;
    }

    if (proc == NULL) {
        LOG_WM("dummy is destory");
        return;
    }

    LOG_WM("#PM_RESUME@wakeup_reason: %d\n", wakeup_reason);
    if (wakup_enter_new_window_policy(proc, wakeup_reason) == 0) {
        return;
    }

    msg.union_body.extra.pm_wakeup_reason = wakeup_reason;
    msg.union_body.extra.window_id = ID_WINDOW_PUMP;
    msg.union_body.extra.is_background = 1;
    wm->dispatch_new_window_args(wm, &msg);
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


static int exit_event_cb(window_lift_cycle_e new_state, 
    const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_dummy_t *proc = CONTAINER_OF(thiz, ui_proc_dummy_t, args);

    switch (new_state) {
        case ID_SCR_STATE_ON_PAUSE:
            LOG_WM("ON_PAUSE\n");
            unregister_broadcast_receiver(&proc->receiver);
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
    LOG_WM("set backlight off");
    ui_manager_context_t *ui = wm->priv.ui;
    ui->set_ui_backlight(ui, 0);
    ui_proc_dummy_t *proc = u_malloz(sizeof(ui_proc_dummy_t));
    proc->misc = (misc_manager_context_t *)find_module(ID_MODULE_MISC);
    proc->st = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    #if IS_SUPPORT_MODULE_RES
    proc->res = (resource_manager_context_t *)find_module(ID_MODULE_RES);
    #endif
    SET_WINDOW_CONTEXT(proc);
    /// 监听PM状态改变.
    LOG_WM("register receiver\n");
    proc->receiver.id.code = ID_BROADCAST_PM_STATE_CHANGE;
    proc->receiver.context = proc;
    proc->receiver.name = "dummy_screen";
    proc->receiver.receiver_cb = on_dummy_window_receive;
    register_broadcast_receiver(&proc->receiver);

    window_context_t *w_ctx = &proc->args;
    w_ctx->wm = wm;
    w_ctx->new_window_id = window_id;
    w_ctx->evt_cb_exit = exit_event_cb;
    w_ctx->not_push_history = 1;
    w_ctx->user_data = NULL;
    wm->dispatch_new_window(w_ctx);

    return ID_WINDOW_DISPLAY_OK;
}



