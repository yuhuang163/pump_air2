/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "peripheral_bus_api.h"
#include "root_common.h"
#include "root_common_api.h" 
#include "ui_manager.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static ui_manager_context_t *thiz = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void ui_play_animation_lock(const char *tag)
{
    if (tag) {
        LOG_UI("=>Take: %s.", tag);
    }
    MUTEX_LOCK(thiz->priv.ui_draw_lock);
}

static void ui_play_animation_unlock(const char *tag)
{
    MUTEX_UNLOCK(thiz->priv.ui_draw_lock);
    if (tag) {
        LOG_UI("=>Give: %s.", tag);
    }
}

static void reset_play_count_cb(ui_animation_play_t *thiz)
{
    thiz->played_count = 0;
}

static void ui_animation_play_init(ui_animation_play_t *ani, ui_play_info_t play_info)
{
    ani->played_count = 0;
    ani->user_data = play_info.user_data;
    ani->res_array = (uint16_t *)play_info.res_array;
    ani->res_count = play_info.res_count;
    ani->progress_cb = play_info.progress_cb;
    ani->tag = (char *)play_info.tag;
    ani->reset_play_count_cb = reset_play_count_cb;
}

static int post_msg_to_flush(const char *tag, multi_func_msg_data_t *msg)
{
    EXCEPTION_CHECK_INT(msg);
    return MSG_QUEUE_SEND(thiz->priv.ui_hdr_thd.msg_id, &msg, 0);
}

static void ui_animation_play_status_update(ui_manager_context_t *thiz, bool flag)
{
    EXCEPTION_CHECK_VOID(thiz);
    thiz->priv.is_playing_ui_animation = flag;
}

static bool is_ui_backlight_on(ui_manager_context_t *thiz)
{
    EXCEPTION_CHECK_BOOL(thiz);
    return thiz->priv.ui_backlight_val > 0 ? true : false;
}

static void set_ui_backlight(ui_manager_context_t *thiz, uint16_t val)
{
    EXCEPTION_CHECK_VOID(thiz);
    DEVICE_IOCTL_ARGS_NO_RET(thiz->priv.panel_dev, OP_CODE_PANEL_SET_BL, &val);
}

static void ui_handler_thread(const void *args)
{
    multi_func_msg_data_t msg;
    thread_context_t *thd = (thread_context_t *)args;
    CREATE_MSG_TYPE_QUEUE(&thd->msg_id, DISP_DISPATCH_EVT_MESQ_NUM, sizeof(uint8_t));
    while (1) {
        MSG_QUEUE_RECV(thd->msg_id, &msg, OS_WAIT_FOREVER);
    }
}

static int on_post_ui_init(void *self)
{
    peripheral_bus_context_t *pbus = (peripheral_bus_context_t *)find_module(ID_MODULE_DEVICE_BUS);
    if (pbus) {
        thiz->priv.panel_dev = pbus->find_device(ID_DEVICE_DISPLAY_PANEL);
    }
    /// 调用外部钩子.
    if (thiz->hook->on_post_init_route) {
        thiz->hook->on_post_init_route(thiz);
    }
    RUN_NEW_THREAD(&thiz->priv.ui_hdr_thd, "ui_hdr_thd", 
        ui_handler_thread, RT_PRIO_LVGL_HANDLER_THREAD, (LVGL_RENDER_THREAD_STACKSIZE));
    return 0;
}

static int on_perpare_ui_init(void *self)
{
    thiz = self;
    /// 调用外部钩子.
    if (thiz->hook->on_perpare_init_route) {
        thiz->hook->on_perpare_init_route(thiz);
    }
    return 0;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
/// 显示管理外部注册接口.
void *register_module_for_ui_manager_new(list_head_t *list_head)
{
    MALLOC_FOR_PTR(ptr, ui_manager_context_t);
    MUTEX_INIT(&ptr->priv.ui_draw_lock, ui_draw_lock);
    ptr->ui_animation_play_init = ui_animation_play_init;
    ptr->ui_animation_play_status_update = ui_animation_play_status_update;
    ptr->is_ui_backlight_on = is_ui_backlight_on;
    ptr->set_ui_backlight = set_ui_backlight;
    ptr->play_animation_lock = ui_play_animation_lock;
    ptr->play_animation_unlock = ui_play_animation_unlock;
    thiz = ptr;
    return ADD_MODULE("ui", ID_MODULE_UI, on_perpare_ui_init, 
        on_post_ui_init, ptr, list_head, sizeof(ui_manager_context_t));
}
