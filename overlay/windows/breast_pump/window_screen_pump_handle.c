/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "window_screen_pump.h"

/*
* DEFINES
*****************************************************************************************
*/
#define PAGE_STATIC_NAME                    pump
#define PAGE_ANIMATION_FREQ                 (12.5f)
#define PAGE_ANIMATION_TIME_MS              (uint16_t)((1000.0f / PAGE_ANIMATION_FREQ) + 0.5f)
#define PAGE_ANIMATION_CNT                  UI_ANIM(PUMP, COMMON, CNT)
#define PUMP_UI_KEY_CODE                    (KEY_CODE_POWER |KEY_CODE_START | KEY_CODE_MODE | KEY_CODE_FREQUENCY | \
                                            KEY_CODE_BREASTFEEDING | KEY_CODE_LEFT_CONTROL | KEY_CODE_RIGHT_CONTROL)
#define PUMP_UI_KEY_TYPE                    (KEY_EVENT_CLICK)
#define GOTO_CHARGE_HDR_TASK_NAME           "goto_charge_ui"
#define RESET_PLAY_ANI_FPS_HDR_TASK_NAME    "reset_play_ani_fps"
#define PLAY_ANI_START_HDR_TASK_NAME        "play_ani_start"
#define PUMP_START_HDR_TASK_NAME            "pump_start"
#define PUMP_STATE_CHANGE_HDR_TASK_NAME     "pump_state_change"
#define PUMP_SHOW_ONCE_HDR_TASK_NAME        "pump_show_once"
#define RECOVERY_DEFAULT_ANI_PLAY_HDR_TASK_NAME "recovery_def_ani_play"

