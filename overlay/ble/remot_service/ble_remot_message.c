/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "ble_manager.h" // IWYU pragma: keep
#include "ble_remot_message.h" // IWYU pragma: keep

/*
* DEFINES
*****************************************************************************************
*/
/*
* LOCAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL VARIABLE DEFINITIONS
*****************************************************************************************
*/

/*
* LOCAL FUNCTION DEFINITIONS
*****************************************************************************************
*/

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
int ble_remot_message_recv_data(void *context, int srv_id, uint8_t *p_data, uint16_t size)
{
    ble_manager_context_t *ctx = (ble_manager_context_t *)context;
    EXCEPTION_CHECK_INT(ctx);
    LOG_BLE("ble_remot_recv_data[%d]", size);
    LOG_HEX("recv_data hex", p_data, size);
    return 0;
}

int ble_remot_srv_send_data(void *context, uint8_t *p_data, uint16_t size)
{
    ble_manager_context_t *ctx = (ble_manager_context_t *)context;
    EXCEPTION_CHECK_INT(ctx);

    if (!p_data || size == 0) {
        LOGE("send data is NULL or size is 0");
        return -1;
    }

    if (!ctx->ble_is_connected()) {
        LOGE("ble is not connected, ignore");
        return -2;
    }
    return 0;
}
