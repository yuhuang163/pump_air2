#ifndef __SERVERS_MANAGER_API_H__
#define __SERVERS_MANAGER_API_H__

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
typedef struct servers_manager_context {
    /// private:
    struct __servers_manager_private {
        thread_context_t servers_hdr_thd; /// Servers显示调度线程
        os_timer_id servers_tick_timer; /// Servers定时器
        os_mutex_id servers_draw_lock; /// Servers显示刷新锁
        broadcast_obj_t *bcd; /// 广播监听对象
        uint8_t encrypt_status;///payload 是否加密
    } priv;

    /// public:
    void (*servers_display_handler)(struct servers_manager_context *thiz);
    struct __servers_manager_hook {
        int (*on_perpare_init_route)(struct servers_manager_context *ctx);
        int (*on_post_init_route)(struct servers_manager_context *ctx);
         /// 广播路由
        int (*on_servers_manager_receiver_route)(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);
    } *hook;
} servers_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 

