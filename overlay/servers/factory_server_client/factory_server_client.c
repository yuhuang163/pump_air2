/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "ble_manager_api.h"
#include "root_common_api.h" // IWYU pragma: keep
#include "factory_server_client.h" // IWYU pragma: keep
#include "common_service.h"
/*
* DEFINES
*****************************************************************************************
*/

/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/
static protocol_manager_context_t *protocol_ctx = NULL;
static ble_manager_context_t *ble_ctx = NULL;

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
static void on_packet_subscribe_callback(int err_code, uint8_t *data, uint16_t len)
{
    LOGI("on_packet_subscribe_callback111, err_code: %d", err_code);
    LOG_HEX("data", data, len);
}

static void on_factory_server_event_cb(void *user_data, uint8_t *data, uint16_t data_len)
{
    LOGE("==> on_factory_server_event_cb: data=%p, len=%d", data, data_len);
    /// 缺少组包功能，先直接订阅
    comm_protocol_subscribe_context_t ctx = {
        .buffer = data,
        .size = data_len,
        .cb = on_packet_subscribe_callback,
    };
    LOGE("==> before call comm_protocol_subscribe_handler, ctx.buffer=%p, ctx.size=%d", ctx.buffer, ctx.size);
    comm_protocol_subscribe_handler(&ctx);
    LOGE("==> after call comm_protocol_subscribe_handler");
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
void factory_server_client_init(void)
{
    ble_ctx = (ble_manager_context_t *)find_module(ID_MODULE_BLE);
    EXCEPTION_CHECK_VOID(ble_ctx);
    protocol_ctx = (protocol_manager_context_t *)find_module(ID_MODULE_PROTOCOL);
    EXCEPTION_CHECK_VOID(protocol_ctx);
    ble_ctx->add_ble_evt_lisenser(ble_ctx, DEVICE_BLE_FAC_MSG_CHANNEL, on_factory_server_event_cb);
}
