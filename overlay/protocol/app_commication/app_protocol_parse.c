/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
//#include "app_protocol_handler.h"
#include "app_protocol_parse.h"
#include "protocol_manager_api.h"

/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static protocol_manager_context_t *thiz = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static uint16_t app_commication_ble_data_recv_cb(void *ctx, uint8_t *p_data, uint16_t ulLen)
{
    EXCEPTION_CHECK_INT(ctx);
    EXCEPTION_CHECK_INT(p_data);
    if (!p_data || ulLen== 0) {
        LOGE("recv data is NULL or size is 0");
        return -1;
    }
    LOGI("app_commication_ble_recv[%d]", ulLen);
    LOG_HEX("app_recv_data hex", p_data, ulLen);
    LOGI("app_recv_data str %s", p_data);

    //选择协议解析器
    if (thiz->protocol_parse == NULL)
    {
        LOGE("protocol_parse is NULL");
        return -1;
    }
    thiz->protocol_parse(p_data, ulLen, PROTO_TYPE_APP_COMMINICATION);

    return 1;
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void set_app_commication_ble_register(protocol_manager_context_t *ctx)
{
    EXCEPTION_CHECK_VOID(ctx);

    /* 注册 BLE 回调（存在时） */
    if (ctx->priv.ble_ctx && ctx->priv.ble_ctx->set_master_ble_register_cb) {
        ctx->priv.ble_ctx->set_master_ble_register_cb(app_commication_ble_data_recv_cb);
        LOGI("register app_commication ble cb");
    } else {
        LOGI("ble ctx or register cb not available");
    }

}

void set_app_commication_ble_unregister(protocol_manager_context_t *ctx)
{
    EXCEPTION_CHECK_VOID(ctx);

    if (ctx->priv.ble_ctx && ctx->priv.ble_ctx->set_master_ble_register_cb) {
        ctx->priv.ble_ctx->set_master_ble_register_cb(NULL); /// 清除工厂模式蓝牙数据接收回调函数.
        LOGI("unregister app_commication ble cb");
    } else {
        LOGI("ble ctx or unregister cb not available");
    }
 
}

void app_commication_send_data_via_ble(uint8_t *data, uint16_t data_len)
{
    const uint16_t MAX_SEND_LEN = 256;

    /* 参数验证 */
    if (!data || data_len == 0) {
        LOGI("invalid data or length");
        return;
    }

    if (!thiz) {
        LOGI("protocol manager context is NULL");
        return;
    }

    /* 长度限制 */
    if (data_len > MAX_SEND_LEN) {
        LOGI("data_len(%d) > MAX_SEND_LEN(%d), truncate", data_len, MAX_SEND_LEN);
        data_len = MAX_SEND_LEN;
    }

    /* 通过 BLE 发送 */
    if (thiz->priv.ble_ctx && thiz->priv.ble_ctx->ble_is_connected &&
        thiz->priv.ble_ctx->ble_is_connected()) {
        if (thiz->priv.ble_ctx->dispatch_ble_event_wrapper) {
            int ret = thiz->priv.ble_ctx->dispatch_ble_event_wrapper(
                thiz->priv.ble_ctx, BLE_MSG_MASTER_SEND_DATA, data, data_len);
            if (ret < 0) {
                LOGI("send via BLE failed, ret=%d", ret);
            } else {
                LOGI("send data via BLE[%d]", data_len);
            }
        } else {
            LOGI("ble dispatch wrapper missing");
        }
    }

}

uint8_t app_commication_protocol_parse_route(uint8_t *data, uint16_t len)
{
    // 这里添加APP协议的解析逻辑
    LOGI("app_commication protocol parse data len: %d", len);
    
    app_commication_send_data_via_ble(data, len); // 回传数据示例
    return 1;
}

int on_app_commication_protocol_br_receiver_route(broadcast_mask_id_t id, void *context, int arg1, int arg2, void *user_data)
{
    protocol_manager_context_t *ctx = context;

    switch (id.code) {
        case ID_BROADCAST_PM_STATE_CHANGE:
            {

            }
            break;
        case ID_BROADCAST_UPDATE_BATTERY_INFO:
            {

            }
            break;
        default:
            break;
    }
}

/// 初始化工厂协议模块
void on_perpare_app_commication_protocol_init(protocol_manager_context_t *ctx)
{

    ble_manager_context_t *ble_ctx = (ble_manager_context_t *)find_module(ID_MODULE_BLE);
    ctx->priv.ble_ctx = ble_ctx;
    LOGI("ble ctx: %p", ctx->priv.ble_ctx);
    thiz = ctx;
}

/// 初始化工厂协议模块
void on_post_app_commication_protocol_init(protocol_manager_context_t *ctx)
{
    set_app_commication_ble_register(ctx);
    thiz = ctx;
}
