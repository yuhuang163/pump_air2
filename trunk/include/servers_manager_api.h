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
#include "app_old_protocol_parser.h"

typedef enum {
    SERVERS_PUBLISH_DATA_ALGO_SAMPLING = 100,        /// 算法采样数据
    SERVERS_PUBLISH_DATA_TESTS_KEY_STATUS = 101,     /// 产测按键状态上报
    SERVERS_PUBLISH_DATA_TESTS_ENCODER_STATUS = 102, /// 产测编码器状态上报
    SERVERS_PUBLISH_DATA_ID_MAX
} servers_publish_data_id_e;

typedef struct {
    int arg1;
    int arg2;
    void *val;
} servers_publish_data_args_t;

struct rt_device_node;
struct protocol_manager_context;
typedef struct servers_manager_context {
    /// private:
    struct __servers_manager_private {
        broadcast_obj_t *bcd; /// 广播监听对象
        struct protocol_manager_context *protocol_ctx; /// 协议管理器
        uint8_t encrypt_status;///payload 是否加密
    } priv;

    /// public:
    struct __servers_manager_hook {
        int (*on_perpare_init_route)(struct servers_manager_context *ctx);
        int (*on_post_init_route)(struct servers_manager_context *ctx);
        int (*factory_server_data_publish)(servers_publish_data_id_e id, servers_publish_data_args_t *args);
        int (*app_server_data_publish)(servers_publish_data_id_e id, servers_publish_data_args_t *args);
        int (*app_old_protocol_server_data_publish)(app_old_comm_cmd_id_e cid, servers_publish_data_args_t *args);
         /// 广播路由
        int (*on_servers_manager_receiver_route)(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data);
    } *hook;
} servers_manager_context_t;

#ifdef __cplusplus
}
#endif

#endif 

