/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_manager_api.h"
#include "ui_manager.h"

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

typedef enum {
    UI_LVGL_MSG_CMD_ANIM_INIT = 1,
    UI_LVGL_MSG_CMD_ANIM_DEINIT,
    UI_LVGL_MSG_CMD_OVERLAY_INIT,
    UI_LVGL_MSG_CMD_OVERLAY_DEINIT,
    UI_LVGL_MSG_CMD_PM_SUSPEND,
    UI_LVGL_MSG_CMD_PM_RESUME,
} ui_lvgl_msg_cmd_t;

typedef struct {
    uint8_t cmd;
    uint8_t reserved[3];
    ui_play_info_t play_info;
} ui_lvgl_msg_t;

typedef struct {
    uint16_t img_id;
    uint8_t is_need_alpha;
} ui_snap_shot_info_t;


/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_manager_context_t *ui_ctx = NULL;

static res_img_info_t snap_shot_info = {0};
const static ui_snap_shot_info_t snap_shot_infos[UI_SNAP_SHOT_ID_MAX] = {
};

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
    // EXCEPTION_CHECK_VOID(thiz && thiz->priv.panel_dev);
    // uint32_t tmp[1] = {level};
    // DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_SET_BL, &tmp);
    // thiz->priv.ui_backlight_val = level;
    // LOG_UI("set lcd backlight:%d", level);
}

static void set_backlight_level(ui_manager_context_t *thiz, uint16_t level)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // thiz->priv.ui_backlight_onoff = level > 0 ? true : false;
    // set_backlight_level_internal(thiz, level);
}

static void set_backlight_onoff(ui_manager_context_t *thiz, bool onoff)
{
//     EXCEPTION_CHECK_VOID(thiz);
//     thiz->priv.ui_backlight_onoff = onoff;
//     LOG_UI("lcd backlight:%d", onoff);
// #if (IS_UI_BACKLIGHT_ADAPTIVE == 0)
//     set_backlight_level_internal(thiz, onoff ? LCD_BACKLIGHT_LEVEL_MAX : 0);
// #endif
}

#if IS_UI_BACKLIGHT_ADAPTIVE
static void ui_backlight_adaptive_thread(void *user_data)
{
    ui_manager_context_t *thiz = (ui_manager_context_t *)user_data;
    EXCEPTION_CHECK_VOID(thiz);
    if (thiz->priv.ui_backlight_onoff == false) {
        return;
    }
    set_backlight_level_internal(thiz, thiz->priv.ui_backlight_adaptive_val);
}
#endif

// static void lvgl_display_handler(struct ui_manager_context *thiz)
// {
// }



static void reset_play_count_cb(ui_animation_play_t *play)
{
    play->played_count = 0;
}

static void ui_animation_play_init(struct ui_manager_context *thiz, ui_play_info_t play_info)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_ANIM_INIT;
    // msg.play_info = play_info; /* copy params to UI thread */
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_play_deinit(struct ui_manager_context *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_ANIM_DEINIT;
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_overlay_init(struct ui_manager_context *thiz, ui_play_info_t play_info)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_OVERLAY_INIT;
    // msg.play_info = play_info; /* copy params to UI thread */
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_overlay_deinit(struct ui_manager_context *thiz)
{
    // EXCEPTION_CHECK_VOID(thiz);
    // ui_lvgl_msg_t msg = {0};
    // msg.cmd = UI_LVGL_MSG_CMD_OVERLAY_DEINIT;
    // MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}


static void ui_thread_init_route(struct ui_manager_context *thiz)
{
    // CREATE_MSG_TYPE_QUEUE(&thiz->priv.ui_hdr_thd.msg_id, UI_LVGL_MSG_QUEUE_NUM, sizeof(ui_lvgl_msg_t));
    // thiz->priv.lvgl_initialized = true;

    LOG_UI("==>lvgl init done");
}

static void lcd_ready_callback(int type, int status)
{
    // EXCEPTION_CHECK_VOID(ui_ctx);
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
}

static int on_post_ui_init_route(struct ui_manager_context *ctx)
{
    EXCEPTION_CHECK_INT(ctx);
// #if IS_UI_BACKLIGHT_ADAPTIVE
//     ctx->priv.ui_backlight_adaptive_val = UI_BL_ON_LEVEL_MIN;
//     misc_manager_context_t *misc = (misc_manager_context_t *)find_module(ID_MODULE_MISC);
//     EXCEPTION_CHECK_INT(misc);
//     misc->add_misc_task(ctx, MISC_TASK_ID_UI_BACKLIGHT_ADAPTIVE, 100, MISC_WORK_MSG_ID_TIMER,
//         ui_backlight_adaptive_thread);
// #endif
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
