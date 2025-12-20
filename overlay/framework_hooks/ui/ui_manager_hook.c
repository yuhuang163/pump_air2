/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "cmsis_os_api.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "ui_img_define.h" // IWYU pragma: keep
#include "ui_manager_api.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_manager_context_t *ui_ctx = NULL;
#if IS_SUPPORT_LVGL
static os_message_id ui_hdr_msg_id = NULL;
#endif
/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void lvgl_display_handler(struct ui_manager_context *thiz)
{
#if IS_SUPPORT_LVGL
    uint32_t ms_till_next = lv_timer_handler();
    if (ms_till_next > 10000) {
        ms_till_next = 200;
    }
    msleep(ms_till_next);
#else
    uint8_t msg = 0;
    MSG_QUEUE_RECV(thiz->priv.ui_hdr_thd.msg_id, &msg, OS_WAIT_FOREVER);
    thiz->play_animation_lock(NULL);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    if (ani->played_count >= ani->res_count) {
        thiz->priv.is_playing_ui_animation = false;
        if (ani->progress_cb) {
            ani->progress_cb(thiz, ani->played_count, ANI_TIMER_DRAW_FINISH);
        }
        goto HANDLER_EXIT;
    }
    ani->progress_cb(thiz, ani->played_count, ANI_TIMER_DRAW_BEFORE);
    ani->progress_cb(thiz, ani->played_count, ANI_TIMER_DRAW_AFTER);
    ani->played_count++;
HANDLER_EXIT:
    thiz->play_animation_unlock(NULL);
#endif
}

#if IS_SUPPORT_LVGL
static void change_images_cb(lv_timer_t *t)
{
    EXCEPTION_CHECK_VOID(ui_ctx);
    uint8_t msg = 0;
    static uint32_t time_stamp = 0;
    // LOGI("==>enter, %d, count: %d", get_timestamp_ms(time_stamp), ui_ctx->priv.play_info.played_count);
    time_stamp = get_timestamp();
    if (ui_ctx->priv.need_stop_timer) {
        lv_timer_delete(t);
        LOGI("==>timer stopped, lvgl timer deleted %d", ui_ctx->priv.need_stop_timer);
        ui_ctx->priv.need_stop_timer = 0;
        ui_ctx->priv.lv_timer = NULL;
        MSG_QUEUE_SEND(ui_hdr_msg_id, &msg, 0);
        return;
    }
    ui_ctx->play_animation_lock(NULL);
    ui_animation_play_t *ani = &ui_ctx->priv.play_info;
    if (ani->played_count >= ani->res_count) {
        ui_ctx->priv.is_playing_ui_animation = false;
        if (ani->progress_cb) {
            ani->progress_cb(ui_ctx, ani->played_count, ANI_TIMER_DRAW_FINISH);
        }
        ui_ctx->play_animation_unlock(NULL);
        return;
    }
    ani->progress_cb(ui_ctx, ani->played_count, ANI_TIMER_DRAW_BEFORE);
    ani->progress_cb(ui_ctx, ani->played_count, ANI_TIMER_DRAW_AFTER);
    ani->played_count++;
HANDLER_EXIT:
    ui_ctx->play_animation_unlock(NULL);
}
#else
static void lv_tick_timer_handler(void *args)
{
   uint8_t msg = 0;
   ui_manager_context_t *ui_ctx = (ui_manager_context_t *)args;
   MSG_QUEUE_SEND(ui_ctx->priv.ui_hdr_thd.msg_id, &msg, 0);
}
#endif

static void reset_play_count_cb(ui_animation_play_t *play)
{
    play->played_count = 0;
}

static void ui_animation_play_init(struct ui_manager_context *thiz, ui_play_info_t play_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->play_animation_lock(play_info.tag);
    ui_animation_play_t *ani = &thiz->priv.play_info;
    ani->played_count = 0;
    ani->user_data = play_info.user_data;
    ani->res_array = (uint16_t *)play_info.res_array;
    ani->res_count = play_info.res_count;
    ani->progress_cb = play_info.progress_cb;
    ani->tag = (char *)play_info.tag;
    ani->reset_play_count_cb = reset_play_count_cb;
    ani->display_prepare_cb = play_info.display_prepare_cb;
    ani->display_remove_cb = play_info.display_remove_cb;
    if (ani->display_prepare_cb) {
        ani->display_prepare_cb(thiz);
    }
#if IS_SUPPORT_LVGL
    uint32_t period = 1000 / play_info.fps;
    LOGI("%s, period: %d", play_info.tag, period);
    lv_lock();
    if (thiz->priv.lv_timer == NULL) {
        thiz->priv.lv_timer = lv_timer_create(change_images_cb, period < 30 ? 30 : period , NULL);
        lv_timer_ready(thiz->priv.lv_timer);
    } else {
        LOGW("==>timer already created");
        lv_timer_set_period(thiz->priv.lv_timer, period < 30 ? 30 : period);
    }
    lv_unlock();
#endif
    thiz->play_animation_unlock(play_info.tag);
}

