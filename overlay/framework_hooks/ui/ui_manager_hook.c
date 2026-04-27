/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
// #include "ui_img_define.h" // IWYU pragma: keep
// #include "ui_manager_api.h"
// #include "ui_manager.h"

/*
* DEFINES
*****************************************************************************************
*/
#define UI_ANIMATION_STOP_TIMEOUT               (1000)
#define IS_PRINT_LVGL_MEM_MONITOR               1
#define UI_LVGL_MSG_QUEUE_NUM                   4
#define UI_BL_ON_LEVEL_MIN                      (20 * 100) /// 开灯最低亮度
#define UI_BL_ON_LEVEL_RANGE                    (50 * 100) /// 开灯调节范围大小 20 - 50

#if IS_SUPPORT_ADAPTIVE_DIMMING
#define IS_UI_BACKLIGHT_ADAPTIVE                (1)
#else
#define IS_UI_BACKLIGHT_ADAPTIVE                (0)
#endif

typedef struct {
    uint16_t img_id;
    uint8_t is_need_alpha;
} ui_snap_shot_info_t;


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_manager_context_t *ui_ctx = NULL;


/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void reset_play_count_cb(ui_animation_play_t *play);

static void set_backlight_level_internal(ui_manager_context_t *thiz, uint16_t level)
{

}

static void set_backlight_level(ui_manager_context_t *thiz, uint16_t level)
{

}

static void set_backlight_onoff(ui_manager_context_t *thiz, bool onoff)
{

}

static void ui_backlight_adaptive_thread(void *user_data)
{

}

static void set_backlight_adaptive_level(ui_manager_context_t *thiz, uint16_t level)
{

}

static void lvgl_mem_monitor_cb(void)
{

}

static void ui_animation_play_init_on_ui_thread(struct ui_manager_context *thiz, 
    const ui_play_info_t *play_info)
{

}

static void ui_animation_play_deinit_on_ui_thread(struct ui_manager_context *thiz)
{

}

static void ui_animation_overlay_init_on_ui_thread(struct ui_manager_context *thiz,
    const ui_play_info_t *play_info)
{

}

static void ui_animation_overlay_deinit_on_ui_thread(struct ui_manager_context *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    // LOG_UI("==> overlay deinit tag: %s", thiz->priv.overlay_play_info.tag);

    // ui_animation_play_t *ani = &thiz->priv.overlay_play_info;
    // thiz->play_animation_lock(ani->tag);
    // thiz->priv.is_playing_ui_overlay = false;
    // thiz->play_animation_unlock(ani->tag);
    // if (ani->display_remove_cb) {
    //     lv_lock();
    //     ani->display_remove_cb(thiz);
    //     lv_unlock();
    // }

    // thiz->play_animation_lock(NULL);
    // memset(ani, 0, sizeof(ui_animation_play_t));
    // thiz->play_animation_unlock(NULL);
}

static void lvgl_display_handler(struct ui_manager_context *thiz)
{
//     ui_lvgl_msg_t msg;
//     uint32_t ms_till_next = 1;
//     uint16_t log_count = 0;

//     while (1) {
//         const bool timer_running = (thiz->priv.lv_timer != NULL) &&
//                                    (lv_timer_get_paused(thiz->priv.lv_timer) == false);
//         if (timer_running) {
//             ms_till_next = lv_timer_handler();
//             if (ms_till_next > 10000) {
//                 LOGE("time error!!!!!!!: %lu", (unsigned long)ms_till_next);
//                 ms_till_next = 200;
//             }

//             if (ms_till_next == 0) {
//                 ms_till_next = 1;
//             }

//             log_count++;
//             if (log_count % 200 == 0) {
//                 LOG_UI("lvgl scheduler:%d", log_count);
// #if IS_PRINT_LVGL_MEM_MONITOR
//                 lvgl_mem_monitor_cb();
// #endif
//             }
//         }

//         os_status_e ret = MSG_QUEUE_RECV(thiz->priv.ui_hdr_thd.msg_id, &msg,
//                                          timer_running ? ms_till_next : OS_WAIT_FOREVER);
//         if (ret != 0) {
//             continue;
//         }
//         switch ((ui_lvgl_msg_cmd_t)msg.cmd) {
//             case UI_LVGL_MSG_CMD_ANIM_INIT:
//                 ui_animation_play_init_on_ui_thread(thiz, &msg.play_info);
//                 break;
//             case UI_LVGL_MSG_CMD_ANIM_DEINIT:
//                 ui_animation_play_deinit_on_ui_thread(thiz);
//                 break;
//             case UI_LVGL_MSG_CMD_OVERLAY_INIT:
//                 ui_animation_overlay_init_on_ui_thread(thiz, &msg.play_info);
//                 break;
//             case UI_LVGL_MSG_CMD_OVERLAY_DEINIT:
//                 ui_animation_overlay_deinit_on_ui_thread(thiz);
//                 break;
//             case UI_LVGL_MSG_CMD_PM_SUSPEND:
//                 break;
//             default:
//                 break;
//         }
//     }
}


