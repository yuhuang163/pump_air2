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

#if CUST_LVGL_SUPPORT
#include "lv_conf.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#endif /// CUST_LVGL_SUPPORT

typedef struct ui_manager_context {
    /// private:
    struct __ui_manager_private {
        #if CUST_LVGL_SUPPORT
        thread_context_t lv_hdr_thd; /// lvgl 显示调度线程
        os_timer_id lv_tick_timer; /// lvgl tick定时器
        uint8_t lv_tick_status; /// lvgl tick状态
        #else
        thread_context_t ui_hdr_thd; /// UI显示调度线程
        #endif
        os_mutex_id ui_draw_lock; /// 显示刷新锁
    } priv;

    /// public:

} ui_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 