static void ui_animation_play_deinit(struct ui_manager_context *thiz)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->play_animation_lock(NULL);
#if IS_SUPPORT_LVGL
    uint8_t msg = 0;
    ui_ctx->priv.need_stop_timer = 1;
    int ret = MSG_QUEUE_RECV(ui_hdr_msg_id, &msg, 1000);
    if (ret != 0) {
        LOGE("==>stop lvgl timer timeout!!!!!!");
    }
#endif
    ui_animation_play_t *ani = &thiz->priv.play_info;
    if (ani->display_remove_cb) {
        ani->display_remove_cb(thiz);
    }
    memset(ani, 0, sizeof(ui_animation_play_t));
    LOGI("ui_animation_play_deinit");
    thiz->play_animation_unlock(NULL);
}

#if IS_SUPPORT_LVGL
static void set_black_background_cb(hdr_post_args_t *args)
{
    uint8_t msg = 0;
    ui_manager_context_t *thiz = (ui_manager_context_t *)args->user_data;
    EXCEPTION_CHECK_VOID(thiz);
    LOGI("==>set black background");
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), LV_PART_MAIN);
    lv_refr_now(NULL);
    LOGI("==>set black background done");
}
#endif

static void ui_thread_init_route(struct ui_manager_context *thiz)
{
#if IS_SUPPORT_LVGL
    lv_init();
    lv_tick_set_cb(get_timestamp);
    lv_delay_set_cb(msleep);
    lv_hal_disp_init();
    CREATE_MSG_TYPE_QUEUE(&ui_hdr_msg_id, 1, sizeof(uint8_t));
    
    /// 标记LVGL初始化完成，防止其他线程在初始化未完成时使用LVGL
    thiz->priv.lvgl_initialized = true;
    LOGI("==>lvgl init done, initialized flag set to true");
    
    POST_TASK_HANDLER_DELAYED_ARG1(set_black_background_cb, 100, thiz, "set_black_background_cb");
#else
    CREATE_MSG_TYPE_QUEUE(&thiz->priv.ui_hdr_thd.msg_id, DISP_DISPATCH_EVT_MESQ_NUM, sizeof(uint8_t));
    CREATE_TIMER(&thiz->priv.lv_timer, "lv_timer", thiz, OS_TIMER_PERIODIC, lv_tick_timer_handler);
    START_TIMER(thiz->priv.lv_timer, LVGL_TIME_TICK_PERIOD);
#endif
}

static void on_ui_broadcast_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, broadcast_priv_data_t *pdata)
{
    ui_manager_context_t *ui = context;
    EXCEPTION_CHECK_VOID(ui);
    LOG_UI("broadcast id: %d, arg1: %d, arg2: %d", id.code, arg1, arg2);
    switch (id.code) {
        case ID_BROADCAST_PM_WAKEUP_BY_USER:
            if (ui->priv.ui_backlight_val != 100) {
                ui->hook->set_ui_backlight(ui, 100);
            }
            break;
        case ID_BROADCAST_SET_BLACKLIGHT_BY_USER:
            if (ui->priv.ui_backlight_val != 100) {
                ui->hook->set_ui_backlight(ui, 100);
            }
            break;
        case ID_BROADCAST_SYS_POWER_STATE_CHANGE:
            if (arg1 == ID_BROADCAST_SYS_POWER_OFF) {
                ui->hook->set_ui_backlight(ui, 0);
            } else if (arg1 == ID_BROADCAST_SYS_POWER_ON) {
                ui->hook->set_ui_backlight(ui, 100);
            }
            break;
        case ID_BROADCAST_PM_STATE_CHANGE: 
            if (arg1 == ID_SYS_PM_SUSPEND) {
                ui->priv.is_playing_ui_animation = false;
                #if IS_SUPPORT_LVGL
                    THREAD_SUSPEND(ui->priv.ui_hdr_thd.tid);
                #else
                    STOP_TIMER(ui->priv.lv_timer);
                #endif
                ui->priv.lv_tick_timer_running = false;
            }
            else {
                #if IS_SUPPORT_LVGL
                    THREAD_RESUME(ui->priv.ui_hdr_thd.tid);
                #else
                    STOP_TIMER(ui->priv.lv_timer);
                    START_TIMER(ui->priv.lv_timer, LVGL_TIME_TICK_PERIOD);
                #endif
                ui->priv.lv_tick_timer_running = true;
            }
            break;

        default:
            break;
    }
}

