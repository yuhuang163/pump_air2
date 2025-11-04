#ifndef __UI_MANAGER_API_H__
#define __UI_MANAGER_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "root_common_base.h" // IWYU pragma: keep
#include "broadcast_manager_api.h" // IWYU pragma: keep
#include "root_message.h" // IWYU pragma: keep

struct rt_device_node;
typedef struct ui_manager_context {
    /// private:
    struct __ui_manager_private {
        thread_context_t ui_hdr_thd; /// UI显示调度线程
        os_mutex_id ui_draw_lock; /// 显示刷新锁
        struct rt_device_node *panel_dev; /// 显示设备
        bool is_playing_ui_animation; /// 是否正在播放UI动画
        uint8_t ui_backlight_val; /// 背光亮度
    } priv;

    /// public:
    void (*set_ui_backlight)(struct ui_manager_context *thiz, uint16_t val);
    bool (*is_ui_backlight_on)(struct ui_manager_context *thiz);
    void (*ui_animation_play_status_update)(struct ui_manager_context *thiz, bool flag);
} ui_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 

