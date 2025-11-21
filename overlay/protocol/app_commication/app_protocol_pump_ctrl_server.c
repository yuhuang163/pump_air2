#include "root_common_api.h" // IWYU pragma: keep
#include "app_protocol_handler.h"
#include "protocol_manager_api.h"
#include "storage_manager_api.h"
#include "app_protocol_pump_ctrl_server.h"

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
//0xd0
void app_old_comm_report_device_status(void)
{
    LOG_PROTOCOL(" Reporting device status\n");
    uint8_t report_data[45] = {0};
    uint16_t data_len = 0;
/*
    report_data[data_len++] = app_old_comm_dev_status.battery;
    report_data[data_len++] = (uint8_t)app_old_comm_dev_status.status;
    report_data[data_len++] = app_old_comm_dev_status.left_right;
    report_data[data_len++] = (uint8_t)app_old_comm_dev_status.milk_mode;
    report_data[data_len++] = app_old_comm_dev_status.milk_gear;
    report_data[data_len++] = app_old_comm_dev_status.custom_mode;
    report_data[data_len++] = app_old_comm_dev_status.custom_gear;
    report_data[data_len++] = app_old_comm_dev_status.custom_freq;
    report_data[data_len++] = (app_old_comm_dev_status.total_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.total_time >> 8) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.mode_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.mode_time >> 8) & 0xFF;
    report_data[data_len++] = app_old_comm_dev_status.milk_volume;
    report_data[data_len++] = app_old_comm_dev_status.error_flag;
    report_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 0) & 0xFF;
    report_data[data_len++] = (app_old_comm_dev_status.custom_total_time >> 8) & 0xFF;
    report_data[data_len++] = app_old_comm_dev_status.travel_lock;*/

    app_old_comm_cmd_send(APP_OLD_COMM_CT_NOTIFY, APP_OLD_COMM_CID_NOTIFY_STATUS, data_len, report_data);
}

//0xb0
void app_old_comm_handle_get_status_req(void)
{
    LOG_PROTOCOL(" Handling get device status request\n");
    uint8_t ack_data[45] = {0};
    uint16_t data_len = 0;



    LOG_PROTOCOL(" Sending device status ACK (data len: %d)\n", data_len);
    app_old_comm_cmd_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_STATUS, data_len, ack_data);
}

//0xc0
void app_old_comm_handle_ctrl_dev_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling device control request\n");
    if (cal < 0x06) {
        LOG_PROTOCOL(" Invalid data length for CTRL_DEV (expected >=6, got %d)\n", cal);
        return;
    }

    AppOldCommDevStatus new_status = (AppOldCommDevStatus)cab[0];
    AppOldCommMilkMode new_mode = (AppOldCommMilkMode)cab[1];
    uint8_t new_gear = cab[2];

    LOG_PROTOCOL(" Updating device status - Status: %d, Mode: %d, Gear: %d\n",
           new_status, new_mode, new_gear);

    // TODO: Send parameters to pump valve component
    app_old_comm_cmd_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_CTRL_DEV, 0, NULL);
    app_old_comm_report_device_status();
}

//0xc4
void app_old_comm_handle_set_user_define_req(uint8_t cal, uint8_t *cab)
{


}

//0xf0
void app_old_comm_handle_get_dev_info_req(uint8_t cal, uint8_t *cab)
{
    LOG_PROTOCOL(" Handling get device info request\n");
    if (cal != 0x04) {
        LOG_PROTOCOL(" Invalid data length for GET_DEV_INFO (expected 4, got %d)\n", cal);
        return;
    }
    storage_manager_context_t *ctx = (storage_manager_context_t *)find_module(ID_MODULE_STORAGE);
    factory_dev_info_t factory_dev_info;
    AppOldCommDeviceInfo* comm_dev_info = app_old_comm_dev_info_get();
    EXCEPTION_CHECK_VOID(ctx);
    EXCEPTION_CHECK_VOID(cab);
    uint8_t ack_data[64] = {0};
    uint16_t data_len = 0;

    ack_data[data_len++] = ctx->priv.ver_info.sys_local_protocol_id;
    ack_data[data_len++] = ctx->priv.ver_info.soc_version;
    ack_data[data_len++] = ctx->priv.ver_info.hw_version;
    ack_data[data_len++] = ctx->priv.ver_info.sw_version;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[0];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[1];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[2];//custom_mode_id;
    ack_data[data_len++] = ctx->priv.sys_info.pump_info.custom_steps_parameter.id.dat[3];//custom_mode_id;
    ack_data[data_len++] = 0x00;//milk_full_ver;
    ack_data[data_len++] = 0x00;//milk_full_switch;
    ctx->get_store_val(STORE_ID_FACTORY_DEV_INFO, 0, 0, &factory_dev_info);
    memcpy(ack_data + data_len, factory_dev_info.sn, factory_dev_info.sn_len);
    data_len += factory_dev_info.sn_len;
#if BLE_PAYLOAD_CRYPTION
    ack_data[data_len++] = comm_dev_info->encrypt_status;
    ack_data[data_len++] = comm_dev_info->encrypt_key;
#endif

    LOG_PROTOCOL(" Sending device info ACK (data len: %d)\n", data_len);
   app_old_comm_cmd_send(APP_OLD_COMM_CT_ACK, APP_OLD_COMM_CID_GET_DEV_INFO, data_len, ack_data);
}