static void reset_play_count_cb(ui_animation_play_t *play)
{
    play->played_count = 0;
}

static void ui_animation_play_init(struct ui_manager_context *thiz, ui_play_info_t play_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_ANIM_INIT;
    // msg.play_info = play_info; /* copy params to UI thread */
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_play_deinit(struct ui_manager_context *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_ANIM_DEINIT;
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_overlay_init(struct ui_manager_context *thiz, ui_play_info_t play_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_OVERLAY_INIT;
    // msg.play_info = play_info; /* copy params to UI thread */
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_overlay_deinit(struct ui_manager_context *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_OVERLAY_DEINIT;
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_snap_shot_perpare(struct ui_manager_context *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);

    // if (snap_shot_obj == NULL || !lv_obj_is_valid(snap_shot_obj)) {
    //     snap_shot_obj = lv_image_create(lv_screen_active());
    //     EXCEPTION_CHECK_VOID(snap_shot_obj);
    //     lv_obj_align(snap_shot_obj, LV_ALIGN_CENTER, 0, 0);
    //     lv_obj_set_style_image_opa(snap_shot_obj, LV_OPA_COVER, 0);
    //     lv_obj_add_flag(snap_shot_obj, LV_OBJ_FLAG_HIDDEN); /// 隐藏
    // }
    
    // for(uint8_t i = 0; i < UI_SNAP_SHOT_ID_MAX; i++) {
    //     common_ui_img_resource_init(thiz, snap_shot_infos[i].img_id, &snap_shot_info);
    //     EXCEPTION_CHECK_VOID(snap_shot_info.data);
    //     lv_fs_path_ex_t background_path;
    //     lv_fs_make_path_from_buffer(&background_path, LV_FS_MEMFS_LETTER, snap_shot_info.data,
    //             snap_shot_info.size, "png");
    //     lv_image_set_src(snap_shot_obj, (const void *)&background_path);
    //     lv_refr_now(NULL); /// 等待渲染完成后再创建快照
    //     thiz->priv.lv_snap_shot[i] = lv_snapshot_take(snap_shot_obj, 
    //         snap_shot_infos[i].is_need_alpha ? LV_COLOR_FORMAT_NATIVE_WITH_ALPHA : LV_COLOR_FORMAT_NATIVE);
    // }
    // common_ui_img_resource_deinit(thiz, &snap_shot_info);
    // LVGL_OBJ_DELETE(snap_shot_obj);
}

static void ui_thread_init_route(struct ui_manager_context *thiz)
{
    // CREATE_MSG_TYPE_QUEUE(&thiz->priv.ui_hdr_thd.msg_id, UI_LVGL_MSG_QUEUE_NUM, sizeof(ui_lvgl_msg_t));
    // thiz->priv.lvgl_initialized = true;
    // ui_snap_shot_perpare(thiz);
    // LOG_UI("==>lvgl init done");
}

static void lcd_ready_callback(int type, int status)
{
    EXCEPTION_CHECK_VOID(ui_ctx);
    // if (status) { 
    //     lv_refr_now(NULL);
    // }
}

static void on_ui_broadcast_receiver_route(broadcast_mask_id_t id, void *context, int arg1,
    int arg2, broadcast_priv_data_t *pdata)
{
    // ui_manager_context_t *ui = context;
    // EXCEPTION_CHECK_VOID(ui);
    // LOG_UI("broadcast id: %d, arg1: %d, arg2: %d", id.code, arg1, arg2);
    // switch (id.code) {
    //     case ID_BROADCAST_SYS_STATE_CHANGE:
    //         if (arg1 == SYSTEM_BROADCAST_POWER_OFF) {
    //             set_backlight_onoff(ui, false);
    //         } else if (arg1 == SYSTEM_BROADCAST_POWER_ON) {
    //             set_backlight_onoff(ui, true);
    //         }
    //         break;
    //     case ID_BROADCAST_PM_STATE_CHANGE: 
    //         if (arg1 == PM_BROADCAST_SYS_SUSPEND) {
    //             set_backlight_onoff(ui, false);
    //             ui_lvgl_msg_t msg = {0};
    //             msg.cmd = UI_LVGL_MSG_CMD_PM_SUSPEND;
    //             MSG_QUEUE_SEND(ui->priv.ui_hdr_thd.msg_id, &msg, 0);
    //         } else if (arg1 == PM_BROADCAST_SYS_RESUME) {
    //             set_backlight_onoff(ui, true);
    //         }
    //         break;
    //     default:
    //         break;
    // }
    // ui_manager_context_t *ui = context;
    // EXCEPTION_CHECK_VOID(ui);
    // LOG_UI("broadcast id: %d, arg1: %d, arg2: %d", id.code, arg1, arg2);
    // switch (id.code) {
    //     case ID_BROADCAST_SYS_STATE_CHANGE:
    //         if (arg1 == SYSTEM_BROADCAST_POWER_OFF) {
    //             set_backlight_onoff(ui, false);
    //         } else if (arg1 == SYSTEM_BROADCAST_POWER_ON) {
    //             set_backlight_onoff(ui, true);
    //         }
    //         break;
    //     case ID_BROADCAST_PM_STATE_CHANGE: 
    //         if (arg1 == PM_BROADCAST_SYS_SUSPEND) {
    //             set_backlight_onoff(ui, false);
    //             ui_lvgl_msg_t msg = {0};
    //             msg.cmd = UI_LVGL_MSG_CMD_PM_SUSPEND;
    //             MSG_QUEUE_SEND(ui->priv.ui_hdr_thd.msg_id, &msg, 0);
    //         } else if (arg1 == PM_BROADCAST_SYS_RESUME) {
    //             set_backlight_onoff(ui, true);
    //         }
    //         break;
    //     default:
    //         break;
    // }
}

static int on_post_ui_init_route(struct ui_manager_context *ctx)
{
    EXCEPTION_CHECK_INT(ctx);

    return 0;
}

static int on_perpare_ui_init_route(struct ui_manager_context *thiz)
{    
    // thiz->priv.lvgl_initialized = false;
    // if (thiz->priv.panel_dev) {
    //     DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_ADD_READY_CB,
    //         lcd_ready_callback);
    // }
    // ui_ctx = thiz;
    // thiz->priv.lvgl_initialized = false;
    // if (thiz->priv.panel_dev) {
    //     DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_ADD_READY_CB,
    //         lcd_ready_callback);
    // }
    // ui_ctx = thiz;
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_ui_manager_hook(ui_manager_context_t *ctx)
{
    // ctx->hook = u_malloz(sizeof(struct __ui_manager_hook));
    // ctx->hook->on_post_init_route = on_post_ui_init_route;
    // ctx->hook->on_perpare_init_route = on_perpare_ui_init_route;
    // ctx->hook->ui_animation_play_init = ui_animation_play_init;
    // ctx->hook->ui_animation_play_deinit = ui_animation_play_deinit;
    // ctx->hook->ui_animation_overlay_init = ui_animation_overlay_init;
    // ctx->hook->ui_animation_overlay_deinit = ui_animation_overlay_deinit;
    // ctx->hook->on_ui_broadcast_receiver_route = on_ui_broadcast_receiver_route;
    // ctx->hook->ui_thread_init_route = ui_thread_init_route;
    // ctx->hook->lvgl_display_handler = lvgl_display_handler;
    // ctx->hook->set_ui_backlight_onoff = set_backlight_onoff;
    // ctx->hook->set_ui_backlight_level = set_backlight_level;
    // ctx->hook->set_ui_backlight_adaptive_level = set_backlight_adaptive_level;
}