#define REMOVE_ALL_HANDER_TASK() \
do { \
    LOG_UI("==>REMOVE_ALL_HANDER_TASK"); \
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
static void local_work_completed(window_manager_context_t *wm)
{
    LOG_UI("pump play finish.\n");
}

static void on_breast_pump_result_cb(void *user_data, breast_pump_result_t *result)
{

}

static const char *ui_state_to_string(int state)
{
    switch (state) {
        case LOOP_SHOW_SWITCH_MODE:
            return "ui:switch_mode";
        case LOOP_SHOW_PUMPING:
            return "ui:brushing";
        default:
            return "ui:unknown";
    }
}

static void pump_ui_common_cb(hdr_post_args_t *args)
{
    ui_proc_pump_t *proc = args->user_data;
    int action = args->arg1;

    if (proc->margic != UI_WINDOW_MARGIC_CODE) {
        LOG_UI("invalid margic, ignore");
        return;
    }
    window_dispatch_lock("pump_ui_hdr");
    switch (action) {
        case HDR_ACTION_START_PUMP: {
            LOG_UI("%s", args->name);
        }
        break;
        
        case HDR_ACTION_CHANGE_PUMP_STATUS: {
            int new_state = args->arg2;
            LOG_UI("%s, new_state: %d", args->name, new_state);
            if (proc->ui_show_state != new_state) {
                proc->ui_show_state = new_state;
            }
        }
        break;
        
        case HDR_ACTION_LOOP_SHOW: {
            /// TODO: 轮播显示
        }
        break;
        
        case HDR_ACTION_SHOW_ONCE: {
            int new_state = args->arg2;
            LOG_UI("%s, new_state: %d", args->name, new_state);
            if (proc->ui_show_state != new_state) {
                proc->ui_show_state = new_state;
            }
        }
        break;

        case HDR_ACTION_SHOW_THEMES_ANI: {
            LOG_UI("%s", args->name);
            /// TODO: 播放主题动画
        }
        break;

        case HDR_ACTION_RECOVER_DEFAULT_ANI: {
            LOG_UI("%s: recovery-play..", args->name);
            start_pump_ui_animation(proc);
        }
        break;

        case HDR_ACTION_PROGRESS_STOP: {
            /// TODO: 停止进度条
        }
        break;

        case HDR_ACTION_GOTO_CHARGE_UI: {
            window_manager_context_t *wm = proc->w_ctx.wm;
            LOG_UI("%s", args->name);
            if (wm->priv.misc && wm->priv.misc->is_charge_plug_in()) {
                LOG_UI("goto charge_ui.");
            } else {
                LOG_UI("not charge plug in.");
            }
            proc->goto_charge_ui_flag = 0;
        }
        break;

        default:
        break;
    }
    window_dispatch_unlock("pump_ui_hdr");
}


static int on_pump_ui_keyevent_hdr(void *user_data, uint16_t keycode, uint16_t keytype)
{
    ui_proc_pump_t *proc = user_data;
    breast_pump_manager_context_t *pump = proc->w_ctx.wm->priv.pump;
    key_input_context_t *key_ctx = proc->w_ctx.wm->priv.key;
    breast_pump_info_t pump_info;
    EXCEPTION_CHECK_INT(pump);
    pump_info.status = pump->get_breast_pump_status();
    pump_info.mode = pump->get_breast_pump_mode();
    pump_info.do_work = pump->is_on_working();
    const char *key_name = NULL;
    if (key_ctx && key_ctx->hook && key_ctx->hook->get_key_code_name) {
        key_name = key_ctx->hook->get_key_code_name(keycode);
    } else {
        key_name = "@unknow";
    }

    LOG_UI("%s, keyevent:%d/%d, pump working: %d.", key_name, keycode, keytype, pump_info.do_work);

    if (proc->goto_charge_ui_flag != 0) {
        LOG_UI("goto charge ui, ignore key event.");
        return 0;
    }

    window_dispatch_lock("pump_ui-keyevent");
    switch (keycode) {
        case KEY_CODE_POWER:
            switch_pump_power_ui(proc, pump);
            break;
        case KEY_CODE_START:
            switch_pump_status_ui(proc, pump, pump_info.status);
            break;
        case KEY_CODE_MODE:
            switch_pump_common_mode_ui(proc, pump, &pump_info);
            break;
        case KEY_CODE_FREQUENCY:
            switch_pump_frequency_ui(proc, pump, &pump_info);
            break;
        case KEY_CODE_BREASTFEEDING:
            switch_pump_breastfeeding_mode_ui(proc, pump, &pump_info);
            break;
        case KEY_CODE_LEFT_CONTROL:
            switch_pump_control_type_ui(proc, pump, 0);
            break;
        case KEY_CODE_RIGHT_CONTROL:
            switch_pump_control_type_ui(proc, pump, 1);
            break;

        default:
            LOG_UI("unknow keycode: %d, ignore.", keycode);
            break;
    }
    window_dispatch_unlock("pump_ui-keyevent");
    LOG_UI("keyevent#EXIT: %d: %d", keycode, keytype);

    return 0;
}

static void on_pump_ui_broadcast_receive_cb(broadcast_mask_id_t id, void *context, int arg1, int arg2,
     broadcast_priv_data_t *pdata)
{
    window_dispatch_lock("pump_ui_bcr_cb");
    if(!GET_WINDOW_CONTEXT()) {
        LOG_UI("window context not present, return.");
        window_dispatch_unlock("pump_ui_bcr_cb");
        return;
    }

    ui_proc_pump_t *proc = (ui_proc_pump_t *)context;
    breast_pump_manager_context_t *pump = proc->w_ctx.wm->priv.pump;
    /// 如果泵奶管理器不存在，忽略.
    if (!pump) {
        LOG_UI("pump is NULL, return.");
        window_dispatch_unlock("pump_ui_bcr_cb");
        return;
    }
    /// 获取泵奶状态.
    int pump_status = pump->get_breast_pump_status();
    int pump_mode = pump->get_breast_pump_mode();
    int pump_working = pump->is_on_working();
    /// 如果用户暂停，忽略.
    if (proc->user_pause != 0) {
        LOG_UI("user_pause ignore.");
        window_dispatch_unlock("pump_ui_bcr_cb");
        return;
    }

    LOG_UI("receive broadcast, code: %lx, code_v2: %lx, flag: %d, status: %d, mode: %ld,\
            working: %d, arg1: %d, arg2: %d.", id.code, id.code_v2, proc->is_mode_changed_by_device,
            pump_status, pump_mode, pump_working, arg1, arg2);

    switch (id.code) {
        case ID_BROADCAST_BREAST_PUMP_MODE_CHANGE: {
            /// 监测APP端切换模式动作，同步手柄端
            if (!proc->is_mode_changed_by_device) {
                /// 泵奶中切换模式，4秒后进入计时界面
                if (pump_working && PUMP_IS_GOING(pump_status)) {
                    REMOVE_ALL_HANDER_TASK();
                    LOG_UI("start pump state change task after %ds", PUMP_MODE_CHANGE_TIMEOUT);
                    POST_TASK_HANDLER_DELAYED_ARG5(pump_ui_common_cb, PUMP_MODE_CHANGE_TIMEOUT,
                            HDR_ACTION_CHANGE_PUMP_STATUS, LOOP_SHOW_PUMPING, proc, PUMP_STATE_CHANGE_HDR_TASK_NAME);
                }
                proc->current_ui_mode = get_ui_pump_mode_index(pump_mode);
                proc->ui_show_state = LOOP_SHOW_SWITCH_MODE;
                proc->is_mode_changed_by_app = true;
                /// 接收到APP事件之后仅刷新进入休眠时间
                proc->w_ctx.wm->priv.pm->pm_wakeup(PM_WAKEUP_REASON_BLE_REQ);
            } else {
                proc->is_mode_changed_by_device = false;
                proc->is_mode_changed_by_app = false;
            }
            break;
        }
        
        case ID_BROADCAST_SYS_PERSIST_INFO_SAVE: {
            init_themes_pump_img_res_id(proc);  
        }
        break;

        default:
        break;
    }
    window_dispatch_unlock("pump_ui_bcr_cb");
}

static int entry_event_cb(window_lift_cycle_e new_state,
    const struct window_context *thiz, const struct window_context *old_scr)
{
    window_manager_context_t *wm = thiz->wm;
    ui_proc_pump_t *proc = CONTAINER_OF(thiz, ui_proc_pump_t, w_ctx);
    struct storage_manager_context *st = wm->priv.st;
    /// 获取泵奶控制类型
    if (st && st->get_store_val) {
        st->get_store_val(STORE_ID_PUMP_CONTROL_TYPE, 0, 0, &proc->info.control_type);
    }

    if (new_state == ID_SCR_STATE_ON_RESUME) {
        LOG_UI("==>ON_RESUME #add pump keyevent handler");
        wm->priv.key->add_keyevent_handler("pump_wm", proc, PUMP_UI_KEY_CODE, PUMP_UI_KEY_TYPE, on_pump_ui_keyevent_hdr);
        if (proc->need_start_pump) {
            proc->need_start_pump = 0;
            POST_TASK_HANDLER_DELAYED_ARG2(pump_ui_common_cb, 100, HDR_ACTION_START_PUMP, proc,
                 PUMP_START_HDR_TASK_NAME);
        }

        if (wm->priv.misc && wm->priv.misc->is_charge_plug_in()) {
            LOG_UI("goto charge ui");
            proc->goto_charge_ui_flag = 1; /// 设置跳转充电UI标志
            POST_TASK_HANDLER_DELAYED_ARG2(pump_ui_common_cb, 1500, HDR_ACTION_GOTO_CHARGE_UI, proc, 
                GOTO_CHARGE_HDR_TASK_NAME);
        }
    } else if (new_state == ID_SCR_STATE_ON_CREATE) {
        LOG_UI("==>ON_CREATE");
        wm->set_is_playing_rt_ani(false); /// 清除动画播放标志
        start_pump_ui_animation(proc);
    }
    return 0;
}

static int exit_event_cb(window_lift_cycle_e new_state,
                         const struct window_context *thiz, const struct window_context *new_scr)
{
    ui_proc_pump_t *proc = CONTAINER_OF(thiz, ui_proc_pump_t, w_ctx);
    window_manager_context_t *wm = proc->w_ctx.wm;
    switch (new_state) {

        case ID_SCR_STATE_ON_PAUSE: {
            wm->set_is_playing_rt_ani(false);
            window_dispatch_lock("pump_ui-ON_PAUSE");
            proc->user_pause = 1;
            window_dispatch_unlock("pump_ui-ON_PAUSE");
            LOG_UI("==>ON_PAUSE# remove_keyevent_handler");
            wm->priv.key->remove_keyevent_handler(on_pump_ui_keyevent_hdr);
            // REMOVE_TASK_HANDLER_DIRECTLY("reset_play_ani_fps");
            // REMOVE_TASK_HANDLER_DIRECTLY("play_ani_start");
            if (proc->goto_charge_ui_flag != 0) {
                proc->goto_charge_ui_flag = 0;
                REMOVE_TASK_HANDLER_DIRECTLY(GOTO_CHARGE_HDR_TASK_NAME);
            }
        }
        break;

        case ID_SCR_STATE_ON_DESTORY:
            LOG_UI("==>ON_DESTORY start");
            draw_buffer_reset(proc);
            /// 被弹出框临时打断，不释放资源.
            if (new_scr->is_popup_window) {
                LOG_UI("popup_window(%d) interrupt, not free instance!", new_scr->new_window_id);
                return 1;
            }
            REMOVE_ALL_HANDER_TASK();
            LOG_UI("==>unregister receiver");
            unregister_broadcast_receiver(&proc->receiver);
            window_dispatch_lock("pump_ui-ON_DESTORY");
            if (wm->priv.pump) {
                int status = wm->priv.pump->get_breast_pump_status();
                if (status > BREAST_PUMP_IDLE && status < BREAST_PUMP_FINISH) {
                    LOG_UI("==>BREAST_PUMP_FINISH");
                    wm->priv.pump->set_breast_pump_status(BREAST_PUMP_FINISH);
                }
                LOG_UI("==>remove breast_pump_result_listener");
                wm->priv.pump->remove_breast_pump_result_listener(on_breast_pump_result_cb);
            }
            /// TODO: 释放图片资源.
            u_free(proc);
            proc = NULL;
            SET_WINDOW_CONTEXT(NULL);
            window_dispatch_unlock("pump_ui-ON_DESTORY");
            LOG_UI("==>ON_DESTORY end");
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
    ui_proc_pump_t *proc = u_malloz(sizeof(ui_proc_pump_t));
    SET_WINDOW_CONTEXT(proc);
    /// 设置窗口标识.
    proc->margic = UI_WINDOW_MARGIC_CODE;
    /// 注册广播监听.
    proc->receiver.context = proc;
    proc->receiver.name = "model_delete_update";
    proc->receiver.id.code |= ID_BROADCAST_BREAST_PUMP_MODE_CHANGE;
    proc->receiver.id.code |= ID_BROADCAST_SYS_PERSIST_INFO_SAVE;
    proc->receiver.receiver_cb = on_pump_ui_broadcast_receive_cb;
    register_broadcast_receiver(&proc->receiver);
    /// 设置泵奶状态.
    if (wm->priv.pump) {
        wm->priv.pump->set_breast_pump_status(BREAST_PUMP_IDLE);
        wm->priv.pump->add_breast_pump_result_listener(proc, on_breast_pump_result_cb);
    }
    /// 设置窗口上下文.
    window_context_t *w_ctx = &proc->w_ctx;
    w_ctx->wm = wm;
    w_ctx->new_window_id = window_id;
    w_ctx->evt_cb_entry = entry_event_cb;
    w_ctx->evt_cb_exit = exit_event_cb;
    w_ctx->not_push_history = 1;
    // args->user_data = &proc->bridge;
    dispatch_new_window(w_ctx);

    /// 配置动画播放

    return ID_WINDOW_DISPLAY_OK;
}