static bool is_ui_backlight_on(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_BOOL(thiz);
    return thiz->priv.ui_backlight_val > 0 ? true : false;
}

static void set_ui_backlight(ui_manager_context_t *thiz, uint16_t val)
{
    EXCEPTION_CHECK_VOID(thiz);
    power_manager_context_t *pm = (power_manager_context_t *)find_module(ID_MODULE_POWER);
    EXCEPTION_CHECK_VOID(pm);

    if (pm->is_system_power_on() == 0) {
        LOGE("system power off, ignore set ui backlight.");
        return;
    }
    /// 设置面板背光
    if (thiz->priv.panel_dev) {
        DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_SET_BL, &val);
        thiz->priv.ui_backlight_val = val;
    }
    /// 设置LED背光
    if (thiz->priv.led_dev) {
        led_status_msg_t msg = {
            .led_index = PUMP_LED_ALL_ID,
            .led_status = {
                .mode = LED_MODE_LEVEL,
                .un.luminous.level = val > LED_LEVEL_MAX ? LED_LEVEL_MAX : val,
            },
        };
        thiz->priv.led_dev->ioctl(thiz->priv.led_dev, OP_CODE_LED_STATUS_SET, (void *)&msg);
    }
}

static void set_led_blink(ui_manager_context_t *thiz, led_blink_info_t blink_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    led_status_msg_t msg = {
        .led_index = blink_info.led_index,
        .led_status = {
            .mode = LED_MODE_BLINK,
            .un.blink.period = blink_info.period,
            .un.blink.continue_time = blink_info.continue_time,
            .un.blink.first_status = blink_info.first_status,
            .un.blink.finish_status = blink_info.finish_status,
        },
    };
    thiz->priv.led_dev->ioctl(thiz->priv.led_dev, OP_CODE_LED_STATUS_SET, (void *)&msg);
}

static void set_led_level(ui_manager_context_t *thiz, led_level_info_t level_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    led_status_msg_t msg = {
        .led_index = level_info.led_index,
        .led_status = {
            .mode = LED_MODE_LEVEL,
            .un.luminous.level = level_info.level,
        },
    };
    thiz->priv.led_dev->ioctl(thiz->priv.led_dev, OP_CODE_LED_STATUS_SET, (void *)&msg);
}

static void set_led_onoff(ui_manager_context_t *thiz, led_onoff_info_t onoff_info)
{
    EXCEPTION_CHECK_VOID(thiz);
    led_status_msg_t msg = {
        .led_index = onoff_info.led_index,
        .led_status = {
            .mode = LED_MODE_ONOFF,
            .un.onoff.status = onoff_info.onoff,
        },
    };
    thiz->priv.led_dev->ioctl(thiz->priv.led_dev, OP_CODE_LED_STATUS_SET, (void *)&msg);
}

static int on_post_ui_init_route(struct ui_manager_context *ctx)
{
    return 0;
}

static int on_perpare_ui_init_route(struct ui_manager_context *thiz)
{
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    EXCEPTION_CHECK_INT(pbus);
    thiz->priv.led_dev = (struct rt_device_node *)pbus->find_device(ID_DEVICE_LED);
    
    #if IS_SUPPORT_LVGL
    /// 初始化LVGL标志为false，等待ui_thread_init_route完成后设置为true
    thiz->priv.lvgl_initialized = false;
    #endif
    
    ui_ctx = thiz;
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void register_ui_manager_hook(ui_manager_context_t *ctx)
{
    ctx->hook = u_malloz(sizeof(struct __ui_manager_hook));
    ctx->hook->on_post_init_route = on_post_ui_init_route;
    ctx->hook->on_perpare_init_route = on_perpare_ui_init_route;
    ctx->hook->ui_animation_play_init = ui_animation_play_init;
    ctx->hook->ui_animation_play_deinit = ui_animation_play_deinit;
    ctx->hook->ui_thread_init_route = ui_thread_init_route;
    ctx->hook->lvgl_display_handler = lvgl_display_handler;
    ctx->hook->on_ui_broadcast_receiver_route = on_ui_broadcast_receiver_route;
    ctx->hook->set_ui_backlight = set_ui_backlight;
    ctx->hook->is_ui_backlight_on = is_ui_backlight_on;
    ctx->hook->set_led_blink = set_led_blink;
    ctx->hook->set_led_level = set_led_level;
    ctx->hook->set_led_onoff = set_led_onoff;
}
