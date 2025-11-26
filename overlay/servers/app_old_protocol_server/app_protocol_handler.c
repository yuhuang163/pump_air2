/*
* INCLUDE FILES
*****************************************************************************************
*/
#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_handler.h"
#include "protocol_manager_api.h"
#include "storage_manager_api.h"

/*
* DEFINES
*****************************************************************************************
*/
app_old_protocol_handler_context_t ctx_app_old_protocol_handler;

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
static void app_old_comm_poll_timer_handler(void *handle)
{
    LOG_PROTOCOL(" Poll timer triggered - reporting device status\n");
    app_old_comm_report_device_status(APP_OLD_COMM_LEFT_CT);
    app_old_comm_report_device_status(APP_OLD_COMM_RIGHT_CT);
}

/*
* GLOBAL FUNCTION DEFINITIONS
*****************************************************************************************
*/
// Handle request commands (APP->device)
void app_old_comm_handle_request_cmd(app_old_comm_cmd_id_e cid, uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling %s request (data len: %d)\n",
           app_old_comm_get_cmd_id_str(cid), cal);

    // Print command data if available
    if (cal > 0 && cab != NULL) {
        LOG_PROTOCOL(" Command data: ");
        LOG_HEX("app_recv_data hex", cab, cal);
    }
#if 0
    // 对除了传输加密key和测试以外的所有指令数据进行加密
    if (ctx_app_old_protocol_handler.encrypt_status == 1) {
        if ((cid != APP_OLD_COMM_CID_GET_ENCRYPT) && (cid != APP_OLD_COMM_CID_GET_DEV_INFO)) {
            app_old_protocol_data_cryption(ctx_app_old_protocol_handler.encrypt_key, cab, cal); //解密密ble数据
        }
    }
    LOG_HEX("app_recv_data cryption hex", cab, cal);
#endif
    switch (cid) {
        case APP_OLD_COMM_CID_HEARTBEAT:
            app_old_comm_handle_heartbeat_req();
            break;
        case APP_OLD_COMM_CID_GET_DEV_INFO:
            app_old_comm_handle_get_dev_info_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_GET_DEV_SN:
        case APP_OLD_COMM_CID_GET_SN:
            app_old_comm_handle_get_sn_req();
        case APP_OLD_COMM_CID_SET_SN:
            app_old_comm_handle_set_sn_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SET_RTC:
            app_old_comm_handle_set_rtc_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SYS_CTRL:
            app_old_comm_handle_sys_ctrl_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_GET_ENCRYPT:
            app_old_comm_handle_get_encrypt_req();
            break;
        case APP_OLD_COMM_CID_GET_STATUS:
            app_old_comm_handle_get_status_req(APP_OLD_COMM_LEFT_CT);
            app_old_comm_handle_get_status_req(APP_OLD_COMM_RIGHT_CT);
            break;
        case APP_OLD_COMM_CID_CTRL_DEV:
            app_old_comm_handle_ctrl_dev_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SET_BIND:
            app_old_comm_handle_set_bind_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_USER_DEFINE:
            app_old_comm_handle_set_user_define_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_PARA_DOWNLOAD:
            app_old_comm_handle_set_user_define_wr_flash_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SET_TRAVEL_LOCK:
            app_old_comm_handle_set_travel_lock_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_GET_TRIPLE:
            app_old_comm_handle_get_triple_req();
            break;
        case APP_OLD_COMM_CID_SET_PRODUCTID:
            app_old_comm_handle_set_productid_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SET_DEVICEID:
            app_old_comm_handle_set_deviceid_req(cal, cab);
            break;
        case APP_OLD_COMM_CID_SET_ENCRYPTIONKEY:
            app_old_comm_handle_set_encryption_key_req(cal, cab);
            break;
            break;
        default:
            LOG_PROTOCOL(" Unknown request command 0x%02X\n", cid);
            break;
    }
}

// Handle response/report commands (device->APP)
void app_old_comm_handle_response_cmd(app_old_commcmd_type_e ct, app_old_comm_cmd_id_e cid, uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling %s (data len: %d)\n", app_old_comm_get_cmd_id_str(cid), cal);

    switch (cid) {
        case APP_OLD_COMM_CID_NOTIFY_STATUS:
            app_old_comm_report_device_status(APP_OLD_COMM_LEFT_CT);
            app_old_comm_report_device_status(APP_OLD_COMM_RIGHT_CT);
            break;
        case APP_OLD_COMM_CID_NOTIFY_BIND:
            app_old_comm_report_bind_status(1, 0);
            break;
        case APP_OLD_COMM_CID_NOTIFY_DISCON:
            app_old_comm_report_discon_status();
            break;
        default:
            LOG_PROTOCOL(" Unhandled response command %s (0x%02X)\n",
                   app_old_comm_get_cmd_id_str(cid), cid);
            break;
    }
}

void app_old_protocol_hander_frame_builder_send(app_old_commcmd_type_e cmdtype, app_old_comm_cmd_id_e cid,
                                                uint8_t data_len, uint8_t *data)
{
    EXCEPTION_CHECK_VOID(ctx_app_old_protocol_handler.protocol_ctx);
#if 0
    // 对除了传输加密key和测试以外的所有指令数据进行加密
    if (ctx_app_old_protocol_handler.encrypt_status == 1) {
        if ((cid != APP_OLD_COMM_CID_GET_ENCRYPT) && (cid != APP_OLD_COMM_CID_GET_DEV_INFO)) {
            app_old_protocol_data_cryption(ctx_app_old_protocol_handler.encrypt_key, data, data_len); //解密密ble数据
        }
    }
    LOG_HEX("app_recv_data cryption hex", data, data_len);
#endif
    ctx_app_old_protocol_handler.protocol_ctx->app_old_protocol_frame_builder_send(cmdtype, cid, data_len, data);
}

void app_old_protocol_ctx_init(void)
{
    memset(&ctx_app_old_protocol_handler, 0, sizeof(app_old_protocol_handler_context_t));
    CREATE_TIMER(&ctx_app_old_protocol_handler.poll_timer, "protocol_poll_timer", &ctx_app_old_protocol_handler,
                 OS_TIMER_PERIODIC, app_old_comm_poll_timer_handler);
    START_TIMER(ctx_app_old_protocol_handler.poll_timer, APP_OLD_COMM_HEARTBEAT_INTERVAL);
    ctx_app_old_protocol_handler.protocol_ctx = (protocol_manager_context_t *)find_module(ID_MODULE_PROTOCOL);
}